# GPS koordináta feldolgoz
- A projekt `CLion 2025.1.4` verziószámú IDE-vel készült. `Ubuntu 24.04.2 LTS` operációs rendszeren.
## Részletes magyarázat
**1) Cél és bemenet/kimenet**\
   A program GPS pontok sorozata alapján minden szomszédos pontpárra kiszámolja a légvonalbeli távolságot és az irányszöget, majd az eredményeket JSON fájlba írja. A mértékegységrendszer (metrikus vagy angolszász) parancssori kapcsolóval választható.

**Használat:**
```bash
program_nev <input.json> <output.json> [--metric | --imperial]
```
Ennél a projektnél a program futtatása:
```bash
./GPSProcessor ../input.json ../output.json --metric
```
- `input.json`: bemeneti fájl, amely az input kulcs alatt pontok tömbjét tartalmazza.
- `output.json`: a generált kimenet útvonala.
- `--metric` vagy `--imperial`: mértékegység-rendszer kiválasztása (a távolság és esetleg az irány formátuma ezen múlik – a tényleges egységeket az `IUnitConverter` megvalósításai határozzák meg).

**Megjegyzés:** A kódban hivatkozott `IUnitConverter`, `MetricConverter`, `ImperialConverter` és `GPSPoint` típusok nincsenek itt definiálva, de a név alapján:

- `IUnitConverter` egy interfész, ami legalább `calculateDistance(GPSPoint, GPSPoint)` és `calculateBearing(GPSPoint, GPSPoint)` függvényeket tartalmaz.
- `MetricConverter/ImperialConverter` ezeket saját mértékegység-rendszerben valósítja meg.
- `GPSPoint` feltehetően `lat` és `lon` (fokban megadott) double mezőket tartalmaz.

**2) Parancssori argumentumok ellenőrzése**
- Ha kevesebb mint 3 paramétert kap (programnéven felül), kiírja a használati útmutatót és **1-es hibakóddal** kilép.

**3) Mértékegység mód kiválasztása**
- `--metric` → `MetricConverter` 
- `--imperial` → `ImperialConverter`
- Más érték esetén hibaüzenet és **2-es hibakód**.

**4) Bemeneti JSON fájl megnyitása és beolvasása**
- Ha a fájl nem nyitható meg → hibaüzenet, **3-as hibakód**.
- Ha a JSON parse közben hiba van → hibaüzenet, **4-es hibakód.**

**5) Adatszerkezet elvárás**
- A program a `inputJson["input"]` tömböt olvassa.
- Minden elemben a kód így fér hozzá: `points[i]["GPSP"]["lat"], points[i]["GPSP"]["lon"]`.
- Elvárt szerkezet például:
```json
{
    "input": [
        { "GPSP": { "lat": 47.497913, "lon": 19.040236 } },
        { "GPSP": { "lat": 47.500000, "lon": 19.050000 } },
        { "GPSP": { "lat": 47.510000, "lon": 19.060000 } }
    ]
}
```
> [!WARNING]
> A kód nem ellenőrzi, hogy ezek a kulcsok ténylegesen léteznek-e és számok-e. Ha hiányoznak vagy rossz típusúak, futásidejű kivétel/hiba történhet.

**6) Számítás a pontpárokon**
- A ciklus `i = 1`-től indul, tehát minden iterációban az `(i-1)` és `i` indexű pont között számol.
- A távolságot és irányszöget a `converter` (metrikus/imperial) számítja:
  - `calculateDistance(from, to)` → `dist` (double).
  - `calculateBearing(from, to)` → `bearing` (double).
- **Kimenet formázás:**
- A kiinduló pont `(fromGPSP) lat/lon` értékei **5 tizedesre** kerekítve kerülnek a kimenetbe.
- A `distance` és `bearing` **egészre kerekítve** (és `int`-re konvertálva) kerülnek a kimenetbe.
  - A távolság egysége attól függ, melyik konvertert használod (pl. méter vs. láb/mérföld), az irányszög általában **fokban** értendő 0–360 tartományban, feltételezhetően az északtól az óramutató járásával megegyezően, de ezt a konverter implementációja dönti el.

**7) Kimeneti JSON felépítése és írása**
- A kimenet `output["output"]` tömböt tartalmaz. Minden elem egy ilyen objektum:

```json
{
    "fromGPSP": { "lat": 47.49791, "lon": 19.04024 },
    "distance": 123
}
```
- A fájl 2 szóközös behúzással `(std::setw(2))` kerül kiírásra.
- Siker esetén kiírja: `Sikeresen kiírva: <outputPath>` és **0-val** tér vissza.

**8) Kilépési kódok összefoglalója**
- 0 – siker
- 1 – rossz parancssori használat / hiányzó argumentumok
- 2 – ismeretlen mértékegység mód `(--metric/--imperial)`
- 3 – input fájl nem nyitható
- 4 – hibás JSON formátum

**Példa bemenet és kimenet**
***Bemenet (`input.json`):***

```json
{
    "input": [
        { "GPSP": { "lat": 47.497913, "lon": 19.040236 } },
        { "GPSP": { "lat": 47.498500, "lon": 19.045000 } },
        { "GPSP": { "lat": 47.500000, "lon": 19.050000 } }
    ]
}
```
***Futtatás:***
```bash
./gpsproc input.json output.json --metric
```
***Lehetséges kimenet (`output.json`) (az értékek a konverter megvalósításától függnek):***
```json
{
    "output": [
        {
            "fromGPSP": { "lat": 47.49791, "lon": 19.04024 },
            "distance": 412,
            "bearing": 63
        },
        {
            "fromGPSP": { "lat": 47.4985, "lon": 19.045 },
            "distance": 405,
            "bearing": 66
        }
    ]
}
```

## Fordítás és futtatás (Ubuntu + g++, nlohmann/json)
Használat előtti telepítés _Ununtu-n(Linux)_:
```bash
sudo apt install cmake
sudo apt install build-essential
```
Használt külső lib: **nlohmann/json**\
Telepíthető Ubuntu-n:
```bash
sudo apt install nlohmann-json3-dev
```
Ha a `nlohmann/json` egysoros fejléce elérhető (pl. `/usr/include/nlohmann/json.hpp`), egy egyszerű fordítás:
```bash
g++ -std=c++17 -O2 -Wall -Wextra -pedantic \
  main.cpp MetricConverter.cpp ImperialConverter.cpp \
  -o gpsproc
```

**CMake** (`CMakeLists.txt`):
```c
make_minimum_required(VERSION 3.10)
project(GPSProcessor)

set(CMAKE_CXX_STANDARD 17)

add_executable(GPSProcessor
        src/main.cpp
        src/MetricConverter.cpp
        src/ImperialConverter.cpp
        include/ImperialConverter.h
        include/MetricConverter.h
)

target_include_directories(GPSProcessor PRIVATE include)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(GPSProcessor PRIVATE nlohmann_json::nlohmann_json)
```
Készült egy automatizált szript is `run.sh` ez ellenőrzi a `cmake`, `make`, telepítve van-e,ha nincs telepíti.\
Majd ezután készít egy `build` fordítást és futtatja az elkészült binárist.
```bash
#!/bin/bash

# Ellenőrizzük, hogy a cmake telepítve van-e
if ! command -v cmake &> /dev/null
then
    echo "❌ A 'cmake' nincs telepítve. Telepítsd az alábbi paranccsal:"
    echo "   sudo apt install cmake"
    exit 1
fi

# Ellenőrizzük, hogy a make telepítve van-e
if ! command -v make &> /dev/null
then
    echo "❌ A 'make' nincs telepítve. Telepítsd az alábbi paranccsal:"
    echo "   sudo apt install build-essential"
    exit 1
fi

# Build könyvtár létrehozása és fordítás
mkdir -p build
cd build
cmake ..
make

# Bináris futtatása metrikus módban
if [[ -f GPSProcessor ]]; then
    ./GPSProcessor ../input.json ../output.json --metric
else
    echo "❌ A program fordítása nem sikerült. Nincs GPSProcessor bináris."
    exit 2
fi


```
**A `run.sh` futtatása:**
```bash
chmod +x run.sh
./run.sh
```




