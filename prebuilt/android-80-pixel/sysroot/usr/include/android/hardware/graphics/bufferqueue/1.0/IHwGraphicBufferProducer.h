#ifndef HIDL_GENERATED_ANDROID_HARDWARE_GRAPHICS_BUFFERQUEUE_V1_0_IHWGRAPHICBUFFERPRODUCER_H
#define HIDL_GENERATED_ANDROID_HARDWARE_GRAPHICS_BUFFERQUEUE_V1_0_IHWGRAPHICBUFFERPRODUCER_H

#include <android/hardware/graphics/bufferqueue/1.0/IGraphicBufferProducer.h>

#include <android/hardware/graphics/bufferqueue/1.0/BnHwProducerListener.h>
#include <android/hardware/graphics/bufferqueue/1.0/BpHwProducerListener.h>
#include <android/hardware/graphics/common/1.0/hwtypes.h>
#include <android/hardware/media/1.0/hwtypes.h>
#include <android/hidl/base/1.0/BnHwBase.h>
#include <android/hidl/base/1.0/BpHwBase.h>

#include <hidl/Status.h>
#include <hwbinder/IBinder.h>
#include <hwbinder/Parcel.h>

namespace android {
namespace hardware {
namespace graphics {
namespace bufferqueue {
namespace V1_0 {
::android::status_t readEmbeddedFromParcel(
        const IGraphicBufferProducer::FenceTimeSnapshot &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t writeEmbeddedToParcel(
        const IGraphicBufferProducer::FenceTimeSnapshot &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t readEmbeddedFromParcel(
        const IGraphicBufferProducer::FrameEventsDelta &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t writeEmbeddedToParcel(
        const IGraphicBufferProducer::FrameEventsDelta &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t readEmbeddedFromParcel(
        const IGraphicBufferProducer::FrameEventHistoryDelta &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t writeEmbeddedToParcel(
        const IGraphicBufferProducer::FrameEventHistoryDelta &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t readEmbeddedFromParcel(
        const IGraphicBufferProducer::QueueBufferInput &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t writeEmbeddedToParcel(
        const IGraphicBufferProducer::QueueBufferInput &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t readEmbeddedFromParcel(
        const IGraphicBufferProducer::QueueBufferOutput &obj,
        const ::android::hardware::Parcel &parcel,
        size_t parentHandle,
        size_t parentOffset);

::android::status_t writeEmbeddedToParcel(
        const IGraphicBufferProducer::QueueBufferOutput &obj,
        ::android::hardware::Parcel *parcel,
        size_t parentHandle,
        size_t parentOffset);

}  // namespace V1_0
}  // namespace bufferqueue
}  // namespace graphics
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_GRAPHICS_BUFFERQUEUE_V1_0_IHWGRAPHICBUFFERPRODUCER_H
