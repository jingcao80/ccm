#ifndef HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_BNHWOMX_H
#define HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_BNHWOMX_H

#include <android/hardware/media/omx/1.0/IHwOmx.h>

namespace android {
namespace hardware {
namespace media {
namespace omx {
namespace V1_0 {

struct BnHwOmx : public ::android::hidl::base::V1_0::BnHwBase {
    explicit BnHwOmx(const ::android::sp<IOmx> &_hidl_impl);
    explicit BnHwOmx(const ::android::sp<IOmx> &_hidl_impl, const std::string& HidlInstrumentor_package, const std::string& HidlInstrumentor_interface);

    ::android::status_t onTransact(
            uint32_t _hidl_code,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            uint32_t _hidl_flags = 0,
            TransactCallback _hidl_cb = nullptr) override;

    ::android::sp<IOmx> getImpl() { return _hidl_mImpl; };
private:
    // Methods from IOmx follow.

    // Methods from ::android::hidl::base::V1_0::IBase follow.
    ::android::hardware::Return<void> ping();
    using getDebugInfo_cb = ::android::hidl::base::V1_0::IBase::getDebugInfo_cb;
    ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb);

    ::android::sp<IOmx> _hidl_mImpl;
};

}  // namespace V1_0
}  // namespace omx
}  // namespace media
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_BNHWOMX_H
