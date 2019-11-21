#ifndef HIDL_GENERATED_ANDROID_HIDL_MANAGER_V1_0_ISERVICEMANAGER_H
#define HIDL_GENERATED_ANDROID_HIDL_MANAGER_V1_0_ISERVICEMANAGER_H

#include <android/hidl/base/1.0/IBase.h>
#include <android/hidl/base/1.0/types.h>
#include <android/hidl/manager/1.0/IServiceNotification.h>

#include <android/hidl/manager/1.0/IServiceNotification.h>

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hidl {
namespace manager {
namespace V1_0 {

struct IServiceManager : public ::android::hidl::base::V1_0::IBase {
    enum class Transport : uint8_t {
        EMPTY = 0,
        HWBINDER = 1,
        PASSTHROUGH = 2,
    };

    enum class PidConstant : int32_t {
        NO_PID = -1, // (-1)
    };

    struct InstanceDebugInfo final {
        ::android::hardware::hidl_string interfaceName __attribute__ ((aligned(8)));
        ::android::hardware::hidl_string instanceName __attribute__ ((aligned(8)));
        int32_t pid __attribute__ ((aligned(4)));
        ::android::hardware::hidl_vec<int32_t> clientPids __attribute__ ((aligned(8)));
        ::android::hidl::base::V1_0::DebugInfo::Architecture arch __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(IServiceManager::InstanceDebugInfo, interfaceName) == 0, "wrong offset");
    static_assert(offsetof(IServiceManager::InstanceDebugInfo, instanceName) == 16, "wrong offset");
    static_assert(offsetof(IServiceManager::InstanceDebugInfo, pid) == 32, "wrong offset");
    static_assert(offsetof(IServiceManager::InstanceDebugInfo, clientPids) == 40, "wrong offset");
    static_assert(offsetof(IServiceManager::InstanceDebugInfo, arch) == 56, "wrong offset");
    static_assert(sizeof(IServiceManager::InstanceDebugInfo) == 64, "wrong size");
    static_assert(__alignof(IServiceManager::InstanceDebugInfo) == 8, "wrong alignment");

    virtual bool isRemote() const override { return false; }


    virtual ::android::hardware::Return<::android::sp<::android::hidl::base::V1_0::IBase>> get(const ::android::hardware::hidl_string& fqName, const ::android::hardware::hidl_string& name) = 0;

    virtual ::android::hardware::Return<bool> add(const ::android::hardware::hidl_string& name, const ::android::sp<::android::hidl::base::V1_0::IBase>& service) = 0;

    virtual ::android::hardware::Return<IServiceManager::Transport> getTransport(const ::android::hardware::hidl_string& fqName, const ::android::hardware::hidl_string& name) = 0;

    using list_cb = std::function<void(const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& fqInstanceNames)>;
    virtual ::android::hardware::Return<void> list(list_cb _hidl_cb) = 0;

    using listByInterface_cb = std::function<void(const ::android::hardware::hidl_vec<::android::hardware::hidl_string>& instanceNames)>;
    virtual ::android::hardware::Return<void> listByInterface(const ::android::hardware::hidl_string& fqName, listByInterface_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<bool> registerForNotifications(const ::android::hardware::hidl_string& fqName, const ::android::hardware::hidl_string& name, const ::android::sp<IServiceNotification>& callback) = 0;

    using debugDump_cb = std::function<void(const ::android::hardware::hidl_vec<IServiceManager::InstanceDebugInfo>& info)>;
    virtual ::android::hardware::Return<void> debugDump(debugDump_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<void> registerPassthroughClient(const ::android::hardware::hidl_string& fqName, const ::android::hardware::hidl_string& name) = 0;

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
    static ::android::hardware::Return<::android::sp<IServiceManager>> castFrom(const ::android::sp<IServiceManager>& parent, bool emitError = false);
    static ::android::hardware::Return<::android::sp<IServiceManager>> castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError = false);

    static const char* descriptor;

    static ::android::sp<IServiceManager> tryGetService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IServiceManager> tryGetService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return tryGetService(str, getStub); }
    static ::android::sp<IServiceManager> tryGetService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return tryGetService(str, getStub); }
    static ::android::sp<IServiceManager> tryGetService(bool getStub) { return tryGetService("default", getStub); }
    static ::android::sp<IServiceManager> getService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IServiceManager> getService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return getService(str, getStub); }
    static ::android::sp<IServiceManager> getService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return getService(str, getStub); }
    static ::android::sp<IServiceManager> getService(bool getStub) { return getService("default", getStub); }
    ::android::status_t registerAsService(const std::string &serviceName="default");
    static bool registerForNotifications(
            const std::string &serviceName,
            const ::android::sp<IServiceNotification> &notification);
};

constexpr uint8_t operator|(const IServiceManager::Transport lhs, const IServiceManager::Transport rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
}

constexpr uint8_t operator|(const uint8_t lhs, const IServiceManager::Transport rhs) {
    return static_cast<uint8_t>(lhs | static_cast<uint8_t>(rhs));
}

constexpr uint8_t operator|(const IServiceManager::Transport lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) | rhs);
}

constexpr uint8_t operator&(const IServiceManager::Transport lhs, const IServiceManager::Transport rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs));
}

constexpr uint8_t operator&(const uint8_t lhs, const IServiceManager::Transport rhs) {
    return static_cast<uint8_t>(lhs & static_cast<uint8_t>(rhs));
}

constexpr uint8_t operator&(const IServiceManager::Transport lhs, const uint8_t rhs) {
    return static_cast<uint8_t>(static_cast<uint8_t>(lhs) & rhs);
}

constexpr uint8_t &operator|=(uint8_t& v, const IServiceManager::Transport e) {
    v |= static_cast<uint8_t>(e);
    return v;
}

constexpr uint8_t &operator&=(uint8_t& v, const IServiceManager::Transport e) {
    v &= static_cast<uint8_t>(e);
    return v;
}

template<typename>
std::string toString(uint8_t o);
template<>
std::string toString<IServiceManager::Transport>(uint8_t o);

std::string toString(IServiceManager::Transport o);

constexpr int32_t operator|(const IServiceManager::PidConstant lhs, const IServiceManager::PidConstant rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const IServiceManager::PidConstant rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const IServiceManager::PidConstant lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const IServiceManager::PidConstant lhs, const IServiceManager::PidConstant rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const IServiceManager::PidConstant rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const IServiceManager::PidConstant lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const IServiceManager::PidConstant e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const IServiceManager::PidConstant e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<IServiceManager::PidConstant>(int32_t o);

std::string toString(IServiceManager::PidConstant o);

std::string toString(const IServiceManager::InstanceDebugInfo&);

bool operator==(const IServiceManager::InstanceDebugInfo&, const IServiceManager::InstanceDebugInfo&);

bool operator!=(const IServiceManager::InstanceDebugInfo&, const IServiceManager::InstanceDebugInfo&);

std::string toString(const ::android::sp<IServiceManager>&);

}  // namespace V1_0
}  // namespace manager
}  // namespace hidl
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HIDL_MANAGER_V1_0_ISERVICEMANAGER_H
