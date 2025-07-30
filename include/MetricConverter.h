#ifndef METRICCONVERTER_H
#define METRICCONVERTER_H

#include "IUnitConverter.h"
#include <cmath>

/**
 * MetricConverter deklaráció
 */
class MetricConverter : public IUnitConverter {
public:
    double calculateDistance(const GPSPoint &from, const GPSPoint &to) override;

    double calculateBearing(const GPSPoint &from, const GPSPoint &to) override;

    const char *distanceUnit() const override;

    const char *bearingUnit() const override;

private:
    double deg2rad(double deg) const;

    double rad2deg(double rad) const;
};

#endif
