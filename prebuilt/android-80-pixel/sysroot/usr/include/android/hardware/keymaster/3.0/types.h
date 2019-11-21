#ifndef HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_TYPES_H
#define HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_TYPES_H

#include <hidl/HidlSupport.h>
#include <hidl/MQDescriptor.h>
#include <utils/NativeHandle.h>
#include <utils/misc.h>

namespace android {
namespace hardware {
namespace keymaster {
namespace V3_0 {

enum class TagType : uint32_t {
    INVALID = 0u, // (0 << 28)
    ENUM = 268435456u, // (1 << 28)
    ENUM_REP = 536870912u, // (2 << 28)
    UINT = 805306368u, // (3 << 28)
    UINT_REP = 1073741824u, // (4 << 28)
    ULONG = 1342177280u, // (5 << 28)
    DATE = 1610612736u, // (6 << 28)
    BOOL = 1879048192u, // (7 << 28)
    BIGNUM = 2147483648u, // (8 << 28)
    BYTES = 2415919104u, // (9 << 28)
    ULONG_REP = 2684354560u, // (10 << 28)
};

enum class Tag : uint32_t {
    INVALID = 0u, // (TagType:INVALID | 0)
    PURPOSE = 536870913u, // (TagType:ENUM_REP | 1)
    ALGORITHM = 268435458u, // (TagType:ENUM | 2)
    KEY_SIZE = 805306371u, // (TagType:UINT | 3)
    BLOCK_MODE = 536870916u, // (TagType:ENUM_REP | 4)
    DIGEST = 536870917u, // (TagType:ENUM_REP | 5)
    PADDING = 536870918u, // (TagType:ENUM_REP | 6)
    CALLER_NONCE = 1879048199u, // (TagType:BOOL | 7)
    MIN_MAC_LENGTH = 805306376u, // (TagType:UINT | 8)
    KDF = 536870921u, // (TagType:ENUM_REP | 9)
    EC_CURVE = 268435466u, // (TagType:ENUM | 10)
    RSA_PUBLIC_EXPONENT = 1342177480u, // (TagType:ULONG | 200)
    ECIES_SINGLE_HASH_MODE = 1879048393u, // (TagType:BOOL | 201)
    INCLUDE_UNIQUE_ID = 1879048394u, // (TagType:BOOL | 202)
    BLOB_USAGE_REQUIREMENTS = 268435757u, // (TagType:ENUM | 301)
    BOOTLOADER_ONLY = 1879048494u, // (TagType:BOOL | 302)
    ACTIVE_DATETIME = 1610613136u, // (TagType:DATE | 400)
    ORIGINATION_EXPIRE_DATETIME = 1610613137u, // (TagType:DATE | 401)
    USAGE_EXPIRE_DATETIME = 1610613138u, // (TagType:DATE | 402)
    MIN_SECONDS_BETWEEN_OPS = 805306771u, // (TagType:UINT | 403)
    MAX_USES_PER_BOOT = 805306772u, // (TagType:UINT | 404)
    ALL_USERS = 1879048692u, // (TagType:BOOL | 500)
    USER_ID = 805306869u, // (TagType:UINT | 501)
    USER_SECURE_ID = 2684355062u, // (TagType:ULONG_REP | 502)
    NO_AUTH_REQUIRED = 1879048695u, // (TagType:BOOL | 503)
    USER_AUTH_TYPE = 268435960u, // (TagType:ENUM | 504)
    AUTH_TIMEOUT = 805306873u, // (TagType:UINT | 505)
    ALLOW_WHILE_ON_BODY = 1879048698u, // (TagType:BOOL | 506)
    ALL_APPLICATIONS = 1879048792u, // (TagType:BOOL | 600)
    APPLICATION_ID = 2415919705u, // (TagType:BYTES | 601)
    EXPORTABLE = 1879048794u, // (TagType:BOOL | 602)
    APPLICATION_DATA = 2415919804u, // (TagType:BYTES | 700)
    CREATION_DATETIME = 1610613437u, // (TagType:DATE | 701)
    ORIGIN = 268436158u, // (TagType:ENUM | 702)
    ROLLBACK_RESISTANT = 1879048895u, // (TagType:BOOL | 703)
    ROOT_OF_TRUST = 2415919808u, // (TagType:BYTES | 704)
    OS_VERSION = 805307073u, // (TagType:UINT | 705)
    OS_PATCHLEVEL = 805307074u, // (TagType:UINT | 706)
    UNIQUE_ID = 2415919811u, // (TagType:BYTES | 707)
    ATTESTATION_CHALLENGE = 2415919812u, // (TagType:BYTES | 708)
    ATTESTATION_APPLICATION_ID = 2415919813u, // (TagType:BYTES | 709)
    ATTESTATION_ID_BRAND = 2415919814u, // (TagType:BYTES | 710)
    ATTESTATION_ID_DEVICE = 2415919815u, // (TagType:BYTES | 711)
    ATTESTATION_ID_PRODUCT = 2415919816u, // (TagType:BYTES | 712)
    ATTESTATION_ID_SERIAL = 2415919817u, // (TagType:BYTES | 713)
    ATTESTATION_ID_IMEI = 2415919818u, // (TagType:BYTES | 714)
    ATTESTATION_ID_MEID = 2415919819u, // (TagType:BYTES | 715)
    ATTESTATION_ID_MANUFACTURER = 2415919820u, // (TagType:BYTES | 716)
    ATTESTATION_ID_MODEL = 2415919821u, // (TagType:BYTES | 717)
    ASSOCIATED_DATA = 2415920104u, // (TagType:BYTES | 1000)
    NONCE = 2415920105u, // (TagType:BYTES | 1001)
    AUTH_TOKEN = 2415920106u, // (TagType:BYTES | 1002)
    MAC_LENGTH = 805307371u, // (TagType:UINT | 1003)
    RESET_SINCE_ID_ROTATION = 1879049196u, // (TagType:BOOL | 1004)
};

enum class Algorithm : uint32_t {
    RSA = 1u, // 1
    EC = 3u, // 3
    AES = 32u, // 32
    HMAC = 128u, // 128
};

enum class BlockMode : uint32_t {
    ECB = 1u, // 1
    CBC = 2u, // 2
    CTR = 3u, // 3
    GCM = 32u, // 32
};

enum class PaddingMode : uint32_t {
    NONE = 1u, // 1
    RSA_OAEP = 2u, // 2
    RSA_PSS = 3u, // 3
    RSA_PKCS1_1_5_ENCRYPT = 4u, // 4
    RSA_PKCS1_1_5_SIGN = 5u, // 5
    PKCS7 = 64u, // 64
};

enum class Digest : uint32_t {
    NONE = 0u, // 0
    MD5 = 1u, // 1
    SHA1 = 2u, // 2
    SHA_2_224 = 3u, // 3
    SHA_2_256 = 4u, // 4
    SHA_2_384 = 5u, // 5
    SHA_2_512 = 6u, // 6
};

enum class EcCurve : uint32_t {
    P_224 = 0u, // 0
    P_256 = 1u, // 1
    P_384 = 2u, // 2
    P_521 = 3u, // 3
};

enum class KeyOrigin : uint32_t {
    GENERATED = 0u, // 0
    DERIVED = 1u, // 1
    IMPORTED = 2u, // 2
    UNKNOWN = 3u, // 3
};

enum class KeyBlobUsageRequirements : uint32_t {
    STANDALONE = 0u, // 0
    REQUIRES_FILE_SYSTEM = 1u, // 1
};

enum class KeyPurpose : uint32_t {
    ENCRYPT = 0u, // 0
    DECRYPT = 1u, // 1
    SIGN = 2u, // 2
    VERIFY = 3u, // 3
    DERIVE_KEY = 4u, // 4
    WRAP_KEY = 5u, // 5
};

enum class ErrorCode : uint32_t {
    OK = 0u, // 0
    ROOT_OF_TRUST_ALREADY_SET = 4294967295u, // (-1)
    UNSUPPORTED_PURPOSE = 4294967294u, // (-2)
    INCOMPATIBLE_PURPOSE = 4294967293u, // (-3)
    UNSUPPORTED_ALGORITHM = 4294967292u, // (-4)
    INCOMPATIBLE_ALGORITHM = 4294967291u, // (-5)
    UNSUPPORTED_KEY_SIZE = 4294967290u, // (-6)
    UNSUPPORTED_BLOCK_MODE = 4294967289u, // (-7)
    INCOMPATIBLE_BLOCK_MODE = 4294967288u, // (-8)
    UNSUPPORTED_MAC_LENGTH = 4294967287u, // (-9)
    UNSUPPORTED_PADDING_MODE = 4294967286u, // (-10)
    INCOMPATIBLE_PADDING_MODE = 4294967285u, // (-11)
    UNSUPPORTED_DIGEST = 4294967284u, // (-12)
    INCOMPATIBLE_DIGEST = 4294967283u, // (-13)
    INVALID_EXPIRATION_TIME = 4294967282u, // (-14)
    INVALID_USER_ID = 4294967281u, // (-15)
    INVALID_AUTHORIZATION_TIMEOUT = 4294967280u, // (-16)
    UNSUPPORTED_KEY_FORMAT = 4294967279u, // (-17)
    INCOMPATIBLE_KEY_FORMAT = 4294967278u, // (-18)
    UNSUPPORTED_KEY_ENCRYPTION_ALGORITHM = 4294967277u, // (-19)
    UNSUPPORTED_KEY_VERIFICATION_ALGORITHM = 4294967276u, // (-20)
    INVALID_INPUT_LENGTH = 4294967275u, // (-21)
    KEY_EXPORT_OPTIONS_INVALID = 4294967274u, // (-22)
    DELEGATION_NOT_ALLOWED = 4294967273u, // (-23)
    KEY_NOT_YET_VALID = 4294967272u, // (-24)
    KEY_EXPIRED = 4294967271u, // (-25)
    KEY_USER_NOT_AUTHENTICATED = 4294967270u, // (-26)
    OUTPUT_PARAMETER_NULL = 4294967269u, // (-27)
    INVALID_OPERATION_HANDLE = 4294967268u, // (-28)
    INSUFFICIENT_BUFFER_SPACE = 4294967267u, // (-29)
    VERIFICATION_FAILED = 4294967266u, // (-30)
    TOO_MANY_OPERATIONS = 4294967265u, // (-31)
    UNEXPECTED_NULL_POINTER = 4294967264u, // (-32)
    INVALID_KEY_BLOB = 4294967263u, // (-33)
    IMPORTED_KEY_NOT_ENCRYPTED = 4294967262u, // (-34)
    IMPORTED_KEY_DECRYPTION_FAILED = 4294967261u, // (-35)
    IMPORTED_KEY_NOT_SIGNED = 4294967260u, // (-36)
    IMPORTED_KEY_VERIFICATION_FAILED = 4294967259u, // (-37)
    INVALID_ARGUMENT = 4294967258u, // (-38)
    UNSUPPORTED_TAG = 4294967257u, // (-39)
    INVALID_TAG = 4294967256u, // (-40)
    MEMORY_ALLOCATION_FAILED = 4294967255u, // (-41)
    IMPORT_PARAMETER_MISMATCH = 4294967252u, // (-44)
    SECURE_HW_ACCESS_DENIED = 4294967251u, // (-45)
    OPERATION_CANCELLED = 4294967250u, // (-46)
    CONCURRENT_ACCESS_CONFLICT = 4294967249u, // (-47)
    SECURE_HW_BUSY = 4294967248u, // (-48)
    SECURE_HW_COMMUNICATION_FAILED = 4294967247u, // (-49)
    UNSUPPORTED_EC_FIELD = 4294967246u, // (-50)
    MISSING_NONCE = 4294967245u, // (-51)
    INVALID_NONCE = 4294967244u, // (-52)
    MISSING_MAC_LENGTH = 4294967243u, // (-53)
    KEY_RATE_LIMIT_EXCEEDED = 4294967242u, // (-54)
    CALLER_NONCE_PROHIBITED = 4294967241u, // (-55)
    KEY_MAX_OPS_EXCEEDED = 4294967240u, // (-56)
    INVALID_MAC_LENGTH = 4294967239u, // (-57)
    MISSING_MIN_MAC_LENGTH = 4294967238u, // (-58)
    UNSUPPORTED_MIN_MAC_LENGTH = 4294967237u, // (-59)
    UNSUPPORTED_KDF = 4294967236u, // (-60)
    UNSUPPORTED_EC_CURVE = 4294967235u, // (-61)
    KEY_REQUIRES_UPGRADE = 4294967234u, // (-62)
    ATTESTATION_CHALLENGE_MISSING = 4294967233u, // (-63)
    KEYMASTER_NOT_CONFIGURED = 4294967232u, // (-64)
    ATTESTATION_APPLICATION_ID_MISSING = 4294967231u, // (-65)
    CANNOT_ATTEST_IDS = 4294967230u, // (-66)
    UNIMPLEMENTED = 4294967196u, // (-100)
    VERSION_MISMATCH = 4294967195u, // (-101)
    UNKNOWN_ERROR = 4294966296u, // (-1000)
};

enum class KeyDerivationFunction : uint32_t {
    NONE = 0u, // 0
    RFC5869_SHA256 = 1u, // 1
    ISO18033_2_KDF1_SHA1 = 2u, // 2
    ISO18033_2_KDF1_SHA256 = 3u, // 3
    ISO18033_2_KDF2_SHA1 = 4u, // 4
    ISO18033_2_KDF2_SHA256 = 5u, // 5
};

enum class HardwareAuthenticatorType : uint32_t {
    NONE = 0u, // 0
    PASSWORD = 1u, // (1 << 0)
    FINGERPRINT = 2u, // (1 << 1)
    ANY = 4294967295u, // 0xFFFFFFFF
};

struct KeyParameter final {
    union IntegerParams final {
        Algorithm algorithm __attribute__ ((aligned(4)));
        BlockMode blockMode __attribute__ ((aligned(4)));
        PaddingMode paddingMode __attribute__ ((aligned(4)));
        Digest digest __attribute__ ((aligned(4)));
        EcCurve ecCurve __attribute__ ((aligned(4)));
        KeyOrigin origin __attribute__ ((aligned(4)));
        KeyBlobUsageRequirements keyBlobUsageRequirements __attribute__ ((aligned(4)));
        KeyPurpose purpose __attribute__ ((aligned(4)));
        KeyDerivationFunction keyDerivationFunction __attribute__ ((aligned(4)));
        HardwareAuthenticatorType hardwareAuthenticatorType __attribute__ ((aligned(4)));
        bool boolValue __attribute__ ((aligned(1)));
        uint32_t integer __attribute__ ((aligned(4)));
        uint64_t longInteger __attribute__ ((aligned(8)));
        uint64_t dateTime __attribute__ ((aligned(8)));
    };

    static_assert(offsetof(KeyParameter::IntegerParams, algorithm) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, blockMode) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, paddingMode) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, digest) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, ecCurve) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, origin) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, keyBlobUsageRequirements) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, purpose) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, keyDerivationFunction) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, hardwareAuthenticatorType) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, boolValue) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, integer) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, longInteger) == 0, "wrong offset");
    static_assert(offsetof(KeyParameter::IntegerParams, dateTime) == 0, "wrong offset");
    static_assert(sizeof(KeyParameter::IntegerParams) == 8, "wrong size");
    static_assert(__alignof(KeyParameter::IntegerParams) == 8, "wrong alignment");

    Tag tag __attribute__ ((aligned(4)));
    KeyParameter::IntegerParams f __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<uint8_t> blob __attribute__ ((aligned(8)));
};

static_assert(offsetof(KeyParameter, tag) == 0, "wrong offset");
static_assert(offsetof(KeyParameter, f) == 8, "wrong offset");
static_assert(offsetof(KeyParameter, blob) == 16, "wrong offset");
static_assert(sizeof(KeyParameter) == 32, "wrong size");
static_assert(__alignof(KeyParameter) == 8, "wrong alignment");

struct KeyCharacteristics final {
    ::android::hardware::hidl_vec<KeyParameter> softwareEnforced __attribute__ ((aligned(8)));
    ::android::hardware::hidl_vec<KeyParameter> teeEnforced __attribute__ ((aligned(8)));
};

static_assert(offsetof(KeyCharacteristics, softwareEnforced) == 0, "wrong offset");
static_assert(offsetof(KeyCharacteristics, teeEnforced) == 16, "wrong offset");
static_assert(sizeof(KeyCharacteristics) == 32, "wrong size");
static_assert(__alignof(KeyCharacteristics) == 8, "wrong alignment");

struct HardwareAuthToken final {
    uint64_t challenge __attribute__ ((aligned(8)));
    uint64_t userId __attribute__ ((aligned(8)));
    uint64_t authenticatorId __attribute__ ((aligned(8)));
    uint32_t authenticatorType __attribute__ ((aligned(4)));
    uint64_t timestamp __attribute__ ((aligned(8)));
    ::android::hardware::hidl_array<uint8_t, 32> hmac __attribute__ ((aligned(1)));
};

static_assert(offsetof(HardwareAuthToken, challenge) == 0, "wrong offset");
static_assert(offsetof(HardwareAuthToken, userId) == 8, "wrong offset");
static_assert(offsetof(HardwareAuthToken, authenticatorId) == 16, "wrong offset");
static_assert(offsetof(HardwareAuthToken, authenticatorType) == 24, "wrong offset");
static_assert(offsetof(HardwareAuthToken, timestamp) == 32, "wrong offset");
static_assert(offsetof(HardwareAuthToken, hmac) == 40, "wrong offset");
static_assert(sizeof(HardwareAuthToken) == 72, "wrong size");
static_assert(__alignof(HardwareAuthToken) == 8, "wrong alignment");

enum class SecurityLevel : uint32_t {
    SOFTWARE = 0u, // 0
    TRUSTED_ENVIRONMENT = 1u, // 1
};

enum class KeyFormat : uint32_t {
    X509 = 0u, // 0
    PKCS8 = 1u, // 1
    RAW = 3u, // 3
};

typedef uint64_t OperationHandle;

constexpr uint32_t operator|(const TagType lhs, const TagType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const TagType rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const TagType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const TagType lhs, const TagType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const TagType rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const TagType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const TagType e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const TagType e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<TagType>(uint32_t o);

std::string toString(TagType o);

constexpr uint32_t operator|(const Tag lhs, const Tag rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const Tag rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const Tag lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const Tag lhs, const Tag rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const Tag rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const Tag lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const Tag e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const Tag e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<Tag>(uint32_t o);

std::string toString(Tag o);

constexpr uint32_t operator|(const Algorithm lhs, const Algorithm rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const Algorithm rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const Algorithm lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const Algorithm lhs, const Algorithm rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const Algorithm rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const Algorithm lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const Algorithm e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const Algorithm e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<Algorithm>(uint32_t o);

std::string toString(Algorithm o);

constexpr uint32_t operator|(const BlockMode lhs, const BlockMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const BlockMode rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const BlockMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const BlockMode lhs, const BlockMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const BlockMode rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const BlockMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const BlockMode e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const BlockMode e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<BlockMode>(uint32_t o);

std::string toString(BlockMode o);

constexpr uint32_t operator|(const PaddingMode lhs, const PaddingMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const PaddingMode rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const PaddingMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const PaddingMode lhs, const PaddingMode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const PaddingMode rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const PaddingMode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const PaddingMode e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const PaddingMode e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<PaddingMode>(uint32_t o);

std::string toString(PaddingMode o);

constexpr uint32_t operator|(const Digest lhs, const Digest rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const Digest rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const Digest lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const Digest lhs, const Digest rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const Digest rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const Digest lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const Digest e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const Digest e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<Digest>(uint32_t o);

std::string toString(Digest o);

constexpr uint32_t operator|(const EcCurve lhs, const EcCurve rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const EcCurve rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const EcCurve lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const EcCurve lhs, const EcCurve rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const EcCurve rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const EcCurve lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const EcCurve e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const EcCurve e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<EcCurve>(uint32_t o);

std::string toString(EcCurve o);

constexpr uint32_t operator|(const KeyOrigin lhs, const KeyOrigin rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const KeyOrigin rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const KeyOrigin lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const KeyOrigin lhs, const KeyOrigin rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const KeyOrigin rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const KeyOrigin lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const KeyOrigin e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const KeyOrigin e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<KeyOrigin>(uint32_t o);

std::string toString(KeyOrigin o);

constexpr uint32_t operator|(const KeyBlobUsageRequirements lhs, const KeyBlobUsageRequirements rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const KeyBlobUsageRequirements rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const KeyBlobUsageRequirements lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const KeyBlobUsageRequirements lhs, const KeyBlobUsageRequirements rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const KeyBlobUsageRequirements rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const KeyBlobUsageRequirements lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const KeyBlobUsageRequirements e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const KeyBlobUsageRequirements e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<KeyBlobUsageRequirements>(uint32_t o);

std::string toString(KeyBlobUsageRequirements o);

constexpr uint32_t operator|(const KeyPurpose lhs, const KeyPurpose rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const KeyPurpose rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const KeyPurpose lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const KeyPurpose lhs, const KeyPurpose rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const KeyPurpose rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const KeyPurpose lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const KeyPurpose e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const KeyPurpose e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<KeyPurpose>(uint32_t o);

std::string toString(KeyPurpose o);

constexpr uint32_t operator|(const ErrorCode lhs, const ErrorCode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const ErrorCode rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const ErrorCode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const ErrorCode lhs, const ErrorCode rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const ErrorCode rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const ErrorCode lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const ErrorCode e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const ErrorCode e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<ErrorCode>(uint32_t o);

std::string toString(ErrorCode o);

constexpr uint32_t operator|(const KeyDerivationFunction lhs, const KeyDerivationFunction rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const KeyDerivationFunction rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const KeyDerivationFunction lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const KeyDerivationFunction lhs, const KeyDerivationFunction rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const KeyDerivationFunction rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const KeyDerivationFunction lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const KeyDerivationFunction e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const KeyDerivationFunction e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<KeyDerivationFunction>(uint32_t o);

std::string toString(KeyDerivationFunction o);

constexpr uint32_t operator|(const HardwareAuthenticatorType lhs, const HardwareAuthenticatorType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const HardwareAuthenticatorType rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const HardwareAuthenticatorType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const HardwareAuthenticatorType lhs, const HardwareAuthenticatorType rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const HardwareAuthenticatorType rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const HardwareAuthenticatorType lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const HardwareAuthenticatorType e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const HardwareAuthenticatorType e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<HardwareAuthenticatorType>(uint32_t o);

std::string toString(HardwareAuthenticatorType o);

std::string toString(const KeyParameter::IntegerParams&);

// operator== and operator!= are not generated for IntegerParams

std::string toString(const KeyParameter&);

// operator== and operator!= are not generated for KeyParameter

std::string toString(const KeyCharacteristics&);

// operator== and operator!= are not generated for KeyCharacteristics

std::string toString(const HardwareAuthToken&);

bool operator==(const HardwareAuthToken&, const HardwareAuthToken&);

bool operator!=(const HardwareAuthToken&, const HardwareAuthToken&);

constexpr uint32_t operator|(const SecurityLevel lhs, const SecurityLevel rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const SecurityLevel rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const SecurityLevel lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const SecurityLevel lhs, const SecurityLevel rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const SecurityLevel rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const SecurityLevel lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const SecurityLevel e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const SecurityLevel e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<SecurityLevel>(uint32_t o);

std::string toString(SecurityLevel o);

constexpr uint32_t operator|(const KeyFormat lhs, const KeyFormat rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const uint32_t lhs, const KeyFormat rhs) {
    return static_cast<uint32_t>(lhs | static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator|(const KeyFormat lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) | rhs);
}

constexpr uint32_t operator&(const KeyFormat lhs, const KeyFormat rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const uint32_t lhs, const KeyFormat rhs) {
    return static_cast<uint32_t>(lhs & static_cast<uint32_t>(rhs));
}

constexpr uint32_t operator&(const KeyFormat lhs, const uint32_t rhs) {
    return static_cast<uint32_t>(static_cast<uint32_t>(lhs) & rhs);
}

constexpr uint32_t &operator|=(uint32_t& v, const KeyFormat e) {
    v |= static_cast<uint32_t>(e);
    return v;
}

constexpr uint32_t &operator&=(uint32_t& v, const KeyFormat e) {
    v &= static_cast<uint32_t>(e);
    return v;
}

template<typename>
std::string toString(uint32_t o);
template<>
std::string toString<KeyFormat>(uint32_t o);

std::string toString(KeyFormat o);


}  // namespace V3_0
}  // namespace keymaster
}  // namespace hardware
}  // namespace android

#endif  // HIDL_GENERATED_ANDROID_HARDWARE_KEYMASTER_V3_0_TYPES_H
