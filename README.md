# Labb 1 - Grafer

Implementation av elementära grafalgoritmer: DFS, BFS och Dijkstras
algoritm. Grafen representeras som en granne-matris (adjacency matrix).

## Filer

| Fil          | Innehåll                                         |
| ------------ | ------------------------------------------------ |
| `reader.h/.cpp` | Inläsning av indatafilen (kommentarer, M-rader, kant-rader) |
| `graph.h/.cpp`  | Grafklass med matris, DFS, BFS och Dijkstra |
| `main.cpp`      | Drivrutin som svarar på labbens frågor      |
| `input.txt`     | Exempeldata för labben                       |
| `input_broken.txt` | Exempeldata med en isolerad nod (för demo) |
| `Makefile`      | Bygg-skript                                  |

## Bygga och köra

```bash
make
./labb1                  # läser input.txt
./labb1 input_broken.txt # läser annan fil
```

## Indataformat

```
# kommentar
M <nod-id> <namn>
<nod1> <nod2> <vikt> <beskrivning>
```

Reader.cpp förstår tre olika rader: kommentarer, meta (M) och kanter.
Den medföljande readern hanterar inte tomma rader, så håll filen
kompakt utan blanka rader mellan posterna.

## Algoritmer

### DFS (Depth-First Search)
Iterativ implementation med `std::stack`. Vi börjar i startnoden,
markerar den besökt, och pushar alla obesökta grannar på stacken.
Upprepa tills stacken är tom. Om någon nod inte besöktes är grafen
inte sammanhängande.

### BFS (Breadth-First Search)
Som DFS men med `std::queue` istället. Det innebär att noder besöks
i nivå-ordning från startnoden. Samma test för sammanhängande.

### Dijkstras algoritm
Klassisk O(V²) version. Sätt avstånd[start] = 0 och alla andra till
oändligheten. Upprepa: ta den obesökta nod med minst avstånd, markera
besökt, och slappa kanter till alla grannar. När mål-noden är besökt
är vi klara. Vägen byggs upp genom att gå bakåt via en `prev`-tabell.

## Är grafen sammanhängande?

Med exempeldatan i `input.txt` säger både DFS och BFS att grafen är
**sammanhängande**. Båda algoritmerna ger samma svar (vilket de ska -
de utforskar samma uppsättning nåbara noder, bara i olika ordning).

### Hur rättar man till bekymmer?

Om DFS/BFS hittar att grafen INTE är sammanhängande skriver programmet
ut alla noder som inte gick att nå från startnoden, t.ex.:

```
Bekymmer i grafen! Följande noder kan inte nås från startnoden "Universitetet":
  Nod 4: Förrådet
```

För att laga grafen lägger man till minst en kant från en nåbar nod
till en av de isolerade noderna. Exempel:

```
0 4 200 Universitetet -> Förrådet
```

Det går också att kontrollera indatafilen efter saknade rader eller
felstavade nod-id.

## Svar på labbens frågor

Svar baserade på exempeldatan i `input.txt`:

### Nackstavägen → Förrådet
- **Total väglängd: 250**
- Promenad: Nackstavägen → Förrådet (en direkt kant)

### L319 → D025
- **Total väglängd: 115**
- Promenad: L319 → L-huset → D-huset → D025
- Vi går först ut ur L319 till L-husets korridor (30), sedan över
  förbindelsegången till D-huset (60), och slutligen in i D025 (25).

### Universitetet → Bite Line Västra
- **Total väglängd: 150**
- Promenad: Universitetet → Bite Line Västra (direkt kant)
- Notera: vägen via Studenttorget (50 + 120 = 170) är längre, så
  Dijkstra väljer den direkta kanten.

> **OBS:** Värdena ovan kommer från exempeldatan i `input.txt`. Om
> du har den riktiga indatafilen från kursen, kör då `./labb1 <filnamn>`
> så räknar programmet ut korrekta svar för den datan istället.
