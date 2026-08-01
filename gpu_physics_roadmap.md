# GPU Fizik + Motor Performans Yol Haritası (GnuChan Engine)

> **Hedef:** Render, threading, bellek ve fizik dahil gömülü motorun tamamında performansı, hedeflenen kullanım senaryosu olan **Unity seviyesindeki oyun performansına** çekmek. Çekirdek: GPU fizik + CPU/GPU dengeli boru hattı.

> Amaç: Mevcut CPU tabanlı `PhysicsServerSW` çözücüsünü koruyarak, sayısal fiziğin ağır yükünü GPU'ya taşımak. CPU/GPU dengesi: **broad-phase + query + API sahne integrasyonu CPU'da, integrasyon + çarpışma + çözücü GPU'da.**
> Yöntem: Her faz bağımsız tamamlanır ve doğrulanır. Bir faz bitmeden diğerine geçilmez.

---

## Mevcut Durum ve Temel Gerçekler

| Konu | Durum |
|---|---|
| Çözücü | `src/servers/physics/step_sw.cpp` — CPU, tek aşamalı (integrate → broadphase → island → solve → integrate_velocities) |
| Geniş faz | `src/servers/physics/space_sw.cpp` — CPU BVH/Octree |
| Motor seçimi | `PhysicsServerManager` (`physics/3d/physics_engine` ayarı) — `register_server()` ile yeni motor kaydı destekli |
| Render context (Windows) | `src/context_gl_windows.cpp` — **OpenGL 4.3 core profile** → compute shader, SSBO, atomics, image load/store **mevcut** |
| Offscreen GL | `ContextGL_Windows` içinde `hRC_offscreen` / `make_current_offscreen()` / `set_offscreen_gl_rc()` **hazır** — fizik ayrı bir paylaşımlı context'te koşabilir |
| Yeni motor kayıt noktası | `src/servers/register_server_types.cpp` (`_createGodotPhysicsCallback()`) |
| GPU fisik kalıntısı | `src/servers/physics/gpu_physics.windows.opt.tools.64.obj` — kaynak kodu yok, derleme kalıntısı. Yeni sistem `servers/physics/gpu/` altında sıfırdan yazılacak |

**Mimari karar:** `PhysicsServer` API'si (RID, body/space/area, query, callback) CPU'da sabit kalır — scene tree, Area sinyalleri ve raycast bunlara bağlı. Sayısal çekirdek GPU'ya taşınır. Yeni motor `PhysicsServerManager` üzerinden `GpuPhysics` adıyla kaydedilir; `physics/3d/physics_engine = "GpuPhysics"` seçilince devreye girer, `GodotPhysics` geri dönüşü korunur.

**Tasarım kısıtları:**
- Frame başına **tek** readback: transform'lar çift buffer (PBO/persistent map) ile CPU'ya döner. Substep başına readback YOK (performans darboğazı).
- GPU state SSBO'larda yaşar; kuvvet/impulse girişleri `glBufferSubData` ile append edilir, atomik sayaçlarla tüketilir.
- Parçacık-akışkan tarzı sistemler de aynı SSBO/manifold boru hattını kullanır (Faz 8'e kadar rigid-body odaklı).

---

## FAZ 0 — Altyapı: GPU fizik iskeleti ve derleme yolu

**Kapsam:**
- `src/servers/physics/gpu/` klasörü açılır; `SCsub`'a eklenir (`servers/physics/SCsub`).
- `GpuPhysicsServer` (CPU) — `PhysicsServer`'dan türeyen, şimdilik **tüm çağrıları `PhysicsServerSW`'e passthrough** yapan katman. `register_server_types.cpp`'ye `GpuPhysics` olarak kaydedilir.
- GL 4.3 compute + SSBO için ayrı `GpuPhysicsContext`: paylaşımlı offscreen context (Windows: `ContextGL_Windows::set_offscreen_gl_rc`), `glad` yüklemesi, compute shader derleme yardımcıları.
- Compute shader kaynakları için derleme yolu (`gles_builders.py` benzeri bir builder veya inline `#include` string'leri). İlk shader: boş `#version 430` kernel.

**Dosyalar:**
- `src/servers/physics/gpu/gpu_physics_server.h/cpp` (passthrough)
- `src/servers/physics/gpu/gpu_physics_context.h/cpp` (context + SSBO + shader yardımcıları)
- `src/servers/physics/gpu/shaders/init.comp` (boş/başlangıç kernel)
- `src/servers/physics/SCsub`, `src/servers/register_server_types.cpp`

**Kabul kriteri:**
- `physics/3d/physics_engine = "GpuPhysics"` seçilince oyun hiç fark edilmeden aynı davranır (passthrough).
- Doğrulama kernel'i çalışır, `glGetError()` temiz, CPU'da üretilen marker sonucu `glMapBuffer` ile okunur.

---

## FAZ 1 — Body state SSBO'su (CPU ↔ GPU veri taşıma)

**Kapsam:**
- Aktif cisimler için SSBO şeması: `Transform` (mat3x4), `linear/angular velocity`, `inv_mass`, `inv_inertia`, `gravity_scale`, friction/bounce, axis_lock bitleri, uyku bayrağı.
- Cismin oluşumu/değişimi (create/set_state/set_param) CPU tarafında SSBO'ya `glBufferSubData` ile yazılır; silme işlemi ölü listesi ile frame sonunda işlenir.
- `BodySW` → SSBO index eşlemesi: `RID` → index tablosu; GPU'da index sabit kalır (fragman yok, sadece aktif bayrağı).

**Dosyalar:**
- `src/servers/physics/gpu/body_ssbo.h/cpp`
- `gpu_physics_server.h/cpp` içine eşleme ve yükleyici

**Kabul kriteri:**
- 10.000 cisimlik yükleme/okuma döngüsü: GPU'dan okunan transform set, CPU'daki referansla bit-bit aynı (test programı veya hata ayıklama çıktısı).

---

## FAZ 2 — GPU integrasyon (kuvvet + hız)

**Kapsam:**
- Kernel `integrate_forces.comp`: gravity (Area etkisi ilk fazda dünya yerçekimi), toplam kuvvet/tork appender'larının tüketimi, `linear_damp`/`angular_damp`, yarı-implicit Euler: `v' = v + a*dt`; `x' = x + v'*dt`.
- `StepSW` pas geçişi: `GpuPhysicsServer::step()` CPU'da broadphase öncesi broadphase + island + solve hâlâ `PhysicsServerSW`'e delege edilir; ama **integrate_forces + integrate_velocities** GPU'da koşar ve transformlar CPU yapısına `body_set_state` ile geri yazılır. (Kontrollü geçiş — tüm boru hattı aynı anda GPU'ya alınmaz.)
- Güvenlik anahtarı: `run_on_gpu` cismi bazlı bayrak — sadece GPU'ya geçen cisimler kernel'e girer; kalanlar CPU'da kalır.

**Dosyalar:**
- `src/servers/physics/gpu/shaders/integrate_forces.comp`
- `src/servers/physics/gpu/physics_step_gpu.h/cpp` (GPU adımı yürütücü)

**Kabul kriteri:**
- Serbest düşüş: 60 fps'te 5 sn sonra GPU cisminin pozisyonu, CPU referans integrasyonuyla `1e-4` tolerans içinde.
- 50.000 cisim: GPU adım süresi < 1ms (CPU'da aynı iş ~birkaç ms).

---

## FAZ 3 — GPU geniş faz (uniform grid)

**Kapsam:**
- Aktif cisimlerin AABB'lerinden uniform grid'e yerleşim: hücre → cisim listesi (atomik sayaç + chunk tablosu).
- `sort` ve `dedup`: pair listesi SSBO'da (radix sort ya da hash-based dedup).
- CPU BVH **geçiş dönemi korunur**: `GpuPhysics` kendi pair listesini üretir, `PhysicsServerSW`'in broadphase'ine besler (doğruluk karşılaştırması için).

**Dosyalar:**
- `src/servers/physics/gpu/shaders/broadphase_grid.comp`
- `src/servers/physics/gpu/gpu_broadphase.h/cpp`

**Kabul kriteri:**
- 64×64×64 grid, 20.000 rastgele yerleşimli cisim: üretilen pair set, CPU BVH pair setiyle birebir aynı (sıralanmış karşılaştırma).
- Pair üretimi GPU'da < 1.5ms.

---

## FAZ 4 — GPU dar faz + contact manifold

**Kapsam:**
- Şekil desteği sıralı: sphere → box → capsule → convex (bu fazda sphere/box).
- Kernel `narrowphase.comp`: her pair için CPU tarafındaki `collision_solver_sat` / `gjk_epa` mantığının GLSL karşılığı; contact noktası, normal, penetrasyon derinliği.
- Contact manifold SSBO'su (max contact/saniye başına atomik sayaç); çarpışma istatistikleri (collision_pairs) CPU'ya okunur.

**Dosyalar:**
- `src/servers/physics/gpu/shaders/narrowphase.comp`
- `src/servers/physics/gpu/gpu_narrowphase.h/cpp`

**Kabul kriteri:**
- Bir küpün yere düşüşü: manifold normali +Y, penetrasyon sıfıra yakınsar; CPU referansıyla aynı normal (test sahnesi).

---

## FAZ 5 — GPU çözücü (island başına workgroup)

**Kapsam:**
- Çözüm yöntemi: **XPBD** (position-based) veya sequential impulse — küçük island'lar için XPBD daha stabil ve GPU dostu.
- Island tespiti: pair bağlılığından union-find (GPU'da iteratif); her island bir workgroup'e atanır; island kimlikleri kontakt manifold'larına bağlanır.
- Kernel `solve.comp`: `num_iterations` iterasyon — yüzey geri itme, sürtünme (Coulomb), motor/bounce parametreleri.
- Sleeping: düşük hız eşiğine ulaşan island'lar uykuya alınır (CPU'da onaylanır).

**Dosyalar:**
- `src/servers/physics/gpu/shaders/islands.comp`, `solve.comp`
- `src/servers/physics/gpu/gpu_solver.h/cpp`

**Kabul kriteri:**
- 10 katmanlı kutu yığını (100 kutu) 60 fps'te stabil durur — çökme, zıplama, iç içe geçme yok; 5sn boyunca.
- Denge: kutu yığını sarsıntısız; friction parametresi CPU davranışıyla eşleşir.

---

## FAZ 6 — Readback ve render senkronu

**Kapsam:**
- Çift buffer SSBO/PBO: frame N çözülürken frame N-1'in transformları CPU'ya eşlenir (`glMapBufferRange` + fence).
- GPU transform → `BodySW` senkronu: `body_set_state(BODY_STATE_TRANSFORM)` vb. doğrudan GPU buffer'ından yazılır (Python/script değil, C++; VisualServer'daki `VisualServerBody` güncelleme yolu).
- Area monitoring ve signal verileri manifold'dan beslenir.

**Dosyalar:**
- `src/servers/physics/gpu/gpu_readback.h/cpp`
- `gpu_physics_server.h/cpp` — `step()` sonu senkron

**Kabul kriteri:**
- 20.000 cisim render'da GPU pozisyonlarında, akıcı (frame pacing bozulmadan).
- Readback toplam gecikmesi ≤ 1 frame; CPU maliyeti `memcpy` seviyesinde.

---

## FAZ 7 — Query ve API köprüsü

**Kapsam:**
- `intersect_ray`, `intersect_point`, `intersect_shape`, `cast_motion` gibi sorgular: GPU transformlar CPU'ya geldikten sonra CPU'da çalışır (sorgu sıklığı düşük olduğundan CPU yeterli; GPU'ya taşımak getiri sağlamaz).
- `PhysicsDirectSpaceState` / `PhysicsDirectBodyState` arayüzleri GPU server'ında yeniden uygulanır (yarı-uyumlu ilk sürüm: transform/velocity/contact okumaları SSBO'dan).
- `body_test_motion` (kinematik karakterler için) CPU'da kalır.

**Dosyalar:**
- `src/servers/physics/gpu/gpu_direct_state.h/cpp`

**Kabul kriteri:**
- Karakter (CharacterBody) hareketi + raycast, GodotPhysics'ten ayırt edilemeyecek sonuçlar verir (test sahnesi karşılaştırması).

---

## FAZ 8 — Joint'ler, yumuşak gövde, CCD ve parçacık/akışkan uzantıları

**Kapsam:**
- Joint'ler: pin, hinge, slider, cone_twist, 6DOF (önce pin/hinge; geri kalan CPU fallback).
- Soft body: `soft_body_*` API'si GPU'da (cloth/XPBD) — ilk sürüm GPU integrasyon + pins.
- CCD: yüksek hız için sweep — sadece hıza duyarlı cisimlerde.
- Takip eden: GPU parçacık/akışkan/kumaş sistemleri aynı manifold boru hattını kullanır.

**Dosyalar:**
- `src/servers/physics/gpu/shaders/joints.comp`, `softbody.comp`
- `src/servers/physics/gpu/gpu_joints.h/cpp`, `gpu_softbody.h/cpp`

**Kabul kriteri:**
- Mevcut Godot demo senaryoları (joint testi, soft body, çarpışma sahnesi) `GpuPhysics` altında çalışır; doğrulanabilir görsel tutarlılık.

---

## Faz Dışı / Kararlar

- **Bullet/PhysX entegrasyonu:** kod tabanında Bullet yok; plan dahilinde değil. GPU çekirdeğimiz sıfırdan.
- **Query'ler GPU'da:** yapılmaz (karmaşıklık/getiri dengesi CPU lehine).
- **GL cihaz bağımlılığı:** compute shader GL 4.3 gerektirir (Windows context zaten 4.3 açıyor). ES cihazlar / düşük GL sürümleri için `GpuPhysics` otomatik `GodotPhysics`'e düşer (runtime check).
- **Geçiş anahtarı:** `physics/3d/physics_engine = "GpuPhysics"` — varsayılan `GodotPhysics` korunur; her faz kendi `project.gnuchan` test projesiyle doğrulanır.

---

## MOTOR GENELİ PERFORMANS FAZLARI (Unity Seviyesi Hedef)

> Fizik fazlarının (FAZ 0-8) yanı sıra, Unity'ye denk performans için gereken sistem geneli optimizasyonlar. Her faz bağımsız doğrulanır. Öncelik sırası: render darboğazları > threading > bellek > sahne.

### PERF-1 — Ölçüm ve Darboğaz Temel Hattı
- Kalıcı benchmarking altyapısı: `--benchmark` çıktısı + `Performance` singleton + frame profiler (CPU/GPU timestamp, draw calls, physics ms).
- Her fazdan önce/sonra aynı test sahneleriyle (10k statik + 2k dinamik nesne) sayısal hedefler karşılaştırılır.
- `performance.md`'deki Quick Win ayar listesi bir "perf template" olarak projeye gömülür: async shader cache, depth prepass, batching, s3tc/bptc import.

### PERF-2 — Render Draw-Call ve State Yönetimi
- 3D: render state sıralama (sort key: shader → material → mesh), state change patlamalarını azaltma.
- 3D: static/dynamic instancing — statik örgü ve GPUParticles için otomatik instance buffer; frustum + distance culling uç durumları.
- 2D: mevcut batching tamamlayıcısı — batch limitine ulaşınca otomatik partisyon, `use_multirect` açık.
- Hedef: aynı sahne için draw call sayısında %30-50 azalma (Unity 2022 seviyesi).

### PERF-3 — Threading / Job Sistemi
- `rendering/threads/thread_model=3` (Separate 3D) tam destek + güvenli fallback.
- Fizik step'ini ana thread'den ayırma: `PhysicsServerSW::step` → işçi thread (GPU fizik devreye girdikçe zaten GPU'ya kayar).
- Parçacık güncelleme, navmesh ve area query'leri job olarak parçalanır (mevcut command queue altyapısı üzerine).
- Hedef: dört çekirdekli sistemde ana thread frame süresi %40 altına; sistem GPU-bound kalır.

### PERF-4 — Bellek ve Allocator
- Ana oyun döngüsündeki alloc/destroy patlamaları için object pooling: Nodes/RIDs/parçacıklar.
- RID pool prealloc (`memory/limits/multithreaded_server/rid_pool_prealloc`) sahne boyutuna göre ayarlanır + dokümanlanır.
- Vertex data: `split_stream` + 16-bit index + vertex compression default açık; texture VRAM formatları s3tc/bptc.
- Hedef: RAM sabitlenmiş (GC baskısı yok), VRAM bazaltının altında.

### PERF-5 — Culling ve Görünürlük
- Portal rendering zaten var → release'de default aktif, PVS basitleştirme.
- Occlusion culling: mevcut sphere/polygon CPU sürümüne ek olarak GPU occlusion query veya software rasterizer (Unity soft-occlusion eşdeğeri) — `rendering/misc/occlusion_culling/*` genişletilir.
- Spatial partition: `render_tree_balance` ve 3D BVH güncel tutma.
- Hedef: açık dünya segmentlerinde çizilen nesne sayısında %50+ azalma.

### PERF-6 — Sahne ve Script Çalışma Zamanı
- Scene tree işleme maliyeti: `NOTIFICATION_PROCESS/PHYSICS_PROCESS` filtreleme (`set_process(false)` rehberi + editor otomatik analiz aracı).
- GDScript bytecode derleme ve sabit sayıda arayüz çağrısı; sıcak döngülerde C++ tarafına taşıma rehberi.
- Gömülü profiler çıktısı: frame spikes, script ms, physics ms.

---

## İlerleme Durumu

- [ ] PERF-1 — Ölçüm ve darboğaz temel hattı
- [ ] PERF-2 — Render draw-call ve state yönetimi
- [ ] PERF-3 — Threading / job sistemi
- [ ] PERF-4 — Bellek ve allocator
- [ ] PERF-5 — Culling ve görünürlük
- [ ] PERF-6 — Sahne ve script çalışma zamanı
- [ ] FAZ 0 — GPU fizik iskeleti ve derleme yolu
- [ ] FAZ 1 — Body state SSBO'su
- [ ] FAZ 2 — GPU integrasyon
- [ ] FAZ 3 — GPU geniş faz (uniform grid)
- [ ] FAZ 4 — GPU dar faz + contact manifold
- [ ] FAZ 5 — GPU çözücü (island başına workgroup)
- [ ] FAZ 6 — Readback ve render senkronu
- [ ] FAZ 7 — Query ve API köprüsü
- [ ] FAZ 8 — Joint'ler, yumuşak gövde, CCD, parçacık/akışkan uzantıları
