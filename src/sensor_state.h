#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

// Hardware-independent state, shared by the firmware and host-side tests.
class SensorState {
public:
    static constexpr std::size_t capacity = 50;

    bool record(float temperatureC, float humidity, std::uint32_t now) {
        valid_ = std::isfinite(temperatureC) && std::isfinite(humidity);
        if (!valid_) {
            return false;
        }
        temperatureC_ = temperatureC;
        humidity_ = humidity;
        lastSuccessMs_ = now;
        // Keep the last 50 successful samples, not a fixed time window.
        temperatures_[nextSampleIndex_] = temperatureC;
        nextSampleIndex_ = (nextSampleIndex_ + 1) % capacity;
        if (sampleCount_ < capacity) {
            ++sampleCount_;
        }
        return true;
    }

    bool valid() const { return valid_; }
    std::size_t sampleCount() const { return sampleCount_; }
    float temperatureC() const { return valid_ ? temperatureC_ : unavailable(); }
    float humidity() const { return valid_ ? humidity_ : unavailable(); }

    float averageTemperatureC() const {
        // Retain history across failures, but do not present it as current data.
        if (!valid_ || sampleCount_ == 0) {
            return unavailable();
        }
        float sum = 0.0f;
        for (std::size_t i = 0; i < sampleCount_; ++i) {
            sum += temperatures_[i];
        }
        return sum / static_cast<float>(sampleCount_);
    }

    std::uint32_t lastSampleAgeMs(std::uint32_t now) const {
        return now - lastSuccessMs_;
    }

private:
    static float unavailable() { return std::numeric_limits<float>::quiet_NaN(); }

    std::array<float, capacity> temperatures_{};
    std::size_t nextSampleIndex_ = 0;
    std::size_t sampleCount_ = 0;
    float temperatureC_ = 0.0f;
    float humidity_ = 0.0f;
    std::uint32_t lastSuccessMs_ = 0;
    bool valid_ = false;
};
