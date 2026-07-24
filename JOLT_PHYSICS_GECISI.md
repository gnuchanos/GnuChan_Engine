# Jolt Physics Geçişi — Master Plan

> **Hedef:** Godot Physics SW + Bullet Physics'i tamamen kaldır, Jolt Physics entegre et.
> **Tarih:** 2025
> **Proje:** GnuChan Engine

---

## 📋 Faz 0: Ön Hazırlık

- [ ] Jolt Physics reposunu forkla / source'u al (https://github.com/jrouwe/JoltPhysics)
- [ ] Jolt Physics lisansını kontrol et (MIT — GPLv3 ile uyumlu) ✅
- [ ] Jolt'un build sistemini tanı (CMake + SCons opsiyonu var mı?)
- [ ] Jolt'un Godot 3.x portu var mı araştır (community portları)
- [ ] Mevcut Godot Physics API'si ile Jolt API'si arasındaki farkları belirle

---

## 📋 Faz 1: ESKİ FİZİK MOTORLARINI KALDIR

### 1.1 Bullet Physics Temizliği ✅

- [x] `src/modules/bullet/` — TÜM KLASÖR SİL (~200+ dosya)
- [x] `src/thirdparty/bullet/` — TÜM KLASÖR SİL
- [x] `src/modules/SCsub` — bullet alt dizinini kaldır
- [x] `src/modules/register_module_types.cpp` — bullet referanslarını kaldır
- [x] `src/modules/modules_builders.py` — bullet builder'ı kaldır
- [x] `src/modules/modules_enabled.gen.h` — bullet define'larını kaldır
- [x] `SConstruct` — builtin_bullet seçeneğini kaldır
- [x] `COPYRIGHT.txt` — thirdparty/bullet lisans bloğunu kaldır
- [ ] `src/editor/editor_export.cpp` — Bu dosyada bullet referansı yoktu
- [ ] `src/main/main.cpp` — Bullet include'ları kontrol edilecek
- [ ] `config.py` — modül zaten silindi, gerek kalmadı

### 1.2 Godot Physics SW 3D Temizliği ✅

- [x] `src/servers/physics/` — TÜM KLASÖR SİL (~40+ dosya)
- [x] `src/servers/SCsub` — physics/ satırı kaldırıldı
- [x] `src/servers/register_server_types.cpp` — `_createGodotPhysicsCallback` kaldırıldı, 3D kaydı kaldırıldı
- [x] `src/servers/physics_server.cpp` — `PhysicsServerSW` include'ı kaldırıldı
- [ ] `src/main/main.cpp` — Bullet include kontrol edilecek
- [ ] `src/core/project_settings.cpp` — physics/3d/ ayarları temizlenecek
- [ ] `src/scene/3d/` — physics SW referansları güncellenecek

### 1.3 MIT Embedded Kod Temizliği (İsteğe bağlı)

Şu an engine içinde embedded MIT kodu var:
- `src/servers/physics_2d/joints_2d_sw.cpp` — Chipmunk physics (MIT) → **KALABİLİR** (2D fizik, GPL-uyumlu)
- `src/scene/animation/easing_equations.h` — Robert Penner easing (MIT) → **KALABİLİR** (GPL-uyumlu)

> 🔴 **Not:** Bu iki dosyadaki MIT bildirimleri orijinal yazarların lisansını belirtir. GPLv3 ile uyumludur, kaldırılması ŞART DEĞİLDİR. İstersen embed edilmiş MIT kodunun orijinal referansını koruyarak ilerleyebiliriz.

---

## 📋 Faz 2: JOLT PHYSICS ENTEGRASYONU

### 2.1 Thirdparty Ekleme

- [ ] `src/thirdparty/jolt/` — Jolt Physics source'unu ekle
- [ ] Jolt'un LICENSE dosyasını ekle (MIT)
- [ ] `src/thirdparty/SCsub` — jolt/ build hedefini ekle
- [ ] `COPYRIGHT.txt` — Jolt Physics lisans bloğunu ekle

### 2.2 Jolt Wrapper Modülü Oluşturma

- [ ] `src/modules/jolt/` — yeni modül klasörü oluştur
- [ ] `src/modules/jolt/config.py` — can_build, configure, get_doc_classes
- [ ] `src/modules/jolt/SCsub` — build tanımı
- [ ] `src/modules/jolt/jolt_physics_server.h` — PhysicsServer arayüzü
- [ ] `src/modules/jolt/jolt_physics_server.cpp` — PhysicsServer implementasyonu
- [ ] `src/modules/jolt/jolt_body.h` / `.cpp` — Rigid body wrapper
- [ ] `src/modules/jolt/jolt_shape.h` / `.cpp` — Shape wrapper
- [ ] `src/modules/jolt/jolt_space.h` / `.cpp` — Space/World wrapper
- [ ] `src/modules/jolt/jolt_joint.h` / `.cpp` — Joint wrapper
- [ ] `src/modules/jolt/jolt_area.h` / `.cpp` — Area/Trigger wrapper
- [ ] `src/modules/jolt/jolt_collision.h` / `.cpp` — Collision detection bridge
- [ ] `src/modules/jolt/jolt_broadphase.h` / `.cpp` — Broadphase (Jolt'un kendi BP'si kullanılacak)

### 2.3 API Köprüsü

| Godot Physics API | Jolt Physics Mapping |
|---|---|
| `PhysicsServer::body_create()` | `JoltBody::create()` |
| `PhysicsServer::body_set_mode()` | `JoltBody::set_motion_type()` |
| `PhysicsServer::body_set_state()` | `JoltBody::set_position/rotation/velocity()` |
| `PhysicsServer::body_apply_impulse()` | `JoltBody::AddImpulse()` |
| `PhysicsServer::body_add_force()` | `JoltBody::AddForce()` |
| `PhysicsServer::area_create()` | `JoltArea::create()` |
| `PhysicsServer::shape_create()` | `JoltShape::create()` |
| `SpaceSW::test_body_motion()` | `Jolt::NarrowPhaseQuery::CastShape()` |
| `CollisionSolverSW::solve()` | `Jolt::NarrowPhaseQuery::CollideShape()` |
| `broad_phase_bvh` | Jolt'un kendi BP (built-in) |

### 2.4 Build Sistemi Güncellemeleri

- [ ] `SConstruct` — `builtin_jolt` seçeneği ekle
- [ ] `SConstruct` — `module_jolt_enabled` seçeneği ekle (default: yes)
- [ ] `SConstruct` — `builtin_bullet` seçeneğini KALDIR
- [ ] Jolt compile flags: `/std:c++17` gerekli mi kontrol et
- [ ] Jolt'un SSE4.2/AVX gereksinimini build'e yansıt

---

## 📋 Faz 3: TEST & DOĞRULAMA

- [ ] Build: `scons platform=windows target=debug` — derleniyor mu?
- [ ] Build: `scons platform=windows target=release_debug tools=yes` — editor derleniyor mu?
- [ ] Build: `scons platform=x11 target=debug` — Linux derleniyor mu?
- [ ] Runtime test: Boş sahne açılıyor mu?
- [ ] Runtime test: 3D RigidBody düşüyor mu?
- [ ] Runtime test: KinematicBody hareket ediyor mu?
- [ ] Runtime test: Area trigger çalışıyor mu?
- [ ] Runtime test: Joint (pin/hinge/cone) çalışıyor mu?
- [ ] Runtime test: RayCast sorgusu çalışıyor mu?
- [ ] Collision layer/mask testi
- [ ] Continuous collision detection testi
- [ ] Performance benchmark (eski vs yeni)

---

## 📋 Faz 4: DOKÜMANTASYON & TEMİZLİK

- [ ] `gles2_remove.md`, `gles3_to_gles4.md`, `gpl_fix.md` — güncelle veya birleştir
- [ ] `_gpl_modified.txt`, `_modified_files.txt`, `_errors.txt` — geçici dosyaları sil
- [ ] Jolt geçiş dokümanını `docs/` altına taşı
- [ ] Çakışan dosyaları temizle (`_temp/` içindeki build logları)

---

## 📦 Özet: Silinecek / Eklenecek Dosya İstatistikleri

| İşlem | Dosya Sayısı |
|---|---|
| **Silinecek:** `src/modules/bullet/` | ~200+ |
| **Silinecek:** `src/thirdparty/bullet/` | ~300+ |
| **Silinecek:** `src/servers/physics/` (Godot Physics SW) | ~40 |
| **Eklenecek:** `src/thirdparty/jolt/` | ~500+ (header-only + source) |
| **Eklenecek:** `src/modules/jolt/` (wrapper) | ~15-20 |
| **Değişecek:** build sistemi, scene, main, editor | ~10-15 |
| **Toplam değişiklik** | ~1000+ dosya |

---

## 🚨 KRİTİK NOTLAR

1. **Jolt Physics MIT lisanslı** → GPLv3 ile tam uyumlu ✅
2. **Jolt C++17 gerektiriyor** → Mevcut `-std=gnu++14` yükseltilmeli
3. **Jolt SSE4.2+ gerektiriyor** → x86_64'te sorun yok, ARM'de fallback gerekebilir
4. **Jolt'un kendi broadphase'i var** → Ayrı broadphase implementasyonu GEREKMEZ
5. **2D fizik DEĞİŞMİYOR** → `servers/physics_2d/` olduğu gibi kalır
6. **Bullet module build option** → Kaldırılmalı, Jolt ile değiştirilmeli
