#ifndef HIDL_GENERATED_ANDROID_HARDWARE_TV_CEC_V1_0_IHDMICEC_H
#define HIDL_GENERATED_ANDROID_HARDWARE_TV_CEC_V1_0_IHDMICEC_H

#include <android/hardware/tv/cec/1.0/IHdmiCecCallback.h>
#include <android/hardware/tv/cec/1.0/types.h>
#include <android/hidl/base/1.0/IBase.h>

#include <android/hidl/manager/1.0/IServiceNotification.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace tv {
namespace cec {
namespace V1_0 {

struct IHdmiCec : public ::android::hidl::base::V1_0::IBase {
    virtual bool isRemote() const override { return false; }


    // @callflow(next="*")
    virtual ::android::hardware::Return<Result> addLogicalAddress(CecLogicalAddress addr) = 0;

    // @callflow(next="addLogicalAddress") @exit
    virtual ::android::hardware::Return<void> clearLogicalAddress() = 0;

    using getPhysicalAddress_cb = std::function<void(Result result, uint16_t addr)>;
    // @callflow(next="*")
    virtual ::android::hardware::Return<void> getPhysicalAddress(getPhysicalAddress_cb _hidl_cb) = 0;

    // @callflow(next="*")
    virtual ::android::hardware::Return<SendMessageResult> sendMessage(const CecMessage& message) = 0;

    // @callflow(next="addLogicalAddress") @entry
    virtual ::android::hardware::Return<void> setCallback(const ::android::sp<IHdmiCecCallback>& callback) = 0;

    // @callflow(next="*")
    virtual ::android::hardware::Return<int32_t> getCecVersion() = 0;

    // @callflow(next="*")
    virtual ::android::hardware::Return<uint32_t> getVendorId() = 0;

    using getPortInfo_cb = std::function<void(const ::android::hardware::hidl_vec<HdmiPortInfo>& infos)>;
    // @callflow(next="*")
    virtual ::android::hardware::Return<void> getPortInfo(getPortInfo_cb _hidl_cb) = 0;

    // @callflow(next="*")
    virtual ::android::hardware::Return<void> setOption(OptionKey key, bool value) = 0;

    // @callflow(next="*")
    virtual ::android::hardware::Return<void> setLanguage(const ::android::hardware::hidl_string& language) = 0;

    // @callflow(next="*")
    virtual ::android::hardware::Return<void> enableAudioReturnChannel(int32_t portId, bool enable) = 0;

    // @callflow(next="*")
    virtual ::android::hardware::Return<bool> isConnected(int32_t portId) = 0;

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
    static ::android::hardware::Return<::android::sp<IHdmiCec>> castFrom(const ::android::sp<IHdmiCec>& parent, bool emitError = false);
    static ::android::hardware::Return<::android::sp<IHdmiCec>> castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError = false);

    static const char* descriptor;

    static ::android::sp<IHdmiCec> tryGetService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IHdmiCec> tryGetService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return tryGetService(str, getStub); }
    static ::android::sp<IHdmiCec> tryGetService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return tryGetService(str, getStub); }
    static ::android::sp<IHdmiCec> tryGetService(bool getStub) { return tryGetService("default", getStub); }
    static ::android::sp<IHdmiCec> getService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IHdmiCec> getService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return getService(str, getStub); }
    static ::android::sp<IHdmiCec> getService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return getService(str, getStub); }
    static ::android::sp<IHdmiCec> getService(bool getStub) { return getService("default", getStub); }
    ::android::status_t registerAsService(const std::string &serviceName="default");
    static bool registerForNotifications(
            const std::string &serviceName,
            const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification);
};

std::string toString(const ::android::sp<IHdmiCec>&);

}  // namespace V1_0
}  // namespace cec
}  // namespace tv
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_TV_CEC_V1_0_IHDMICEC_H
