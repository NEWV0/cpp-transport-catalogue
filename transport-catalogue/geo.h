#pragma once
#include <cmath>
#include <iostream>

struct Coordinates {
    double latitude;
    double longitude;
    
    bool operator==(const Coordinates& other) const {
        return latitude == other.latitude 
            && longitude == other.longitude;
    }
};
 
inline double ComputeDistance(Coordinates start, Coordinates end) {
    using namespace std;
    if (!(start == end)) {
        const double dr = 3.1415926535 / 180.;
        const double earth_rad = 6371000; 
        return acos(sin(start.latitude * dr) * sin(end.latitude * dr)
                    + cos(start.latitude * dr) * cos(end.latitude * dr) 
                    * cos(abs(start.longitude - end.longitude) * dr)) * earth_rad;
    } else {
        return 0.0;
    }
}