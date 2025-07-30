# MetricConverter.cpp és ImperialConverter.cpp osztályok

***két GPS‑pont (szélesség/hosszúság) közötti távolság és irányszög (bearing) kiszámítása, különböző mértékegységekben
***

## MetricConverter.cpp – metrikus rendszer (méter és fok)

**_Metódusok_**

### double MetricConverter::calculateDistance(const GPSPoint& from, const GPSPoint& to)

_Feladata_: két GPS‑pont közötti felszíni távolság kiszámítása a Földön, méterben, a Haversine-képlet alapján.
_Lépései_:

1. constexpr double R = 6371000; – a Föld sugara méterben (kb. 6 371 km) mint konstans.
2. A bemeneti pontok fokban adott földrajzi koordinátáit (lat/lon) radiánra alakítja:

- lat1 = deg2rad(from.lat), lon1 = deg2rad(from.lon), illetve lat2, lon2

3. Kiszámítja a különbségeket:

- dlat = lat2 - lat1, dlon = lon2 - lon1.

4. Haversine-formula:

- a = sin²(dlat/2) + cos(lat1)*cos(lat2)*sin²(dlon/2)
- c = 2 * atan2(sqrt(a), sqrt(1-a))

5. A távolság: distance = R * c, méterben adja vissza.

### double MetricConverter::calculateBearing(const GPSPoint& from, const GPSPoint& to)

_Feladata_: a kezdő irány (initial bearing / forward azimuth) kiszámítása fokban a kiindulási ponttól a célpont felé.
_Lépések_:

1. A két pont szélessége/hosszúsága radiánban.
2. Segédváltozók:

- x = cos(lat2) * sin(dlon)
- y = cos(lat1)*sin(lat2) - sin(lat1)*cos(lat2)*cos(dlon)

3. Nyers irány radiánban: bearing = atan2(x, y) vagy atan2(y, x) – a konkrét sorrend a kódrészlettől függ; a lényeg,
   hogy atan2 adja a megfelelő kvadránst.
4. Fokba konvertálás: rad2deg(bearing).
5. Normalizálás 0–360° közé (jellemzően fmod + hozzáadás, ha negatív).

### const char* MetricConverter::distanceUnit() const

- Visszatérési érték: "m" – a távolság méter egységű.

### const char* MetricConverter::bearingUnit() const

- Visszatérési érték: "deg" – az irányszög fok egységű.

**Segédfüggvények**

### double MetricConverter::deg2rad(double deg) const és double MetricConverter::rad2deg(double rad) const

- deg2rad: fok → radián (deg * M_PI / 180.0)
- rad2deg: radián → fok (rad * 180.0 / M_PI)

## ImperialConverter.cpp – angolszász rendszer (láb és radián)

### double ImperialConverter::calculateDistance(const GPSPoint& from, const GPSPoint& to)

_Feladata_: távolság számítása lábban (ft).
_Működés_:

1. Létrehoz egy MetricConverter példányt: MetricConverter metric;
2. Meghívja a metrikus számítást: metric.calculateDistance(from, to) → eredmény méterben.
3. Egységváltás méterről lábra: * 0.000621371 * 5280.

- 0.000621371 ≈ mérföld / méter (1 m ≈ 0,000621371 mi)
- 5280 = láb / mérföld A kettő szorzata: 1 m ≈ 3.280839895 ft (klasszikus m→ft váltó).

4. Visszaadja az értéket lábban.

### double ImperialConverter::calculateBearing(const GPSPoint& from, const GPSPoint& to)

_Feladata_: irányszög számítása radiánban.

_Működés_:

1. MetricConverter példányon keresztül lekéri az irányszöget fokban: metric.calculateBearing(from, to) → fok.
2. Fok → radián átszámítás: * M_PI / 180.0.
3. Visszaadja az értéket radiánban.

### const char* ImperialConverter::distanceUnit() const

- Visszatérési érték: "ft" – távolság láb egységben.

### const char* ImperialConverter::bearingUnit() const

- Visszatérési érték: "rad" – irányszög radiánban.
