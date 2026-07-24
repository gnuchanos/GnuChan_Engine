# GPL İncelemesi ve Alternatifler

Proje: GnuChan Engine (fork of Godot Engine, MIT licensed)
Tarih: 2025

---

## Durum Özeti

**Projenin kendi kodu tamamen MIT lisanslıdır.** Hiçbir engine kaynak dosyası GPL altında değildir. Aşağıda listelenenler ya üçüncü parti kütüphaneler ya da kök dizindeki lisans metni dosyasıdır.

---

## 1. `COPYING.txt` (KÖK DİZİN) — GPLv2 Lisans Metni

| Dosya | Lisans |
|---|---|
| `COPYING.txt` | GPLv2 |

**Durum:** Bu dosya Godot'un eski sürümlerinden kalma bir lisans metnidir. Godot (ve GnuChan) MIT lisanslı olduğu için bu dosyanın kök dizinde bulunması **yanıltıcıdır**.

**Alternatif:** Bu dosya ya tamamen silinmeli ya da içeriği `src/LICENSE.txt`'daki MIT lisans metni ile değiştirilmelidir. Eğer Godot'un orijinal GPLv2 metnini referans olarak saklamak isteniyorsa, `docs/` veya `misc/` altına taşınabilir.

---

## 2. `src/thirdparty/freetype/` — FreeType Kütüphanesi

| Dosya | Lisans(lar) |
|---|---|
| `src/thirdparty/freetype/` | FTL (FreeType License) veya **GPLv2** (seçimlik) |

**Durum:** FreeType iki lisans altında sunulur: FTL (BSD-benzeri, MIT-uyumlu) veya GPLv2. GnuChan Engine FTL lisansını kullanmaktadır (`src/thirdparty/freetype/LICENSE.TXT`'de her iki seçenek de belirtilmiştir, ancak FTL geçerlidir). GPLv2 seçeneği yalnızca **seçimliktir** ve kullanılmamaktadır.

**Alternatif:** Eğer FreeType GPL endişesi yaratıyorsa, alternatif font renderer'lar:
- **stb_truetype.h** — Public domain / MIT, tek dosya, hafif. Godot zaten stb kullanıyor.
- **libschrift** — MIT lisanslı, saf C TrueType renderer
- **Allsorts** — Apache 2.0, Rust ile yazılmış
- **DirectWrite** (Windows) / **CoreText** (macOS) / **FreeType** FTL ile sorunsuz

Mevcut FTL kullanımı MIT projesi için **sorun teşkil etmez**, FTL MIT ile uyumludur.

---

## 3. `src/thirdparty/certs/` — SSL Sertifikaları

| Dosya | Lisans |
|---|---|
| `ca-certificates.crt` | MPL 2.0 |

**Durum:** Mozilla CA sertifika paketi MPL 2.0 lisanslıdır. MPL 2.0 **GPL-uyumlu** ve MIT ile de uyumludur (tek dosya, ayrı çalışma olarak kabul edilir).

**Alternatif:** 
- **certifi** paketi (MPL 2.0) — aynı
- Sistem sertifikalarını kullanmak (`--system-certs-path`)
- **Let's Encrypt ISRG Root X1** (public domain / özel lisans)
- **curl.se/ca** (aynı Mozilla paketi)

Gerekli değildir, mevcut kullanım MIT ile uyumludur.

---

## 4. `src/thirdparty/oidn/` — Intel Open Image Denoise

| Dosya | Lisans |
|---|---|
| `src/thirdparty/oidn/` | Apache 2.0 |

**Durum:** Apache 2.0, GPLv3 ile uyumlu, MIT ile de uyumlu. GPLv2 ile dar alanda uyumsuzdur ancak bu proje MIT lisanslı olduğu için **sorun yoktur**.

**Alternatif:** Gerekmez. Ancak istenirse:
- **BM3D** algoritması (C++ kaynak kodu mevcut, çeşitli lisanslar)
- **NVidia AI Denoiser** (özel lisans)
- Kendi basit denoiser'ını yazmak

---

## 5. Proje İçinde GPL OlMAYAN (ama yanlışlıkla GPL sanılabilecek) YERLER

Aşağıdaki dosyalar `findstr` ile GPL metninde eşleşti, ancak **MIT lisans metninin standart garanti reddinin** bir parçası olarak "General Public License" ifadesini içerirler. Bunlar GPL değildir:

```
src/core/*.cpp
src/core/*.h
src/core/bind/core_bind.cpp
src/core/crypto/aes_context.cpp
src/core/io/logger.cpp
src/core/math/bvh.h
src/core/math/expression.cpp
src/editor/*.cpp
src/main/main.cpp
src/modules/mono/mono_gd/gd_mono_class.cpp
... (ve diğer tüm MIT lisanslı engine dosyaları)
```

Bu dosyaların hepsi **MIT/Expat** lisansı altındadır. GPL değildir.

---

## SONUÇ

| Öğe | GPL mi? | Risk | Alternatif Gerekli mi? |
|---|---|---|---|
| Root `COPYING.txt` | **Evet** (GPLv2 metni) | Düşük — yalnızca lisans metni, kod değil | Silinmeli veya taşınmalı |
| `thirdparty/freetype/` | FTL kullanılıyor, GPL seçimlik | Yok | Gerekmez |
| `thirdparty/certs/` | MPL 2.0 | Yok | Gerekmez |
| `thirdparty/oidn/` | Apache 2.0 | Yok | Gerekmez |
| Tüm engine `.cpp/.h/.py` | **MIT** | Yok | — |

**Önerilen aksiyon:** Root `COPYING.txt` silinmeli. İçindeki GPLv2 metninin bu projeyle ilgisi yoktur.
