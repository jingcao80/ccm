#ifndef HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_IOMX_H
#define HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_IOMX_H

#include <android/hardware/graphics/bufferqueue/1.0/IGraphicBufferProducer.h>
#include <android/hardware/media/omx/1.0/IGraphicBufferSource.h>
#include <android/hardware/media/omx/1.0/IOmxNode.h>
#include <android/hardware/media/omx/1.0/IOmxObserver.h>
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

struct IOmx : public ::android::hidl::base::V1_0::IBase {
    struct ComponentInfo final {
        ::android::hardware::hidl_string mName __attribute__ ((aligned(8)));
        ::android::hardware::hidl_vec<::android::hardware::hidl_string> mRoles __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(IOmx::ComponentInfo, mName) == 0, "wrong offset");
    static_assert(offsetof(IOmx::ComponentInfo, mRoles) == 16, "wrong offset");
    static_assert(sizeof(IOmx::ComponentInfo) == 32, "wrong size");
    static_assert(__alignof(IOmx::ComponentInfo) == 8, "wrong alignment");

    virtual bool isRemote() const override { return false; }


    using listNodes_cb = std::function<void(Status status, const ::android::hardware::hidl_vec<IOmx::ComponentInfo>& nodeList)>;
    virtual ::android::hardware::Return<void> listNodes(listNodes_cb _hidl_cb) = 0;

    using allocateNode_cb = std::function<void(Status status, const ::android::sp<IOmxNode>& omxNode)>;
    virtual ::android::hardware::Return<void> allocateNode(const ::android::hardware::hidl_string& name, const ::android::sp<IOmxObserver>& observer, allocateNode_cb _hidl_cb) = 0;

    using createInputSurface_cb = std::function<void(Status status, const ::android::sp<::android::hardware::graphics::bufferqueue::V1_0::IGraphicBufferProducer>& producer, const ::android::sp<IGraphicBufferSource>& source)>;
    virtual ::android::hardware::Return<void> createInputSurface(createInputSurface_cb _hidl_cb) = 0;

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
    static ::android::hardware::Return<::android::sp<IOmx>> castFrom(const ::android::sp<IOmx>& parent, bool emitError = false);
    static ::android::hardware::Return<::android::sp<IOmx>> castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError = false);

    static const char* descriptor;

    static ::android::sp<IOmx> tryGetService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IOmx> tryGetService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return tryGetService(str, getStub); }
    static ::android::sp<IOmx> tryGetService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return tryGetService(str, getStub); }
    static ::android::sp<IOmx> tryGetService(bool getStub) { return tryGetService("default", getStub); }
    static ::android::sp<IOmx> getService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IOmx> getService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return getService(str, getStub); }
    static ::android::sp<IOmx> getService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return getService(str, getStub); }
    static ::android::sp<IOmx> getService(bool getStub) { return getService("default", getStub); }
    ::android::status_t registerAsService(const std::string &serviceName="default");
    static bool registerForNotifications(
            const std::string &serviceName,
            const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification);
};

std::string toString(const IOmx::ComponentInfo&);

bool operator==(const IOmx::ComponentInfo&, const IOmx::ComponentInfo&);

bool operator!=(const IOmx::ComponentInfo&, const IOmx::ComponentInfo&);

std::string toString(const ::android::sp<IOmx>&);

}  // namespace V1_0
}  // namespace omx
}  // namespace media
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_IOMX_H
