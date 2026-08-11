# GnuChan Engine — Görev Listesi

## Amaç
Native (C++) hariç tüm programlama dillerini motordan kaldırmak.
Motor artık hiçbir script dili barındırmıyor; yeni dil doğrudan C++ `ScriptLanguage` arayüzüyle (örn. `src/modules/`) eklenecek.

## Durum: TAMAMLANDI ✅

- [x] Kaldırılacak dillerin envanteri: GDScript, GDNative, VisualScript, GCL
- [x] GDScript devre dışı (`src/modules/gdscript/config.py` → `is_enabled() = False`)
- [x] GDNative devre dışı (`src/modules/gdnative/config.py` → `is_enabled() = False`)
- [x] VisualScript devre dışı (`src/modules/visual_script/config.py` → `is_enabled() = False`)
- [x] GCL devre dışı (`src/modules/gcl/config.py` → `is_enabled() = False`)
- [x] `main.cpp`: GDScript LSP blokları `MODULE_GDSCRIPT_ENABLED` guard'ına alındı
- [x] Build başarılı: `[BUILD] SUCCESS -> D:\GnuChan_Engine\src\bin\GnuChanIDE.exe`
- [x] Doğrulama: `modules_enabled.gen.h` içinde GDSCRIPT/GDNATIVE/VISUAL_SCRIPT/GCL yok

## Not
Kendi dili eklemek için GDNative GEREKMEZ — `ScriptLanguage` C++ arayüzü motorun içinde yaşar.
