#ifndef HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_BPHWKEYMASTERDEVICE_H
#define HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_BPHWKEYMASTERDEVICE_H

#include <hidl/HidlTransportSupport.h>

#include <android/hardware/keymaster/3.0/IHwKeymasterDevice.h>

namespace android {
namespace hardware {
namespace keymaster {
namespace V3_0 {

struct BpHwKeymasterDevice : public ::android::hardware::BpInterface<IKeymasterDevice>, public ::android::hardware::details::HidlInstrumentor {
    explicit BpHwKeymasterDevice(const ::android::sp<::android::hardware::IBinder> &_hidl_impl);

    virtual bool isRemote() const override { return true; }

    // Methods from IKeymasterDevice follow.
    ::android::hardware::Return<void> getHardwareFeatures(getHardwareFeatures_cb _hidl_cb) override;
    ::android::hardware::Return<ErrorCode> addRngEntropy(const ::android::hardware::hidl_vec<uint8_t>& data) override;
    ::android::hardware::Return<void> generateKey(const ::android::hardware::hidl_vec<KeyParameter>& keyParams, generateKey_cb _hidl_cb) override;
    ::android::hardware::Return<void> importKey(const ::android::hardware::hidl_vec<KeyParameter>& params, KeyFormat keyFormat, const ::android::hardware::hidl_vec<uint8_t>& keyData, importKey_cb _hidl_cb) override;
    ::android::hardware::Return<void> getKeyCharacteristics(const ::android::hardware::hidl_vec<uint8_t>& keyBlob, const ::android::hardware::hidl_vec<uint8_t>& clientId, const ::android::hardware::hidl_vec<uint8_t>& appData, getKeyCharacteristics_cb _hidl_cb) override;
    ::android::hardware::Return<void> exportKey(KeyFormat keyFormat, const ::android::hardware::hidl_vec<uint8_t>& keyBlob, const ::android::hardware::hidl_vec<uint8_t>& clientId, const ::android::hardware::hidl_vec<uint8_t>& appData, exportKey_cb _hidl_cb) override;
    ::android::hardware::Return<void> attestKey(const ::android::hardware::hidl_vec<uint8_t>& keyToAttest, const ::android::hardware::hidl_vec<KeyParameter>& attestParams, attestKey_cb _hidl_cb) override;
    ::android::hardware::Return<void> upgradeKey(const ::android::hardware::hidl_vec<uint8_t>& keyBlobToUpgrade, const ::android::hardware::hidl_vec<KeyParameter>& upgradeParams, upgradeKey_cb _hidl_cb) override;
    ::android::hardware::Return<ErrorCode> deleteKey(const ::android::hardware::hidl_vec<uint8_t>& keyBlob) override;
    ::android::hardware::Return<ErrorCode> deleteAllKeys() override;
    ::android::hardware::Return<ErrorCode> destroyAttestationIds() override;
    ::android::hardware::Return<void> begin(KeyPurpose purpose, const ::android::hardware::hidl_vec<uint8_t>& key, const ::android::hardware::hidl_vec<KeyParameter>& inParams, begin_cb _hidl_cb) override;
    ::android::hardware::Return<void> update(uint64_t operationHandle, const ::android::hardware::hidl_vec<KeyParameter>& inParams, const ::android::hardware::hidl_vec<uint8_t>& input, update_cb _hidl_cb) override;
    ::android::hardware::Return<void> finish(uint64_t operationHandle, const ::android::hardware::hidl_vec<KeyParameter>& inParams, const ::android::hardware::hidl_vec<uint8_t>& input, const ::android::hardware::hidl_vec<uint8_t>& signature, finish_cb _hidl_cb) override;
    ::android::hardware::Return<ErrorCode> abort(uint64_t operationHandle) override;

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

}  // namespace V3_0
}  // namespace keymaster
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_BPHWKEYMASTERDEVICE_H
