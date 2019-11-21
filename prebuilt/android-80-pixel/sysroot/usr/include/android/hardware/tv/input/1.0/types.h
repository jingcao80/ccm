#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TV_INPUT_V1_0_TYPES_H
#define HIDL_GENERATED_ANDROID_HARDWARE_TV_INPUT_V1_0_TYPES_H

#include <android/hardware/audio/common/2.0/types.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace tv {
namespace input {
namespace V1_0 {

enum class Result : int32_t {
    OK = 0,
    UNKNOWN = 1,
    NO_RESOURCE = 2,
    INVALID_ARGUMENTS = 3,
    INVALID_STATE = 4,
};

enum class TvInputType : int32_t {
    OTHER = 1,
    TUNER = 2,
    COMPOSITE = 3,
    SVIDEO = 4,
    SCART = 5,
    COMPONENT = 6,
    VGA = 7,
    DVI = 8,
    HDMI = 9,
    DISPLAY_PORT = 10,
};

enum class CableConnectionStatus : int32_t {
    UNKNOWN = 0,
    CONNECTED = 1,
    DISCONNECTED = 2,
};

struct TvInputDeviceInfo final {
    int32_t deviceId __attribute__ ((aligned(4)));
    TvInputType type __attribute__ ((aligned(4)));
    uint32_t portId __attribute__ ((aligned(4)));
    CableConnectionStatus cableConnectionStatus __attribute__ ((aligned(4)));
    ::android::hardware::audio::common::V2_0::AudioDevice audioType __attribute__ ((aligned(4)));
    ::android::hardware::hidl_array<uint8_t, 32> audioAddress __attribute__ ((aligned(1)));
};

static_assert(offsetof(TvInputDeviceInfo, deviceId) == 0, "wrong offset");
static_assert(offsetof(TvInputDeviceInfo, type) == 4, "wrong offset");
static_assert(offsetof(TvInputDeviceInfo, portId) == 8, "wrong offset");
static_assert(offsetof(TvInputDeviceInfo, cableConnectionStatus) == 12, "wrong offset");
static_assert(offsetof(TvInputDeviceInfo, audioType) == 16, "wrong offset");
static_assert(offsetof(TvInputDeviceInfo, audioAddress) == 20, "wrong offset");
static_assert(sizeof(TvInputDeviceInfo) == 52, "wrong size");
static_assert(__alignof(TvInputDeviceInfo) == 4, "wrong alignment");

enum class TvInputEventType : int32_t {
    DEVICE_AVAILABLE = 1,
    DEVICE_UNAVAILABLE = 2,
    STREAM_CONFIGURATIONS_CHANGED = 3,
};

struct TvInputEvent final {
    TvInputEventType type __attribute__ ((aligned(4)));
    TvInputDeviceInfo deviceInfo __attribute__ ((aligned(4)));
};

static_assert(offsetof(TvInputEvent, type) == 0, "wrong offset");
static_assert(offsetof(TvInputEvent, deviceInfo) == 4, "wrong offset");
static_assert(sizeof(TvInputEvent) == 56, "wrong size");
static_assert(__alignof(TvInputEvent) == 4, "wrong alignment");

struct TvStreamConfig final {
    int32_t streamId __attribute__ ((aligned(4)));
    uint32_t maxVideoWidth __attribute__ ((aligned(4)));
    uint32_t maxVideoHeight __attribute__ ((aligned(4)));
};

static_assert(offsetof(TvStreamConfig, streamId) == 0, "wrong offset");
static_assert(offsetof(TvStreamConfig, maxVideoWidth) == 4, "wrong offset");
static_assert(offsetof(TvStreamConfig, maxVideoHeight) == 8, "wrong offset");
static_assert(sizeof(TvStreamConfig) == 12, "wrong size");
static_assert(__alignof(TvStreamConfig) == 4, "wrong alignment");

constexpr int32_t operator|(const Result lhs, const Result rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const Result rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const Result lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const Result lhs, const Result rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const Result rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const Result lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const Result e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const Result e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<Result>(int32_t o);

std::string toString(Result o);

constexpr int32_t operator|(const TvInputType lhs, const TvInputType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const TvInputType rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const TvInputType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const TvInputType lhs, const TvInputType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const TvInputType rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const TvInputType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const TvInputType e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const TvInputType e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<TvInputType>(int32_t o);

std::string toString(TvInputType o);

constexpr int32_t operator|(const CableConnectionStatus lhs, const CableConnectionStatus rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const CableConnectionStatus rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const CableConnectionStatus lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const CableConnectionStatus lhs, const CableConnectionStatus rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const CableConnectionStatus rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const CableConnectionStatus lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const CableConnectionStatus e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const CableConnectionStatus e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<CableConnectionStatus>(int32_t o);

std::string toString(CableConnectionStatus o);

std::string toString(const TvInputDeviceInfo&);

bool operator==(const TvInputDeviceInfo&, const TvInputDeviceInfo&);

bool operator!=(const TvInputDeviceInfo&, const TvInputDeviceInfo&);

constexpr int32_t operator|(const TvInputEventType lhs, const TvInputEventType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const TvInputEventType rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const TvInputEventType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const TvInputEventType lhs, const TvInputEventType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const TvInputEventType rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const TvInputEventType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const TvInputEventType e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const TvInputEventType e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<TvInputEventType>(int32_t o);

std::string toString(TvInputEventType o);

std::string toString(const TvInputEvent&);

bool operator==(const TvInputEvent&, const TvInputEvent&);

bool operator!=(const TvInputEvent&, const TvInputEvent&);

std::string toString(const TvStreamConfig&);

bool operator==(const TvStreamConfig&, const TvStreamConfig&);

bool operator!=(const TvStreamConfig&, const TvStreamConfig&);


}  // namespace V1_0
}  // namespace input
}  // namespace tv
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TV_INPUT_V1_0_TYPES_H
