#ifndef HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_BPHWGRAPHICBUFFERSOURCE_H
#define HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_BPHWGRAPHICBUFFERSOURCE_H

#include <hidl/HidlTransportSupport.h>

#include <android/hardware/media/omx/1.0/IHwGraphicBufferSource.h>

namespace android {
namespace hardware {
namespace media {
namespace omx {
namespace V1_0 {

struct BpHwGraphicBufferSource : public ::android::hardware::BpInterface<IGraphicBufferSource>, public ::android::hardware::details::HidlInstrumentor {
    explicit BpHwGraphicBufferSource(const ::android::sp<::android::hardware::IBinder> &_hidl_impl);

    virtual bool isRemote() const override { return true; }

    // Methods from IGraphicBufferSource follow.
    ::android::hardware::Return<Status> configure(const ::android::sp<IOmxNode>& omxNode, ::android::hardware::graphics::common::V1_0::Dataspace dataspace) override;
    ::android::hardware::Return<Status> setSuspend(bool suspend, int64_t timeUs) override;
    ::android::hardware::Return<Status> setRepeatPreviousFrameDelayUs(int64_t repeatAfterUs) override;
    ::android::hardware::Return<Status> setMaxFps(float maxFps) override;
    ::android::hardware::Return<Status> setTimeLapseConfig(double fps, double captureFps) override;
    ::android::hardware::Return<Status> setStartTimeUs(int64_t startTimeUs) override;
    ::android::hardware::Return<Status> setStopTimeUs(int64_t stopTimeUs) override;
    ::android::hardware::Return<void> getStopTimeOffsetUs(getStopTimeOffsetUs_cb _hidl_cb) override;
    ::android::hardware::Return<Status> setColorAspects(const ColorAspects& aspects) override;
    ::android::hardware::Return<Status> setTimeOffsetUs(int64_t timeOffsetUs) override;
    ::android::hardware::Return<Status> signalEndOfInputStream() override;

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    ::android::hardware::Return<void> interfaceChain(interfaceChain_cb _hidl_cb) override;
    ::android::hardware::Return<void> debug(const ::android::hardware::hidl_handle& fd, const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& options) override;
    ::android::hardware::Return<void> interfaceDescriptor(interfaceDescriptor_cb _hidl_cb) override;
    ::android::hardware::Return<void> getHashChain(getHashChain_cb _hidl_cb) override;
    ::android::hardware::Return<void> setHALInstrumentation() override;
    ::android::hardware::Return<bool> linkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient, uint64_t cookie) override;
    ::android::hardware::Return<void> ping() override;
    ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb) override;
    ::android::hardware::Return<void> notifySyspropsChanged() override;
    ::android::hardware::Return<bool> unlinkToDeath(const ::android::sp<::android::hardware::hidl_death_recipient>& recipient) override;

private:
    std::mutex _hidl_mMutex;
    std::vector<::android::sp<::android::hardware::hidl_binder_death_recipient>> _hidl_mDeathRecipients;
};

}  // namespace V1_0
}  // namespace omx
}  // namespace media
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_BPHWGRAPHICBUFFERSOURCE_H
