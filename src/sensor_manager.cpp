#include <Arduino.h>
#include <DHT.h>

#include "sensor_manager.h"
#include "sensor_state.h"

namespace {
constexpr int sensorPin = 27;
// A ten-second interval avoids unnecessary reads and defines the history cadence.
constexpr unsigned long sensorIntervalMs = 10000;
DHT sensor(sensorPin, DHT22);
SensorState readings;
unsigned long lastSensorReadMs = 0;
}  // namespace

void initSensor() {
    sensor.begin();
    lastSensorReadMs = millis();
}

bool handleSensor(unsigned long now) {
    if (now - lastSensorReadMs < sensorIntervalMs) {
        return false;
    }
    lastSensorReadMs = now;
    const float humidity = sensor.readHumidity();
    const float temperatureC = sensor.readTemperature();
    if (!readings.record(temperatureC, humidity, millis())) {
        Serial.println("Sensor read failed; check sensor power and wiring.");
    }
    return true;
}

bool hasValidSensorReading() { return readings.valid(); }
float getTemperature() { return readings.temperatureC(); }
float getHumidity() { return readings.humidity(); }
float getAverageTemp() { return readings.averageTemperatureC(); }
std::size_t getSampleCount() { return readings.sampleCount(); }
std::uint32_t getLastSampleAgeMs() { return readings.lastSampleAgeMs(millis()); }
