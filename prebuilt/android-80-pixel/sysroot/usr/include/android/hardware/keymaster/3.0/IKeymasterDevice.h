#ifndef HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_IKEYMASTERDEVICE_H
#define HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_IKEYMASTERDEVICE_H

#include <android/hardware/keymaster/3.0/types.h>
#include <android/hidl/base/1.0/IBase.h>

#include <android/hidl/manager/1.0/IServiceNotification.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace keymaster {
namespace V3_0 {

struct IKeymasterDevice : public ::android::hidl::base::V1_0::IBase {
    virtual bool isRemote() const override { return false; }


    using getHardwareFeatures_cb = std::function<void(bool isSecure, bool supportsEllipticCurve, bool supportsSymmetricCryptography, bool supportsAttestation, bool supportsAllDigests, const ::android::hardware::hidl_string& keymasterName, const ::android::hardware::hidl_string& keymasterAuthorName)>;
    virtual ::android::hardware::Return<void> getHardwareFeatures(getHardwareFeatures_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<ErrorCode> addRngEntropy(const ::android::hardware::hidl_vec<uint8_t>& data) = 0;

    using generateKey_cb = std::function<void(ErrorCode error, const ::android::hardware::hidl_vec<uint8_t>& keyBlob, const KeyCharacteristics& keyCharacteristics)>;
    virtual ::android::hardware::Return<void> generateKey(const ::android::hardware::hidl_vec<KeyParameter>& keyParams, generateKey_cb _hidl_cb) = 0;

    using importKey_cb = std::function<void(ErrorCode error, const ::android::hardware::hidl_vec<uint8_t>& keyBlob, const KeyCharacteristics& keyCharacteristics)>;
    virtual ::android::hardware::Return<void> importKey(const ::android::hardware::hidl_vec<KeyParameter>& params, KeyFormat keyFormat, const ::android::hardware::hidl_vec<uint8_t>& keyData, importKey_cb _hidl_cb) = 0;

    using getKeyCharacteristics_cb = std::function<void(ErrorCode error, const KeyCharacteristics& keyCharacteristics)>;
    virtual ::android::hardware::Return<void> getKeyCharacteristics(const ::android::hardware::hidl_vec<uint8_t>& keyBlob, const ::android::hardware::hidl_vec<uint8_t>& clientId, const ::android::hardware::hidl_vec<uint8_t>& appData, getKeyCharacteristics_cb _hidl_cb) = 0;

    using exportKey_cb = std::function<void(ErrorCode error, const ::android::hardware::hidl_vec<uint8_t>& keyMaterial)>;
    virtual ::android::hardware::Return<void> exportKey(KeyFormat keyFormat, const ::android::hardware::hidl_vec<uint8_t>& keyBlob, const ::android::hardware::hidl_vec<uint8_t>& clientId, const ::android::hardware::hidl_vec<uint8_t>& appData, exportKey_cb _hidl_cb) = 0;

    using attestKey_cb = std::function<void(ErrorCode error, const ::android::hardware::hidl_vec<::android::hardware::hidl_vec<uint8_t>>& certChain)>;
    virtual ::android::hardware::Return<void> attestKey(const ::android::hardware::hidl_vec<uint8_t>& keyToAttest, const ::android::hardware::hidl_vec<KeyParameter>& attestParams, attestKey_cb _hidl_cb) = 0;

    using upgradeKey_cb = std::function<void(ErrorCode error, const ::android::hardware::hidl_vec<uint8_t>& upgradedKeyBlob)>;
    virtual ::android::hardware::Return<void> upgradeKey(const ::android::hardware::hidl_vec<uint8_t>& keyBlobToUpgrade, const ::android::hardware::hidl_vec<KeyParameter>& upgradeParams, upgradeKey_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<ErrorCode> deleteKey(const ::android::hardware::hidl_vec<uint8_t>& keyBlob) = 0;

    virtual ::android::hardware::Return<ErrorCode> deleteAllKeys() = 0;

    virtual ::android::hardware::Return<ErrorCode> destroyAttestationIds() = 0;

    using begin_cb = std::function<void(ErrorCode error, const ::android::hardware::hidl_vec<KeyParameter>& outParams, uint64_t operationHandle)>;
    virtual ::android::hardware::Return<void> begin(KeyPurpose purpose, const ::android::hardware::hidl_vec<uint8_t>& key, const ::android::hardware::hidl_vec<KeyParameter>& inParams, begin_cb _hidl_cb) = 0;

    using update_cb = std::function<void(ErrorCode error, uint32_t inputConsumed, const ::android::hardware::hidl_vec<KeyParameter>& outParams, const ::android::hardware::hidl_vec<uint8_t>& output)>;
    virtual ::android::hardware::Return<void> update(uint64_t operationHandle, const ::android::hardware::hidl_vec<KeyParameter>& inParams, const ::android::hardware::hidl_vec<uint8_t>& input, update_cb _hidl_cb) = 0;

    using finish_cb = std::function<void(ErrorCode error, const ::android::hardware::hidl_vec<KeyParameter>& outParams, const ::android::hardware::hidl_vec<uint8_t>& output)>;
    virtual ::android::hardware::Return<void> finish(uint64_t operationHandle, const ::android::hardware::hidl_vec<KeyParameter>& inParams, const ::android::hardware::hidl_vec<uint8_t>& input, const ::android::hardware::hidl_vec<uint8_t>& signature, finish_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<ErrorCode> abort(uint64_t operationHandle) = 0;

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
    static ::android::hardware::Return<::android::sp<IKeymasterDevice>> castFrom(const ::android::sp<IKeymasterDevice>& parent, bool emitError = false);
    static ::android::hardware::Return<::android::sp<IKeymasterDevice>> castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError = false);

    static const char* descriptor;

    static ::android::sp<IKeymasterDevice> tryGetService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IKeymasterDevice> tryGetService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return tryGetService(str, getStub); }
    static ::android::sp<IKeymasterDevice> tryGetService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return tryGetService(str, getStub); }
    static ::android::sp<IKeymasterDevice> tryGetService(bool getStub) { return tryGetService("default", getStub); }
    static ::android::sp<IKeymasterDevice> getService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IKeymasterDevice> getService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return getService(str, getStub); }
    static ::android::sp<IKeymasterDevice> getService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return getService(str, getStub); }
    static ::android::sp<IKeymasterDevice> getService(bool getStub) { return getService("default", getStub); }
    ::android::status_t registerAsService(const std::string &serviceName="default");
    static bool registerForNotifications(
            const std::string &serviceName,
            const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification);
};

std::string toString(const ::android::sp<IKeymasterDevice>&);

}  // namespace V3_0
}  // namespace keymaster
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_IKEYMASTERDEVICE_H
