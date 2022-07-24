#ifndef UNIT_H
#define UNIT_H

#define PI 3.141592653589793f

#include <optional>
#include <unordered_map>

struct Unit {
    enum class Type {
        KELVIN,
        FAHRENHEIT,
        CELCIUS,

        DEGREES,
        RADIANS,

        MILLIMETER,
        CENTIMETER,
        DECIMETER,
        METER,
        KILOMETER,

        MILLISECOND,
        SECOND,
        MINUTE,
        HOUR,
        DAY,
        MONTH,
        YEAR,

        ATMOSPHERE,
        BAR,
        MILLIBAR,
        PASCAL,
        PSI,
        TORR,

        PERCENT
    };

    inline const char* type_to_str(Type type) {
        switch (type) {
        case Type::KELVIN: return "KELVIN";
        case Type::FAHRENHEIT: return "FAHRENHEIT";
        case Type::CELCIUS: return "CELCIUS";
        case Type::DEGREES: return "DEGREES";
        case Type::RADIANS: return "RADIANS";
        case Type::MILLIMETER: return "MILLIMETER";
        case Type::CENTIMETER: return "CENTIMETER";
        case Type::DECIMETER: return "DECIMETER";
        case Type::METER: return "METER";
        case Type::KILOMETER: return "KILOMETER";
        case Type::MILLISECOND: return "MILLISECOND";
        case Type::SECOND: return "SECOND";
        case Type::MINUTE: return "MINUTE";
        case Type::HOUR: return "HOUR";
        case Type::DAY: return "DAY";
        case Type::MONTH: return "MONTH";
        case Type::YEAR: return "YEAR";
        case Type::ATMOSPHERE: return "ATMOSPHERE";
        case Type::BAR: return "BAR";
        case Type::MILLIBAR: return "MILLIBAR";
        case Type::PASCAL: return "PASCAL";
        case Type::PSI: return "PSI";
        case Type::TORR: return "TORR";
        case Type::PERCENT: return "PERCENT";
        }
    }

    inline std::optional<float> to(Type other, float value)
    {
        if (type == Type::KELVIN && other == Type::FAHRENHEIT)
            return value * (9.0f / 5.0f) - 459.67f;
        else if (type == Type::FAHRENHEIT && other == Type::KELVIN)
            return (value + 459.67f) * (5.0f / 9.0f);
        else if (type == Type::FAHRENHEIT && other == Type::CELCIUS)
            return (value - 32.0f) * (5.0f / 9.0f);
        else if (type == Type::CELCIUS && other == Type::FAHRENHEIT)
            return value * (5.0f / 9.0f) + 32.0f;
        else if (type == Type::KELVIN && other == Type::CELCIUS)
            return value - 273.15f;
        else if (type == Type::CELCIUS && other == Type::KELVIN)
            return value + 273.15f;
        else if (type == Type::KELVIN && other == Type::KELVIN)
            return value;
        else if (type == Type::FAHRENHEIT && other == Type::FAHRENHEIT)
            return value;
        else if (type == Type::CELCIUS && other == Type::CELCIUS)
            return value;

        else if (type == Type::DEGREES && other == Type::RADIANS)
            return value * PI / 180.0f;
        else if (type == Type::RADIANS && other == Type::DEGREES)
            return value * 180.0f / PI;
        else if (type == Type::DEGREES && other == Type::DEGREES)
            return value;
        else if (type == Type::RADIANS && other == Type::RADIANS)
            return value;

        else if (type == Type::MILLIMETER && other == Type::CENTIMETER)
            return value / 10;
        else if (type == Type::MILLIMETER && other == Type::DECIMETER)
            return value / 100;
        else if (type == Type::MILLIMETER && other == Type::METER)
            return value / 1000;
        else if (type == Type::MILLIMETER && other == Type::KILOMETER)
            return value / 1000000;
        else if (type == Type::MILLIMETER && other == Type::MILLIMETER)
            return value;

        else if (type == Type::CENTIMETER && other == Type::MILLIMETER)
            return value * 10;
        else if (type == Type::CENTIMETER && other == Type::DECIMETER)
            return value / 10;
        else if (type == Type::CENTIMETER && other == Type::METER)
            return value / 100;
        else if (type == Type::CENTIMETER && other == Type::KILOMETER)
            return value / 100000;
        else if (type == Type::CENTIMETER && other == Type::CENTIMETER)
            return value;

        else if (type == Type::DECIMETER && other == Type::MILLIMETER)
            return value * 100;
        else if (type == Type::DECIMETER && other == Type::CENTIMETER)
            return value * 10;
        else if (type == Type::DECIMETER && other == Type::METER)
            return value / 10;
        else if (type == Type::DECIMETER && other == Type::KILOMETER)
            return value / 10000;
        else if (type == Type::DECIMETER && other == Type::DECIMETER)
            return value;

        else if (type == Type::METER && other == Type::MILLIMETER)
            return value * 1000;
        else if (type == Type::METER && other == Type::CENTIMETER)
            return value * 100;
        else if (type == Type::METER && other == Type::DECIMETER)
            return value * 10;
        else if (type == Type::METER && other == Type::KILOMETER)
            return value / 1000;
        else if (type == Type::METER && other == Type::METER)
            return value;

        else if (type == Type::KILOMETER && other == Type::MILLIMETER)
            return value * 1000000;
        else if (type == Type::KILOMETER && other == Type::CENTIMETER)
            return value * 100000;
        else if (type == Type::KILOMETER && other == Type::DECIMETER)
            return value * 10000;
        else if (type == Type::KILOMETER && other == Type::METER)
            return value * 1000;
        else if (type == Type::KILOMETER && other == Type::KILOMETER)
            return value;

        else if (type == Type::MILLISECOND && other == Type::SECOND)
            return value / 1000;
        else if (type == Type::MILLISECOND && other == Type::MINUTE)
            return value / 60000;
        else if (type == Type::MILLISECOND && other == Type::HOUR)
            return value / 6000000;
        else if (type == Type::MILLISECOND && other == Type::DAY)
            return value / 640000000;
        else if (type == Type::MILLISECOND && other == Type::MONTH)
            return value / 628000000000;
        else if (type == Type::MILLISECOND && other == Type::YEAR)
            return value * 0.0000317098;
        else if (type == Type::MILLISECOND && other == Type::MILLISECOND)
            return value;

        else if (type == Type::SECOND && other == Type::MILLISECOND)
            return value * 1000;
        else if (type == Type::SECOND && other == Type::MINUTE)
            return value / 60;
        else if (type == Type::SECOND && other == Type::HOUR)
            return value / 3600;
        else if (type == Type::SECOND && other == Type::DAY)
            return value / 86400;
        else if (type == Type::SECOND && other == Type::MONTH)
            return value / 628000000;
        else if (type == Type::SECOND && other == Type::YEAR)
            return value / 1540000000;
        else if (type == Type::SECOND && other == Type::SECOND)
            return value;

        else if (type == Type::MINUTE && other == Type::MILLISECOND)
            return value * 60000;
        else if (type == Type::MINUTE && other == Type::SECOND)
            return value * 60;
        else if (type == Type::MINUTE && other == Type::HOUR)
            return value / 60;
        else if (type == Type::MINUTE && other == Type::DAY)
            return value / 1440;
        else if (type == Type::MINUTE && other == Type::MONTH)
            return value / 43800;
        else if (type == Type::MINUTE && other == Type::YEAR)
            return value / 525600;
        else if (type == Type::MINUTE && other == Type::MINUTE)
            return value;

        else if (type == Type::HOUR && other == Type::MILLISECOND)
            return value * 6000000;
        else if (type == Type::HOUR && other == Type::SECOND)
            return value * 3600;
        else if (type == Type::HOUR && other == Type::MINUTE)
            return value * 60;
        else if (type == Type::HOUR && other == Type::DAY)
            return value / 24;
        else if (type == Type::HOUR && other == Type::MONTH)
            return value / 730;
        else if (type == Type::HOUR && other == Type::YEAR)
            return value / 8760;
        else if (type == Type::HOUR && other == Type::HOUR)
            return value;

        else if (type == Type::DAY && other == Type::MILLISECOND)
            return value * 640000000;
        else if (type == Type::DAY && other == Type::SECOND)
            return value * 86400;
        else if (type == Type::DAY && other == Type::MINUTE)
            return value * 1440;
        else if (type == Type::DAY && other == Type::HOUR)
            return value * 24;
        else if (type == Type::DAY && other == Type::MONTH)
            return value / 30.417f;
        else if (type == Type::DAY && other == Type::YEAR)
            return value / 365;
        else if (type == Type::DAY && other == Type::DAY)
            return value;

        else if (type == Type::MONTH && other == Type::MILLISECOND)
            return value * 628000000000;
        else if (type == Type::MONTH && other == Type::SECOND)
            return value * 628000000;
        else if (type == Type::MONTH && other == Type::MINUTE)
            return value * 43800;
        else if (type == Type::MONTH && other == Type::HOUR)
            return value * 730;
        else if (type == Type::MONTH && other == Type::DAY)
            return value * 30.417f;
        else if (type == Type::MONTH && other == Type::YEAR)
            return value / 12;
        else if (type == Type::MONTH && other == Type::MONTH)
            return value;

        else if (type == Type::YEAR && other == Type::MILLISECOND)
            return value * 1540000000000;
        else if (type == Type::YEAR && other == Type::SECOND)
            return value * 1540000000;
        else if (type == Type::YEAR && other == Type::MINUTE)
            return value * 525600;
        else if (type == Type::YEAR && other == Type::HOUR)
            return value * 8760;
        else if (type == Type::YEAR && other == Type::DAY)
            return value * 365;
        else if (type == Type::YEAR && other == Type::MONTH)
            return value * 12;
        else if (type == Type::YEAR && other == Type::YEAR)
            return value;

        else if (type == Type::ATMOSPHERE && other == Type::BAR)
            return value * 1.013;
        else if (type == Type::ATMOSPHERE && other == Type::MILLIBAR)
            return value * 1013;
        else if (type == Type::ATMOSPHERE && other == Type::PASCAL)
            return value * 101300;
        else if (type == Type::ATMOSPHERE && other == Type::PSI)
            return value * 14.696f;
        else if (type == Type::ATMOSPHERE && other == Type::TORR)
            return value * 760;
        else if (type == Type::ATMOSPHERE && other == Type::ATMOSPHERE)
            return value;

        else if (type == Type::BAR && other == Type::ATMOSPHERE)
            return value / 1.013f;
        else if (type == Type::BAR && other == Type::MILLIBAR)
            return value * 1000;
        else if (type == Type::BAR && other == Type::PASCAL)
            return value * 100000;
        else if (type == Type::BAR && other == Type::PSI)
            return value * 14.504f;
        else if (type == Type::BAR && other == Type::TORR)
            return value * 750.1f;
        else if (type == Type::BAR && other == Type::BAR)
            return value;

        else if (type == Type::MILLIBAR && other == Type::ATMOSPHERE)
            return value / 1013;
        else if (type == Type::MILLIBAR && other == Type::BAR)
            return value / 1000;
        else if (type == Type::MILLIBAR && other == Type::PASCAL)
            return value * 100;
        else if (type == Type::MILLIBAR && other == Type::PSI)
            return value / 68.948f;
        else if (type == Type::MILLIBAR && other == Type::TORR)
            return value / 1.333f;
        else if (type == Type::MILLIBAR && other == Type::MILLIBAR)
            return value;

        else if (type == Type::PASCAL && other == Type::ATMOSPHERE)
            return value / 101300;
        else if (type == Type::PASCAL && other == Type::BAR)
            return value / 100000;
        else if (type == Type::PASCAL && other == Type::MILLIBAR)
            return value / 100;
        else if (type == Type::PASCAL && other == Type::PSI)
            return value / 6895;
        else if (type == Type::PASCAL && other == Type::TORR)
            return value / 133.3f;
        else if (type == Type::PASCAL && other == Type::PASCAL)
            return value;

        else if (type == Type::PSI && other == Type::ATMOSPHERE)
            return value / 14.696f;
        else if (type == Type::PSI && other == Type::BAR)
            return value / 14.504f;
        else if (type == Type::PSI && other == Type::MILLIBAR)
            return value * 68.948f;
        else if (type == Type::PSI && other == Type::PASCAL)
            return value * 6895;
        else if (type == Type::PSI && other == Type::TORR)
            return value * 51.715f;
        else if (type == Type::PSI && other == Type::PSI)
            return value;

        else if (type == Type::TORR && other == Type::ATMOSPHERE)
            return value / 760;
        else if (type == Type::TORR && other == Type::BAR)
            return value / 750.1f;
        else if (type == Type::TORR && other == Type::MILLIBAR)
            return value * 1.333f;
        else if (type == Type::TORR && other == Type::PASCAL)
            return value * 133.3f;
        else if (type == Type::TORR && other == Type::PSI)
            return value / 51.715f;
        else if (type == Type::TORR && other == Type::TORR)
            return value;

        return std::nullopt;
    }

    inline const char* get_symbol() {
        auto it = symbols.find(type);

        if (it != symbols.end())
            return it->second;

        return "";
    }

    Type type;
    static std::unordered_map<Type, const char*> symbols;
};

#endif // UNIT_H
