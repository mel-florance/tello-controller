#ifndef METRIC_H
#define METRIC_H

#include <any>
#include <numbers>
#include <typeinfo>
#include <unordered_map>

#include <QString>
#include <QVector3D>
#include "qdebug.h"

#include "../include/unit.h"

#define INT_TYPEID typeid(int)
#define FLOAT_TYPEID typeid(float)
#define DOUBLE_TYPEID typeid(double)
#define VEC2_TYPEID typeid(QVector2D)
#define VEC3_TYPEID typeid(QVector3D)
#define VEC4_TYPEID typeid(QVector4D)

inline bool operator < (const QVector2D& lhs, const QVector2D& rhs) {
    return lhs.x() < rhs.x() && lhs.y() < rhs.y();
}
inline bool operator <= (const QVector2D& lhs, const QVector2D& rhs) {
    return lhs.x() <= rhs.x() && lhs.y() <= rhs.y();
}
inline bool operator > (const QVector2D& lhs, const QVector2D& rhs) {
    return lhs.x() > rhs.x() && lhs.y() > rhs.y();
}
inline bool operator >= (const QVector2D& lhs, const QVector2D& rhs) {
    return lhs.x() >= rhs.x() && lhs.y() >= rhs.y();
}

inline bool operator < (const QVector3D& lhs, const QVector3D& rhs) {
    return lhs.x() < rhs.x() && lhs.y() < rhs.y() && lhs.z() < rhs.z();
}
inline bool operator <= (const QVector3D& lhs, const QVector3D& rhs) {
    return lhs.x() <= rhs.x() && lhs.y() <= rhs.y() && lhs.z() <= rhs.z();
}
inline bool operator > (const QVector3D& lhs, const QVector3D& rhs) {
    return lhs.x() > rhs.x() && lhs.y() > rhs.y() && lhs.z() > rhs.z();
}
inline bool operator >= (const QVector3D& lhs, const QVector3D& rhs) {
    return lhs.x() >= rhs.x() && lhs.y() >= rhs.y() && lhs.z() >= rhs.z();
}

inline bool operator < (const QVector4D& lhs, const QVector4D& rhs) {
    return lhs.x() < rhs.x() && lhs.y() < rhs.y() && lhs.z() < rhs.z() && lhs.w() < rhs.w();
}
inline bool operator <= (const QVector4D& lhs, const QVector4D& rhs) {
    return lhs.x() <= rhs.x() && lhs.y() <= rhs.y() && lhs.z() <= rhs.z() && lhs.w() <= rhs.w();
}
inline bool operator > (const QVector4D& lhs, const QVector4D& rhs) {
    return lhs.x() > rhs.x() && lhs.y() > rhs.y() && lhs.z() > rhs.z() && lhs.w() > rhs.w();
}
inline bool operator >= (const QVector4D& lhs, const QVector4D& rhs) {
    return lhs.x() >= rhs.x() && lhs.y() >= rhs.y() && lhs.z() >= rhs.z() && lhs.w() >= rhs.w();
}

enum class ConstraintType {
    EQ,
    NEQ,
    LT,
    LTE,
    GT,
    GTE
};

class Metric {
public:
    Metric(const QString& name, const std::any& value = 0) :
        name(name),
        value(value) {
    };

    Metric(Metric& metric) {
        name = metric.name;
        value = metric.value;
        unit = metric.unit;
        constraints = metric.constraints;
    };


    Metric(const Metric& metric) {
        name = metric.name;
        value = metric.value;
        unit = metric.unit;
        constraints = metric.constraints;
    };

    Metric(Metric&& metric) {
        name = metric.name;
        value = metric.value;
        unit = metric.unit;
        constraints = metric.constraints;

        metric.name.clear();
        metric.value.reset();
        metric.constraints.clear();
    };

    Metric& operator=(Metric& metric) {
        name = metric.name;
        value = metric.value;
        unit = metric.unit;
        constraints = metric.constraints;

        metric.name.clear();
        metric.value.reset();
        metric.constraints.clear();
    }

    ~Metric() = default;

    inline void set_value(const std::any& v) {
        if (check_constraints(v)) {
            this->value = v;
        }
    }

    template<typename T>
    inline std::optional<T> get_value(const std::any& v = std::any()) {
        try {
            return std::any_cast<T>(v.has_value() ? v : value);
        }
        catch(std::bad_any_cast& e) {
            qDebug() << e.what();
        }

        return std::nullopt;
    }

    inline operator QString() {
        QString str(name + ": ");

        if (value.type() == INT_TYPEID)
            str += QString::number(get_value<int>(value).value()) + ' ' + unit.get_symbol();
        else if (value.type() == FLOAT_TYPEID)
            str += QString::number(get_value<float>(value).value()) + ' ' + unit.get_symbol();
        else if (value.type() == DOUBLE_TYPEID)
            str += QString::number(get_value<double>(value).value()) + ' ' + unit.get_symbol();
        else if (value.type() == VEC2_TYPEID) {
            auto vec = get_value<QVector2D>(value).value();
            str += QString("(%1, %2)").arg(vec.x()).arg(vec.y());
        }
        else if (value.type() == VEC3_TYPEID) {
            auto vec = get_value<QVector3D>(value).value();
            str += QString("(%1, %2, %3)").arg(vec.x()).arg(vec.y()).arg(vec.z());
        }
        else if (value.type() == VEC4_TYPEID) {
            auto vec = get_value<QVector4D>(value).value();
            str += QString("(%1, %2, %3, %4)").arg(vec.x()).arg(vec.y()).arg(vec.z()).arg(vec.w());
        }

        return str;
    }

    inline void set_constraint(ConstraintType type, const std::any& limit) {
        constraints[type] = limit;
    }

    inline bool check_constraints(const std::any& v) {
        std::size_t passed = 0;

        for (auto it = constraints.begin(); it != constraints.end(); ++it) {
            switch (it->first) {
            case ConstraintType::EQ: {
                if (it->second.type() == INT_TYPEID && v.type() == INT_TYPEID) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 == v2) ++passed;
                }
                if (it->second.type() == FLOAT_TYPEID && v.type() == FLOAT_TYPEID) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 == v2) ++passed;
                }
                if (it->second.type() == DOUBLE_TYPEID && v.type() == DOUBLE_TYPEID) {
                    auto v1 = get_value<double>(v);
                    auto v2 = get_value<double>(it->second);
                    if (v1 == v2) ++passed;
                }
                if (it->second.type() == VEC2_TYPEID && v.type() == VEC2_TYPEID) {
                    auto v1 = get_value<QVector2D>(v);
                    auto v2 = get_value<QVector2D>(it->second);
                    if (v1 == v2) ++passed;
                }
                if (it->second.type() == VEC3_TYPEID && v.type() == VEC3_TYPEID) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 == v2) ++passed;
                }
                if (it->second.type() == VEC4_TYPEID && v.type() == VEC4_TYPEID) {
                    auto v1 = get_value<QVector4D>(v);
                    auto v2 = get_value<QVector4D>(it->second);
                    if (v1 == v2) ++passed;
                }
                break;
            }
            case ConstraintType::NEQ: {
                if (it->second.type() == INT_TYPEID && v.type() == INT_TYPEID) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 != v2) ++passed;
                }
                if (it->second.type() == FLOAT_TYPEID && v.type() == FLOAT_TYPEID) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 != v2) ++passed;
                }
                if (it->second.type() == DOUBLE_TYPEID && v.type() == DOUBLE_TYPEID) {
                    auto v1 = get_value<double>(v);
                    auto v2 = get_value<double>(it->second);
                    if (v1 != v2) ++passed;
                }
                if (it->second.type() == VEC2_TYPEID && v.type() == VEC2_TYPEID) {
                    auto v1 = get_value<QVector2D>(v);
                    auto v2 = get_value<QVector2D>(it->second);
                    if (v1 != v2) ++passed;
                }
                if (it->second.type() == VEC3_TYPEID && v.type() == VEC3_TYPEID) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 != v2) ++passed;
                }
                if (it->second.type() == VEC4_TYPEID && v.type() == VEC4_TYPEID) {
                    auto v1 = get_value<QVector4D>(v);
                    auto v2 = get_value<QVector4D>(it->second);
                    if (v1 != v2) ++passed;
                }
                break;
            }
            case ConstraintType::LT: {
                if (it->second.type() == INT_TYPEID && v.type() == INT_TYPEID) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 < v2) ++passed;
                }
                if (it->second.type() == FLOAT_TYPEID && v.type() == FLOAT_TYPEID) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 < v2) ++passed;
                }
                if (it->second.type() == DOUBLE_TYPEID && v.type() == DOUBLE_TYPEID) {
                    auto v1 = get_value<double>(v);
                    auto v2 = get_value<double>(it->second);
                    if (v1 < v2) ++passed;
                }
                if (it->second.type() == VEC2_TYPEID && v.type() == VEC2_TYPEID) {
                    auto v1 = get_value<QVector2D>(v);
                    auto v2 = get_value<QVector2D>(it->second);
                    if (v1 < v2) ++passed;
                }
                if (it->second.type() == VEC3_TYPEID && v.type() == VEC3_TYPEID) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 < v2) ++passed;
                }
                if (it->second.type() == VEC4_TYPEID && v.type() == VEC4_TYPEID) {
                    auto v1 = get_value<QVector4D>(v);
                    auto v2 = get_value<QVector4D>(it->second);
                    if (v1 < v2) ++passed;
                }
                break;
            }
            case ConstraintType::LTE: {
                if (it->second.type() == INT_TYPEID && v.type() == INT_TYPEID) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 <= v2) ++passed;
                }
                if (it->second.type() == FLOAT_TYPEID && v.type() == FLOAT_TYPEID) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 <= v2) ++passed;
                }
                if (it->second.type() == DOUBLE_TYPEID && v.type() == DOUBLE_TYPEID) {
                    auto v1 = get_value<double>(v);
                    auto v2 = get_value<double>(it->second);
                    if (v1 <= v2) ++passed;
                }
                if (it->second.type() == VEC2_TYPEID && v.type() == VEC2_TYPEID) {
                    auto v1 = get_value<QVector2D>(v);
                    auto v2 = get_value<QVector2D>(it->second);
                    if (v1 <= v2) ++passed;
                }
                if (it->second.type() == VEC3_TYPEID && v.type() == VEC3_TYPEID) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 <= v2) ++passed;
                }
                if (it->second.type() == VEC4_TYPEID && v.type() == VEC4_TYPEID) {
                    auto v1 = get_value<QVector4D>(v);
                    auto v2 = get_value<QVector4D>(it->second);
                    if (v1 <= v2) ++passed;
                }
                break;
            }
            case ConstraintType::GT: {
                if (it->second.type() == INT_TYPEID && v.type() == INT_TYPEID) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 > v2) ++passed;
                }
                if (it->second.type() == FLOAT_TYPEID && v.type() == FLOAT_TYPEID) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 > v2) ++passed;
                }
                if (it->second.type() == DOUBLE_TYPEID && v.type() == DOUBLE_TYPEID) {
                    auto v1 = get_value<double>(v);
                    auto v2 = get_value<double>(it->second);
                    if (v1 > v2) ++passed;
                }
                if (it->second.type() == VEC2_TYPEID && v.type() == VEC2_TYPEID) {
                    auto v1 = get_value<QVector2D>(v);
                    auto v2 = get_value<QVector2D>(it->second);
                    if (v1 > v2) ++passed;
                }
                if (it->second.type() == VEC3_TYPEID && v.type() == VEC3_TYPEID) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 > v2) ++passed;
                }
                if (it->second.type() == VEC4_TYPEID && v.type() == VEC4_TYPEID) {
                    auto v1 = get_value<QVector4D>(v);
                    auto v2 = get_value<QVector4D>(it->second);
                    if (v1 > v2) ++passed;
                }
                break;
            }
            case ConstraintType::GTE: {
                if (it->second.type() == INT_TYPEID && v.type() == INT_TYPEID) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 >= v2) ++passed;
                }
                if (it->second.type() == FLOAT_TYPEID && v.type() == FLOAT_TYPEID) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 >= v2) ++passed;
                }
                if (it->second.type() == DOUBLE_TYPEID && v.type() == DOUBLE_TYPEID) {
                    auto v1 = get_value<double>(v);
                    auto v2 = get_value<double>(it->second);
                    if (v1 >= v2) ++passed;
                }
                if (it->second.type() == VEC2_TYPEID && v.type() == VEC2_TYPEID) {
                    auto v1 = get_value<QVector2D>(v);
                    auto v2 = get_value<QVector2D>(it->second);
                    if (v1 >= v2) ++passed;
                }
                if (it->second.type() == VEC3_TYPEID && v.type() == VEC3_TYPEID) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 >= v2) ++passed;
                }
                if (it->second.type() == VEC4_TYPEID && v.type() == VEC4_TYPEID) {
                    auto v1 = get_value<QVector4D>(v);
                    auto v2 = get_value<QVector4D>(it->second);
                    if (v1 >= v2) ++passed;
                }
                break;
            }
            }
        }

        return passed == constraints.size();
    }

    inline void set_unit(Unit::Type type) {
        unit.type = type;
    }

    template<typename T>
    inline std::optional<T> convert_to(Unit::Type type) {
        if (value.type() == INT_TYPEID || value.type() == FLOAT_TYPEID || value.type() == DOUBLE_TYPEID)
            return unit.to(type, get_value<T>(value).value());

        return std::nullopt;
    };

private:
    Unit unit;
    QString name;
    std::any value;
    std::unordered_map<ConstraintType, std::any> constraints;
};

#endif // METRIC_H
