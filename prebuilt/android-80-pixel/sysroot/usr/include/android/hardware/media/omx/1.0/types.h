#ifndef HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_TYPES_H
#define HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_TYPES_H

#include <android/hardware/media/1.0/types.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace media {
namespace omx {
namespace V1_0 {

typedef uint32_t BufferId;

enum class Status : int32_t {
    OK = 0,
    NO_ERROR = 0,
    NAME_NOT_FOUND = -2, // (-2)
    WOULD_BLOCK = -11, // (-11)
    NO_MEMORY = -12, // (-12)
    ALREADY_EXISTS = -17, // (-17)
    NO_INIT = -19, // (-19)
    BAD_VALUE = -22, // (-22)
    DEAD_OBJECT = -32, // (-32)
    INVALID_OPERATION = -38, // (-38)
    TIMED_OUT = -110, // (-110)
    ERROR_UNSUPPORTED = -1010, // (-1010)
    UNKNOWN_ERROR = -2147483648, // (-2147483648)
    BUFFER_NEEDS_REALLOCATION = 1, // 0x1
    RELEASE_ALL_BUFFERS = 2, // 0x2
};

struct Message final {
    enum class Type : uint32_t {
        EVENT = 0u, // 0
        EMPTY_BUFFER_DONE = 1u, // 1
        FILL_BUFFER_DONE = 2u, // 2
        FRAME_RENDERED = 3u, // 3
    };

    struct EventData final {
        uint32_t event __attribute__ ((aligned(4)));
        uint32_t data1 __attribute__ ((aligned(4)));
        uint32_t data2 __attribute__ ((aligned(4)));
        uint32_t data3 __attribute__ ((aligned(4)));
        uint32_t data4 __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(Message::EventData, event) == 0, "wrong offset");
    static_assert(offsetof(Message::EventData, data1) == 4, "wrong offset");
    static_assert(offsetof(Message::EventData, data2) == 8, "wrong offset");
    static_assert(offsetof(Message::EventData, data3) == 12, "wrong offset");
    static_assert(offsetof(Message::EventData, data4) == 16, "wrong offset");
    static_assert(sizeof(Message::EventData) == 20, "wrong size");
    static_assert(__alignof(Message::EventData) == 4, "wrong alignment");

    struct BufferData final {
        uint32_t buffer __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(Message::BufferData, buffer) == 0, "wrong offset");
    static_assert(sizeof(Message::BufferData) == 4, "wrong size");
    static_assert(__alignof(Message::BufferData) == 4, "wrong alignment");

    struct ExtendedBufferData final {
        uint32_t buffer __attribute__ ((aligned(4)));
        uint32_t rangeOffset __attribute__ ((aligned(4)));
        uint32_t rangeLength __attribute__ ((aligned(4)));
        uint32_t flags __attribute__ ((aligned(4)));
        uint64_t timestampUs __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(Message::ExtendedBufferData, buffer) == 0, "wrong offset");
    static_assert(offsetof(Message::ExtendedBufferData, rangeOffset) == 4, "wrong offset");
    static_assert(offsetof(Message::ExtendedBufferData, rangeLength) == 8, "wrong offset");
    static_assert(offsetof(Message::ExtendedBufferData, flags) == 12, "wrong offset");
    static_assert(offsetof(Message::ExtendedBufferData, timestampUs) == 16, "wrong offset");
    static_assert(sizeof(Message::ExtendedBufferData) == 24, "wrong size");
    static_assert(__alignof(Message::ExtendedBufferData) == 8, "wrong alignment");

    struct RenderData final {
        uint64_t timestampUs __attribute__ ((aligned(8)));
        int64_t systemTimeNs __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(Message::RenderData, timestampUs) == 0, "wrong offset");
    static_assert(offsetof(Message::RenderData, systemTimeNs) == 8, "wrong offset");
    static_assert(sizeof(Message::RenderData) == 16, "wrong size");
    static_assert(__alignof(Message::RenderData) == 8, "wrong alignment");

    union Data final {
        Message::EventData eventData __attribute__ ((aligned(4)));
        Message::BufferData bufferData __attribute__ ((aligned(4)));
        Message::ExtendedBufferData extendedBufferData __attribute__ ((aligned(8)));
        Message::RenderData renderData __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(Message::Data, eventData) == 0, "wrong offset");
    static_assert(offsetof(Message::Data, bufferData) == 0, "wrong offset");
    static_assert(offsetof(Message::Data, extendedBufferData) == 0, "wrong offset");
    static_assert(offsetof(Message::Data, renderData) == 0, "wrong offset");
    static_assert(sizeof(Message::Data) == 24, "wrong size");
    static_assert(__alignof(Message::Data) == 8, "wrong alignment");

    Message::Type type __attribute__ ((aligned(4)));
    ::android::hardware::hidl_handle fence __attribute__ ((aligned(8)));
    Message::Data data __attribute__ ((aligned(8)));
};

static_assert(offsetof(Message, type) == 0, "wrong offset");
static_assert(offsetof(Message, fence) == 8, "wrong offset");
static_assert(offsetof(Message, data) == 24, "wrong offset");
static_assert(sizeof(Message) == 48, "wrong size");
static_assert(__alignof(Message) == 8, "wrong alignment");

struct CodecBuffer final {
    enum class Type : int32_t {
        INVALID = 0,
        PRESET = 1,
        SHARED_MEM = 2,
        ANW_BUFFER = 3,
        NATIVE_HANDLE = 4,
    };

    struct PresetAttributes final {
        uint32_t rangeOffset __attribute__ ((aligned(4)));
        uint32_t rangeLength __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(CodecBuffer::PresetAttributes, rangeOffset) == 0, "wrong offset");
    static_assert(offsetof(CodecBuffer::PresetAttributes, rangeLength) == 4, "wrong offset");
    static_assert(sizeof(CodecBuffer::PresetAttributes) == 8, "wrong size");
    static_assert(__alignof(CodecBuffer::PresetAttributes) == 4, "wrong alignment");

    union Attributes final {
        CodecBuffer::PresetAttributes preset __attribute__ ((aligned(4)));
        ::android::hardware::media::V1_0::AnwBufferAttributes anwBuffer __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(CodecBuffer::Attributes, preset) == 0, "wrong offset");
    static_assert(offsetof(CodecBuffer::Attributes, anwBuffer) == 0, "wrong offset");
    static_assert(sizeof(CodecBuffer::Attributes) == 40, "wrong size");
    static_assert(__alignof(CodecBuffer::Attributes) == 8, "wrong alignment");

    CodecBuffer::Type type __attribute__ ((aligned(4)));
    CodecBuffer::Attributes attr __attribute__ ((aligned(8)));
    ::android::hardware::hidl_handle nativeHandle __attribute__ ((aligned(8)));
    ::android::hardware::hidl_memory sharedMemory __attribute__ ((aligned(8)));
};

static_assert(offsetof(CodecBuffer, type) == 0, "wrong offset");
static_assert(offsetof(CodecBuffer, attr) == 8, "wrong offset");
static_assert(offsetof(CodecBuffer, nativeHandle) == 48, "wrong offset");
static_assert(offsetof(CodecBuffer, sharedMemory) == 64, "wrong offset");
static_assert(sizeof(CodecBuffer) == 104, "wrong size");
static_assert(__alignof(CodecBuffer) == 8, "wrong alignment");

enum class PortMode : int32_t {
    PRESET_START = 0,
    PRESET_BYTE_BUFFER = 1,
    PRESET_ANW_BUFFER = 2,
    PRESET_SECURE_BUFFER = 3,
    PRESET_END = 4,
    DYNAMIC_START = 100,
    DYNAMIC_ANW_BUFFER = 101,
    DYNAMIC_NATIVE_HANDLE = 102,
    DYNAMIC_END = 103,
};

struct ColorAspects final {
    enum class Range : uint32_t {
        UNSPECIFIED = 0u, // 0
        FULL = 1u, // 1
        LIMITED = 2u, // 2
        OTHER = 255u, // 0xff
    };

    enum class Primaries : uint32_t {
        UNSPECIFIED = 0u, // 0
        BT709_5 = 1u, // 1
        BT470_6M = 2u, // 2
        BT601_6_625 = 3u, // 3
        BT601_6_525 = 4u, // 4
        GENERIC_FILM = 5u, // 5
        BT2020 = 6u, // 6
        OTHER = 255u, // 0xff
    };

    enum class Transfer : uint32_t {
        UNSPECIFIED = 0u, // 0
        LINEAR = 1u, // 1
        SRGB = 2u, // 2
        SMPTE170M = 3u, // 3
        GAMMA22 = 4u, // 4
        GAMMA28 = 5u, // 5
        ST2084 = 6u, // 6
        HLG = 7u, // 7
        SMPTE240M = 64u, // 0x40
        XVYCC = 65u, // 65
        BT1361 = 66u, // 66
        ST428 = 67u, // 67
        OTHER = 255u, // 0xff
    };

    enum class MatrixCoeffs : uint32_t {
        UNSPECIFIED = 0u, // 0
        BT709_5 = 1u, // 1
        BT470_6M = 2u, // 2
        BT601_6 = 3u, // 3
        SMPTE240M = 4u, // 4
        BT2020 = 5u, // 5
        BT2020CONSTANT = 6u, // 6
        OTHER = 255u, // 0xff
    };

    ColorAspects::Range range __attribute__ ((aligned(4)));
    ColorAspects::Primaries primaries __attribute__ ((aligned(4)));
    ColorAspects::Transfer transfer __attribute__ ((aligned(4)));
    ColorAspects::MatrixCoeffs matrixCoeffs __attribute__ ((aligned(4)));
};

static_assert(offsetof(ColorAspects, range) == 0, "wrong offset");
static_assert(offsetof(ColorAspects, primaries) == 4, "wrong offset");
static_assert(offsetof(ColorAspects, transfer) == 8, "wrong offset");
static_assert(offsetof(ColorAspects, matrixCoeffs) == 12, "wrong offset");
static_assert(sizeof(ColorAspects) == 16, "wrong size");
static_assert(__alignof(ColorAspects) == 4, "wrong alignment");

constexpr int32_t operator|(const Status lhs, const Status rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const Status rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const Status lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const Status lhs, const Status rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const Status rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const Status lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const Status e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const Status e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<Status>(int32_t o);

std::string toString(Status o);

constexpr uint32_t operator|(const Message::Type lhs, const Message::Type rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const Message::Type rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const Message::Type lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const Message::Type lhs, const Message::Type rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const Message::Type rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const Message::Type lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const Message::Type e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const Message::Type e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<Message::Type>(uint32_t o);

std::string toString(Message::Type o);

std::string toString(const Message::EventData&);

bool operator==(const Message::EventData&, const Message::EventData&);

bool operator!=(const Message::EventData&, const Message::EventData&);

std::string toString(const Message::BufferData&);

bool operator==(const Message::BufferData&, const Message::BufferData&);

bool operator!=(const Message::BufferData&, const Message::BufferData&);

std::string toString(const Message::ExtendedBufferData&);

bool operator==(const Message::ExtendedBufferData&, const Message::ExtendedBufferData&);

bool operator!=(const Message::ExtendedBufferData&, const Message::ExtendedBufferData&);

std::string toString(const Message::RenderData&);

bool operator==(const Message::RenderData&, const Message::RenderData&);

bool operator!=(const Message::RenderData&, const Message::RenderData&);

std::string toString(const Message::Data&);

// operator== and operator!= are not generated for Data

std::string toString(const Message&);

// operator== and operator!= are not generated for Message

constexpr int32_t operator|(const CodecBuffer::Type lhs, const CodecBuffer::Type rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const CodecBuffer::Type rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const CodecBuffer::Type lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const CodecBuffer::Type lhs, const CodecBuffer::Type rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const CodecBuffer::Type rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const CodecBuffer::Type lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const CodecBuffer::Type e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const CodecBuffer::Type e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<CodecBuffer::Type>(int32_t o);

std::string toString(CodecBuffer::Type o);

std::string toString(const CodecBuffer::PresetAttributes&);

bool operator==(const CodecBuffer::PresetAttributes&, const CodecBuffer::PresetAttributes&);

bool operator!=(const CodecBuffer::PresetAttributes&, const CodecBuffer::PresetAttributes&);

std::string toString(const CodecBuffer::Attributes&);

// operator== and operator!= are not generated for Attributes

std::string toString(const CodecBuffer&);

// operator== and operator!= are not generated for CodecBuffer

constexpr int32_t operator|(const PortMode lhs, const PortMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const PortMode rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const PortMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const PortMode lhs, const PortMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const PortMode rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const PortMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const PortMode e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const PortMode e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<PortMode>(int32_t o);

std::string toString(PortMode o);

constexpr uint32_t operator|(const ColorAspects::Range lhs, const ColorAspects::Range rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const ColorAspects::Range rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const ColorAspects::Range lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const ColorAspects::Range lhs, const ColorAspects::Range rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const ColorAspects::Range rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const ColorAspects::Range lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const ColorAspects::Range e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const ColorAspects::Range e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<ColorAspects::Range>(uint32_t o);

std::string toString(ColorAspects::Range o);

constexpr uint32_t operator|(const ColorAspects::Primaries lhs, const ColorAspects::Primaries rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const ColorAspects::Primaries rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const ColorAspects::Primaries lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const ColorAspects::Primaries lhs, const ColorAspects::Primaries rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const ColorAspects::Primaries rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const ColorAspects::Primaries lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const ColorAspects::Primaries e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const ColorAspects::Primaries e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<ColorAspects::Primaries>(uint32_t o);

std::string toString(ColorAspects::Primaries o);

constexpr uint32_t operator|(const ColorAspects::Transfer lhs, const ColorAspects::Transfer rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const ColorAspects::Transfer rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const ColorAspects::Transfer lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const ColorAspects::Transfer lhs, const ColorAspects::Transfer rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const ColorAspects::Transfer rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const ColorAspects::Transfer lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const ColorAspects::Transfer e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const ColorAspects::Transfer e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<ColorAspects::Transfer>(uint32_t o);

std::string toString(ColorAspects::Transfer o);

constexpr uint32_t operator|(const ColorAspects::MatrixCoeffs lhs, const ColorAspects::MatrixCoeffs rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const ColorAspects::MatrixCoeffs rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const ColorAspects::MatrixCoeffs lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const ColorAspects::MatrixCoeffs lhs, const ColorAspects::MatrixCoeffs rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const ColorAspects::MatrixCoeffs rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const ColorAspects::MatrixCoeffs lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const ColorAspects::MatrixCoeffs e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const ColorAspects::MatrixCoeffs e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<ColorAspects::MatrixCoeffs>(uint32_t o);

std::string toString(ColorAspects::MatrixCoeffs o);

std::string toString(const ColorAspects&);

bool operator==(const ColorAspects&, const ColorAspects&);

bool operator!=(const ColorAspects&, const ColorAspects&);


}  // namespace V1_0
}  // namespace omx
}  // namespace media
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_TYPES_H
