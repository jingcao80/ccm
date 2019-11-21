#ifndef HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_V1_0_TYPES_H
#define HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_V1_0_TYPES_H

#include <android/hardware/graphics/common/1.0/types.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace media {
namespace V1_0 {

typedef ::android::hardware::hidl_handle FileDescriptor;

typedef ::android::hardware::hidl_handle Fence;

typedef ::android::hardware::hidl_vec<uint8_t> Bytes;

struct AnwBufferAttributes final {
    uint32_t width __attribute__ ((aligned(4)));
    uint32_t height __attribute__ ((aligned(4)));
    uint32_t stride __attribute__ ((aligned(4)));
    ::android::hardware::graphics::common::V1_0::PixelFormat format __attribute__ ((aligned(4)));
    uint32_t usage __attribute__ ((aligned(4)));
    uint32_t generationNumber __attribute__ ((aligned(4)));
    uint64_t layerCount __attribute__ ((aligned(8)));
    uint64_t id __attribute__ ((aligned(8)));
};

static_assert(offsetof(AnwBufferAttributes, width) == 0, "wrong offset");
static_assert(offsetof(AnwBufferAttributes, height) == 4, "wrong offset");
static_assert(offsetof(AnwBufferAttributes, stride) == 8, "wrong offset");
static_assert(offsetof(AnwBufferAttributes, format) == 12, "wrong offset");
static_assert(offsetof(AnwBufferAttributes, usage) == 16, "wrong offset");
static_assert(offsetof(AnwBufferAttributes, generationNumber) == 20, "wrong offset");
static_assert(offsetof(AnwBufferAttributes, layerCount) == 24, "wrong offset");
static_assert(offsetof(AnwBufferAttributes, id) == 32, "wrong offset");
static_assert(sizeof(AnwBufferAttributes) == 40, "wrong size");
static_assert(__alignof(AnwBufferAttributes) == 8, "wrong alignment");

struct AnwBuffer final {
    ::android::hardware::hidl_handle nativeHandle __attribute__ ((aligned(8)));
    AnwBufferAttributes attr __attribute__ ((aligned(8)));
};

static_assert(offsetof(AnwBuffer, nativeHandle) == 0, "wrong offset");
static_assert(offsetof(AnwBuffer, attr) == 16, "wrong offset");
static_assert(sizeof(AnwBuffer) == 56, "wrong size");
static_assert(__alignof(AnwBuffer) == 8, "wrong alignment");

struct Rect final {
    int32_t left __attribute__ ((aligned(4)));
    int32_t top __attribute__ ((aligned(4)));
    int32_t right __attribute__ ((aligned(4)));
    int32_t bottom __attribute__ ((aligned(4)));
};

static_assert(offsetof(Rect, left) == 0, "wrong offset");
static_assert(offsetof(Rect, top) == 4, "wrong offset");
static_assert(offsetof(Rect, right) == 8, "wrong offset");
static_assert(offsetof(Rect, bottom) == 12, "wrong offset");
static_assert(sizeof(Rect) == 16, "wrong size");
static_assert(__alignof(Rect) == 4, "wrong alignment");

typedef ::android::hardware::hidl_vec<Rect> Region;

std::string toString(const AnwBufferAttributes&);

bool operator==(const AnwBufferAttributes&, const AnwBufferAttributes&);

bool operator!=(const AnwBufferAttributes&, const AnwBufferAttributes&);

std::string toString(const AnwBuffer&);

// operator== and operator!= are not generated for AnwBuffer

std::string toString(const Rect&);

bool operator==(const Rect&, const Rect&);

bool operator!=(const Rect&, const Rect&);


}  // namespace V1_0
}  // namespace media
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_V1_0_TYPES_H
