#ifndef HELPER_H
#define HELPER_H

#include <array>
class Helper {
public:
    static inline double map_value(double value, const std::array<double, 2>& a, const std::array<double, 2>& b) {
        return (value - a[0]) * (b[1] - b[0]) / (a[1] - a[0]) + b[0];
    };
};

#endif // HELPER_H
