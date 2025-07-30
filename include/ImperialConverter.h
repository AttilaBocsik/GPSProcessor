#ifndef IMPERIALCONVERTER_H
#define IMPERIALCONVERTER_H

#include "IUnitConverter.h"

/**
 * ImperialConverter deklaráció
 */
class ImperialConverter : public IUnitConverter {
public:
    double calculateDistance(const GPSPoint &from, const GPSPoint &to) override;

    double calculateBearing(const GPSPoint &from, const GPSPoint &to) override;

    const char *distanceUnit() const override;

    const char *bearingUnit() const override;
};

#endif
