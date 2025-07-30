/**
* Fájlnév: MetricConverter.cpp definíció – metrikus rendszer (méter és fok)
* Cél: Két GPS-pont közötti távolság és irányszög számítása metrikus egységekben (m, deg).
* Módszer: Haversine-képlet a távolsághoz; initial bearing számítása fokban.
*/
// Közös interfész / típusok (pl. GPSPoint, absztrakt konverter).
#include "IUnitConverter.h"
// <cmath>: trigonometria és matematikai konstansok (sin, cos, atan2, sqrt, fmod, M_PI).
#include <cmath>
// Osztály deklarációja: MetricConverter (metrikus egységek).
#include "MetricConverter.h"

/**
 * Metódus név: alculateDistance
 * két GPS‑pont közötti felszíni távolság kiszámítása a Földön, méterben, a Haversine-képlet alapján
 * @param from kiindulási pont (GPSPoint)
 * @param to célpont (GPSPoint)
 * @return
 */
double MetricConverter::calculateDistance(const GPSPoint &from, const GPSPoint &to) {
    // A Föld sugara méterben (gömbi közelítés).
    constexpr double R = 6371000;
    // Kiindulási és célkoordináták fok -> radián átalakítása a trigonometrikus függvényekhez.
    double lat1 = deg2rad(from.lat), lon1 = deg2rad(from.lon);
    double lat2 = deg2rad(to.lat), lon2 = deg2rad(to.lon);
    // A földrajzi különbségek radiánban: szélesség (dlat) és hosszúság (dlon).
    double dlat = lat2 - lat1;
    double dlon = lon2 - lon1;
    // Haversine 'a' komponense: sin^2(dlat/2) + cos(lat1) * cos(lat2) * sin^2(dlon/2).
    double a = sin(dlat / 2) * sin(dlat / 2) +
               cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return R * c;
}

/**
 * metódus név: calculateBearing
 * a kezdő irány (initial bearing / forward azimuth) kiszámítása fokban a kiindulási ponttól a célpont felé
 * @param from kiindulási pont (GPSPoint)
 * @param to célpont (GPSPoint)
 * @return
 */
double MetricConverter::calculateBearing(const GPSPoint &from, const GPSPoint &to) {
    // Kiindulási és célkoordináták fok -> radián átalakítása a trigonometrikus függvényekhez.
    double lat1 = deg2rad(from.lat), lon1 = deg2rad(from.lon);
    double lat2 = deg2rad(to.lat), lon2 = deg2rad(to.lon);
    double dlon = lon2 - lon1;

    double y = sin(dlon) * cos(lat2);
    double x = cos(lat1) * sin(lat2) -
               sin(lat1) * cos(lat2) * cos(dlon);
    double bearing = atan2(y, x);
    return fmod((rad2deg(bearing) + 360.0), 360.0);
}

// A távolságegység megnevezése (visszatérési érték): méter.
const char *MetricConverter::distanceUnit() const {
    return "m";
}

// Az irányszög egység megnevezése: fok (degree).
const char *MetricConverter::bearingUnit() const {
    return "deg";
}

// Fok -> radián segédfüggvény.
double MetricConverter::deg2rad(double deg) const {
    // 180° = π rad alapján.
    return deg * M_PI / 180.0;
}

// Radián -> fok segédfüggvény.
double MetricConverter::rad2deg(double rad) const {
    // π rad = 180° alapján.
    return rad * 180.0 / M_PI;
}
