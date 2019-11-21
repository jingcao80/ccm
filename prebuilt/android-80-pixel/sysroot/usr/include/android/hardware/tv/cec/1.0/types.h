#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TV_CEC_V1_0_TYPES_H
#define HIDL_GENERATED_ANDROID_HARDWARE_TV_CEC_V1_0_TYPES_H

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace tv {
namespace cec {
namespace V1_0 {

enum class MaxLength : int32_t {
    MESSAGE_BODY = 15,
};

enum class CecDeviceType : int32_t {
    INACTIVE = -1, // (-1)
    TV = 0,
    RECORDER = 1,
    TUNER = 3,
    PLAYBACK = 4,
    AUDIO_SYSTEM = 5,
    MAX = 5, // AUDIO_SYSTEM
};

enum class CecLogicalAddress : int32_t {
    TV = 0,
    RECORDER_1 = 1,
    RECORDER_2 = 2,
    TUNER_1 = 3,
    PLAYBACK_1 = 4,
    AUDIO_SYSTEM = 5,
    TUNER_2 = 6,
    TUNER_3 = 7,
    PLAYBACK_2 = 8,
    RECORDER_3 = 9,
    TUNER_4 = 10,
    PLAYBACK_3 = 11,
    FREE_USE = 14,
    UNREGISTERED = 15,
    BROADCAST = 15,
};

enum class CecMessageType : int32_t {
    FEATURE_ABORT = 0, // 0x00
    IMAGE_VIEW_ON = 4, // 0x04
    TUNER_STEP_INCREMENT = 5, // 0x05
    TUNER_STEP_DECREMENT = 6, // 0x06
    TUNER_DEVICE_STATUS = 7, // 0x07
    GIVE_TUNER_DEVICE_STATUS = 8, // 0x08
    RECORD_ON = 9, // 0x09
    RECORD_STATUS = 10, // 0x0A
    RECORD_OFF = 11, // 0x0B
    TEXT_VIEW_ON = 13, // 0x0D
    RECORD_TV_SCREEN = 15, // 0x0F
    GIVE_DECK_STATUS = 26, // 0x1A
    DECK_STATUS = 27, // 0x1B
    SET_MENU_LANGUAGE = 50, // 0x32
    CLEAR_ANALOG_TIMER = 51, // 0x33
    SET_ANALOG_TIMER = 52, // 0x34
    TIMER_STATUS = 53, // 0x35
    STANDBY = 54, // 0x36
    PLAY = 65, // 0x41
    DECK_CONTROL = 66, // 0x42
    TIMER_CLEARED_STATUS = 67, // 0x43
    USER_CONTROL_PRESSED = 68, // 0x44
    USER_CONTROL_RELEASED = 69, // 0x45
    GIVE_OSD_NAME = 70, // 0x46
    SET_OSD_NAME = 71, // 0x47
    SET_OSD_STRING = 100, // 0x64
    SET_TIMER_PROGRAM_TITLE = 103, // 0x67
    SYSTEM_AUDIO_MODE_REQUEST = 112, // 0x70
    GIVE_AUDIO_STATUS = 113, // 0x71
    SET_SYSTEM_AUDIO_MODE = 114, // 0x72
    REPORT_AUDIO_STATUS = 122, // 0x7A
    GIVE_SYSTEM_AUDIO_MODE_STATUS = 125, // 0x7D
    SYSTEM_AUDIO_MODE_STATUS = 126, // 0x7E
    ROUTING_CHANGE = 128, // 0x80
    ROUTING_INFORMATION = 129, // 0x81
    ACTIVE_SOURCE = 130, // 0x82
    GIVE_PHYSICAL_ADDRESS = 131, // 0x83
    REPORT_PHYSICAL_ADDRESS = 132, // 0x84
    REQUEST_ACTIVE_SOURCE = 133, // 0x85
    SET_STREAM_PATH = 134, // 0x86
    DEVICE_VENDOR_ID = 135, // 0x87
    VENDOR_COMMAND = 137, // 0x89
    VENDOR_REMOTE_BUTTON_DOWN = 138, // 0x8A
    VENDOR_REMOTE_BUTTON_UP = 139, // 0x8B
    GIVE_DEVICE_VENDOR_ID = 140, // 0x8C
    MENU_REQUEST = 141, // 0x8D
    MENU_STATUS = 142, // 0x8E
    GIVE_DEVICE_POWER_STATUS = 143, // 0x8F
    REPORT_POWER_STATUS = 144, // 0x90
    GET_MENU_LANGUAGE = 145, // 0x91
    SELECT_ANALOG_SERVICE = 146, // 0x92
    SELECT_DIGITAL_SERVICE = 147, // 0x93
    SET_DIGITAL_TIMER = 151, // 0x97
    CLEAR_DIGITAL_TIMER = 153, // 0x99
    SET_AUDIO_RATE = 154, // 0x9A
    INACTIVE_SOURCE = 157, // 0x9D
    CEC_VERSION = 158, // 0x9E
    GET_CEC_VERSION = 159, // 0x9F
    VENDOR_COMMAND_WITH_ID = 160, // 0xA0
    CLEAR_EXTERNAL_TIMER = 161, // 0xA1
    SET_EXTERNAL_TIMER = 162, // 0xA2
    INITIATE_ARC = 192, // 0xC0
    REPORT_ARC_INITIATED = 193, // 0xC1
    REPORT_ARC_TERMINATED = 194, // 0xC2
    REQUEST_ARC_INITIATION = 195, // 0xC3
    REQUEST_ARC_TERMINATION = 196, // 0xC4
    TERMINATE_ARC = 197, // 0xC5
    ABORT = 255, // 0xFF
};

enum class AbortReason : int32_t {
    UNRECOGNIZED_MODE = 0,
    NOT_IN_CORRECT_MODE = 1,
    CANNOT_PROVIDE_SOURCE = 2,
    INVALID_OPERAND = 3,
    REFUSED = 4,
    UNABLE_TO_DETERMINE = 5,
};

enum class Result : int32_t {
    SUCCESS = 0,
    FAILURE_UNKNOWN = 1,
    FAILURE_INVALID_ARGS = 2,
    FAILURE_INVALID_STATE = 3,
    FAILURE_NOT_SUPPORTED = 4,
    FAILURE_BUSY = 5,
};

enum class SendMessageResult : int32_t {
    SUCCESS = 0,
    NACK = 1,
    BUSY = 2,
    FAIL = 3,
};

enum class HdmiPortType : int32_t {
    INPUT = 0,
    OUTPUT = 1,
};

enum class OptionKey : int32_t {
    WAKEUP = 1,
    ENABLE_CEC = 2,
    SYSTEM_CEC_CONTROL = 3,
};

struct CecMessage final {
    CecLogicalAddress initiator __attribute__ ((aligned(4)));
    CecLogicalAddress destination __attribute__ ((aligned(4)));
    ::android::hardware::hidl_vec<uint8_t> body __attribute__ ((aligned(8)));
};

static_assert(offsetof(CecMessage, initiator) == 0, "wrong offset");
static_assert(offsetof(CecMessage, destination) == 4, "wrong offset");
static_assert(offsetof(CecMessage, body) == 8, "wrong offset");
static_assert(sizeof(CecMessage) == 24, "wrong size");
static_assert(__alignof(CecMessage) == 8, "wrong alignment");

struct HotplugEvent final {
    bool connected __attribute__ ((aligned(1)));
    uint32_t portId __attribute__ ((aligned(4)));
};

static_assert(offsetof(HotplugEvent, connected) == 0, "wrong offset");
static_assert(offsetof(HotplugEvent, portId) == 4, "wrong offset");
static_assert(sizeof(HotplugEvent) == 8, "wrong size");
static_assert(__alignof(HotplugEvent) == 4, "wrong alignment");

struct HdmiPortInfo final {
    HdmiPortType type __attribute__ ((aligned(4)));
    uint32_t portId __attribute__ ((aligned(4)));
    bool cecSupported __attribute__ ((aligned(1)));
    bool arcSupported __attribute__ ((aligned(1)));
    uint16_t physicalAddress __attribute__ ((aligned(2)));
};

static_assert(offsetof(HdmiPortInfo, type) == 0, "wrong offset");
static_assert(offsetof(HdmiPortInfo, portId) == 4, "wrong offset");
static_assert(offsetof(HdmiPortInfo, cecSupported) == 8, "wrong offset");
static_assert(offsetof(HdmiPortInfo, arcSupported) == 9, "wrong offset");
static_assert(offsetof(HdmiPortInfo, physicalAddress) == 10, "wrong offset");
static_assert(sizeof(HdmiPortInfo) == 12, "wrong size");
static_assert(__alignof(HdmiPortInfo) == 4, "wrong alignment");

constexpr int32_t operator|(const MaxLength lhs, const MaxLength rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const MaxLength rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const MaxLength lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const MaxLength lhs, const MaxLength rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const MaxLength rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const MaxLength lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const MaxLength e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const MaxLength e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<MaxLength>(int32_t o);

std::string toString(MaxLength o);

constexpr int32_t operator|(const CecDeviceType lhs, const CecDeviceType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const CecDeviceType rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const CecDeviceType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const CecDeviceType lhs, const CecDeviceType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const CecDeviceType rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const CecDeviceType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const CecDeviceType e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const CecDeviceType e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<CecDeviceType>(int32_t o);

std::string toString(CecDeviceType o);

constexpr int32_t operator|(const CecLogicalAddress lhs, const CecLogicalAddress rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const CecLogicalAddress rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const CecLogicalAddress lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const CecLogicalAddress lhs, const CecLogicalAddress rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const CecLogicalAddress rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const CecLogicalAddress lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const CecLogicalAddress e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const CecLogicalAddress e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<CecLogicalAddress>(int32_t o);

std::string toString(CecLogicalAddress o);

constexpr int32_t operator|(const CecMessageType lhs, const CecMessageType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const CecMessageType rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const CecMessageType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const CecMessageType lhs, const CecMessageType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const CecMessageType rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const CecMessageType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const CecMessageType e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const CecMessageType e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<CecMessageType>(int32_t o);

std::string toString(CecMessageType o);

constexpr int32_t operator|(const AbortReason lhs, const AbortReason rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AbortReason rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AbortReason lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AbortReason lhs, const AbortReason rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AbortReason rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AbortReason lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AbortReason e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AbortReason e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AbortReason>(int32_t o);

std::string toString(AbortReason o);

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

constexpr int32_t operator|(const SendMessageResult lhs, const SendMessageResult rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const SendMessageResult rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const SendMessageResult lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const SendMessageResult lhs, const SendMessageResult rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const SendMessageResult rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const SendMessageResult lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const SendMessageResult e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const SendMessageResult e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<SendMessageResult>(int32_t o);

std::string toString(SendMessageResult o);

constexpr int32_t operator|(const HdmiPortType lhs, const HdmiPortType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const HdmiPortType rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const HdmiPortType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const HdmiPortType lhs, const HdmiPortType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const HdmiPortType rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const HdmiPortType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const HdmiPortType e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const HdmiPortType e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<HdmiPortType>(int32_t o);

std::string toString(HdmiPortType o);

constexpr int32_t operator|(const OptionKey lhs, const OptionKey rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const OptionKey rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const OptionKey lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const OptionKey lhs, const OptionKey rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const OptionKey rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const OptionKey lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const OptionKey e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const OptionKey e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<OptionKey>(int32_t o);

std::string toString(OptionKey o);

std::string toString(const CecMessage&);

bool operator==(const CecMessage&, const CecMessage&);

bool operator!=(const CecMessage&, const CecMessage&);

std::string toString(const HotplugEvent&);

bool operator==(const HotplugEvent&, const HotplugEvent&);

bool operator!=(const HotplugEvent&, const HotplugEvent&);

std::string toString(const HdmiPortInfo&);

bool operator==(const HdmiPortInfo&, const HdmiPortInfo&);

bool operator!=(const HdmiPortInfo&, const HdmiPortInfo&);


}  // namespace V1_0
}  // namespace cec
}  // namespace tv
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TV_CEC_V1_0_TYPES_H
