#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TV_CEC_V1_0_BPHWHDMICEC_H
#define HIDL_GENERATED_ANDROID_HARDWARE_TV_CEC_V1_0_BPHWHDMICEC_H

#include <hidl/HidlTransportSupport.h>

#include <android/hardware/tv/cec/1.0/IHwHdmiCec.h>

namespace android {
namespace hardware {
namespace tv {
namespace cec {
namespace V1_0 {

struct BpHwHdmiCec : public ::android::hardware::BpInterface<IHdmiCec>, public ::android::hardware::details::HidlInstrumentor {
    explicit BpHwHdmiCec(const ::android::sp<::android::hardware::IBinder> &_hidl_impl);

    virtual bool isRemote() const override { return true; }

    // Methods from IHdmiCec follow.
    ::android::hardware::Return<Result> addLogicalAddress(CecLogicalAddress addr) override;
    ::android::hardware::Return<void> clearLogicalAddress() override;
    ::android::hardware::Return<void> getPhysicalAddress(getPhysicalAddress_cb _hidl_cb) override;
    ::android::hardware::Return<SendMessageResult> sendMessage(const CecMessage& message) override;
    ::android::hardware::Return<void> setCallback(const ::android::sp<IHdmiCecCallback>& callback) override;
    ::android::hardware::Return<int32_t> getCecVersion() override;
    ::android::hardware::Return<uint32_t> getVendorId() override;
    ::android::hardware::Return<void> getPortInfo(getPortInfo_cb _hidl_cb) override;
    ::android::hardware::Return<void> setOption(OptionKey key, bool value) override;
    ::android::hardware::Return<void> setLanguage(const ::android::hardware::hidl_string& language) override;
    ::android::hardware::Return<void> enableAudioReturnChannel(int32_t portId, bool enable) override;
    ::android::hardware::Return<bool> isConnected(int32_t portId) override;

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
}  // namespace cec
}  // namespace tv
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TV_CEC_V1_0_BPHWHDMICEC_H
