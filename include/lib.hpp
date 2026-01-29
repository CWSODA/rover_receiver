#pragma once

// il matto can't process floats
// must convert voltage to int
inline uint8_t v_to_uint8(float voltage) {
    return std::round(voltage / 3.3f * 255.0f);
}

inline float uint8_to_v(uint8_t num) { return num / 255.0f * 3.3f; }