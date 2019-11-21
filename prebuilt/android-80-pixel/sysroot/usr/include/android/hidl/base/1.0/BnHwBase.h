#ifndef HIDL_GENERATED_ANDROID_HIDL_BASE_V1_0_BNHWBASE_H
#define HIDL_GENERATED_ANDROID_HIDL_BASE_V1_0_BNHWBASE_H

#include <android/hidl/base/1.0/IHwBase.h>

namespace android {
namespace hidl {
namespace base {
namespace V1_0 {

struct BnHwBase : public ::android::hardware::BHwBinder, public ::android::hardware::details::HidlInstrumentor {
    explicit BnHwBase(const ::android::sp<IBase> &_hidl_impl);
    explicit BnHwBase(const ::android::sp<IBase> &_hidl_impl, const std::string& HidlInstrumentor_package, const std::string& HidlInstrumentor_interface);

    ::android::status_t onTransact(
            uint32_t _hidl_code,
            const ::android::hardware::Parcel &_hidl_data,
            ::android::hardware::Parcel *_hidl_reply,
            uint32_t _hidl_flags = 0,
            TransactCallback _hidl_cb = nullptr) override;

    ::android::sp<IBase> getImpl() { return _hidl_mImpl; };
private:
    // Methods from IBase follow.
    ::android::hardware::Return<void> ping();
    using getDebugInfo_cb = IBase::getDebugInfo_cb;
    ::android::hardware::Return<void> getDebugInfo(getDebugInfo_cb _hidl_cb);

    ::android::sp<IBase> _hidl_mImpl;
};

}  // namespace V1_0
}  // namespace base
}  // namespace hidl
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HIDL_BASE_V1_0_BNHWBASE_H
