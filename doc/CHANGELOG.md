# GCL Changelog

## 2026-08-14 — Shift operator & char array fixes

### `<<` / `>>` shift operatörleri düzeltildi
`find_best_op` karşılaştırma taramasında shift operatörünün ikinci karakterini de
atlamıyordu; `1 << 4` ifadesinde ikinci `<` karşılaştırma sanılıp `1 < (4)` → `1`
dönüyordu. Artık `1 << 4 = 16` doğru hesaplanıyor. Aynı düzeltme `>>` için de geçerli.

### `char x[] = {'a', 'b', 'c'}` dizi literal desteği
`char`/`gcChar` dizileri `{...}` literal'den String'e birleştiriliyor:
`char alphabet[] = {'a', 'b', 'c'}` artık `%s` ile `abc` basıyor
(önceden `'a', 'b', 'c'` kalıntısı yazdırılıyordu).

---

## 2026-08-14 — Runtime scope & operator fixes

### Time.Sleep artık lokal (non-blocking)
`Time.Sleep(0.5)` yalnızca içinde bulunduğu fonksiyonun kalan satırlarını erteler.
`Update` / `UpdatePhysics` gövdesi bekleme sırasında da her kare çalışmaya devam eder;
motor `OS::delay_usec` ile asla kilitlenmez. Aynı `Sleep` satırının her kare yeniden
tetiklenmesi süreyi kaydırmaz.

**Önce:** tüm script runtime'ı duruyordu (fonksiyon içindeki sleep global gibiydi).
**Sonra:** yalnızca ilgili fonksiyonun kalan satırları ertelenir.

### Fonksiyon kapsamı (scope) onarımı
Kullanıcı fonksiyonu çağıranın kapsamında çalışıyor:
- `delta`, `self`, global değişkenler fonksiyon içinde görünür.
- Fonksiyonun yazdığı global değişiklikler geriye yansır.
- Parametreler yerel kalır.

### Karşılaştırma onarımı
`<=` ve `>=` operatörleri düzeltildi. Önceden `<` / `>` gibi işlenip RHS'de
`= deger` kalıyordu; `while (i <= n)` gibi koşullar yanlış sonuç veriyordu.

### switch/case içinde obje üye ataması
`Body.Rotation.y = 5` gibi member-chain atamaları `switch`/`case` gövdelerinde
sessizce yutulmuyor, artık çalışıyor.

---

## Dokümantasyon

Root'taki dağınık dosyalar `doc/` altında toplandı:

- `doc/gcl.html` — derlenmiş renkli döküman (Time.Sleep, scope, son değişiklikler eklendi)
- `doc/gcl.md` — motor odaklı referans (hazır fonksiyonlar, node türleri, Time.Sleep, scope)
- `doc/gcl_language.md` — dil tasarımı / sözdizimi
- `doc/CHANGELOG.md` — bu dosya

Silinen dosyalar (yol kirliliği):
- `gcl.md` (root) → `doc/gcl.md`
- `gcl_language.md` (root) → `doc/gcl_language.md`
- `fps_node.md` → FPSController dahili tasarım notu; GCL dokümanı değil
- `build_log*.txt` → geçici derleme kayıtları
