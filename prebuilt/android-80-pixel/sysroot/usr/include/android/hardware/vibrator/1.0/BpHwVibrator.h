#ifndef HIDL_GENERATED_ANDROID_HARDWARE_VIBRATOR_V1_0_BPHWVIBRATOR_H
#define HIDL_GENERATED_ANDROID_HARDWARE_VIBRATOR_V1_0_BPHWVIBRATOR_H

#include <hidl/HidlTransportSupport.h>

#include <android/hardware/vibrator/1.0/IHwVibrator.h>

namespace android {
namespace hardware {
namespace vibrator {
namespace V1_0 {

struct BpHwVibrator : public ::android::hardware::BpInterface<IVibrator>, public ::android::hardware::details::HidlInstrumentor {
    explicit BpHwVibrator(const ::android::sp<::android::hardware::IBinder> &_hidl_impl);

    virtual bool isRemote() const override { return true; }

    // Methods from IVibrator follow.
    ::android::hardware::Return<Status> on(uint32_t timeoutMs) override;
    ::android::hardware::Return<Status> off() override;
    ::android::hardware::Return<bool> supportsAmplitudeControl() override;
    ::android::hardware::Return<Status> setAmplitude(uint8_t amplitude) override;
    ::android::hardware::Return<void> perform(Effect effect, EffectStrength strength, perform_cb _hidl_cb) override;

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
}  // namespace vibrator
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_VIBRATOR_V1_0_BPHWVIBRATOR_H
