#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <any>
#include <optional>
#include <unordered_map>
#include <fstream>
#include <chrono>
#include <filesystem>

#include "../include/metric.h"
#include "../include/flightstate.h"

namespace fs = std::filesystem;

class Telemetry
{
public:
    Telemetry();
    typedef std::unordered_map<std::string, Metric> MetricList;

    void start_record(const std::string& filename = std::string());
    void record(const FlightState& flight_state);
    void stop_record();

    std::optional<std::pair<Telemetry::MetricList::iterator, bool>>
    create_metric(const char* name, const std::any& value, Unit::Type unit);

    template<typename T>
    inline std::optional<T> get_metric(const char* name) {
        auto metric = metrics.find(name);

        if (metric != metrics.end())
           return metric->second.get_value<T>().value();

        return std::nullopt;
    }

    void set_metric(const char* name, const Metric& metric);
    void set_metric_value(const char* name, const std::any& value);
    void set_metric_unit(const char* name, Unit::Type unit);
    void print_metrics();

    void serialize(const std::string& filename);

private:
    MetricList metrics;
    std::ofstream file;
    bool initialized;
};

#endif // TELEMETRY_H
