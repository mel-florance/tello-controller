#include "../include/telemetry.h"

Telemetry::Telemetry() :
    initialized(false)
{
}

void Telemetry::start_record(const std::string &filename)
{
    if (!fs::exists(filename)) {
        file = std::ofstream(filename);
        file << "time;speed_len;speed_x;speed_y;speed_z;height;tof;pressure;battery;flight_duration;temp_min;temp_max;imu_length;imu_yaw;imu_pitch;imu_roll;accel_len;accel_x;accel_y;accel_z\n";
        initialized = true;
        qDebug() << "Started recording flight metrics.";
    }
}

void Telemetry::record(const FlightState &state)
{
    if (!initialized)
        return;

    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    file << timestamp << ';';
    file << state.speed.length() << ';';
    file << state.speed.x() << ';';
    file << state.speed.y() << ';';
    file << state.speed.z() << ';';
    file << state.height << ';';
    file << state.tof << ';';
    file << state.barometer << ';';
    file << state.battery << ';';
    file << state.flight_duration << ';';
    file << state.temperature.x() << ';';
    file << state.temperature.y() << ';';
    file << state.imu.length() << ';';;
    file << state.imu.x() << ';';
    file << state.imu.y() << ';';
    file << state.imu.z() << ';';
    file << state.acceleration.length() << ';';
    file << state.acceleration.x() << ';';
    file << state.acceleration.y() << ';';
    file << state.acceleration.z();
    file << '\n';
}

void Telemetry::stop_record()
{
    if (!initialized)
        return;

    qDebug() << "Stopped recording flight metrics.";

    file.close();
}

std::optional<std::pair<Telemetry::MetricList::iterator, bool>> Telemetry::create_metric(const char *name, const std::any &value, Unit::Type unit)
{
    auto it = metrics.find(name);

    if (it == metrics.end()) {
       Metric metric(name);
       metric.set_value(value);
       metric.set_unit(unit);
       return metrics.insert(std::make_pair(name, metric));
    }

    return std::nullopt;
};


void Telemetry::set_metric(const char* name, const Metric& metric)
{
    metrics.insert(std::make_pair(name, metric));
}

void Telemetry::set_metric_value(const char* name, const std::any &value)
{
    auto it = metrics.find(name);

    if (it != metrics.end())
        it->second.set_value(value);
}

void Telemetry::set_metric_unit(const char* name, Unit::Type unit)
{
    auto it = metrics.find(name);

    if (it != metrics.end())
        it->second.set_unit(unit);
};

void Telemetry::print_metrics()
{
   auto debug = qDebug();
   debug.noquote();

   debug << "Metrics:\n";

   for (auto& metric : metrics)
       debug << metric.second << '\n';
}

void Telemetry::serialize(const std::string &filename)
{
    if (!initialized)
        return;

    std::vector<std::string> headers = {"time"};

    for (auto it = metrics.begin(); it != metrics.end(); ++it)
        if (std::find(headers.begin(), headers.end(), it->first) == headers.end())
            headers.push_back(it->first);

    const auto now = std::chrono::system_clock::now();
    const auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

    file << timestamp;

    if (metrics.size() > 0)
        file << ',';

    for (auto it = metrics.begin(); it != metrics.end(); ++it) {
        auto str = QString(it->second);
        file << str.toStdString();

        if (metrics.count(it->first) != 0)
            file << ',';
    }

    file.close();
}
