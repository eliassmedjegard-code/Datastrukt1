# Labb 1 - Grafer

Implementation av elementära grafalgoritmer: DFS, BFS och Dijkstras
algoritm. Grafen representeras som en granne-matris (adjacency matrix).

## Filer

| Fil               | Innehåll                                              |
| ----------------- | ----------------------------------------------------- |
| `reader.h/.cpp`   | Inläsning av indatafilen (oförändrad från labbens exempel) |
| `graph.h/.cpp`    | Grafklass med matris, DFS, BFS, Dijkstra och bekymmer-detektor |
| `main.cpp`        | Drivrutin som svarar på labbens frågor                |
| `export.txt`      | Kursens datafil (Sundsvall västra)                    |
| `input.txt`       | Liten testfil med tio noder                           |
| `input_broken.txt`| Testfil med en isolerad nod (för demo)                |
| `Makefile`        | Bygg-skript                                           |

## Bygga och köra

```bash
make
./labb1                  # läser export.txt (default)
./labb1 input.txt        # läser annan fil
```

## Algoritmer

**DFS** är iterativt skriven med `std::stack`. Vi börjar i startnoden,
markerar den besökt och pushar alla obesökta grannar på stacken.

**BFS** är samma sak fast med `std::queue`. Det innebär att noderna
besöks i nivå-ordning från startnoden.

**Dijkstra** är skriven i den enkla O(V²)-varianten: sätt
`dist[start] = 0`, alla andra till oändligheten. Upprepa: hitta den
obesökta nod med minst avstånd, markera besökt, slappa kanter till
alla grannar. Vägen byggs upp genom att gå bakåt från målet via en
`prev`-tabell.

## Bekymmer i grafen

Indatafilens header säger `Directional edges: 115` - ett **udda** tal.
Eftersom de flesta kanter ligger som "fram + tillbaka"-par är det en
indikation på att exakt en kant saknar sin motsats. Programmet
hittar den automatiskt:

```
Följande riktade kanter saknar sin motsatsriktning:
  19 -> 3 (vikt 92.03)  [Bite Line West -> Genväg 1]
```

Det innebär att man kan gå **från** Bite Line West **till** Genväg 1,
men inte tvärtom. Alla andra kanter i filen finns i båda riktningar.

**Åtgärd:** lägg till motsvarande omvända rad i datafilen:

```
3 19 92.03 [Stängd pizzeria]
```

Vår grafmatris symmetriserar redan kanterna när den byggs (vi sätter
både `matrix[a][b]` och `matrix[b][a]`), så Dijkstra fungerar trots
felet. DFS och BFS rapporterar därmed att grafen är **sammanhängande**
(vilket den blir när asymmetrin behandlas som ett dubbelriktat kanten).

## Svar på labbens frågor

Nodnamnen i frågorna matchas mot de namn som faktiskt finns i `export.txt`:

| Frågans formulering | Nod i datafilen           |
| ------------------- | ------------------------- |
| Nackstavägen        | `Nackstavägen Axvägen 1` (id 24) |
| Förrådet            | `Förrådet` (id 37)        |
| L319                | `L319` (id 46)            |
| D025                | `D025` (id 47)            |
| Universitetet       | `Storgatan Rondell Universitetet` (id 20) |
| Bite Line Västra    | `Bite Line West` (id 19)  |

### 1. Nackstavägen → Förrådet

**Total väglängd: 1726,18 m**

```
Nackstavägen Axvägen 1
 -> Bergsgatan Nackstavägen
 -> Bultgatan 21
 -> Bultgatan 19
 -> Genväg 1
 -> Genväg 2
 -> Järnvägsövergång
 -> Genväg Åkanten
 -> Åkanten1
 -> Åkanten infart parkering
 -> Sidsjövägen campus 1
 -> Grönborgsgatan Väderkvarnsbacken
 -> Förrådet
```

Promenaden går baklänges längs Nackstavägen, in på Bultgatan, via
genvägen genom Åkanten ner till Sidsjövägen och upp till Förrådet.

### 2. L319 → D025

**Total väglängd: 479,65 m**

```
L319
 -> Entré Hus L            (0,00 m, trapphuset/hissen)
 -> Rondell Campus         (69,72 m)
 -> Storgatan Rondell Universitetet  (89,24 m)
 -> Sidsjövägen campus 1   (69,72 m)
 -> Grönborgsgatan Väderkvarnsbacken  (156,16 m)
 -> D025                   (94,81 m)
```

Notera att kanten `L319 -> Entré Hus L` har vikt 0 (det är
samma trapphus). Vägen går alltså ut ur L-huset, runt rondellen vid
Universitetet, ner längs Sidsjövägen och in i D-huset.

### 3. Universitetet → Bite Line Västra

**Total väglängd: 1207,50 m**

```
Storgatan Rondell Universitetet
 -> Sidsjövägen campus 1
 -> Åkanten infart parkering
 -> Åkanten1
 -> Genväg Åkanten
 -> Järnvägsövergång
 -> Genväg 2
 -> Genväg 1
 -> Bite Line West
```

Vägen går från rondellen vid Universitetet, ner längs Sidsjövägen och
sedan via Åkanten-genvägen och järnvägsövergången till Bite Line West.

> Den kortaste vägen kräver att asymmetrin på kanten `19 -> 3` är
> åtgärdad. Vår matris symmetriserar automatiskt så Dijkstra hittar
> vägen ändå - men den underliggande kanten i datafilen behöver kompletteras
> som beskrivet ovan.
