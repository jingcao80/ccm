#ifndef HIDL_GENERATED_ANDROID_HARDWARE_AUDIO_COMMON_V2_0_TYPES_H
#define HIDL_GENERATED_ANDROID_HARDWARE_AUDIO_COMMON_V2_0_TYPES_H

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace audio {
namespace common {
namespace V2_0 {

typedef int32_t AudioIoHandle;

typedef int32_t AudioModuleHandle;

typedef int32_t AudioPortHandle;

typedef int32_t AudioPatchHandle;

typedef uint32_t AudioHwSync;

enum class AudioHandleConsts : int32_t {
    AUDIO_IO_HANDLE_NONE = 0,
    AUDIO_MODULE_HANDLE_NONE = 0,
    AUDIO_PORT_HANDLE_NONE = 0,
    AUDIO_PATCH_HANDLE_NONE = 0,
};

struct Uuid final {
    uint32_t timeLow __attribute__ ((aligned(4)));
    uint16_t timeMid __attribute__ ((aligned(2)));
    uint16_t versionAndTimeHigh __attribute__ ((aligned(2)));
    uint16_t variantAndClockSeqHigh __attribute__ ((aligned(2)));
    ::android::hardware::hidl_array<uint8_t, 6> node __attribute__ ((aligned(1)));
};

static_assert(offsetof(Uuid, timeLow) == 0, "wrong offset");
static_assert(offsetof(Uuid, timeMid) == 4, "wrong offset");
static_assert(offsetof(Uuid, versionAndTimeHigh) == 6, "wrong offset");
static_assert(offsetof(Uuid, variantAndClockSeqHigh) == 8, "wrong offset");
static_assert(offsetof(Uuid, node) == 10, "wrong offset");
static_assert(sizeof(Uuid) == 16, "wrong size");
static_assert(__alignof(Uuid) == 4, "wrong alignment");

enum class AudioStreamType : int32_t {
    DEFAULT = -1, // (-1)
    MIN = 0,
    VOICE_CALL = 0,
    SYSTEM = 1,
    RING = 2,
    MUSIC = 3,
    ALARM = 4,
    NOTIFICATION = 5,
    BLUETOOTH_SCO = 6,
    ENFORCED_AUDIBLE = 7,
    DTMF = 8,
    TTS = 9,
    ACCESSIBILITY = 10,
    REROUTING = 11,
    PATCH = 12,
    PUBLIC_CNT = 11, // (ACCESSIBILITY + 1)
    FOR_POLICY_CNT = 12, // PATCH
    CNT = 13, // (PATCH + 1)
};

enum class AudioSource : int32_t {
    DEFAULT = 0,
    MIC = 1,
    VOICE_UPLINK = 2,
    VOICE_DOWNLINK = 3,
    VOICE_CALL = 4,
    CAMCORDER = 5,
    VOICE_RECOGNITION = 6,
    VOICE_COMMUNICATION = 7,
    REMOTE_SUBMIX = 8,
    UNPROCESSED = 9,
    CNT = 10,
    MAX = 9, // (CNT - 1)
    FM_TUNER = 1998,
    HOTWORD = 1999,
};

typedef int32_t AudioSession;

enum class AudioSessionConsts : int32_t {
    OUTPUT_STAGE = -1, // (-1)
    OUTPUT_MIX = 0,
    ALLOCATE = 0,
    NONE = 0,
};

enum class AudioFormat : uint32_t {
    INVALID = 4294967295u, // 0xFFFFFFFFUL
    DEFAULT = 0u, // 0
    PCM = 0u, // 0x00000000UL
    MP3 = 16777216u, // 0x01000000UL
    AMR_NB = 33554432u, // 0x02000000UL
    AMR_WB = 50331648u, // 0x03000000UL
    AAC = 67108864u, // 0x04000000UL
    HE_AAC_V1 = 83886080u, // 0x05000000UL
    HE_AAC_V2 = 100663296u, // 0x06000000UL
    VORBIS = 117440512u, // 0x07000000UL
    OPUS = 134217728u, // 0x08000000UL
    AC3 = 150994944u, // 0x09000000UL
    E_AC3 = 167772160u, // 0x0A000000UL
    DTS = 184549376u, // 0x0B000000UL
    DTS_HD = 201326592u, // 0x0C000000UL
    IEC61937 = 218103808u, // 0x0D000000UL
    DOLBY_TRUEHD = 234881024u, // 0x0E000000UL
    EVRC = 268435456u, // 0x10000000UL
    EVRCB = 285212672u, // 0x11000000UL
    EVRCWB = 301989888u, // 0x12000000UL
    EVRCNW = 318767104u, // 0x13000000UL
    AAC_ADIF = 335544320u, // 0x14000000UL
    WMA = 352321536u, // 0x15000000UL
    WMA_PRO = 369098752u, // 0x16000000UL
    AMR_WB_PLUS = 385875968u, // 0x17000000UL
    MP2 = 402653184u, // 0x18000000UL
    QCELP = 419430400u, // 0x19000000UL
    DSD = 436207616u, // 0x1A000000UL
    FLAC = 452984832u, // 0x1B000000UL
    ALAC = 469762048u, // 0x1C000000UL
    APE = 486539264u, // 0x1D000000UL
    AAC_ADTS = 503316480u, // 0x1E000000UL
    SBC = 520093696u, // 0x1F000000UL
    APTX = 536870912u, // 0x20000000UL
    APTX_HD = 553648128u, // 0x21000000UL
    AC4 = 570425344u, // 0x22000000UL
    LDAC = 587202560u, // 0x23000000UL
    MAIN_MASK = 4278190080u, // 0xFF000000UL
    SUB_MASK = 16777215u, // 0x00FFFFFFUL
    PCM_SUB_16_BIT = 1u, // 0x1
    PCM_SUB_8_BIT = 2u, // 0x2
    PCM_SUB_32_BIT = 3u, // 0x3
    PCM_SUB_8_24_BIT = 4u, // 0x4
    PCM_SUB_FLOAT = 5u, // 0x5
    PCM_SUB_24_BIT_PACKED = 6u, // 0x6
    MP3_SUB_NONE = 0u, // 0x0
    AMR_SUB_NONE = 0u, // 0x0
    AAC_SUB_MAIN = 1u, // 0x1
    AAC_SUB_LC = 2u, // 0x2
    AAC_SUB_SSR = 4u, // 0x4
    AAC_SUB_LTP = 8u, // 0x8
    AAC_SUB_HE_V1 = 16u, // 0x10
    AAC_SUB_SCALABLE = 32u, // 0x20
    AAC_SUB_ERLC = 64u, // 0x40
    AAC_SUB_LD = 128u, // 0x80
    AAC_SUB_HE_V2 = 256u, // 0x100
    AAC_SUB_ELD = 512u, // 0x200
    VORBIS_SUB_NONE = 0u, // 0x0
    PCM_16_BIT = 1u, // (PCM | PCM_SUB_16_BIT)
    PCM_8_BIT = 2u, // (PCM | PCM_SUB_8_BIT)
    PCM_32_BIT = 3u, // (PCM | PCM_SUB_32_BIT)
    PCM_8_24_BIT = 4u, // (PCM | PCM_SUB_8_24_BIT)
    PCM_FLOAT = 5u, // (PCM | PCM_SUB_FLOAT)
    PCM_24_BIT_PACKED = 6u, // (PCM | PCM_SUB_24_BIT_PACKED)
    AAC_MAIN = 67108865u, // (AAC | AAC_SUB_MAIN)
    AAC_LC = 67108866u, // (AAC | AAC_SUB_LC)
    AAC_SSR = 67108868u, // (AAC | AAC_SUB_SSR)
    AAC_LTP = 67108872u, // (AAC | AAC_SUB_LTP)
    AAC_HE_V1 = 67108880u, // (AAC | AAC_SUB_HE_V1)
    AAC_SCALABLE = 67108896u, // (AAC | AAC_SUB_SCALABLE)
    AAC_ERLC = 67108928u, // (AAC | AAC_SUB_ERLC)
    AAC_LD = 67108992u, // (AAC | AAC_SUB_LD)
    AAC_HE_V2 = 67109120u, // (AAC | AAC_SUB_HE_V2)
    AAC_ELD = 67109376u, // (AAC | AAC_SUB_ELD)
    AAC_ADTS_MAIN = 503316481u, // (AAC_ADTS | AAC_SUB_MAIN)
    AAC_ADTS_LC = 503316482u, // (AAC_ADTS | AAC_SUB_LC)
    AAC_ADTS_SSR = 503316484u, // (AAC_ADTS | AAC_SUB_SSR)
    AAC_ADTS_LTP = 503316488u, // (AAC_ADTS | AAC_SUB_LTP)
    AAC_ADTS_HE_V1 = 503316496u, // (AAC_ADTS | AAC_SUB_HE_V1)
    AAC_ADTS_SCALABLE = 503316512u, // (AAC_ADTS | AAC_SUB_SCALABLE)
    AAC_ADTS_ERLC = 503316544u, // (AAC_ADTS | AAC_SUB_ERLC)
    AAC_ADTS_LD = 503316608u, // (AAC_ADTS | AAC_SUB_LD)
    AAC_ADTS_HE_V2 = 503316736u, // (AAC_ADTS | AAC_SUB_HE_V2)
    AAC_ADTS_ELD = 503316992u, // (AAC_ADTS | AAC_SUB_ELD)
};

enum class FixedChannelCount : int32_t {
    FCC_2 = 2,
    FCC_8 = 8,
};

enum class AudioChannelMask : uint32_t {
    REPRESENTATION_POSITION = 0u, // 0
    REPRESENTATION_INDEX = 2u, // 2
    NONE = 0u, // 0x0
    INVALID = 3221225472u, // 0xC0000000
    OUT_FRONT_LEFT = 1u, // 0x1
    OUT_FRONT_RIGHT = 2u, // 0x2
    OUT_FRONT_CENTER = 4u, // 0x4
    OUT_LOW_FREQUENCY = 8u, // 0x8
    OUT_BACK_LEFT = 16u, // 0x10
    OUT_BACK_RIGHT = 32u, // 0x20
    OUT_FRONT_LEFT_OF_CENTER = 64u, // 0x40
    OUT_FRONT_RIGHT_OF_CENTER = 128u, // 0x80
    OUT_BACK_CENTER = 256u, // 0x100
    OUT_SIDE_LEFT = 512u, // 0x200
    OUT_SIDE_RIGHT = 1024u, // 0x400
    OUT_TOP_CENTER = 2048u, // 0x800
    OUT_TOP_FRONT_LEFT = 4096u, // 0x1000
    OUT_TOP_FRONT_CENTER = 8192u, // 0x2000
    OUT_TOP_FRONT_RIGHT = 16384u, // 0x4000
    OUT_TOP_BACK_LEFT = 32768u, // 0x8000
    OUT_TOP_BACK_CENTER = 65536u, // 0x10000
    OUT_TOP_BACK_RIGHT = 131072u, // 0x20000
    OUT_MONO = 1u, // OUT_FRONT_LEFT
    OUT_STEREO = 3u, // (OUT_FRONT_LEFT | OUT_FRONT_RIGHT)
    OUT_2POINT1 = 11u, // ((OUT_FRONT_LEFT | OUT_FRONT_RIGHT) | OUT_LOW_FREQUENCY)
    OUT_QUAD = 51u, // (((OUT_FRONT_LEFT | OUT_FRONT_RIGHT) | OUT_BACK_LEFT) | OUT_BACK_RIGHT)
    OUT_QUAD_BACK = 51u, // OUT_QUAD
    OUT_QUAD_SIDE = 1539u, // (((OUT_FRONT_LEFT | OUT_FRONT_RIGHT) | OUT_SIDE_LEFT) | OUT_SIDE_RIGHT)
    OUT_SURROUND = 263u, // (((OUT_FRONT_LEFT | OUT_FRONT_RIGHT) | OUT_FRONT_CENTER) | OUT_BACK_CENTER)
    OUT_PENTA = 55u, // (OUT_QUAD | OUT_FRONT_CENTER)
    OUT_5POINT1 = 63u, // (((((OUT_FRONT_LEFT | OUT_FRONT_RIGHT) | OUT_FRONT_CENTER) | OUT_LOW_FREQUENCY) | OUT_BACK_LEFT) | OUT_BACK_RIGHT)
    OUT_5POINT1_BACK = 63u, // OUT_5POINT1
    OUT_5POINT1_SIDE = 1551u, // (((((OUT_FRONT_LEFT | OUT_FRONT_RIGHT) | OUT_FRONT_CENTER) | OUT_LOW_FREQUENCY) | OUT_SIDE_LEFT) | OUT_SIDE_RIGHT)
    OUT_6POINT1 = 319u, // ((((((OUT_FRONT_LEFT | OUT_FRONT_RIGHT) | OUT_FRONT_CENTER) | OUT_LOW_FREQUENCY) | OUT_BACK_LEFT) | OUT_BACK_RIGHT) | OUT_BACK_CENTER)
    OUT_7POINT1 = 1599u, // (((((((OUT_FRONT_LEFT | OUT_FRONT_RIGHT) | OUT_FRONT_CENTER) | OUT_LOW_FREQUENCY) | OUT_BACK_LEFT) | OUT_BACK_RIGHT) | OUT_SIDE_LEFT) | OUT_SIDE_RIGHT)
    OUT_ALL = 262143u, // (((((((((((((((((OUT_FRONT_LEFT | OUT_FRONT_RIGHT) | OUT_FRONT_CENTER) | OUT_LOW_FREQUENCY) | OUT_BACK_LEFT) | OUT_BACK_RIGHT) | OUT_FRONT_LEFT_OF_CENTER) | OUT_FRONT_RIGHT_OF_CENTER) | OUT_BACK_CENTER) | OUT_SIDE_LEFT) | OUT_SIDE_RIGHT) | OUT_TOP_CENTER) | OUT_TOP_FRONT_LEFT) | OUT_TOP_FRONT_CENTER) | OUT_TOP_FRONT_RIGHT) | OUT_TOP_BACK_LEFT) | OUT_TOP_BACK_CENTER) | OUT_TOP_BACK_RIGHT)
    IN_LEFT = 4u, // 0x4
    IN_RIGHT = 8u, // 0x8
    IN_FRONT = 16u, // 0x10
    IN_BACK = 32u, // 0x20
    IN_LEFT_PROCESSED = 64u, // 0x40
    IN_RIGHT_PROCESSED = 128u, // 0x80
    IN_FRONT_PROCESSED = 256u, // 0x100
    IN_BACK_PROCESSED = 512u, // 0x200
    IN_PRESSURE = 1024u, // 0x400
    IN_X_AXIS = 2048u, // 0x800
    IN_Y_AXIS = 4096u, // 0x1000
    IN_Z_AXIS = 8192u, // 0x2000
    IN_VOICE_UPLINK = 16384u, // 0x4000
    IN_VOICE_DNLINK = 32768u, // 0x8000
    IN_MONO = 16u, // IN_FRONT
    IN_STEREO = 12u, // (IN_LEFT | IN_RIGHT)
    IN_FRONT_BACK = 48u, // (IN_FRONT | IN_BACK)
    IN_6 = 252u, // (((((IN_LEFT | IN_RIGHT) | IN_FRONT) | IN_BACK) | IN_LEFT_PROCESSED) | IN_RIGHT_PROCESSED)
    IN_VOICE_UPLINK_MONO = 16400u, // (IN_VOICE_UPLINK | IN_MONO)
    IN_VOICE_DNLINK_MONO = 32784u, // (IN_VOICE_DNLINK | IN_MONO)
    IN_VOICE_CALL_MONO = 49168u, // (IN_VOICE_UPLINK_MONO | IN_VOICE_DNLINK_MONO)
    IN_ALL = 65532u, // (((((((((((((IN_LEFT | IN_RIGHT) | IN_FRONT) | IN_BACK) | IN_LEFT_PROCESSED) | IN_RIGHT_PROCESSED) | IN_FRONT_PROCESSED) | IN_BACK_PROCESSED) | IN_PRESSURE) | IN_X_AXIS) | IN_Y_AXIS) | IN_Z_AXIS) | IN_VOICE_UPLINK) | IN_VOICE_DNLINK)
    COUNT_MAX = 30u, // 30
    INDEX_HDR = 2147483648u, // (REPRESENTATION_INDEX << COUNT_MAX)
    INDEX_MASK_1 = 2147483649u, // (INDEX_HDR | ((1 << 1) - 1))
    INDEX_MASK_2 = 2147483651u, // (INDEX_HDR | ((1 << 2) - 1))
    INDEX_MASK_3 = 2147483655u, // (INDEX_HDR | ((1 << 3) - 1))
    INDEX_MASK_4 = 2147483663u, // (INDEX_HDR | ((1 << 4) - 1))
    INDEX_MASK_5 = 2147483679u, // (INDEX_HDR | ((1 << 5) - 1))
    INDEX_MASK_6 = 2147483711u, // (INDEX_HDR | ((1 << 6) - 1))
    INDEX_MASK_7 = 2147483775u, // (INDEX_HDR | ((1 << 7) - 1))
    INDEX_MASK_8 = 2147483903u, // (INDEX_HDR | ((1 << 8) - 1))
};

enum class AudioInterleave : int32_t {
    LEFT = 0,
    RIGHT = 1,
};

enum class AudioMode : int32_t {
    INVALID = -2, // (-2)
    CURRENT = -1, // (-1)
    NORMAL = 0,
    RINGTONE = 1,
    IN_CALL = 2,
    IN_COMMUNICATION = 3,
    CNT = 4,
    MAX = 3, // (CNT - 1)
};

enum class AudioDevice : uint32_t {
    NONE = 0u, // 0x0
    BIT_IN = 2147483648u, // 0x80000000
    BIT_DEFAULT = 1073741824u, // 0x40000000
    OUT_EARPIECE = 1u, // 0x1
    OUT_SPEAKER = 2u, // 0x2
    OUT_WIRED_HEADSET = 4u, // 0x4
    OUT_WIRED_HEADPHONE = 8u, // 0x8
    OUT_BLUETOOTH_SCO = 16u, // 0x10
    OUT_BLUETOOTH_SCO_HEADSET = 32u, // 0x20
    OUT_BLUETOOTH_SCO_CARKIT = 64u, // 0x40
    OUT_BLUETOOTH_A2DP = 128u, // 0x80
    OUT_BLUETOOTH_A2DP_HEADPHONES = 256u, // 0x100
    OUT_BLUETOOTH_A2DP_SPEAKER = 512u, // 0x200
    OUT_AUX_DIGITAL = 1024u, // 0x400
    OUT_HDMI = 1024u, // OUT_AUX_DIGITAL
    OUT_ANLG_DOCK_HEADSET = 2048u, // 0x800
    OUT_DGTL_DOCK_HEADSET = 4096u, // 0x1000
    OUT_USB_ACCESSORY = 8192u, // 0x2000
    OUT_USB_DEVICE = 16384u, // 0x4000
    OUT_REMOTE_SUBMIX = 32768u, // 0x8000
    OUT_TELEPHONY_TX = 65536u, // 0x10000
    OUT_LINE = 131072u, // 0x20000
    OUT_HDMI_ARC = 262144u, // 0x40000
    OUT_SPDIF = 524288u, // 0x80000
    OUT_FM = 1048576u, // 0x100000
    OUT_AUX_LINE = 2097152u, // 0x200000
    OUT_SPEAKER_SAFE = 4194304u, // 0x400000
    OUT_IP = 8388608u, // 0x800000
    OUT_BUS = 16777216u, // 0x1000000
    OUT_PROXY = 33554432u, // 0x2000000
    OUT_USB_HEADSET = 67108864u, // 0x4000000
    OUT_DEFAULT = 1073741824u, // BIT_DEFAULT
    OUT_ALL = 1207959551u, // (((((((((((((((((((((((((((OUT_EARPIECE | OUT_SPEAKER) | OUT_WIRED_HEADSET) | OUT_WIRED_HEADPHONE) | OUT_BLUETOOTH_SCO) | OUT_BLUETOOTH_SCO_HEADSET) | OUT_BLUETOOTH_SCO_CARKIT) | OUT_BLUETOOTH_A2DP) | OUT_BLUETOOTH_A2DP_HEADPHONES) | OUT_BLUETOOTH_A2DP_SPEAKER) | OUT_HDMI) | OUT_ANLG_DOCK_HEADSET) | OUT_DGTL_DOCK_HEADSET) | OUT_USB_ACCESSORY) | OUT_USB_DEVICE) | OUT_REMOTE_SUBMIX) | OUT_TELEPHONY_TX) | OUT_LINE) | OUT_HDMI_ARC) | OUT_SPDIF) | OUT_FM) | OUT_AUX_LINE) | OUT_SPEAKER_SAFE) | OUT_IP) | OUT_BUS) | OUT_PROXY) | OUT_USB_HEADSET) | OUT_DEFAULT)
    OUT_ALL_A2DP = 896u, // ((OUT_BLUETOOTH_A2DP | OUT_BLUETOOTH_A2DP_HEADPHONES) | OUT_BLUETOOTH_A2DP_SPEAKER)
    OUT_ALL_SCO = 112u, // ((OUT_BLUETOOTH_SCO | OUT_BLUETOOTH_SCO_HEADSET) | OUT_BLUETOOTH_SCO_CARKIT)
    OUT_ALL_USB = 67133440u, // ((OUT_USB_ACCESSORY | OUT_USB_DEVICE) | OUT_USB_HEADSET)
    IN_COMMUNICATION = 2147483649u, // (BIT_IN | 0x1)
    IN_AMBIENT = 2147483650u, // (BIT_IN | 0x2)
    IN_BUILTIN_MIC = 2147483652u, // (BIT_IN | 0x4)
    IN_BLUETOOTH_SCO_HEADSET = 2147483656u, // (BIT_IN | 0x8)
    IN_WIRED_HEADSET = 2147483664u, // (BIT_IN | 0x10)
    IN_AUX_DIGITAL = 2147483680u, // (BIT_IN | 0x20)
    IN_HDMI = 2147483680u, // IN_AUX_DIGITAL
    IN_VOICE_CALL = 2147483712u, // (BIT_IN | 0x40)
    IN_TELEPHONY_RX = 2147483712u, // IN_VOICE_CALL
    IN_BACK_MIC = 2147483776u, // (BIT_IN | 0x80)
    IN_REMOTE_SUBMIX = 2147483904u, // (BIT_IN | 0x100)
    IN_ANLG_DOCK_HEADSET = 2147484160u, // (BIT_IN | 0x200)
    IN_DGTL_DOCK_HEADSET = 2147484672u, // (BIT_IN | 0x400)
    IN_USB_ACCESSORY = 2147485696u, // (BIT_IN | 0x800)
    IN_USB_DEVICE = 2147487744u, // (BIT_IN | 0x1000)
    IN_FM_TUNER = 2147491840u, // (BIT_IN | 0x2000)
    IN_TV_TUNER = 2147500032u, // (BIT_IN | 0x4000)
    IN_LINE = 2147516416u, // (BIT_IN | 0x8000)
    IN_SPDIF = 2147549184u, // (BIT_IN | 0x10000)
    IN_BLUETOOTH_A2DP = 2147614720u, // (BIT_IN | 0x20000)
    IN_LOOPBACK = 2147745792u, // (BIT_IN | 0x40000)
    IN_IP = 2148007936u, // (BIT_IN | 0x80000)
    IN_BUS = 2148532224u, // (BIT_IN | 0x100000)
    IN_PROXY = 2164260864u, // (BIT_IN | 0x1000000)
    IN_USB_HEADSET = 2181038080u, // (BIT_IN | 0x2000000)
    IN_DEFAULT = 3221225472u, // (BIT_IN | BIT_DEFAULT)
    IN_ALL = 3273654271u, // (((((((((((((((((((((((IN_COMMUNICATION | IN_AMBIENT) | IN_BUILTIN_MIC) | IN_BLUETOOTH_SCO_HEADSET) | IN_WIRED_HEADSET) | IN_HDMI) | IN_TELEPHONY_RX) | IN_BACK_MIC) | IN_REMOTE_SUBMIX) | IN_ANLG_DOCK_HEADSET) | IN_DGTL_DOCK_HEADSET) | IN_USB_ACCESSORY) | IN_USB_DEVICE) | IN_FM_TUNER) | IN_TV_TUNER) | IN_LINE) | IN_SPDIF) | IN_BLUETOOTH_A2DP) | IN_LOOPBACK) | IN_IP) | IN_BUS) | IN_PROXY) | IN_USB_HEADSET) | IN_DEFAULT)
    IN_ALL_SCO = 2147483656u, // IN_BLUETOOTH_SCO_HEADSET
    IN_ALL_USB = 2181044224u, // ((IN_USB_ACCESSORY | IN_USB_DEVICE) | IN_USB_HEADSET)
};

enum class AudioOutputFlag : int32_t {
    NONE = 0, // 0x0
    DIRECT = 1, // 0x1
    PRIMARY = 2, // 0x2
    FAST = 4, // 0x4
    DEEP_BUFFER = 8, // 0x8
    COMPRESS_OFFLOAD = 16, // 0x10
    NON_BLOCKING = 32, // 0x20
    HW_AV_SYNC = 64, // 0x40
    TTS = 128, // 0x80
    RAW = 256, // 0x100
    SYNC = 512, // 0x200
    IEC958_NONAUDIO = 1024, // 0x400
    DIRECT_PCM = 8192, // 0x2000
    MMAP_NOIRQ = 16384, // 0x4000
    VOIP_RX = 32768, // 0x8000
};

enum class AudioInputFlag : int32_t {
    NONE = 0, // 0x0
    FAST = 1, // 0x1
    HW_HOTWORD = 2, // 0x2
    RAW = 4, // 0x4
    SYNC = 8, // 0x8
    MMAP_NOIRQ = 16, // 0x10
    VOIP_TX = 32, // 0x20
};

enum class AudioUsage : int32_t {
    UNKNOWN = 0,
    MEDIA = 1,
    VOICE_COMMUNICATION = 2,
    VOICE_COMMUNICATION_SIGNALLING = 3,
    ALARM = 4,
    NOTIFICATION = 5,
    NOTIFICATION_TELEPHONY_RINGTONE = 6,
    NOTIFICATION_COMMUNICATION_REQUEST = 7,
    NOTIFICATION_COMMUNICATION_INSTANT = 8,
    NOTIFICATION_COMMUNICATION_DELAYED = 9,
    NOTIFICATION_EVENT = 10,
    ASSISTANCE_ACCESSIBILITY = 11,
    ASSISTANCE_NAVIGATION_GUIDANCE = 12,
    ASSISTANCE_SONIFICATION = 13,
    GAME = 14,
    VIRTUAL_SOURCE = 15,
    ASSISTANT = 16,
    CNT = 17,
    MAX = 16, // (CNT - 1)
};

struct AudioOffloadInfo final {
    uint32_t sampleRateHz __attribute__ ((aligned(4)));
    AudioChannelMask channelMask __attribute__ ((aligned(4)));
    AudioFormat format __attribute__ ((aligned(4)));
    AudioStreamType streamType __attribute__ ((aligned(4)));
    uint32_t bitRatePerSecond __attribute__ ((aligned(4)));
    int64_t durationMicroseconds __attribute__ ((aligned(8)));
    bool hasVideo __attribute__ ((aligned(1)));
    bool isStreaming __attribute__ ((aligned(1)));
    uint32_t bitWidth __attribute__ ((aligned(4)));
    uint32_t bufferSize __attribute__ ((aligned(4)));
    AudioUsage usage __attribute__ ((aligned(4)));
};

static_assert(offsetof(AudioOffloadInfo, sampleRateHz) == 0, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, channelMask) == 4, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, format) == 8, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, streamType) == 12, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, bitRatePerSecond) == 16, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, durationMicroseconds) == 24, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, hasVideo) == 32, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, isStreaming) == 33, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, bitWidth) == 36, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, bufferSize) == 40, "wrong offset");
static_assert(offsetof(AudioOffloadInfo, usage) == 44, "wrong offset");
static_assert(sizeof(AudioOffloadInfo) == 48, "wrong size");
static_assert(__alignof(AudioOffloadInfo) == 8, "wrong alignment");

struct AudioConfig final {
    uint32_t sampleRateHz __attribute__ ((aligned(4)));
    AudioChannelMask channelMask __attribute__ ((aligned(4)));
    AudioFormat format __attribute__ ((aligned(4)));
    AudioOffloadInfo offloadInfo __attribute__ ((aligned(8)));
    uint64_t frameCount __attribute__ ((aligned(8)));
};

static_assert(offsetof(AudioConfig, sampleRateHz) == 0, "wrong offset");
static_assert(offsetof(AudioConfig, channelMask) == 4, "wrong offset");
static_assert(offsetof(AudioConfig, format) == 8, "wrong offset");
static_assert(offsetof(AudioConfig, offloadInfo) == 16, "wrong offset");
static_assert(offsetof(AudioConfig, frameCount) == 64, "wrong offset");
static_assert(sizeof(AudioConfig) == 72, "wrong size");
static_assert(__alignof(AudioConfig) == 8, "wrong alignment");

enum class AudioGainMode : uint32_t {
    JOINT = 1u, // 0x1
    CHANNELS = 2u, // 0x2
    RAMP = 4u, // 0x4
};

struct AudioGain final {
    AudioGainMode mode __attribute__ ((aligned(4)));
    AudioChannelMask channelMask __attribute__ ((aligned(4)));
    int32_t minValue __attribute__ ((aligned(4)));
    int32_t maxValue __attribute__ ((aligned(4)));
    int32_t defaultValue __attribute__ ((aligned(4)));
    uint32_t stepValue __attribute__ ((aligned(4)));
    uint32_t minRampMs __attribute__ ((aligned(4)));
    uint32_t maxRampMs __attribute__ ((aligned(4)));
};

static_assert(offsetof(AudioGain, mode) == 0, "wrong offset");
static_assert(offsetof(AudioGain, channelMask) == 4, "wrong offset");
static_assert(offsetof(AudioGain, minValue) == 8, "wrong offset");
static_assert(offsetof(AudioGain, maxValue) == 12, "wrong offset");
static_assert(offsetof(AudioGain, defaultValue) == 16, "wrong offset");
static_assert(offsetof(AudioGain, stepValue) == 20, "wrong offset");
static_assert(offsetof(AudioGain, minRampMs) == 24, "wrong offset");
static_assert(offsetof(AudioGain, maxRampMs) == 28, "wrong offset");
static_assert(sizeof(AudioGain) == 32, "wrong size");
static_assert(__alignof(AudioGain) == 4, "wrong alignment");

struct AudioGainConfig final {
    int32_t index __attribute__ ((aligned(4)));
    AudioGainMode mode __attribute__ ((aligned(4)));
    AudioChannelMask channelMask __attribute__ ((aligned(4)));
    ::android::hardware::hidl_array<int32_t, 32 /* (4 * 8) */> values __attribute__ ((aligned(4)));
    uint32_t rampDurationMs __attribute__ ((aligned(4)));
};

static_assert(offsetof(AudioGainConfig, index) == 0, "wrong offset");
static_assert(offsetof(AudioGainConfig, mode) == 4, "wrong offset");
static_assert(offsetof(AudioGainConfig, channelMask) == 8, "wrong offset");
static_assert(offsetof(AudioGainConfig, values) == 12, "wrong offset");
static_assert(offsetof(AudioGainConfig, rampDurationMs) == 140, "wrong offset");
static_assert(sizeof(AudioGainConfig) == 144, "wrong size");
static_assert(__alignof(AudioGainConfig) == 4, "wrong alignment");

enum class AudioPortRole : int32_t {
    NONE = 0,
    SOURCE = 1,
    SINK = 2,
};

enum class AudioPortType : int32_t {
    NONE = 0,
    DEVICE = 1,
    MIX = 2,
    SESSION = 3,
};

struct AudioPortConfigDeviceExt final {
    int32_t hwModule __attribute__ ((aligned(4)));
    AudioDevice type __attribute__ ((aligned(4)));
    ::android::hardware::hidl_array<uint8_t, 32> address __attribute__ ((aligned(1)));
};

static_assert(offsetof(AudioPortConfigDeviceExt, hwModule) == 0, "wrong offset");
static_assert(offsetof(AudioPortConfigDeviceExt, type) == 4, "wrong offset");
static_assert(offsetof(AudioPortConfigDeviceExt, address) == 8, "wrong offset");
static_assert(sizeof(AudioPortConfigDeviceExt) == 40, "wrong size");
static_assert(__alignof(AudioPortConfigDeviceExt) == 4, "wrong alignment");

struct AudioPortConfigSessionExt final {
    int32_t session __attribute__ ((aligned(4)));
};

static_assert(offsetof(AudioPortConfigSessionExt, session) == 0, "wrong offset");
static_assert(sizeof(AudioPortConfigSessionExt) == 4, "wrong size");
static_assert(__alignof(AudioPortConfigSessionExt) == 4, "wrong alignment");

enum class AudioPortConfigMask : uint32_t {
    SAMPLE_RATE = 1u, // 0x1
    CHANNEL_MASK = 2u, // 0x2
    FORMAT = 4u, // 0x4
    GAIN = 8u, // 0x8
    ALL = 15u, // (((SAMPLE_RATE | CHANNEL_MASK) | FORMAT) | GAIN)
};

struct AudioPortConfig final {
    union Ext final {
        struct AudioPortConfigMixExt final {
            union UseCase final {
                AudioStreamType stream __attribute__ ((aligned(4)));
                AudioSource source __attribute__ ((aligned(4)));
            };

            static_assert(offsetof(AudioPortConfig::Ext::AudioPortConfigMixExt::UseCase, stream) == 0, "wrong offset");
            static_assert(offsetof(AudioPortConfig::Ext::AudioPortConfigMixExt::UseCase, source) == 0, "wrong offset");
            static_assert(sizeof(AudioPortConfig::Ext::AudioPortConfigMixExt::UseCase) == 4, "wrong size");
            static_assert(__alignof(AudioPortConfig::Ext::AudioPortConfigMixExt::UseCase) == 4, "wrong alignment");

            int32_t hwModule __attribute__ ((aligned(4)));
            int32_t ioHandle __attribute__ ((aligned(4)));
            AudioPortConfig::Ext::AudioPortConfigMixExt::UseCase useCase __attribute__ ((aligned(4)));
        };

        static_assert(offsetof(AudioPortConfig::Ext::AudioPortConfigMixExt, hwModule) == 0, "wrong offset");
        static_assert(offsetof(AudioPortConfig::Ext::AudioPortConfigMixExt, ioHandle) == 4, "wrong offset");
        static_assert(offsetof(AudioPortConfig::Ext::AudioPortConfigMixExt, useCase) == 8, "wrong offset");
        static_assert(sizeof(AudioPortConfig::Ext::AudioPortConfigMixExt) == 12, "wrong size");
        static_assert(__alignof(AudioPortConfig::Ext::AudioPortConfigMixExt) == 4, "wrong alignment");

        AudioPortConfigDeviceExt device __attribute__ ((aligned(4)));
        AudioPortConfig::Ext::AudioPortConfigMixExt mix __attribute__ ((aligned(4)));
        AudioPortConfigSessionExt session __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(AudioPortConfig::Ext, device) == 0, "wrong offset");
    static_assert(offsetof(AudioPortConfig::Ext, mix) == 0, "wrong offset");
    static_assert(offsetof(AudioPortConfig::Ext, session) == 0, "wrong offset");
    static_assert(sizeof(AudioPortConfig::Ext) == 40, "wrong size");
    static_assert(__alignof(AudioPortConfig::Ext) == 4, "wrong alignment");

    int32_t id __attribute__ ((aligned(4)));
    AudioPortConfigMask configMask __attribute__ ((aligned(4)));
    uint32_t sampleRateHz __attribute__ ((aligned(4)));
    AudioChannelMask channelMask __attribute__ ((aligned(4)));
    AudioFormat format __attribute__ ((aligned(4)));
    AudioGainConfig gain __attribute__ ((aligned(4)));
    AudioPortType type __attribute__ ((aligned(4)));
    AudioPortRole role __attribute__ ((aligned(4)));
    AudioPortConfig::Ext ext __attribute__ ((aligned(4)));
};

static_assert(offsetof(AudioPortConfig, id) == 0, "wrong offset");
static_assert(offsetof(AudioPortConfig, configMask) == 4, "wrong offset");
static_assert(offsetof(AudioPortConfig, sampleRateHz) == 8, "wrong offset");
static_assert(offsetof(AudioPortConfig, channelMask) == 12, "wrong offset");
static_assert(offsetof(AudioPortConfig, format) == 16, "wrong offset");
static_assert(offsetof(AudioPortConfig, gain) == 20, "wrong offset");
static_assert(offsetof(AudioPortConfig, type) == 164, "wrong offset");
static_assert(offsetof(AudioPortConfig, role) == 168, "wrong offset");
static_assert(offsetof(AudioPortConfig, ext) == 172, "wrong offset");
static_assert(sizeof(AudioPortConfig) == 212, "wrong size");
static_assert(__alignof(AudioPortConfig) == 4, "wrong alignment");

struct AudioPortDeviceExt final {
    int32_t hwModule __attribute__ ((aligned(4)));
    AudioDevice type __attribute__ ((aligned(4)));
    ::android::hardware::hidl_array<uint8_t, 32> address __attribute__ ((aligned(1)));
};

static_assert(offsetof(AudioPortDeviceExt, hwModule) == 0, "wrong offset");
static_assert(offsetof(AudioPortDeviceExt, type) == 4, "wrong offset");
static_assert(offsetof(AudioPortDeviceExt, address) == 8, "wrong offset");
static_assert(sizeof(AudioPortDeviceExt) == 40, "wrong size");
static_assert(__alignof(AudioPortDeviceExt) == 4, "wrong alignment");

enum class AudioMixLatencyClass : int32_t {
    LOW = 0,
    NORMAL = 1,
};

struct AudioPortMixExt final {
    int32_t hwModule __attribute__ ((aligned(4)));
    int32_t ioHandle __attribute__ ((aligned(4)));
    AudioMixLatencyClass latencyClass __attribute__ ((aligned(4)));
};

static_assert(offsetof(AudioPortMixExt, hwModule) == 0, "wrong offset");
static_assert(offsetof(AudioPortMixExt, ioHandle) == 4, "wrong offset");
static_assert(offsetof(AudioPortMixExt, latencyClass) == 8, "wrong offset");
static_assert(sizeof(AudioPortMixExt) == 12, "wrong size");
static_assert(__alignof(AudioPortMixExt) == 4, "wrong alignment");

struct AudioPortSessionExt final {
    int32_t session __attribute__ ((aligned(4)));
};

static_assert(offsetof(AudioPortSessionExt, session) == 0, "wrong offset");
static_assert(sizeof(AudioPortSessionExt) == 4, "wrong size");
static_assert(__alignof(AudioPortSessionExt) == 4, "wrong alignment");

struct AudioPort final {
    union Ext final {
        AudioPortDeviceExt device __attribute__ ((aligned(4)));
        AudioPortMixExt mix __attribute__ ((aligned(4)));
        AudioPortSessionExt session __attribute__ ((aligned(4)));
    };

    static_assert(offsetof(AudioPort::Ext, device) == 0, "wrong offset");
    static_assert(offsetof(AudioPort::Ext, mix) == 0, "wrong offset");
    static_assert(offsetof(AudioPort::Ext, session) == 0, "wrong offset");
    static_assert(sizeof(AudioPort::Ext) == 40, "wrong size");
    static_assert(__alignof(AudioPort::Ext) == 4, "wrong alignment");

    int32_t id __attribute__ ((aligned(4)));
    AudioPortRole role __attribute__ ((aligned(4)));
    ::android::hardware::hidl_string name __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<uint32_t> sampleRates __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<AudioChannelMask> channelMasks __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<AudioFormat> formats __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<AudioGain> gains __attribute__ ((aligned(8)));
    AudioPortConfig activeConfig __attribute__ ((aligned(4)));
    AudioPortType type __attribute__ ((aligned(4)));
    AudioPort::Ext ext __attribute__ ((aligned(4)));
};

static_assert(offsetof(AudioPort, id) == 0, "wrong offset");
static_assert(offsetof(AudioPort, role) == 4, "wrong offset");
static_assert(offsetof(AudioPort, name) == 8, "wrong offset");
static_assert(offsetof(AudioPort, sampleRates) == 24, "wrong offset");
static_assert(offsetof(AudioPort, channelMasks) == 40, "wrong offset");
static_assert(offsetof(AudioPort, formats) == 56, "wrong offset");
static_assert(offsetof(AudioPort, gains) == 72, "wrong offset");
static_assert(offsetof(AudioPort, activeConfig) == 88, "wrong offset");
static_assert(offsetof(AudioPort, type) == 300, "wrong offset");
static_assert(offsetof(AudioPort, ext) == 304, "wrong offset");
static_assert(sizeof(AudioPort) == 344, "wrong size");
static_assert(__alignof(AudioPort) == 8, "wrong alignment");

struct ThreadInfo final {
    int64_t pid __attribute__ ((aligned(8)));
    int64_t tid __attribute__ ((aligned(8)));
};

static_assert(offsetof(ThreadInfo, pid) == 0, "wrong offset");
static_assert(offsetof(ThreadInfo, tid) == 8, "wrong offset");
static_assert(sizeof(ThreadInfo) == 16, "wrong size");
static_assert(__alignof(ThreadInfo) == 8, "wrong alignment");

constexpr int32_t operator|(const AudioHandleConsts lhs, const AudioHandleConsts rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioHandleConsts rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioHandleConsts lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioHandleConsts lhs, const AudioHandleConsts rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioHandleConsts rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioHandleConsts lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioHandleConsts e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioHandleConsts e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioHandleConsts>(int32_t o);

std::string toString(AudioHandleConsts o);

std::string toString(const Uuid&);

bool operator==(const Uuid&, const Uuid&);

bool operator!=(const Uuid&, const Uuid&);

constexpr int32_t operator|(const AudioStreamType lhs, const AudioStreamType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioStreamType rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioStreamType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioStreamType lhs, const AudioStreamType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioStreamType rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioStreamType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioStreamType e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioStreamType e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioStreamType>(int32_t o);

std::string toString(AudioStreamType o);

constexpr int32_t operator|(const AudioSource lhs, const AudioSource rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioSource rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioSource lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioSource lhs, const AudioSource rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioSource rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioSource lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioSource e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioSource e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioSource>(int32_t o);

std::string toString(AudioSource o);

constexpr int32_t operator|(const AudioSessionConsts lhs, const AudioSessionConsts rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioSessionConsts rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioSessionConsts lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioSessionConsts lhs, const AudioSessionConsts rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioSessionConsts rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioSessionConsts lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioSessionConsts e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioSessionConsts e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioSessionConsts>(int32_t o);

std::string toString(AudioSessionConsts o);

constexpr uint32_t operator|(const AudioFormat lhs, const AudioFormat rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const AudioFormat rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const AudioFormat lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const AudioFormat lhs, const AudioFormat rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const AudioFormat rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const AudioFormat lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const AudioFormat e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const AudioFormat e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<AudioFormat>(uint32_t o);

std::string toString(AudioFormat o);

constexpr int32_t operator|(const FixedChannelCount lhs, const FixedChannelCount rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const FixedChannelCount rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const FixedChannelCount lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const FixedChannelCount lhs, const FixedChannelCount rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const FixedChannelCount rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const FixedChannelCount lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const FixedChannelCount e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const FixedChannelCount e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<FixedChannelCount>(int32_t o);

std::string toString(FixedChannelCount o);

constexpr uint32_t operator|(const AudioChannelMask lhs, const AudioChannelMask rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const AudioChannelMask rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const AudioChannelMask lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const AudioChannelMask lhs, const AudioChannelMask rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const AudioChannelMask rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const AudioChannelMask lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const AudioChannelMask e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const AudioChannelMask e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<AudioChannelMask>(uint32_t o);

std::string toString(AudioChannelMask o);

constexpr int32_t operator|(const AudioInterleave lhs, const AudioInterleave rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioInterleave rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioInterleave lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioInterleave lhs, const AudioInterleave rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioInterleave rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioInterleave lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioInterleave e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioInterleave e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioInterleave>(int32_t o);

std::string toString(AudioInterleave o);

constexpr int32_t operator|(const AudioMode lhs, const AudioMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioMode rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioMode lhs, const AudioMode rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioMode rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioMode lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioMode e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioMode e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioMode>(int32_t o);

std::string toString(AudioMode o);

constexpr uint32_t operator|(const AudioDevice lhs, const AudioDevice rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const AudioDevice rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const AudioDevice lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const AudioDevice lhs, const AudioDevice rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const AudioDevice rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const AudioDevice lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const AudioDevice e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const AudioDevice e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<AudioDevice>(uint32_t o);

std::string toString(AudioDevice o);

constexpr int32_t operator|(const AudioOutputFlag lhs, const AudioOutputFlag rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioOutputFlag rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioOutputFlag lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioOutputFlag lhs, const AudioOutputFlag rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioOutputFlag rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioOutputFlag lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioOutputFlag e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioOutputFlag e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioOutputFlag>(int32_t o);

std::string toString(AudioOutputFlag o);

constexpr int32_t operator|(const AudioInputFlag lhs, const AudioInputFlag rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioInputFlag rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioInputFlag lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioInputFlag lhs, const AudioInputFlag rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioInputFlag rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioInputFlag lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioInputFlag e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioInputFlag e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioInputFlag>(int32_t o);

std::string toString(AudioInputFlag o);

constexpr int32_t operator|(const AudioUsage lhs, const AudioUsage rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioUsage rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioUsage lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioUsage lhs, const AudioUsage rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioUsage rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioUsage lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioUsage e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioUsage e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioUsage>(int32_t o);

std::string toString(AudioUsage o);

std::string toString(const AudioOffloadInfo&);

bool operator==(const AudioOffloadInfo&, const AudioOffloadInfo&);

bool operator!=(const AudioOffloadInfo&, const AudioOffloadInfo&);

std::string toString(const AudioConfig&);

bool operator==(const AudioConfig&, const AudioConfig&);

bool operator!=(const AudioConfig&, const AudioConfig&);

constexpr uint32_t operator|(const AudioGainMode lhs, const AudioGainMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const AudioGainMode rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const AudioGainMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const AudioGainMode lhs, const AudioGainMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const AudioGainMode rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const AudioGainMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const AudioGainMode e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const AudioGainMode e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<AudioGainMode>(uint32_t o);

std::string toString(AudioGainMode o);

std::string toString(const AudioGain&);

bool operator==(const AudioGain&, const AudioGain&);

bool operator!=(const AudioGain&, const AudioGain&);

std::string toString(const AudioGainConfig&);

bool operator==(const AudioGainConfig&, const AudioGainConfig&);

bool operator!=(const AudioGainConfig&, const AudioGainConfig&);

constexpr int32_t operator|(const AudioPortRole lhs, const AudioPortRole rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioPortRole rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioPortRole lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioPortRole lhs, const AudioPortRole rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioPortRole rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioPortRole lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioPortRole e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioPortRole e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioPortRole>(int32_t o);

std::string toString(AudioPortRole o);

constexpr int32_t operator|(const AudioPortType lhs, const AudioPortType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioPortType rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioPortType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioPortType lhs, const AudioPortType rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioPortType rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioPortType lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioPortType e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioPortType e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioPortType>(int32_t o);

std::string toString(AudioPortType o);

std::string toString(const AudioPortConfigDeviceExt&);

bool operator==(const AudioPortConfigDeviceExt&, const AudioPortConfigDeviceExt&);

bool operator!=(const AudioPortConfigDeviceExt&, const AudioPortConfigDeviceExt&);

std::string toString(const AudioPortConfigSessionExt&);

bool operator==(const AudioPortConfigSessionExt&, const AudioPortConfigSessionExt&);

bool operator!=(const AudioPortConfigSessionExt&, const AudioPortConfigSessionExt&);

constexpr uint32_t operator|(const AudioPortConfigMask lhs, const AudioPortConfigMask rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const AudioPortConfigMask rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const AudioPortConfigMask lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const AudioPortConfigMask lhs, const AudioPortConfigMask rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const AudioPortConfigMask rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const AudioPortConfigMask lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const AudioPortConfigMask e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const AudioPortConfigMask e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<AudioPortConfigMask>(uint32_t o);

std::string toString(AudioPortConfigMask o);

std::string toString(const AudioPortConfig::Ext::AudioPortConfigMixExt::UseCase&);

// operator== and operator!= are not generated for UseCase

std::string toString(const AudioPortConfig::Ext::AudioPortConfigMixExt&);

// operator== and operator!= are not generated for AudioPortConfigMixExt

std::string toString(const AudioPortConfig::Ext&);

// operator== and operator!= are not generated for Ext

std::string toString(const AudioPortConfig&);

// operator== and operator!= are not generated for AudioPortConfig

std::string toString(const AudioPortDeviceExt&);

bool operator==(const AudioPortDeviceExt&, const AudioPortDeviceExt&);

bool operator!=(const AudioPortDeviceExt&, const AudioPortDeviceExt&);

constexpr int32_t operator|(const AudioMixLatencyClass lhs, const AudioMixLatencyClass rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const int32_t lhs, const AudioMixLatencyClass rhs) {
    return static_cast<int32_t>(lhs | static_cast<int32_t>(rhs));
}

constexpr int32_t operator|(const AudioMixLatencyClass lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) | rhs);
}

constexpr int32_t operator&(const AudioMixLatencyClass lhs, const AudioMixLatencyClass rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const int32_t lhs, const AudioMixLatencyClass rhs) {
    return static_cast<int32_t>(lhs & static_cast<int32_t>(rhs));
}

constexpr int32_t operator&(const AudioMixLatencyClass lhs, const int32_t rhs) {
    return static_cast<int32_t>(static_cast<int32_t>(lhs) & rhs);
}

constexpr int32_t &operator|=(int32_t& v, const AudioMixLatencyClass e) {
    v |= static_cast<int32_t>(e);
    return v;
}

constexpr int32_t &operator&=(int32_t& v, const AudioMixLatencyClass e) {
    v &= static_cast<int32_t>(e);
    return v;
}

template<typename>
std::string toString(int32_t o);
template<>
std::string toString<AudioMixLatencyClass>(int32_t o);

std::string toString(AudioMixLatencyClass o);

std::string toString(const AudioPortMixExt&);

bool operator==(const AudioPortMixExt&, const AudioPortMixExt&);

bool operator!=(const AudioPortMixExt&, const AudioPortMixExt&);

std::string toString(const AudioPortSessionExt&);

bool operator==(const AudioPortSessionExt&, const AudioPortSessionExt&);

bool operator!=(const AudioPortSessionExt&, const AudioPortSessionExt&);

std::string toString(const AudioPort::Ext&);

// operator== and operator!= are not generated for Ext

std::string toString(const AudioPort&);

// operator== and operator!= are not generated for AudioPort

std::string toString(const ThreadInfo&);

bool operator==(const ThreadInfo&, const ThreadInfo&);

bool operator!=(const ThreadInfo&, const ThreadInfo&);


}  // namespace V2_0
}  // namespace common
}  // namespace audio
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_AUDIO_COMMON_V2_0_TYPES_H
