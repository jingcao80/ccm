#ifndef HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_IOMXSTORE_H
#define HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_IOMXSTORE_H

#include <android/hardware/media/omx/1.0/IOmx.h>
#include <android/hardware/media/omx/1.0/types.h>
#include <android/hidl/base/1.0/IBase.h>

#include <android/hidl/manager/1.0/IServiceNotification.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace media {
namespace omx {
namespace V1_0 {

struct IOmxStore : public ::android::hidl::base::V1_0::IBase {
    struct Attribute final {
        ::android::hardware::hidl_string key __attribute__ ((aligned(8)));
        ::android::hardware::hidl_string value __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(IOmxStore::Attribute, key) == 0, "wrong offset");
    static_assert(offsetof(IOmxStore::Attribute, value) == 16, "wrong offset");
    static_assert(sizeof(IOmxStore::Attribute) == 32, "wrong size");
    static_assert(__alignof(IOmxStore::Attribute) == 8, "wrong alignment");

    typedef IOmxStore::Attribute ServiceAttribute;

    typedef IOmxStore::Attribute NodeAttribute;

    struct NodeInfo final {
        ::android::hardware::hidl_string name __attribute__ ((aligned(8)));
        ::android::hardware::hidl_string owner __attribute__ ((aligned(8)));
        ::android::hardware::hidl_vec<IOmxStore::Attribute> attributes __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(IOmxStore::NodeInfo, name) == 0, "wrong offset");
    static_assert(offsetof(IOmxStore::NodeInfo, owner) == 16, "wrong offset");
    static_assert(offsetof(IOmxStore::NodeInfo, attributes) == 32, "wrong offset");
    static_assert(sizeof(IOmxStore::NodeInfo) == 48, "wrong size");
    static_assert(__alignof(IOmxStore::NodeInfo) == 8, "wrong alignment");

    struct RoleInfo final {
        ::android::hardware::hidl_string role __attribute__ ((aligned(8)));
        ::android::hardware::hidl_string type __attribute__ ((aligned(8)));
        bool isEncoder __attribute__ ((aligned(1)));
        bool preferPlatformNodes __attribute__ ((aligned(1)));
        ::android::hardware::hidl_vec<IOmxStore::NodeInfo> nodes __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(IOmxStore::RoleInfo, role) == 0, "wrong offset");
    static_assert(offsetof(IOmxStore::RoleInfo, type) == 16, "wrong offset");
    static_assert(offsetof(IOmxStore::RoleInfo, isEncoder) == 32, "wrong offset");
    static_assert(offsetof(IOmxStore::RoleInfo, preferPlatformNodes) == 33, "wrong offset");
    static_assert(offsetof(IOmxStore::RoleInfo, nodes) == 40, "wrong offset");
    static_assert(sizeof(IOmxStore::RoleInfo) == 56, "wrong size");
    static_assert(__alignof(IOmxStore::RoleInfo) == 8, "wrong alignment");

    virtual bool isRemote() const override { return false; }


    using listServiceAttributes_cb = std::function<void(Status status, const ::android::hardware::hidl_vec<IOmxStore::Attribute>& attributes)>;
    virtual ::android::hardware::Return<void> listServiceAttributes(listServiceAttributes_cb _hidl_cb) = 0;

    using getNodePrefix_cb = std::function<void(const ::android::hardware::hidl_string& prefix)>;
    virtual ::android::hardware::Return<void> getNodePrefix(getNodePrefix_cb _hidl_cb) = 0;

    using listRoles_cb = std::function<void(const ::android::hardware::hidl_vec<IOmxStore::RoleInfo>& roleList)>;
    virtual ::android::hardware::Return<void> listRoles(listRoles_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<::android::sp<IOmx>> getOmx(const ::android::hardware::hidl_string& name) = 0;

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
    static ::android::hardware::Return<::android::sp<IOmxStore>> castFrom(const ::android::sp<IOmxStore>& parent, bool emitError = false);
    static ::android::hardware::Return<::android::sp<IOmxStore>> castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError = false);

    static const char* descriptor;

    static ::android::sp<IOmxStore> tryGetService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IOmxStore> tryGetService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return tryGetService(str, getStub); }
    static ::android::sp<IOmxStore> tryGetService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return tryGetService(str, getStub); }
    static ::android::sp<IOmxStore> tryGetService(bool getStub) { return tryGetService("default", getStub); }
    static ::android::sp<IOmxStore> getService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IOmxStore> getService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return getService(str, getStub); }
    static ::android::sp<IOmxStore> getService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return getService(str, getStub); }
    static ::android::sp<IOmxStore> getService(bool getStub) { return getService("default", getStub); }
    ::android::status_t registerAsService(const std::string &serviceName="default");
    static bool registerForNotifications(
            const std::string &serviceName,
            const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification);
};

std::string toString(const IOmxStore::Attribute&);

bool operator==(const IOmxStore::Attribute&, const IOmxStore::Attribute&);

bool operator!=(const IOmxStore::Attribute&, const IOmxStore::Attribute&);

std::string toString(const IOmxStore::NodeInfo&);

bool operator==(const IOmxStore::NodeInfo&, const IOmxStore::NodeInfo&);

bool operator!=(const IOmxStore::NodeInfo&, const IOmxStore::NodeInfo&);

std::string toString(const IOmxStore::RoleInfo&);

bool operator==(const IOmxStore::RoleInfo&, const IOmxStore::RoleInfo&);

bool operator!=(const IOmxStore::RoleInfo&, const IOmxStore::RoleInfo&);

std::string toString(const ::android::sp<IOmxStore>&);

}  // namespace V1_0
}  // namespace omx
}  // namespace media
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_IOMXSTORE_H
