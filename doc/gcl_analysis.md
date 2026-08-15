# GCL — Tam Kapsamlı Analiz (Kod Tabanı Durumu)

Bu doküman `src/modules/gcl/` kaynak kodunun **gerçek analizidir**; dilin ideal
tasarımı değil, yorumlayıcıda şu an **ne çalışıyor / ne bekliyor** anlatılır.

Referanslar:
- `doc/gcl_language.md` — dil tasarımı (hedef sözdizimi)
- `doc/gcl.md` — motor odaklı kullanım kılavuzu
- `doc/gcl.html` — derlenmiş renkli döküman
- Bu dosya — **kod ile eşleşen analiz** (son fix paketi işlenmiştir)

---

## 1. Mimari Özet

```
gcl_script.cpp  (ScriptLanguage entegrasyonu: .gcsf / .gclib)
      │
      ├── gcl_loader.cpp ──→ #include metin birleştirme + yorum temizleme
      ├── Lexer / comment_tokenize ──→ strip_comments (yorum temizleme)
      ├── Parser ──→ extern_scan, variable_scan, function_params_scan
      │           ──→ number_type_get, printf_format_get
      ├── Ast / Ir  (bildirim seviyesinde pipeline; çalışma yolu interpreter)
      ├── Autocomplete ──→ editor tamamlama önerileri
      ├── Highlight    ──→ editor sözdizimi renklendirme
      └── Executor    ──→ satır satır yorumlayıcı + Time.Sleep + class
```

**Kritik bulgu:** Dil iki iş akışına sahip:

1. **Scanner modülleri** (`Lexer`, `Parser`, `Ast`, `Ir`) — token/AST/IR
   üretir ama **çalışma anında kullanılmaz**; daha çok düzen/plan katmanıdır.
   `parser.h`'daki `comment_parse`, `function_parse`, `number_type_get`,
   `printf_format_get` ve `ir.h`'daki lower fonksiyonları referans tablolar
   sunar.
2. **Yorumlayıcı** (`Executor/executor.cpp` + `executor_*.h` + `gcl_script.cpp`
   + `gcl_loader.cpp`) — script'lerin gerçekten çalıştığı yer. Satır satır
   parse eder, `members` tablosu (self sistemi) üzerinde yürütür.

Analiz bundan sonra **yorumlayıcı + entegrasyon** üzerinden ilerler.

---

## 2. Kaynak Dosya Haritası

| Dosya | Görev |
|---|---|
| `gcl_loader.cpp` | `.gcsf/.gclib` yükleme: `#include` metin birleştirme → `strip_comments` → `GCLScript` |
| `Lexer/lexer.h` | `TokenKind`, `comment_tokenize` (sadece yorum tokenları), `strip_comments` |
| `Parser/parser.h` | `function_parse`, `function_is_known`, sayı/printf tabloları, `extern_scan`, `variable_scan`, `function_params_scan` |
| `Ast/ast.h` | `NodeKind` (COMMENT_LINE/BLOCK, FUNCTION, PROGRAM) |
| `Ir/ir.h` | `IrOp`, lower fonksiyon bildirimleri |
| `Parser/variable.cpp` | değişken taraması (imza satırlarını global listeye sokmaz) |
| `Executor/executor_format.h` | printf formatlama, tanımlayıcı kuralları, `split_declaration`, `@N` boyut, `sizeof` |
| `Executor/executor_ops.h` | aritmetik/bitwise/ternary/sizeof, operatör typedef isimleri |
| `Executor/executor_flow.h` | atama, koşul, if/else-if/elif, while, do-while, for, foreach, switch |
| `Executor/executor_core.h` | değer/literal, struct/enum/typedef, PrintF, Time.*, self/extern çağrıları, NODE hiyerarşisi, genel zincir çağrıları |
| `Executor/executor_class.h` | `class`, kalıtım, `head`, `@return`, instance Çağrıları |
| `Executor/executor.cpp` | satır döngüsü `executor_run_ex`, gövde bulma, strip_bodies |
| `Autocomplete/autocomplete.cpp` | editor tamamlama (self., Input., Engine., Time., NODE değişkenleri) |
| `Highlight/gcl_syntax_highlighter.h` | 10 renkli sözdizimi renklendirme |
| `gcl_script.h/.cpp` | `ScriptLanguage/GCLScriptInstance`: Ready/Update/UpdatePhysics, self sistemi, Sleep devamı, `validate`, `execute_file` |
| `gcl_loader.h/.cpp` | `.gcsf` / `.gclib` resource loader/saver |

---

## 3. Çalışma Zamanı (Runtime) Akışı

```
GnuChanEngine (oyun döngüsü)
  └─ Node notification
       ├─ READY          → GCLScriptInstance::call("Ready")
       ├─ PROCESS        → call("Update", delta)      (set_process açıldıysa)
       └─ PHYSICS_PROCESS→ call("UpdatePhysics", delta)
            │
            ├─ members["self"] = sahip Godot node'u
            ├─ Update/UpdatePhysics: members["delta"] = node->get_process_delta_time()
            ├─ Ready: global scope (executor_strip_bodies) çalıştırılır — HER SEFERİNDE DEĞİL, only Ready
            ├─ Time.Sleep kontrolü (süre dolduysa kalan satırlar tek sefer koşar)
            └─ executor_find_body(code, fonksiyon) → executor_run(gövde)
```

### Editor güvenliği
`GCLScriptInstance` yapıcısı `Engine::is_editor_hint()` ise `set_process`
açmaz ve `notification` editor modunda erken döner — yoksa editor sahnesinde
her kare `Update` çalışıp console'a çıktı basardı.

### Yükleme (loader) akışı
```
gcl_loader.cpp::load
  ├─ include_resolve("#include "lib.gclib"", ...)  → YORUM TEMİZLEMEDEN ÖNCE
  │     recursive (16 derinlik), "alıntı" / <açısal> path'ler, res:// / user:// globalize
  ├─ strip_comments                                → yorumları ayıkla
  └─ GCLScript resource
```

---

## 4. Gerçekte Çalışan Dil Özellikleri

### 4.1 Değişkenler
- Tipler: `int8..int128`, `uint8..uint128`, `float16..float128`, `short/int/long`,
  `float/double`, `char`, `gcChar`, `bool`, `void`
- Pointer/dizi imlaları desteklenir: `char *`, `TEXT[]`, `[N]`, `@N` (boyut etiketi)
  — `split_declaration` temizler.
- Bildirim: `<tip> <isim> = <değer>;`, çoklu bildirim `int a, b, c = 5;`
- Niteleyiciler: `public`, `private`, `const`, `static`, `inline`, `global`
- Kullanıcı tipi: `NODE`, `REF`, `Node`, `NodeRef`, typed (struct/enum/typedef adları),
  `tuple`, `dict`, class adları

### 4.2 Literaller / Değerler
- `true / false / null`
- `"metin"`, `'k'`
- `{ ... }` → dict (`anahtar : değer`), struct (`.alan = değer`), array
- `(a, "b", 10)` → tuple → `Array`
- `dict` → `Dictionary` (`.` / `[]` erişim)
- Sayı, değişken, zincir `A.B.C`, indeks `A[0]`

### 4.3 Operatörler (executor_ops.h)
Sıralama (bulma önceliği):
1. `||`, `&&`
2. bitwise `|`, `^`, `&`
3. `==`, `!=` (C imlasında `p_s[i+1]=='!'` kontrolü dikkat: `!=` değil `!!` — kodda `==`'den sonra `!!` aranır; `!=` ayrıca `eval_condition` ve `find_best_op`'ta ayrı ele alınır)
4. `<`, `>` (shift `<<`/`>>` hariç)
5. shift `<<`, `>>`
6. `+`, `-`
7. `*`, `/`, `%`
- Tekli: `!`, `~`, `-`
- Ternary `? :`, `sizeof(tip|değişken)`
- Operatör typedef isimleri: `and`, `or`, `not`, `equals`, `notEquals`,
  `bitAnd`, `bitOr`, `bitXor`, `bitNot`, `leftShift`, `rightShift`
- **`<=`, `>=` düzeltmesi:** `solve_arith` RHS'teki `=` sembolünü tüketir
  (`is_inclusive`); `while (i <= n)` gibi koşullar artık doğru sonuç verir.

### 4.4 Atamalar
- `=` düz atama, `+= -= *= /= %=`, `++ --` (önek/sonek)
- Obje member: `Body.Rotation.y = 5` ve `Body.Rotation.y += x`
- Noktalı zincirde `A.B.y` (Vector3/Color vb. eksen), `A.B` (property),
  Dict zincirleri okuma modunda

### 4.5 Kontrol Akışı (executor_flow.h)
- `if / else if / elif / else` — ilk TRUE dal çalışır, sonraki dallar atlanır
- `while`, `do { } while(...)`, `for` (C tarzı + `for (x in dizi)` foreach)
- `switch (x) { case A: ... break; default: ... }`
  - `case` gövdesinde obje üye ataması düzeltildi (`Body.Rotation.y = 5` artık yutulmuyor)
- `break`, `continue`, `return [ifade]`

### 4.6 Preprocessor
`#define`, `#undef`, `#warning`, `#error`, `#debug`, `#ifdef`, `#ifndef`,
`#if`, `#elif`, `#else`, `#endif`
- `#if windows` özel durumu; `defined(AD)`; aritmetik koşul
- Bölge (region) yönetimi: pasif bölge satırları kod olarak işlenmez

### 4.7 typedef / enum / struct
- `typedef struct { ... } Ad;`, `typedef enum { A, B } Ad;`, `typedef tip takmaAd`
- Düz `enum Ad { ... };` ve `struct Ad { ... };`
- Alan adları `parse_struct_fields` ile çıkarılır ve `GCLTypeRegistry`'ye yazılır

### 4.8 class (executor_class.h)
- `class AD() { void head(...) {...} foo() {...} @return gcChar Call() {...} }`
- Kalıtım: `class CHILD(FATHER)` → base metotlar instance'a eklenir
- `head` kurucu olarak çalışır; `@return` metot = parantezsiz alan erişimi
- Instance: `FATHER ThisFather = FATHER;` / `CHILD c = CHILD(ThisFather)`
- `obj.Metot(args)`, `obj.Alan`, `PrintF("%s", ThisFather.Call)` → `__gcl_method:` sentineli

### 4.9 PrintF / print
- `%d %i %u %x %X %o %f %e %E %g %c %s %%`
- Flags `- + 0 space #`, genişlik, hassasiyet, uzunluk `%ld %lld %Lf`, escape `\n \t \r \\ \" \' \a \b \f \v`

### 4.10 Time.Sleep + Time.GetTicks (lokal, non-blocking) — son onarım
```csharp
void UpdateBody(gcChar BodyName, Node Body) {
    Time.Sleep(0.5);   // yalnızca bu fonksiyonun kalan satırları ertelenir
    PrintF("Body Name: %s \n", BodyName);
}
```
- `OS::delay_usec` kullanılmaz → motor donmaz
- `sleep_armed/sleep_until/sleep_rest/sleep_scope`: ilk tetiklemede kalan
  satırlar + scope kopyalanır; süre dolunca `gcl_script.cpp` `sleep_rest`'i
  saklanan scope ile **bir kez** çalıştırır
- Aynı Sleep satırı Update her frame çağrılsa bile süre kaydırılmaz
- `Update/UpdatePhysics` gövdesi bekleme boyunca her kare çalışmaya devam eder
- **`Time.GetTicksMsec()` / `Time.GetTicksUsec()`** eklendi: `ops_initialize_value`
  içinde statik tablo çözümü — okuma anındaki gerçek `OS` ticks'ini döndürür

### 4.11 Fonksiyon kapsamı (scope) — son onarım
- `resolve_user_call`: kullanıcı fonksiyon gövdesi **çağıranın scope'unda**
  çalışır → `delta`, `self`, globaller fonksiyon içinde görünür
- Parametreler **yereldir**: gövde bitince çağırandaki eski değerler geri yüklenir
- Recursion guard: `call_depth >= 32`

### 4.12 Döngü scope temizliği — yeni fix
- **C-tarzı `for(int i = 0; ...)`**: `i` yalnızca döngü tarafından *yeni*
  bildirildiyse gövde sonunda `members`'tan silinir; önceden var olan aynı
  isimdeki değişkene dokunulmaz
- **`for (x in dizi)` foreach**: iterasyon değişkeni gövde sonunda geri alınır
  (önceki değer varsa geri yüklenir, yoksa silinir) — döngü değişkeni artık
  scope'u kirletmez

### 4.13 Node hiyerarşisi + genel çağrılar — yeni fix
```csharp
NODE REF = GetNode("araba");          # global helper: self üzerinden relative node
REF.GetChild.Find("ali").Hide;        # zincir: REF.GetChild → Find("ali") → Hide()
Node T = GetNode("Kapı/Anahtar");     # path + property erişimi T.Rotation.y = ...
Ref S = load("res://sahne.tscn");     # global kaynak yükleme (preload da var)
```
- `resolve_object_member` başına **genel Node üyeleri** eklendi (FPSController/
  RayCast özel dallarından ÖNCE):
  `GetChild` (ilk çocuk), `Childs` (tüm çocuklar → Array), `Name`,
  `Hide`, `Show`, `Free` (queue_free), `Disable` / `Enable`
- `handle_chain_call` (executor_core.h): `Root.Seg1.SegN(args)` genel zincir
  çağrısı — en son çözülen OBJECT üzerinde `callv`; `self.` ön eki gerektirmez
  (RHS değer üretimi ve satır yürütmede kullanılır)
- Global `GetNode("path")` → owner (self) üzerinden `get_node_or_null`
- `load("res://..")` / `preload("res://..")` → `ResourceLoader::load`
- Mevcut fallback zincir (property → child node → method) korunur

---

## 5. self Sistemi ve Motor Entegrasyonu

`members` tablosu:
```
self          → sahip Godot node'u (Object)
delta         → son kare süresi (Update/UpdatePhysics'e geçilen argüman)
<@extern adı> → ilgili node (FPSController için Raycast/Camera/Head)
<değişkenler> → variable_scan sonucu
```

### FPSController özel üyeleri (executor_core.h)
```
Raycast, Camera, Head, IsColliding, GetBodyName, GetNode,
MoveSpeed, DuckSpeed, RunSpeed, JumpHeight, Gravity,
IsRunning, IsDucking, IsJumping, IsOnFloor
```
- `IsRunning` → `Input::is_physical_key_pressed(KEY_SHIFT)`
- `GetNode` → etkileşim hedefi (interaction target)
- `Raycast/Camera/Head` → `fps->get_interaction_ray()/get_camera()/get_head()`

### RayCast üyeleri
```
IsColliding, GetBodyName, GetNode (çarpılan cisim), length, Free, Skip, SkipList
```
- `GetNode` → **çarpılan cisim** (RayCast'in kendisi değil; FPSController dalıyla tutarlı)

### Genel Node üyeleri (her Node'da çalışır) — yeni fix
```
GetChild (ilk çocuk), Childs (tüm çocuklar), Name, Hide, Show,
Free (queue_free), Disable, Enable
```
- Bu üyeler `resolve_object_member`'da **önce** kontrol edilir; böylece
  `REF.GetChild.Find("ali").Hide` gibi zincirler her nesnede çalışır

### Genel Object çözümleme
- property (harf duyarsız iki deneme) → child node (harf duyarsız) → method
- Primitif eksenler: `Vector3.x/y/z`, `Vector2.x/y`, `Color.r/g/b/a`,
  `Quat.x/y/z/w`, `String.Name -> kendisi`

---

## 6. Editor Entegrasyonu

### Otomatik Tamamlama (autocomplete.cpp)
- `@extern <Sınıf>` → bağlı node sınıf adı önerisi (`get_class`)
- `self.` → kök üyeler (FPSController varsayılan tablo) + extern adları
- `self.Raycast. / Camera. / Head.` → alt üye tabloları
- `Input.` / `Engine.` / `Time.` → özel tablolar
- `<NODE değişkeni>.` → Node üyeleri (variable_scan ile eşleşme)
- Genel: değişkenler + parametreler (scoped, `function_params_scan`) + keyword + fonksiyonlar
- `r_force = true` → Ctrl+Space her durumda pencere açar
- İmleç işareti `0xFFFF` (`TextEdit`) analizden temizlenir; satır analizi
  dosyanın son satırına değil **imleç satırına** göre yapılır

### Sözdizimi Renklendirme (Highlight)
- 10 kategori x kullanıcı paleti: kontrol, tip, identity, operatör,
  direktif, yorum, sayı/değer, string, %-specifier, PrintF
- `TOOLS_ENABLED` altında `register_gcl_syntax_highlighter()` ile editor'e kayıtlı

---

## 7. Bekleyen / Eksik Noktalar (Kod ile Doğrulanmış)

| Alan | Durum |
|---|---|
| `#include "lib.gclib"` | ✅ **Fix'lendi**: loader'da yorum temizlemeden ÖNCE recursive metin birleştirme (16 derinlik), `"..."` / `<...>` path, res:// / user:// globalize |
| `#extern "dll"` + `#register` | ❌ Tasarım; çalışma zamanı DLL yükleme **yok** |
| Lexer→Parser→Ast→IR zinciri | ⚠️ Bildirim düzeyinde; çalışmıyor |
| `for` / `foreach` scope | ✅ **Fix'lendi**: döngü değişkeni gövde sonunda geri alınır |
| `switch` | Yalnızca satır-bazlı `case` gövdeleri; iç içe bloklar kısıtlı |
| Diziler | `@N` boyut sistemi `split_declaration`'da temizlenir ancak çok boyutlu
  dizi nesnesi üretilmez; `[]` indeks yalnızca `Array` üzerinde çalışır |
| pointer | `char *` imlası kabul edilir; gerçek C pointer aritmetiği yok |
| `scanf` | Autocomplete listesinde ama `handle_call_ex` yalnızca `PrintF`/`print` bilir |
| Yorum `#| |#` | Executor satır döngüsü `in_block_comment` taşır; multi-line `#define` yok |
| `validate()` | ✅ **Fix'lendi**: yapısal denge denetimi ({ } ( ) [ ] + string/block yorum
  kapanışı) satır/sütun + açıklama döndürür; fonksiyon listesi doldurulur |
| `execute_file` | ✅ **Fix'lendi**: dosyayı okur, global scope + `Ready` gövdesini
  yorumlar (CLI çalıştırma) |
| `reload` | ⚠️ Kısa devre (`OK`); script değişiminde yeniden yükleme kısıtlı |
| Debugger | `debug_*` fonksiyonları stub |
| `Time.` | ✅ **Fix'lendi**: `Sleep` + `GetTicksMsec()` + `GetTicksUsec()` |
| `NODE` hiyerarşisi | ✅ **Fix'lendi**: `GetChild/Childs/Name/Hide/Show/Disable/Enable/Free`,
  fallback property→child→method, global `GetNode("path")`, `load/preload`,
  zincir çağrılar `REF.GetChild.Find("ali").Hide` |

---

## 8. Öneriler (Öncelikli)

1. ~~Gerçek include/lib~~ — **`#include` yapıldı** (loader, yorum temizlemeden önce).
   Kalan: `#lib` / `#extern "dll"` çalışma zamanı DLL yüklemeyi değerlendir.
2. **Lexer→AST→IR'i ya gerçek bir pipeline yap ya kaldır** — iki yol birden
   bakımı zorlaştırır.
3. **Dizi sistemini** (`@N`) üretilen `Array`'lerle eşle: çok boyutlu indeks.
4. ~~`for` döngü değişkenini gövde sonunda temizle~~ — **yapıldı** (C-for + foreach).
5. **`switch`** içinde çok satırlı blok `{ }` destekle.
6. ~~`Time.GetTicks()`, `frame`, `rand` gibi yardımcılar ekle~~ —
   **`GetTicksMsec()/GetTicksUsec()` yapıldı**; `rand/frame` hâlâ aday.
7. ~~`validate()` gerçek sözdizimi denetimi~~ — **yapıldı** (yapısal denge);
   isteğe bağlı genişletme: tip/tanımlayıcı denetimi.
8. ~~`execute_file` CLI'dan `.gcsf` çalıştırma~~ — **yapıldı** (global + Ready).
9. **`NODE` zincirleri genişlet**: `GetParent`, `Find` (isimle yinelemeli),
   `QueueFree`, `SetPosition`, listener sinyalleri.
10. **`scanf`** desteği (PrintF simetriği) veya autocomplete'ten çıkar.

---

## 9. Sonuç Tablosu

| Başlık | Durum |
|---|---|
| Yorumlayıcı (satır bazlı execute) | ✅ Çalışıyor |
| PrintF / format | ✅ Çalışıyor |
| Kontrol akışı (if/while/for/do/switch) | ✅ Çalışıyor |
| typedef / enum / struct | ✅ Çalışıyor (registry) |
| class + kalıtım + head + @return | ✅ Çalışıyor |
| tuple / dict | ✅ Çalışıyor |
| Preprocessor (define/koşul/mesaj) | ✅ Çalışıyor |
| `#include` metin birleştirme | ✅ Çalışıyor (loader, recursive) |
| Time.Sleep lokal non-blocking | ✅ Çalışıyor (fix'lendi) |
| Time.GetTicksMsec / GetTicksUsec | ✅ Çalışıyor (yeni) |
| Fonksiyon scope (delta/self/global görünür) | ✅ Çalışıyor (fix'lendi) |
| for / foreach döngü scope temizliği | ✅ Çalışıyor (fix'lendi) |
| `<=` / `>=` | ✅ Çalışıyor (fix'lendi) |
| switch içinde üye ataması | ✅ Çalışıyor (fix'lendi) |
| Genel Node üyeleri (GetChild/Childs/Name/Hide/Show/Disable/Enable/Free) | ✅ Çalışıyor (yeni) |
| Global GetNode() / load() / preload() + zincir çağrılar | ✅ Çalışıyor (yeni) |
| self / extern node erişimi | ✅ FPSController/RayCast/Camera/Head + genel Object |
| validate() yapısal denetim | ✅ Çalışıyor (fix'lendi) |
| execute_file CLI çalıştırma | ✅ Çalışıyor (fix'lendi) |
| Autocomplete + Highlight | ✅ Editor'de aktif |
| `#extern "dll"` / `#register` | ❌ Tasarım; yürütme yok |
| Lexer→AST→IR tam pipeline | ⚠️ Bildirim düzeyinde; çalışmıyor |
| Debugger / reload / CLI tam | ⚠️ Stub / kısıtlı |
| @N çok boyutlu dizi / pointer aritmetiği | ⚠️ İmza düzeyinde; nesne yok |
