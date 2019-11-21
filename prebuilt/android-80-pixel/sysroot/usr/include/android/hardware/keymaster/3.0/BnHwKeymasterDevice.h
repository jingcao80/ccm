#ifndef HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_BNHWKEYMASTERDEVICE_H
#define HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_BNHWKEYMASTERDEVICE_H

#include <android/hardware/keymaster/3.0/IHwKeymasterDevice.h>

namespace android {
namespace hardware {
namespace keymaster {
namespace V3_0 {

struct BnHwKeymasterDevice : public ::android::hidl::base::V1_0::BnHwBase {
    explicit BnHwKeymasterDevice(const ::android::sp<IKeymasterDevice> &_hidl_impl);
    explicit BnHwKeymasterDevice(const ::android::sp<IKeymasterDevice> &_hidl_impl, const std::string& HidlInstrumentor_package, const std::string& HidlInstrumentor_interface);

    ::android::status_t onTransact(
            uint32_t _hidl_code,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            uint32_t _hidl_flags = 0,
            TransactCallback _hidl_cb = nullptr) override;

    ::android::sp<IKeymasterDevice> getImpl() { return _hidl_mImpl; };
private:
    // Methods from IKeymasterDevice follow.

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    ::android::hardware::Return<void> ping();
    using getDebugInfo_cb = ::android::hidl::base::V1_0::IBase::getDebugInfo_cb;
    ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb);

    ::android::sp<IKeymasterDevice> _hidl_mImpl;
};

}  // namespace V3_0
}  // namespace keymaster
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_BNHWKEYMASTERDEVICE_H
