#ifndef HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_IOMXNODE_H
#define HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_IOMXNODE_H

#include <android/hardware/media/omx/1.0/IOmxBufferSource.h>
#include <android/hardware/media/omx/1.0/types.h>
#include <android/hardware/media/1.0/types.h>
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

struct IOmxNode : public ::android::hidl::base::V1_0::IBase {
    virtual bool isRemote() const override { return false; }


    virtual ::android::hardware::Return<Status> freeNode() = 0;

    virtual ::android::hardware::Return<Status> sendCommand(uint32_t cmd, int32_t param) = 0;

    using getParameter_cb = std::function<void(Status status, const ::android::hardware::hidl_vec<uint8_t>& outParams)>;
    virtual ::android::hardware::Return<void> getParameter(uint32_t index, const ::android::hardware::hidl_vec<uint8_t>& inParams, getParameter_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<Status> setParameter(uint32_t index, const ::android::hardware::hidl_vec<uint8_t>& params) = 0;

    using getConfig_cb = std::function<void(Status status, const ::android::hardware::hidl_vec<uint8_t>& outConfig)>;
    virtual ::android::hardware::Return<void> getConfig(uint32_t index, const ::android::hardware::hidl_vec<uint8_t>& inConfig, getConfig_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<Status> setConfig(uint32_t index, const ::android::hardware::hidl_vec<uint8_t>& config) = 0;

    virtual ::android::hardware::Return<Status> setPortMode(uint32_t portIndex, PortMode mode) = 0;

    virtual ::android::hardware::Return<Status> prepareForAdaptivePlayback(uint32_t portIndex, bool enable, uint32_t maxFrameWidth, uint32_t maxFrameHeight) = 0;

    using configureVideoTunnelMode_cb = std::function<void(Status status, const ::android::hardware::hidl_handle& sidebandHandle)>;
    virtual ::android::hardware::Return<void> configureVideoTunnelMode(uint32_t portIndex, bool tunneled, uint32_t audioHwSync, configureVideoTunnelMode_cb _hidl_cb) = 0;

    using getGraphicBufferUsage_cb = std::function<void(Status status, uint32_t usage)>;
    virtual ::android::hardware::Return<void> getGraphicBufferUsage(uint32_t portIndex, getGraphicBufferUsage_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<Status> setInputSurface(const ::android::sp<IOmxBufferSource>& bufferSource) = 0;

    using allocateSecureBuffer_cb = std::function<void(Status status, uint32_t buffer, const ::android::hardware::hidl_handle& nativeHandle)>;
    virtual ::android::hardware::Return<void> allocateSecureBuffer(uint32_t portIndex, uint64_t size, allocateSecureBuffer_cb _hidl_cb) = 0;

    using useBuffer_cb = std::function<void(Status status, uint32_t buffer)>;
    virtual ::android::hardware::Return<void> useBuffer(uint32_t portIndex, const CodecBuffer& omxBuffer, useBuffer_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<Status> freeBuffer(uint32_t portIndex, uint32_t buffer) = 0;

    virtual ::android::hardware::Return<Status> fillBuffer(uint32_t buffer, const CodecBuffer& omxBuffer, const ::android::hardware::hidl_handle& fence) = 0;

    virtual ::android::hardware::Return<Status> emptyBuffer(uint32_t buffer, const CodecBuffer& omxBuffer, uint32_t flags, uint64_t timestampUs, const ::android::hardware::hidl_handle& fence) = 0;

    using getExtensionIndex_cb = std::function<void(Status status, uint32_t index)>;
    virtual ::android::hardware::Return<void> getExtensionIndex(const ::android::hardware::hidl_string& parameterName, getExtensionIndex_cb _hidl_cb) = 0;

    virtual ::android::hardware::Return<Status> dispatchMessage(const Message& msg) = 0;

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
    static ::android::hardware::Return<::android::sp<IOmxNode>> castFrom(const ::android::sp<IOmxNode>& parent, bool emitError = false);
    static ::android::hardware::Return<::android::sp<IOmxNode>> castFrom(const ::android::sp<::android::hidl::base::V1_0::IBase>& parent, bool emitError = false);

    static const char* descriptor;

    static ::android::sp<IOmxNode> tryGetService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IOmxNode> tryGetService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return tryGetService(str, getStub); }
    static ::android::sp<IOmxNode> tryGetService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return tryGetService(str, getStub); }
    static ::android::sp<IOmxNode> tryGetService(bool getStub) { return tryGetService("default", getStub); }
    static ::android::sp<IOmxNode> getService(const std::string &serviceName="default", bool getStub=false);
    static ::android::sp<IOmxNode> getService(const char serviceName[], bool getStub=false)  { std::string str(serviceName ? serviceName : "");      return getService(str, getStub); }
    static ::android::sp<IOmxNode> getService(const ::android::hardware::hidl_string& serviceName, bool getStub=false)  { std::string str(serviceName.c_str());      return getService(str, getStub); }
    static ::android::sp<IOmxNode> getService(bool getStub) { return getService("default", getStub); }
    ::android::status_t registerAsService(const std::string &serviceName="default");
    static bool registerForNotifications(
            const std::string &serviceName,
            const ::android::sp<::android::hidl::manager::V1_0::IServiceNotification> &notification);
};

std::string toString(const ::android::sp<IOmxNode>&);

}  // namespace V1_0
}  // namespace omx
}  // namespace media
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_MEDIA_OMX_V1_0_IOMXNODE_H
