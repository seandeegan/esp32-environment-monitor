#include "../src/sensor_state.h"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>

void check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
        std::exit(1);
    }
}

void near(float actual, float expected, const char* message) {
    check(std::isfinite(actual) && std::fabs(actual - expected) < 0.001f, message);
}

int main() {
    SensorState empty;
    check(!empty.valid() && empty.sampleCount() == 0, "starts without a reading");
    check(std::isnan(empty.temperatureC()) && std::isnan(empty.humidity()) &&
          std::isnan(empty.averageTemperatureC()), "empty readings are unavailable");

    SensorState partial;
    check(partial.record(-10.0f, 48.2f, 1000), "accepts a negative temperature");
    near(partial.temperatureC(), -10.0f, "keeps Celsius");
    near(partial.humidity(), 48.2f, "keeps humidity");
    near(partial.averageTemperatureC(), -10.0f, "one-sample average");
    partial.record(20.0f, 50.0f, 2000);
    near(partial.averageTemperatureC(), 5.0f, "warmup uses only populated samples");

    SensorState wrapped;
    for (int i = 1; i <= 125; ++i) {
        wrapped.record(static_cast<float>(i), 50.0f, i * 10000u);
    }
    check(wrapped.sampleCount() == 50, "history capacity remains bounded");
    near(wrapped.averageTemperatureC(), 100.5f, "multiple wraps retain samples 76-125");

    const float nan = std::numeric_limits<float>::quiet_NaN();
    const float infinity = std::numeric_limits<float>::infinity();
    check(!wrapped.record(nan, 50.0f, 1260000), "rejects a failed temperature read");
    check(!wrapped.valid() && std::isnan(wrapped.temperatureC()) &&
          std::isnan(wrapped.humidity()) && std::isnan(wrapped.averageTemperatureC()),
          "failure hides readings and historical average");
    check(!wrapped.record(10.0f, nan, 1270000), "rejects failed humidity read");
    check(!wrapped.record(infinity, 50.0f, 1280000), "rejects infinite temperature");
    check(!wrapped.record(10.0f, infinity, 1290000), "rejects infinite humidity");
    check(wrapped.sampleCount() == 50, "failures do not add samples");
    check(wrapped.lastSampleAgeMs(1300000) == 50000, "failure preserves last success time");
    wrapped.record(126.0f, 51.0f, 1300000);
    near(wrapped.averageTemperatureC(), 101.5f, "recovery retains successful history");
    check(wrapped.valid() && wrapped.lastSampleAgeMs(1300123) == 123, "recovery updates freshness");

    SensorState rollover;
    rollover.record(20.0f, 50.0f, UINT32_MAX - 9);
    check(rollover.lastSampleAgeMs(15) == 25, "age handles timer rollover");

    std::cout << "PASS: startup, warmup, circular buffer, failures, recovery, timer rollover\n";
}
