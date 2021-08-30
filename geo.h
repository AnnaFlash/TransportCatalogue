#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
const int EARTH_RADIUS = 6371000;
namespace distance {
    struct Coordinates {
        double lat;
        double lng;
    };

    inline double ComputeDistance(Coordinates from, Coordinates to) {
        using namespace std;
        //
        static const double dr = M_PI / 180.;
        return acos(sin(from.lat * dr) * sin(to.lat * dr)
            + cos(from.lat * dr) * cos(to.lat * dr) * cos(abs(from.lng - to.lng) * dr))
            * EARTH_RADIUS;
    }
}