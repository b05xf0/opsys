# Első beadandó feladat 2022 tavasz

Itt a tavasz, a nyuszik is készülnek a "Húsvéti Locsolókirály" választásra. A nyuszi fiúk között versenyt szervez "Főnyuszi", az lesz a "Locsolókirály", aki a húsvéti locsolás során a legtöbb piros tojást gyűjti össze a nyuszi lányoktól. A versenyre jelentkezni kell. Főnyuszi a birodalmát az alábbi területekre osztotta: Barátfa, Lovas, Szula, Kígyós-patak, Malom telek, Páskom és Káposztás kert. A jelentkezéskor meg kell adni nyuszinak a nevét (pl:Tapsi Hapsi), a lakóterület elnevezését (pl: Káposztás kert, csak a megadott területnevek használhatók) és azt, hogy hányadik alkalommal jelentkezik a versenyre.

A jelentkezők adatait fájlban tároljuk, az adatfelvételen túl legyen lehetőségünk az adatokon módosítani, törölni és területi alapon vagy teljes listát készíteni a jelentkezőkről.

Készítsen C nyelvű programot ami  ezt a feladatot megoldja, a megoldásnak vagy az opsys.inf.elte.hu kiszolgálón, vagy egy hozzá hasonló Linux rendszeren kell futnia. A megoldást a beadási határidőt követő héten be kell mutatni a gyakorlatvezetőnek.

A forrásállományokat itt töltsék fel ZIP formában.

Beadási határidő: **2022 április 4.**

## Fordítás, futtatás

```txt
gcc reg.c db.c
./a.out
```
vagy

```txt
./reg.sh
```

## Menü/funkciók (kezdőképernyő)

```txt
Husveti Locsolokiraly valasztas
-------------------------------

Valassz az alabbi lehetosegek kozul:

(1) Uj regisztracio
(2) Regisztracio modositasa
(3) Regisztracio torlese
(4) Osszes regisztracio listazasa
(5) Regisztraciok listazasa lakoteruletenkent

vagy (k)ilepes a programbol?
```

## Forrásfájlok

| Fájlnév       | Leírás                                        |
| :---          | :---                                          |
| `reg.c`       | főprogram, felhasználói interakciók kezelése  |
| `db.h`        | adatbázis műveletek deklarációja              |
| `db.c`        | adatbázis (fájl) műveletek megvalósítása      |
| `bunnies.dat` | minta adatbázis fájl                          |
| `reg.sh`      | script fordításhoz és futtatáshoz             |