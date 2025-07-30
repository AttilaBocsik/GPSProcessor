/**
* Fájlnév: ImperialConverter.cpp
* Cél: Két GPS-pont közötti távolság és irányszög kiszámítása angolszász egységekben.
* - Távolság: láb (ft)  [a MetricConverter méter alapú számításából konvertálva]
* - Irányszög: radián (rad)  [a MetricConverter fok alapú eredményéből konvertálva]
* Ha a fordítóban M_PI nem érhető el, győződj meg róla, hogy a <cmath> be van vonva,
* és szükség esetén definiáld saját konstansként: pl. constexpr double PI = 3.141592653589793;
*/
// Saját fejléce ennek az implementációnak.
#include "ImperialConverter.h"
// Az angolszász konverter a metrikus konverterre támaszkodik a nyers számításokhoz, majd egységet vált.
// A metrikus konvertert használjuk újraszámítás helyett -> egységkonverziót végzünk csak.
#include "MetricConverter.h"

/**
 * Metódus név: alculateDistance
 * @param from kiindulási pont (GPSPoint)
 * @param to célpont (GPSPoint)
 * @return
 */
double ImperialConverter::calculateDistance(const GPSPoint &from, const GPSPoint &to) {
    // Távolság számítása: először méterben a MetricConverter-rel, majd m -> ft konverzió.
    MetricConverter metric;
    // 0.000621371 mi/m * 5280 ft/mi = 3.280839895 ft/m  (egyenértékű közvetlen m->ft átváltással).
    return metric.calculateDistance(from, to) * 0.000621371 * 5280;
}

/**
 * metódus név: calculateBearing
 * @param from kiindulási pont (GPSPoint)
 * @param to célpont (GPSPoint)
 * @return
 */
double ImperialConverter::calculateBearing(const GPSPoint &from, const GPSPoint &to) {
    // Irányszög számítása: MetricConverter fokban adja vissza -> fok -> radián konverzió.
    MetricConverter metric;
    return metric.calculateBearing(from, to) * M_PI / 180.0;
}

// Az egység rövidítése: 'ft' (foot/láb).
const char *ImperialConverter::distanceUnit() const { return "ft"; }
// Az egység rövidítése: 'rad' (radián).
const char *ImperialConverter::bearingUnit() const { return "rad"; }
