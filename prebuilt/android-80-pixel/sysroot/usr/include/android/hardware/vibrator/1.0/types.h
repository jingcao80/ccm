#ifndef HIDL_GENERATED_ANDROID_HARDWARE_VIBRATOR_V1_0_TYPES_H
#define HIDL_GENERATED_ANDROID_HARDWARE_VIBRATOR_V1_0_TYPES_H

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace vibrator {
namespace V1_0 {

enum class Status : uint32_t {
    OK = 0u, // 0
    UNKNOWN_ERROR = 1u, // 1
    BAD_VALUE = 2u, // 2
    UNSUPPORTED_OPERATION = 3u, // 3
};

enum class Effect : uint32_t {
    CLICK = 0u, // 0
    DOUBLE_CLICK = 1u, // 1
};

enum class EffectStrength : uint8_t {
    LIGHT = 0,
    MEDIUM = 1,
    STRONG = 2,
};

constexpr uint32_t operator|(const Status lhs, const Status rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const Status rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const Status lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const Status lhs, const Status rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const Status rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const Status lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const Status e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const Status e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<Status>(uint32_t o);

std::string toString(Status o);

constexpr uint32_t operator|(const Effect lhs, const Effect rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const Effect rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const Effect lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const Effect lhs, const Effect rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const Effect rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const Effect lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const Effect e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const Effect e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<Effect>(uint32_t o);

std::string toString(Effect o);

constexpr uint8_t operator|(const EffectStrength lhs, const EffectStrength rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

constexpr uint8_t operator|(const uint8_t lhs, const EffectStrength rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}

constexpr uint8_t operator|(const EffectStrength lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}

constexpr uint8_t operator&(const EffectStrength lhs, const EffectStrength rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

constexpr uint8_t operator&(const uint8_t lhs, const EffectStrength rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}

constexpr uint8_t operator&(const EffectStrength lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}

constexpr uint8_t &operator|=(uint8_t& v, const EffectStrength e) {
    v |= static_cast<uint8_t>(e);
    return v;
}

constexpr uint8_t &operator&=(uint8_t& v, const EffectStrength e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
std::string toString(uint8_t o);
template<>
std::string toString<EffectStrength>(uint8_t o);

std::string toString(EffectStrength o);


}  // namespace V1_0
}  // namespace vibrator
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_VIBRATOR_V1_0_TYPES_H
