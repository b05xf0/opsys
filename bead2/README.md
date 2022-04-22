# Első beadandó feladat 2022 tavasz

Itt a tavasz, a nyuszik is készülnek a "Húsvéti Locsolókirály" választásra. A nyuszi fiúk között versenyt szervez "Főnyuszi", az lesz a "Locsolókirály", aki a húsvéti locsolás során a legtöbb piros tojást gyűjti össze a nyuszi lányoktól. A versenyre jelentkezni kell.

Főnyuszi a birodalmát az alábbi területekre osztotta: Barátfa, Lovas, Szula, Kígyós-patak, Malom telek, Páskom és Káposztás kert. A jelentkezéskor meg kell adni nyuszinak a nevét (pl:Tapsi Hapsi), a lakóterület elnevezését (pl: Káposztás kert, csak a megadott területnevek használhatók) és azt, hogy hányadik alkalommal jelentkezik a versenyre.

A jelentkezők adatait fájlban tároljuk, az adatfelvételen túl legyen lehetőségünk az adatokon módosítani, törölni és területi alapon vagy teljes listát készíteni a jelentkezőkről.

Készítsen C nyelvű programot ami  ezt a feladatot megoldja, a megoldásnak vagy az opsys.inf.elte.hu kiszolgálón, vagy egy hozzá hasonló Linux rendszeren kell futnia. A megoldást a beadási határidőt követő héten be kell mutatni a gyakorlatvezetőnek.

A forrásállományokat itt töltsék fel ZIP formában.

Beadási határidő: **2022 április 4.**

# Második beadandó feladat 2022 tavasz

Itt a tavasz, a nyuszik is készülnek a "Húsvéti Locsolókirály" választásra. A nyuszi fiúk között versenyt szervez "Főnyuszi", az lesz a "Locsolókirály", aki a húsvéti locsolás során a legtöbb piros tojást gyűjti össze a nyuszi lányoktól. A versenyre jelentkezni kell. 

Eljött a locsoló verseny ideje. "Főnyuszi" (szülő) két részre osztja a területeket. Barátfa, Lovas, Kígyós-patak és Káposztás kert területekre küld egy versenyfelügyelőt (gyerek) illetve a maradék Szula, Malom telek, Páskom területekre egy másik versenyfelügyelőt(gyerek). "Főnyuszi" csövön átküldi a versenyfelügyelőknek a területileg illetékes nyuszi versenyzők adatait, akik várják a gyűjtött piros tojások számát az ott élő nyusziktól (véletlen szám 1-100 között).  Miután megvannak az adatok, a versenyfelügyelők visszaküldik "Főnyuszinak" az adatokat szintén csővezetéken, aki kihirdeti(képernyőre írja) a Húsvéti Locsolókirály verseny eredményét. Az lesz a győztes, aki a legtöbb piros tojást gyűjti. 

Készítsen C nyelvű programot(egészítse ki az első beadandó feladatot) ami ezt a feladatot megoldja, a megoldásnak vagy az opsys.inf.elte.hu kiszolgálón, vagy egy hozzá hasonló Linux rendszeren kell futnia. Legyen olyan menü pont, ami választásával a verseny indítható! A megoldást a beadási határidőt követő héten be kell mutatni a gyakorlatvezetőnek.

A forrásállományokat itt töltsék fel ZIP formában.

Beadási határidő: **2022 május 9.** 


## Fordítás, futtatás

```txt
gcc bunnies.c db.c contest.c
./a.out
```
vagy

```txt
./bunnies.sh
```

## Menü/funkciók (kezdőképernyő)

```txt
Husveti Locsolokiraly valasztas
-------------------------------

Valassz az alabbi lehetosegek kozul:

(1) Uj regisztracio
(2) Regisztracio modositasa
(3) Regisztracio torlese
(4) Osszes jelentkezo listaja
(5) Jelentkezok listaja lakoteruletenkent
(6) Verseny indítása

vagy (k)ilepes a programbol?
```

## Forrásfájlok

| Fájlnév       | Leírás                                                     |
| :---          | :---                                                       |
| `bunnies.c`   | főprogram, felhasználói interakciók kezelése               |
| `db.h`        | adatbázis műveletek deklarációja                           |
| `db.c`        | adatbázis (fájl) műveletek megvalósítása                   |
| `contest.h`   | verseny lebonyolításához szükséges műveletek interface-e   |
| `contest.c`   | verseny lebonyolításához szükséges műveletek megvalósítása |
| `bunnies.dat` | adatbázis fájl                                             |
| `bunnies.sh`  | script fordításhoz és futtatáshoz                          |