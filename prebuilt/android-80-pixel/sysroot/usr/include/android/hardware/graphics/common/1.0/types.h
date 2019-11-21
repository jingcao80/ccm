#ifndef HIDL_GENERATED_ANDROID_HARDWARE_GRAPHICS_COMMON_V1_0_TYPES_H
#define HIDL_GENERATED_ANDROID_HARDWARE_GRAPHICS_COMMON_V1_0_TYPES_H

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace graphics {
namespace common {
namespace V1_0 {

enum class PixelFormat : int32_t {
    RGBA_8888 = 1, // 0x1
    RGBX_8888 = 2, // 0x2
    RGB_888 = 3, // 0x3
    RGB_565 = 4, // 0x4
    BGRA_8888 = 5, // 0x5
    YCBCR_422_SP = 16, // 0x10
    YCRCB_420_SP = 17, // 0x11
    YCBCR_422_I = 20, // 0x14
    RGBA_FP16 = 22, // 0x16
    RAW16 = 32, // 0x20
    BLOB = 33, // 0x21
    IMPLEMENTATION_DEFINED = 34, // 0x22
    YCBCR_420_888 = 35, // 0x23
    RAW_OPAQUE = 36, // 0x24
    RAW10 = 37, // 0x25
    RAW12 = 38, // 0x26
    RGBA_1010102 = 43, // 0x2B
    Y8 = 538982489, // 0x20203859
    Y16 = 540422489, // 0x20363159
    YV12 = 842094169, // 0x32315659
};

enum class BufferUsage : uint64_t {
    CPU_READ_MASK = 15ull, // 0xfULL
    CPU_READ_NEVER = 0ull, // 0
    CPU_READ_RARELY = 2ull, // 2
    CPU_READ_OFTEN = 3ull, // 3
    CPU_WRITE_MASK = 240ull, // (0xfULL << 4)
    CPU_WRITE_NEVER = 0ull, // (0 << 4)
    CPU_WRITE_RARELY = 32ull, // (2 << 4)
    CPU_WRITE_OFTEN = 48ull, // (3 << 4)
    GPU_TEXTURE = 256ull, // (1ULL << 8)
    GPU_RENDER_TARGET = 512ull, // (1ULL << 9)
    COMPOSER_OVERLAY = 2048ull, // (1ULL << 11)
    COMPOSER_CLIENT_TARGET = 4096ull, // (1ULL << 12)
    PROTECTED = 16384ull, // (1ULL << 14)
    COMPOSER_CURSOR = 32768ull, // (1ULL << 15)
    VIDEO_ENCODER = 65536ull, // (1ULL << 16)
    CAMERA_OUTPUT = 131072ull, // (1ULL << 17)
    CAMERA_INPUT = 262144ull, // (1ULL << 18)
    RENDERSCRIPT = 1048576ull, // (1ULL << 20)
    VIDEO_DECODER = 4194304ull, // (1ULL << 22)
    SENSOR_DIRECT_DATA = 8388608ull, // (1ULL << 23)
    GPU_DATA_BUFFER = 16777216ull, // (1ULL << 24)
    VENDOR_MASK = 4026531840ull, // (0xfULL << 28)
    VENDOR_MASK_HI = 18446462598732840960ull, // (0xffffULL << 48)
};

enum class Transform : int32_t {
    FLIP_H = 1, // (1 << 0)
    FLIP_V = 2, // (1 << 1)
    ROT_90 = 4, // (1 << 2)
    ROT_180 = 3, // (FLIP_H | FLIP_V)
    ROT_270 = 7, // ((FLIP_H | FLIP_V) | ROT_90)
};

enum class Dataspace : int32_t {
    UNKNOWN = 0, // 0x0
    ARBITRARY = 1, // 0x1
    STANDARD_SHIFT = 16,
    STANDARD_MASK = 4128768, // (63 << STANDARD_SHIFT)
    STANDARD_UNSPECIFIED = 0, // (0 << STANDARD_SHIFT)
    STANDARD_BT709 = 65536, // (1 << STANDARD_SHIFT)
    STANDARD_BT601_625 = 131072, // (2 << STANDARD_SHIFT)
    STANDARD_BT601_625_UNADJUSTED = 196608, // (3 << STANDARD_SHIFT)
    STANDARD_BT601_525 = 262144, // (4 << STANDARD_SHIFT)
    STANDARD_BT601_525_UNADJUSTED = 327680, // (5 << STANDARD_SHIFT)
    STANDARD_BT2020 = 393216, // (6 << STANDARD_SHIFT)
    STANDARD_BT2020_CONSTANT_LUMINANCE = 458752, // (7 << STANDARD_SHIFT)
    STANDARD_BT470M = 524288, // (8 << STANDARD_SHIFT)
    STANDARD_FILM = 589824, // (9 << STANDARD_SHIFT)
    STANDARD_DCI_P3 = 655360, // (10 << STANDARD_SHIFT)
    STANDARD_ADOBE_RGB = 720896, // (11 << STANDARD_SHIFT)
    TRANSFER_SHIFT = 22,
    TRANSFER_MASK = 130023424, // (31 << TRANSFER_SHIFT)
    TRANSFER_UNSPECIFIED = 0, // (0 << TRANSFER_SHIFT)
    TRANSFER_LINEAR = 4194304, // (1 << TRANSFER_SHIFT)
    TRANSFER_SRGB = 8388608, // (2 << TRANSFER_SHIFT)
    TRANSFER_SMPTE_170M = 12582912, // (3 << TRANSFER_SHIFT)
    TRANSFER_GAMMA2_2 = 16777216, // (4 << TRANSFER_SHIFT)
    TRANSFER_GAMMA2_6 = 20971520, // (5 << TRANSFER_SHIFT)
    TRANSFER_GAMMA2_8 = 25165824, // (6 << TRANSFER_SHIFT)
    TRANSFER_ST2084 = 29360128, // (7 << TRANSFER_SHIFT)
    TRANSFER_HLG = 33554432, // (8 << TRANSFER_SHIFT)
    RANGE_SHIFT = 27,
    RANGE_MASK = 939524096, // (7 << RANGE_SHIFT)
    RANGE_UNSPECIFIED = 0, // (0 << RANGE_SHIFT)
    RANGE_FULL = 134217728, // (1 << RANGE_SHIFT)
    RANGE_LIMITED = 268435456, // (2 << RANGE_SHIFT)
    RANGE_EXTENDED = 402653184, // (3 << RANGE_SHIFT)
    SRGB_LINEAR = 512, // 0x200
    V0_SRGB_LINEAR = 138477568, // ((STANDARD_BT709 | TRANSFER_LINEAR) | RANGE_FULL)
    V0_SCRGB_LINEAR = 406913024, // ((STANDARD_BT709 | TRANSFER_LINEAR) | RANGE_EXTENDED)
    SRGB = 513, // 0x201
    V0_SRGB = 142671872, // ((STANDARD_BT709 | TRANSFER_SRGB) | RANGE_FULL)
    V0_SCRGB = 411107328, // ((STANDARD_BT709 | TRANSFER_SRGB) | RANGE_EXTENDED)
    JFIF = 257, // 0x101
    V0_JFIF = 146931712, // ((STANDARD_BT601_625 | TRANSFER_SMPTE_170M) | RANGE_FULL)
    BT601_625 = 258, // 0x102
    V0_BT601_625 = 281149440, // ((STANDARD_BT601_625 | TRANSFER_SMPTE_170M) | RANGE_LIMITED)
    BT601_525 = 259, // 0x103
    V0_BT601_525 = 281280512, // ((STANDARD_BT601_525 | TRANSFER_SMPTE_170M) | RANGE_LIMITED)
    BT709 = 260, // 0x104
    V0_BT709 = 281083904, // ((STANDARD_BT709 | TRANSFER_SMPTE_170M) | RANGE_LIMITED)
    DCI_P3_LINEAR = 139067392, // ((STANDARD_DCI_P3 | TRANSFER_LINEAR) | RANGE_FULL)
    DCI_P3 = 155844608, // ((STANDARD_DCI_P3 | TRANSFER_GAMMA2_6) | RANGE_FULL)
    DISPLAY_P3_LINEAR = 139067392, // ((STANDARD_DCI_P3 | TRANSFER_LINEAR) | RANGE_FULL)
    DISPLAY_P3 = 143261696, // ((STANDARD_DCI_P3 | TRANSFER_SRGB) | RANGE_FULL)
    ADOBE_RGB = 151715840, // ((STANDARD_ADOBE_RGB | TRANSFER_GAMMA2_2) | RANGE_FULL)
    BT2020_LINEAR = 138805248, // ((STANDARD_BT2020 | TRANSFER_LINEAR) | RANGE_FULL)
    BT2020 = 147193856, // ((STANDARD_BT2020 | TRANSFER_SMPTE_170M) | RANGE_FULL)
    BT2020_PQ = 163971072, // ((STANDARD_BT2020 | TRANSFER_ST2084) | RANGE_FULL)
    DEPTH = 4096, // 0x1000
    SENSOR = 4097, // 0x1001
};

enum class ColorMode : int32_t {
    NATIVE = 0,
    STANDARD_BT601_625 = 1,
    STANDARD_BT601_625_UNADJUSTED = 2,
    STANDARD_BT601_525 = 3,
    STANDARD_BT601_525_UNADJUSTED = 4,
    STANDARD_BT709 = 5,
    DCI_P3 = 6,
    SRGB = 7,
    ADOBE_RGB = 8,
    DISPLAY_P3 = 9,
};

enum class ColorTransform : int32_t {
    IDENTITY = 0,
    ARBITRARY_MATRIX = 1,
    VALUE_INVERSE = 2,
    GRAYSCALE = 3,
    CORRECT_PROTANOPIA = 4,
    CORRECT_DEUTERANOPIA = 5,
    CORRECT_TRITANOPIA = 6,
};

enum class Hdr : int32_t {
    DOLBY_VISION = 1,
    HDR10 = 2,
    HLG = 3,
};

constexpr int32_t operator|(const PixelFormat lhs, const PixelFormat rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const PixelFormat rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const PixelFormat lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const PixelFormat lhs, const PixelFormat rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const PixelFormat rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const PixelFormat lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const PixelFormat e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const PixelFormat e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<PixelFormat>(int32_t o);

std::string toString(PixelFormat o);

constexpr uint64_t operator|(const BufferUsage lhs, const BufferUsage rhs) {
    return static_cast<uint64_t>(static_cast<uint64_t>(lhs) | static_cast<uint64_t>(rhs));
}

constexpr uint64_t operator|(const uint64_t lhs, const BufferUsage rhs) {
    return static_cast<uint64_t>(lhs | static_cast<uint64_t>(rhs));
}

constexpr uint64_t operator|(const BufferUsage lhs, const uint64_t rhs) {
    return static_cast<uint64_t>(static_cast<uint64_t>(lhs) | rhs);
}

constexpr uint64_t operator&(const BufferUsage lhs, const BufferUsage rhs) {
    return static_cast<uint64_t>(static_cast<uint64_t>(lhs) & static_cast<uint64_t>(rhs));
}

constexpr uint64_t operator&(const uint64_t lhs, const BufferUsage rhs) {
    return static_cast<uint64_t>(lhs & static_cast<uint64_t>(rhs));
}

constexpr uint64_t operator&(const BufferUsage lhs, const uint64_t rhs) {
    return static_cast<uint64_t>(static_cast<uint64_t>(lhs) & rhs);
}

constexpr uint64_t &operator|=(uint64_t& v, const BufferUsage e) {
    v |= static_cast<uint64_t>(e);
    return v;
}

constexpr uint64_t &operator&=(uint64_t& v, const BufferUsage e) {
    v &= static_cast<uint64_t>(e);
    return v;
}

template<typename>
std::string toString(uint64_t o);
template<>
std::string toString<BufferUsage>(uint64_t o);

std::string toString(BufferUsage o);

constexpr int32_t operator|(const Transform lhs, const Transform rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const Transform rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const Transform lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const Transform lhs, const Transform rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const Transform rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const Transform lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const Transform e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const Transform e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<Transform>(int32_t o);

std::string toString(Transform o);

constexpr int32_t operator|(const Dataspace lhs, const Dataspace rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const Dataspace rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const Dataspace lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const Dataspace lhs, const Dataspace rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const Dataspace rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const Dataspace lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const Dataspace e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const Dataspace e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<Dataspace>(int32_t o);

std::string toString(Dataspace o);

constexpr int32_t operator|(const ColorMode lhs, const ColorMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const ColorMode rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const ColorMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const ColorMode lhs, const ColorMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const ColorMode rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const ColorMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const ColorMode e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const ColorMode e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<ColorMode>(int32_t o);

std::string toString(ColorMode o);

constexpr int32_t operator|(const ColorTransform lhs, const ColorTransform rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const ColorTransform rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const ColorTransform lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const ColorTransform lhs, const ColorTransform rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const ColorTransform rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const ColorTransform lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const ColorTransform e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const ColorTransform e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<ColorTransform>(int32_t o);

std::string toString(ColorTransform o);

constexpr int32_t operator|(const Hdr lhs, const Hdr rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const Hdr rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const Hdr lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const Hdr lhs, const Hdr rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const Hdr rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const Hdr lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const Hdr e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const Hdr e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<Hdr>(int32_t o);

std::string toString(Hdr o);


}  // namespace V1_0
}  // namespace common
}  // namespace graphics
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_GRAPHICS_COMMON_V1_0_TYPES_H
