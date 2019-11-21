#ifndef HIDL_GENERATED_ANDROID_HARDWARE_GRAPHICS_BUFFERQUEUE_V1_0_IGRAPHICBUFFERPRODUCER_H
#define HIDL_GENERATED_ANDROID_HARDWARE_GRAPHICS_BUFFERQUEUE_V1_0_IGRAPHICBUFFERPRODUCER_H

#include <android/hardware/graphics/bufferqueue/1.0/IProducerListener.h>
#include <android/hardware/graphics/common/1.0/types.h>
#include <android/hardware/media/1.0/types.h>
#include <android/hidl/base/1.0/IBase.h>

#include <android/hidl/manager/1.0/IServiceNotification.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace graphics {
namespace bufferqueue {
namespace V1_0 {

struct IGraphicBufferProducer : public ::android::hidl::base::V1_0::IBase {
    typedef int32_t Status;

    struct FenceTimeSnapshot final {
        enum class State : int32_t {
            EMPTY = 0,
            FENCE = 1,
            SIGNAL_TIME = 2,
        };

        IGraphicBufferProducer::FenceTimeSnapshot::State state __attribute__ ((aligned(4)));
        ::android::hardware::hidl_handle fence __attribute__ ((aligned(8)));
        int64_t signalTimeNs __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(IGraphicBufferProducer::FenceTimeSnapshot, state) == 0, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FenceTimeSnapshot, fence) == 8, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FenceTimeSnapshot, signalTimeNs) == 24, "wrong offset");
    static_assert(sizeof(IGraphicBufferProducer::FenceTimeSnapshot) == 32, "wrong size");
    static_assert(__alignof(IGraphicBufferProducer::FenceTimeSnapshot) == 8, "wrong alignment");

    struct FrameEventsDelta final {
        uint32_t index __attribute__ ((aligned(4)));
        uint64_t frameNumber __attribute__ ((aligned(8)));
        bool addPostCompositeCalled __attribute__ ((aligned(1)));
        bool addRetireCalled __attribute__ ((aligned(1)));
        bool addReleaseCalled __attribute__ ((aligned(1)));
        int64_t postedTimeNs __attribute__ ((aligned(8)));
        int64_t requestedPresentTimeNs __attribute__ ((aligned(8)));
        int64_t latchTimeNs __attribute__ ((aligned(8)));
        int64_t firstRefreshStartTimeNs __attribute__ ((aligned(8)));
        int64_t lastRefreshStartTimeNs __attribute__ ((aligned(8)));
        int64_t dequeueReadyTime __attribute__ ((aligned(8)));
        IGraphicBufferProducer::FenceTimeSnapshot gpuCompositionDoneFence __attribute__ ((aligned(8)));
        IGraphicBufferProducer::FenceTimeSnapshot displayPresentFence __attribute__ ((aligned(8)));
        IGraphicBufferProducer::FenceTimeSnapshot displayRetireFence __attribute__ ((aligned(8)));
        IGraphicBufferProducer::FenceTimeSnapshot releaseFence __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, index) == 0, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, frameNumber) == 8, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, addPostCompositeCalled) == 16, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, addRetireCalled) == 17, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, addReleaseCalled) == 18, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, postedTimeNs) == 24, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, requestedPresentTimeNs) == 32, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, latchTimeNs) == 40, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, firstRefreshStartTimeNs) == 48, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, lastRefreshStartTimeNs) == 56, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, dequeueReadyTime) == 64, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, gpuCompositionDoneFence) == 72, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, displayPresentFence) == 104, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, displayRetireFence) == 136, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventsDelta, releaseFence) == 168, "wrong offset");
    static_assert(sizeof(IGraphicBufferProducer::FrameEventsDelta) == 200, "wrong size");
    static_assert(__alignof(IGraphicBufferProducer::FrameEventsDelta) == 8, "wrong alignment");

    struct CompositorTiming final {
        int64_t deadlineNs __attribute__ ((aligned(8)));
        int64_t intervalNs __attribute__ ((aligned(8)));
        int64_t presentLatencyNs __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(IGraphicBufferProducer::CompositorTiming, deadlineNs) == 0, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::CompositorTiming, intervalNs) == 8, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::CompositorTiming, presentLatencyNs) == 16, "wrong offset");
    static_assert(sizeof(IGraphicBufferProducer::CompositorTiming) == 24, "wrong size");
    static_assert(__alignof(IGraphicBufferProducer::CompositorTiming) == 8, "wrong alignment");

    struct FrameEventHistoryDelta final {
        ::android::hardware::hidl_vec<IGraphicBufferProducer::FrameEventsDelta> deltas __attribute__ ((aligned(8)));
        IGraphicBufferProducer::CompositorTiming compositorTiming __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(IGraphicBufferProducer::FrameEventHistoryDelta, deltas) == 0, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::FrameEventHistoryDelta, compositorTiming) == 16, "wrong offset");
    static_assert(sizeof(IGraphicBufferProducer::FrameEventHistoryDelta) == 40, "wrong size");
    static_assert(__alignof(IGraphicBufferProducer::FrameEventHistoryDelta) == 8, "wrong alignment");

    enum class DisconnectMode : int32_t {
        API = 0,
        ALL_LOCAL = 1,
    };

    struct QueueBufferInput final {
        int64_t timestamp __attribute__ ((aligned(8)));
        int32_t isAutoTimestamp __attribute__ ((aligned(4)));
        ::android::hardware::graphics::common::V1_0::Dataspace dataSpace __attribute__ ((aligned(4)));
        ::android::hardware::media::V1_0::Rect crop __attribute__ ((aligned(4)));
        int32_t scalingMode __attribute__ ((aligned(4)));
        uint32_t transform __attribute__ ((aligned(4)));
        uint32_t stickyTransform __attribute__ ((aligned(4)));
        ::android::hardware::hidl_handle fence __attribute__ ((aligned(8)));
        ::android::hardware::hidl_vec<::android::hardware::media::V1_0::Rect> surfaceDamage __attribute__ ((aligned(8)));
        bool getFrameTimestamps __attribute__ ((aligned(1)));
    };

    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, timestamp) == 0, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, isAutoTimestamp) == 8, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, dataSpace) == 12, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, crop) == 16, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, scalingMode) == 32, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, transform) == 36, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, stickyTransform) == 40, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, fence) == 48, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, surfaceDamage) == 64, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferInput, getFrameTimestamps) == 80, "wrong offset");
    static_assert(sizeof(IGraphicBufferProducer::QueueBufferInput) == 88, "wrong size");
    static_assert(__alignof(IGraphicBufferProducer::QueueBufferInput) == 8, "wrong alignment");

    struct QueueBufferOutput final {
        uint32_t width __attribute__ ((aligned(4)));
        uint32_t height __attribute__ ((aligned(4)));
        uint32_t transformHint __attribute__ ((aligned(4)));
        uint32_t numPendingBuffers __attribute__ ((aligned(4)));
        uint64_t nextFrameNumber __attribute__ ((aligned(8)));
        bool bufferReplaced __attribute__ ((aligned(1)));
        IGraphicBufferProducer::FrameEventHistoryDelta frameTimestamps __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(IGraphicBufferProducer::QueueBufferOutput, width) == 0, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferOutput, height) == 4, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferOutput, transformHint) == 8, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferOutput, numPendingBuffers) == 12, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferOutput, nextFrameNumber) == 16, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferOutput, bufferReplaced) == 24, "wrong offset");
    static_assert(offsetof(IGraphicBufferProducer::QueueBufferOutput, frameTimestamps) == 32, "wrong offset");
    static_assert(sizeof(IGraphicBufferProducer::QueueBufferOutput) == 72, "wrong size");
    static_assert(__alignof(IGraphicBufferProducer::QueueBufferOutput) == 8, "wrong alignment");

    virtual bool isRemote() const override { return false; }


    using requestBuffer_cb = std::function<void(int32_t status, const ::android::hardware::media::V1_0::AnwBuffer& buffer)>;
    virtual ::android::hardware::Return<void> requestBuffer(int32_t slot, requestBuffer_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<int32_t> setMaxDequeuedBufferCount(int32_t maxDequeuedBuffers) = 0;

    virtual ::android::hardware::Return<int32_t> setAsyncMode(bool async) = 0;

    using dequeueBuffer_cb = std::function<void(int32_t status, int32_t slot, const ::android::hardware::hidl_handle& fence, const IGraphicBufferProducer::FrameEventHistoryDelta& outTimestamps)>;
    virtual ::android::hardware::Return<void> dequeueBuffer(uint32_t width, uint32_t height, ::android::hardware::graphics::common::V1_0::PixelFormat format, uint32_t usage, bool getFrameTimestamps, dequeueBuffer_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<int32_t> detachBuffer(int32_t slot) = 0;

    using detachNextBuffer_cb = std::function<void(int32_t status, const ::android::hardware::media::V1_0::AnwBuffer& buffer, const ::android::hardware::hidl_handle& fence)>;
    virtual ::android::hardware::Return<void> detachNextBuffer(detachNextBuffer_cb _hidl_cb) = 0;

    using attachBuffer_cb = std::function<void(int32_t status, int32_t slot)>;
    virtual ::android::hardware::Return<void> attachBuffer(const ::android::hardware::media::V1_0::AnwBuffer& buffer, attachBuffer_cb _hidl_cb) = 0;

    using queueBuffer_cb = std::function<void(int32_t status, const IGraphicBufferProducer::QueueBufferOutput& output)>;
    virtual ::android::hardware::Return<void> queueBuffer(int32_t slot, const IGraphicBufferProducer::QueueBufferInput& input, queueBuffer_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<int32_t> cancelBuffer(int32_t slot, const ::android::hardware::hidl_handle& fence) = 0;

    using query_cb = std::function<void(int32_t result, int32_t value)>;
    virtual ::android::hardware::Return<void> query(int32_t what, query_cb _hidl_cb) = 0;

    using connect_cb = std::function<void(int32_t status, const IGraphicBufferProducer::QueueBufferOutput& output)>;
    virtual ::android::hardware::Return<void> connect(const ::android::sp<IProducerListener>& listener, int32_t api, bool producerControlledByApp, connect_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<int32_t> disconnect(int32_t api, IGraphicBufferProducer::DisconnectMode mode) = 0;

    virtual ::android::hardware::Return<int32_t> setSidebandStream(const ::android::hardware::hidl_handle& stream) = 0;

    virtual ::android::hardware::Return<void> allocateBuffers(uint32_t width, uint32_t height, ::android::hardware::graphics::common::V1_0::PixelFormat format, uint32_t usage) = 0;

    virtual ::android::hardware::Return<int32_t> allowAllocation(bool allow) = 0;

    virtual ::android::hardware::Return<int32_t> setGenerationNumber(uint32_t generationNumber) = 0;

    using getConsumerName_cb = std::function<void(const ::android::hardware::hidl_string& name)>;
    virtual ::android::hardware::Return<void> getConsumerName(getConsumerName_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<int32_t> setSharedBufferMode(bool sharedBufferMode) = 0;

    virtual ::android::hardware::Return<int32_t> setAutoRefresh(bool autoRefresh) = 0;

    virtual ::android::hardware::Return<int32_t> setDequeueTimeout(int64_t timeoutNs) = 0;

    using getLastQueuedBuffer_cb = std::function<void(int32_t status, const ::android::hardware::media::V1_0::AnwBuffer& buffer, const ::android::hardware::hidl_handle& fence, const ::android::hardware::hidl_array<float, 16>& transformMatrix)>;
    virtual ::android::hardware::Return<void> getLastQueuedBuffer(getLastQueuedBuffer_cb _hidl_cb) = 0;

    using getFrameTimestamps_cb = std::function<void(const IGraphicBufferProducer::FrameEventHistoryDelta& timeStamps)>;
    virtual ::android::hardware::Return<void> getFrameTimestamps(getFrameTimestamps_cb _hidl_cb) = 0;

    using getUniqueId_cb = std::function<void(int32_t status, uint64_t outId)>;
    virtual ::android::hardware::Return<void> getUniqueId(getUniqueId_cb _hidl_cb) = 0;

    using interfaceChain_cb = std::function<void(const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& descriptors)>;
    virtual ::android::hardware::Return<void> interfaceChain(interfaceChain_cb _hidl_cb) override;

    virtual ::android::hardware::Return<void> debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options) override;

    using interfaceDescriptor_cb = std::function<void(const ::android::hardware::hidl_string& descriptor)>;
    virtual ::android::hardware::Return<void> interfaceDescriptor(interfaceDescriptor_cb _hidl_cb) override;

    using getHashChain_cb = std::function<void(const ::android::hardware::hidl_vec<::android::hardware::hidl_array<uint8_t, 32>>& hashchain)>;
    virtual ::android::hardware::Return<void> getHashChain(getHashChain_cb _hidl_cb) override;

    virtual ::android::hardware::Return<void> setHALInstrumentation() override;

    virtual ::android::hardware::Return<bool> linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie) override;

    virtual ::android::hardware::Return<void> ping() override;

    using getDebugInfo_cb = std::function<void(const ::android::hidl::base::V1_0::DebugInfo& info)>;
    virtual ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb) override;

    virtual ::android::hardware::Return<void> notifySyspropsChanged() override;

    virtual ::android::hardware::Return<bool> unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient) override;
    // cast static functions
    static ::android::hardware::Return<::android::sp<IGraphicBufferProducer>> castFrom(const ::android::sp<IGraphicBufferProducer>& parent, bool emitError = false);
    static ::android::hardware::Return<::android::sp<IGraphicBufferProducer>> castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError = false);

    static const char* descriptor;

    static ::android::sp<IGraphicBufferProducer> tryGetService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IGraphicBufferProducer> tryGetService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return tryGetService(str, getStub); }
    static ::android::sp<IGraphicBufferProducer> tryGetService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return tryGetService(str, getStub); }
    static ::android::sp<IGraphicBufferProducer> tryGetService(bool getStub) { return tryGetService("default", getStub); }
    static ::android::sp<IGraphicBufferProducer> getService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IGraphicBufferProducer> getService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return getService(str, getStub); }
    static ::android::sp<IGraphicBufferProducer> getService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return getService(str, getStub); }
    static ::android::sp<IGraphicBufferProducer> getService(bool getStub) { return getService("default", getStub); }
    ::android::status_t registerAsService(const std::string &serviceName="default");
    static bool registerForNotifications(
            const std::string &serviceName,
            const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification);
};

constexpr int32_t operator|(const IGraphicBufferProducer::FenceTimeSnapshot::State lhs, const IGraphicBufferProducer::FenceTimeSnapshot::State rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const IGraphicBufferProducer::FenceTimeSnapshot::State rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const IGraphicBufferProducer::FenceTimeSnapshot::State lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const IGraphicBufferProducer::FenceTimeSnapshot::State lhs, const IGraphicBufferProducer::FenceTimeSnapshot::State rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const IGraphicBufferProducer::FenceTimeSnapshot::State rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const IGraphicBufferProducer::FenceTimeSnapshot::State lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const IGraphicBufferProducer::FenceTimeSnapshot::State e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const IGraphicBufferProducer::FenceTimeSnapshot::State e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<IGraphicBufferProducer::FenceTimeSnapshot::State>(int32_t o);

std::string toString(IGraphicBufferProducer::FenceTimeSnapshot::State o);

std::string toString(const IGraphicBufferProducer::FenceTimeSnapshot&);

// operator== and operator!= are not generated for FenceTimeSnapshot

std::string toString(const IGraphicBufferProducer::FrameEventsDelta&);

// operator== and operator!= are not generated for FrameEventsDelta

std::string toString(const IGraphicBufferProducer::CompositorTiming&);

bool operator==(const IGraphicBufferProducer::CompositorTiming&, const IGraphicBufferProducer::CompositorTiming&);

bool operator!=(const IGraphicBufferProducer::CompositorTiming&, const IGraphicBufferProducer::CompositorTiming&);

std::string toString(const IGraphicBufferProducer::FrameEventHistoryDelta&);

// operator== and operator!= are not generated for FrameEventHistoryDelta

constexpr int32_t operator|(const IGraphicBufferProducer::DisconnectMode lhs, const IGraphicBufferProducer::DisconnectMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const IGraphicBufferProducer::DisconnectMode rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const IGraphicBufferProducer::DisconnectMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const IGraphicBufferProducer::DisconnectMode lhs, const IGraphicBufferProducer::DisconnectMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const IGraphicBufferProducer::DisconnectMode rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const IGraphicBufferProducer::DisconnectMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const IGraphicBufferProducer::DisconnectMode e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const IGraphicBufferProducer::DisconnectMode e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<IGraphicBufferProducer::DisconnectMode>(int32_t o);

std::string toString(IGraphicBufferProducer::DisconnectMode o);

std::string toString(const IGraphicBufferProducer::QueueBufferInput&);

// operator== and operator!= are not generated for QueueBufferInput

std::string toString(const IGraphicBufferProducer::QueueBufferOutput&);

// operator== and operator!= are not generated for QueueBufferOutput

std::string toString(const ::android::sp<IGraphicBufferProducer>&);

}  // namespace V1_0
}  // namespace bufferqueue
}  // namespace graphics
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_GRAPHICS_BUFFERQUEUE_V1_0_IGRAPHICBUFFERPRODUCER_H
