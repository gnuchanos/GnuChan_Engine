# GLES3 -> GLES4 Dönüşüm Analizi

## Amaç
GLES3 sürücüsünü kaldırıp, tüm referansları GLES4 ile değiştirmek.

## Değişecek Dosyalar

### 1. main/main.cpp
- `GLOBAL_DEF_RST("rendering/quality/driver/driver_name", "GLES3")` -> `"opengl4"`
- `"GLES2,GLES3,opengl4"` -> `"opengl4"`
- `GLOBAL_DEF("rendering/quality/driver/fallback_to_gles2", false)` -> kaldır
- `rendering/gles3/shaders/` referansları -> `rendering/gles4/shaders/` olarak değiştir
- `--debug-shader-fallbacks` açıklaması: "(GL ES 3 only)" -> "(GL ES 4 only)"

### 2. SConstruct
- `GLES3_GLSL` builder -> `GLES4_GLSL` ile birleştir (zaten ayrı)
- `"GLES3_GLSL"` -> zaten duruyor, `GLES4_GLSL` de var, GLES3 builder'ı kaldır

### 3. gles_builders.py
- `build_gles3_headers()` -> kaldır
- GLES3 builder referansını SConstruct'dan kaldır

### 4. scu_builders.py
- `process_folder(["drivers/gles3"])` -> kaldır

### 5. drivers/SCsub
- `SConscript("gles3/SCsub")` -> kaldır

### 6. core/os/os.h
- `VIDEO_DRIVER_GLES3` -> kaldır, `VIDEO_DRIVER_GLES4` -> `0` yap
- `is_low_end()` fonksiyonunu güncelle/kaldır (hep high-end)

### 7. core/os/os.cpp
- `get_video_driver_count()` -> 1 döndür (sadece GLES4)
- `get_video_driver_name()` -> sadece "opengl4" döndür
- `VIDEO_DRIVER_GLES3` referanslarını kaldır

### 8. platform/windows/os_windows.cpp
- `#include "drivers/gles3/rasterizer_gles3.h"` -> kaldır
- `RasterizerGLES3` çağrıları -> kaldır
- GLES3 context logic -> sadece GLES4 kalacak şekilde sadeleştir

### 9. platform/x11/os_x11.cpp
- Aynı windows değişiklikleri

### 10. platform/x11/context_gl_x11.cpp
- `GLES_3_0_COMPATIBLE` -> kaldır

### 11. platform/windows/platform_config.h
- `#define GLES3_INCLUDE_H` -> kalabilir veya kaldır

### 12. platform/x11/platform_config.h
- `#define GLES3_INCLUDE_H` -> kalabilir veya kaldır

### 13. editor/editor_export.cpp
- `test_etc2()` -> "GLES3" string'lerini güncelle
- `test_etc2_or_pvrtc()` -> güncelle

### 14. editor/project_manager.cpp
- GLES3 radio button -> kalabilir, açıklama güncellenebilir
- `RasterizerGLES3::is_viable()` çağrısı -> kaldır/güncelle

### 15. scene/main/viewport.cpp
- `is_low_end()` kontrolleri -> hep false (hepsi kalabilir)

### 16. servers/visual_server.cpp
- `rendering/gles3/shaders/` -> `rendering/gles4/shaders/` 
- `rendering/gles2/compatibility/` -> kaldır

### 17. servers/visual/shader_language.cpp
- `is_low_end()` -> false döndürmeli (artık hep high-end)

### 18. İç referanslar (GLES3 driver içinde GLES2'ye referans veren satırlar)
- gles3/rasterizer_canvas_gles3.cpp, rasterizer_gles3.cpp, rasterizer_storage_gles3.cpp

## Adımlar
1. Önce GLES2'yi tamamen kaldır
2. Sonra GLES3 dosyalarını kaldır
3. GLES3 referanslarını GLES4 ile değiştir
4. Varsayılan driver'ı GLES4 yap
5. Build sistemini güncelle
