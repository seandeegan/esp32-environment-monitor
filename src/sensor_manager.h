#pragma once

#include <cstddef>
#include <cstdint>

void initSensor();
// Returns true when a scheduled read has completed, including a failed read.
bool handleSensor(unsigned long now);
bool hasValidSensorReading();
float getTemperature();
float getHumidity();
float getAverageTemp();
std::size_t getSampleCount();
// Only meaningful after getSampleCount() becomes nonzero.
std::uint32_t getLastSampleAgeMs();
