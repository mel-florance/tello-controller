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

        if (value.type() == typeid(int))
            str += QString::number(get_value<int>(value).value()) + ' ' + unit.get_symbol();
        else if (value.type() == typeid(float))
            str += QString::number(get_value<float>(value).value()) + ' ' + unit.get_symbol();
        else if (value.type() == typeid(QVector3D)) {
            auto vec = get_value<QVector3D>(value).value();
            str += QString("(%1, %2, %3)").arg(vec.x()).arg(vec.y()).arg(vec.z());
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
                if (it->second.type() == typeid(int) && v.type() == typeid(int)) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 == v2) ++passed;
                }
                if (it->second.type() == typeid(float) && v.type() == typeid(float)) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 == v2) ++passed;
                }
                if (it->second.type() == typeid(QVector3D) && v.type() == typeid(QVector3D)) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 == v2) ++passed;
                }
                break;
            }
            case ConstraintType::NEQ: {
                if (it->second.type() == typeid(int) && v.type() == typeid(int)) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 != v2) ++passed;
                }
                if (it->second.type() == typeid(float) && v.type() == typeid(float)) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 != v2) ++passed;
                }
                if (it->second.type() == typeid(QVector3D) && v.type() == typeid(QVector3D)) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 != v2) ++passed;
                }
                break;
            }
            case ConstraintType::LT: {
                if (it->second.type() == typeid(int) && v.type() == typeid(int)) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 < v2) ++passed;
                }
                if (it->second.type() == typeid(float) && v.type() == typeid(float)) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 < v2) ++passed;
                }
                if (it->second.type() == typeid(QVector3D) && v.type() == typeid(QVector3D)) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 < v2) ++passed;
                }
                break;
            }
            case ConstraintType::LTE: {
                if (it->second.type() == typeid(int) && v.type() == typeid(int)) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 <= v2) ++passed;
                }
                if (it->second.type() == typeid(float) && v.type() == typeid(float)) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 <= v2) ++passed;
                }
                if (it->second.type() == typeid(QVector3D) && v.type() == typeid(QVector3D)) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 <= v2) ++passed;
                }
                break;
            }
            case ConstraintType::GT: {
                if (it->second.type() == typeid(int) && v.type() == typeid(int)) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 > v2) ++passed;
                }
                if (it->second.type() == typeid(float) && v.type() == typeid(float)) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 > v2) ++passed;
                }
                if (it->second.type() == typeid(QVector3D) && v.type() == typeid(QVector3D)) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
                    if (v1 > v2) ++passed;
                }
                break;
            }
            case ConstraintType::GTE: {
                if (it->second.type() == typeid(int) && v.type() == typeid(int)) {
                    auto v1 = get_value<int>(v);
                    auto v2 = get_value<int>(it->second);
                    if (v1 >= v2) ++passed;
                }
                if (it->second.type() == typeid(float) && v.type() == typeid(float)) {
                    auto v1 = get_value<float>(v);
                    auto v2 = get_value<float>(it->second);
                    if (v1 >= v2) ++passed;
                }
                if (it->second.type() == typeid(QVector3D) && v.type() == typeid(QVector3D)) {
                    auto v1 = get_value<QVector3D>(v);
                    auto v2 = get_value<QVector3D>(it->second);
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

    inline std::optional<float> convert_to(Unit::Type type) {
        if (value.type() == typeid(int) || value.type() == typeid(float))
            return unit.to(type, get_value<float>(value).value());

        return std::nullopt;
    };

private:
    Unit unit;
    QString name;
    std::any value;
    std::unordered_map<ConstraintType, std::any> constraints;
};

#endif // METRIC_H
