#ifndef IUNITCONVERTER_H
#define IUNITCONVERTER_H

/**
 * GPS koorditáta pont definíciója
 */
struct GPSPoint {
    double lat;
    double lon;
};

/**
 * Közös interfész deklaráció, amely előírja a konverterek által implementálandó metódusokat
 * (calculateDistance, calculateBearing, distanceUnit, bearingUnit).
 */
class IUnitConverter {
public:
    virtual ~IUnitConverter() = default;

    virtual double calculateDistance(const GPSPoint &from, const GPSPoint &to) = 0;

    virtual double calculateBearing(const GPSPoint &from, const GPSPoint &to) = 0;

    virtual const char *distanceUnit() const = 0;

    virtual const char *bearingUnit() const = 0;
};

#endif
