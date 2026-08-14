# GCL — Motor Odaklı Referans (Engine Reference)

Bu dosya GCL dilinin **motor tarafını** açıklar: hazır fonksiyonlar, girdi/motor erişimi,
`@extern` node türleri, `Time.Sleep` ve fonksiyon kapsam kuralları.

Dil sözdizimi (tipler, struct/enum/class, kontrol akışı, PrintF) için:
- `doc/gcl_language.md` — dil tasarımı
- `doc/gcl.html` — derlenmiş, renkli dokümantasyon

## İçindekiler

1. Hazır Fonksiyonlar (Ready / Update / UpdatePhysics)
2. Girdi & Motor (Input / Engine)
3. Node Erişimi (NODE tipi, $ full path)
4. `@extern` Node Türleri
5. `@extern Mesh` (Material)
6. `@extern FPSController` + Raycast
7. **Time.Sleep — lokal non-blocking uyku**
8. **Fonksiyon Kapsamı (scope) kuralları**
9. Operatör notları
10. Derleme

---

## 1. Hazır Fonksiyonlar

```csharp
#// Hazir fonksiyonlar
void Ready() {

}

void Update(float delta) {

}

void UpdatePhysics(float delta) {

}
```

- `Ready()` — sahne başladığında **1 kez** çalışır; global atamalar burada yapılır.
- `Update(delta)` — her kare.
- `UpdatePhysics(delta)` — her fizik karesi.

## 2. Girdi & Motor

```csharp
# Key Event
    Input.IsPressed("e")
    Input.IsUp("e")
    Input.IsDown("e")

#// temel komutlar
Engine.GetFPS
Engine.SetFPS
Engine.GetScreenSize
Engine.GetWindowSize
```

## 3. Node Erişimi

### Genel node sistemleri

```csharp
# call full path
$"FPSController/head/Camera/RayCast"   #// full path node
$"".members
```

```csharp
#// NODE tipi butun nodelari tutabilir
NODE ABC = GetNode("araba");

ABC.GetChild.Name -> child body Name
ABC.GetChild.Find("ali").Free -> getirilen child ozgur birakma
ABC.GetChild.Find("ali").Hide
ABC.GetChild.Find("ali").Show
ABC.GetChild.Find("ali").Disable --> child collision devre disi kalir
ABC.GetChild.Find("ali").Enable
```

### TRASH (tercih sınırlaması)

GCL'de `get_parent().get_parent().get_parent()` zinciri yoktur.
Node erişimi `self.` / `$"..."` / `GetNode` ile yapılır.

---

## 4. `@extern` Node Türleri

```csharp
@extern Scene          -> sahne kok node (Spatial): tum node'larin atasi, sade referans
@extern Body           -> KinematicBody: hareketli karakterler / golgeler (FPSController gibi)
@extern RigidBody      -> fizik motorlu cisim: itilir, duser, carpar (kutu, top)
@extern StaticBody     -> sabit cisim: zemin, duvar, merdiven (hareket etmez)
@extern Area           -> trigger bolge: icine girince / dokununca olay baslatir
@extern Light          -> isik: Directional = gunes, Omni = ampul, Spot = el feneri
@extern Camera         -> camera3D: ana kamera, FPS kamerasi
@extern Particles      -> parcalar: duman, alev, yagmur, patlama
@extern Audio          -> ses kaynagi: 3D pozisyonlu ses (mesafe azaldikca yukselir)
@extern Sprite3D       -> her zaman kameraya donuk resim: HP bar, isaret, buyulu etki
@extern Label3D        -> 3D yazi: isim, tabela, mesafe
@extern Animation      -> animasyon oynatici: yurume/ziplama animasyonlari
@extern Navigation     -> yol bulma: bocek AI duvardan gecmeden koseyi bulur
@extern GridMap        -> grid tabanli harita: kutu kutu harita, labirent
@extern CSG            -> basit sekillerden bina: bosluk acma, kapi, pencere
@extern Joint          -> mentese/bag: kopru, zincir, kapak
@extern Collision      -> coarpisma sekli: mesh/govde uzerine takilir
@extern WorldEnv       -> ortam: sis, arkaplan rengi, gama
@extern MultiMesh      -> ayni mesh'i binlerce kez: agac, cimen, yildiz, tas
@extern RemoteTransform -> baska node'u takip et: silah kameraya, kamera karaktere
```

---

## 5. `@extern Mesh` (3D Mesh Instance + Material)

```csharp
@extern Mesh #// 3d mesh instance
    self.Transform         --> Local yada Global secenek
    self.Position.         --> .y, .x, .z
    self.Rotation.         --> .y, .x, .z
    self.Scale.            --> .y, .x, .z
    self.Name              --> call
    self.Material.Color            --> rgb
    self.Material.Albedo           --> preload("path/image"); or load("path/image");
    self.Material.Metallic         --> 0..1
    self.Material.Metallic.Specular --> 0..1 (metalik bilesen; 1.0 = pure metal)
    self.Material.Metallic.Texture --> doku
    self.Material.Roughness        --> 0..1
    self.Material.Roughness.Texture --> doku
    self.Material.Emission         --> 0..1 (parlama guclu)
    self.Material.EmissionColor    --> rgb (isik rengi)
    self.Material.EmissionTexture  --> doku
    self.Material.NormalMap.Enable --> true/false
    self.Material.NormalMap.Scale  --> 0..1 (normal girinti guclu)
    self.Material.NormalMap.Texture--> doku
    self.Material.AO              --> 0..1 (ambient occlusion, kose karaltisi)
    self.Material.AO.Texture      --> doku
    self.Material.Opacity         --> 0..1 (saydamlik; 1.0 = opak)
    self.Material.Transparent     --> true/false (alfa karisimini acar)
    self.Material.CastShadows     --> true/false (golge dusursun mu)
    self.Material.DoubleSided     --> true/false (iki yuzluluk)
    self.Material.CullBackface    --> true/false (arka yuzu at)
    self.Material.BlendMode       --> mix/add/sub/mul (karisim modu)
    self.Material.ShadingMode     --> unshaded/per-pixel (dokulu gorsel)
    self.Material.UV1Offset       --> x,y (doku kaydirma)
    self.Material.UV1Scale        --> x,y (doku olcegi / tekrari)
    self.Material.Detail.Albedo   --> detay katmani dokusu (uzaktan doku katmani)
    self.Material.Detail.Roughness--> detay roughness dokusu
    self.Material.Detail.Normal   --> detay normal dokusu
    self.Material.Clear()         --> varsayilan ayarlara don
    self.Material.Copy(REF.Material) --> baska materialdan kopyala
    self.Material.Save("res://path/material.tres") --> materiali dosyaya kaydet
    self.Material.Load("res://path/material.tres") --> materiali dosyadan yukle
```

---

## 6. `@extern FPSController`

```csharp
@extern FPSController
    #// surukle birak yapicagimiz referanslar FPSController buradaki referans nodelari
    #// kontrol ediyor duzenliyor ve kullaniyor
    self.Raycast. -> raycast referansi
    self.Camera.  -> camera3D referansi
    self.Head.    -> 3d empty NODE refransi altinda camera var

    #// kontrol
    wasd    -> move
    c, ctrl -> duck
    shift   -> run
    mouse scrools -> walk speed +, - but less then run speed
    right mouse click -> zoom
    esc -> pause true -> karakter hareket edemez kamera hareket edemez fare horunur olur
          tekrar basinca fare gizlenir ve karakter hareket edebilir
    space -> ziplama

    self.MoveSpeed         -> 5
    self.DuckSpeed         -> 3
    self.RunSpeed          -> 8
    self.JumpHeight        -> 3
    self.StandingHeight    -> 1.8
    self.CrouchHeight      -> 0.5
    self.Gravity           -> 9.8 (yercekimi / dusme hizi)
    self.FallSpeed         -> 0..1 (dikey hiz; ziplama/dusme)

    self.IsRunning         -> bool
    self.IsDucking         -> bool
    self.IsJumping         -> bool
    self.IsPaused          -> bool
    self.IsOnFloor         -> true/false (yere degiyor mu; ziplama kilidi)

    self.Camera.Fov        -> 90
    self.Camera.MouseSpeed -> float
    self.Camera.HeadBob    -> true, false
    self.Camera.HeadBobSpeed -> float
    self.Camera.HeadBobAmount -> float
    self.Camera.RunFovChange -> int

    #// self.Raycast sistemi
    self.Raycast.member ->> @extern Raycast

    #// GetNode ile raycasting gordunu nodun alt nodelarina erismek yada direk REF.Name gibi kullanabilmek
    NODE REF = self.Raycast.GetNode;

    #// Child Nodes
    for (i in REF.Childs) {
        printf("Child Name: %s \n", i.Name);

        #// NODE Refferance
        i.member ->> @extern Mesh parametreleri
        i.member ->> @extern Material parametreleri
    }

    #// tek tek Child Nodes
    REF.GetChild.Find("cube1"). buradan sonrasi node ne turunde ise ona gore member cagirma olucak
```

### Raycast (çarpışma) üyeleri

```csharp
@extern Raycast
    self.Skip(self)              #// FPSController gibi body'leri atla
    self.SkipList({"a","b"})     #// isim listesiyle atla
    self.length
    self.IsColliding
    self.GetBodyName             #// carpan cismin adi
    self.GetNode
    self.Free
```

---

## 7. Time.Sleep — Lokal Non-Blocking Uyku

```csharp
void UpdateBody(gcChar BodyName, Node Body) {
    Time.Sleep(0.5);              #// sadece bu fonksiyonun kalan satirlari bekler
    PrintF("Body Name: %s \n", BodyName);
    PrintF("Body Rotation.y %f \n", Body.Rotation.y);
}
```

Kurallar:

- `Time.Sleep(0.5)` **lokal**dir: yalnizca içinde bulunduğu fonksiyonun **kalan satırlarını**
  bekletir; `Update` / `UpdatePhysics` gövdesi beklerken **her frame çalışmaya devam eder**
  (raycast, dönme, hareket, spawn durmaz).
- Non-blocking: `OS::delay_usec` kullanılmaz → motor hiçbir zaman donmaz.
- Aynı `Sleep` satırı her frame yeniden tetiklense bile süre **kaydırılmaz / uzamaz**;
  ertelenen satırlar süre dolunca **bir kez** çalıştırılır.
- Ertelenen fonksiyonun parametre/yerel değerleri süre boyunca saklanır.

Örnek — Update donmadan çalışır:

```csharp
int frame = 0;
void Update(float delta) {
    frame++;
    PrintF("Tick %d \n", frame);       #// her frame basilmaya devam eder
    UpdateBody();                       #// icindeki Sleep(0.5) sadece UpdateBody'nin kalanlarini erteler
}
```

---

## 8. Fonksiyon Kapsamı (Scope)

Fonksiyon gövdesi **çağıranın kapsamında** çalışır (C semantiği):

- Global değişkenler, `self` ve `delta` fonksiyon içinde **görünür**.
- Fonksiyonun yazdığı global değişiklikler geriye **yansır**.
- Parametreler **yereldir**: gövde bittiğinde çağırandaki aynı isimli değişkenler eski
  değerlerine geri döner.

```csharp
void UpdateBody(gcChar BodyName, Node Body) {
    Body.Rotation.y += 5 * delta;    #// delta gorunur (cagiran Update'in parametresi)
}

void Update(float delta) {
    Node Body = self.Raycast.GetNode;
    UpdateBody("cube", Body);        #// func icinde delta/self/global erisimi acik
}
```

---

## 9. Operatör Notları

- `<=` ve `>=` karşılaştırmaları düzeltildi (önceden `<` / `>` gibi işlenip RHS'de
  `= deger` kalıyor, `while (i <= n)` gibi koşullar yanlış sonuç veriyordu).
- `Obje.Property.axis = deger` atamaları **switch/case** gövdeleri içinde de çalışır.
- `++ -- += -= *= /= %=` ve obje üye bileşik atamaları (`Body.Rotation.y += x`) desteklenir.

---

## 10. Derleme

```
py.exe makefile.py --debug      # debug editor build
py.exe makefile.py --release    # release_debug editor build
py.exe makefile.py --clean      # build artifacts temizle
```

Çıktı: `src/bin/GnuChanIDE.exe`
