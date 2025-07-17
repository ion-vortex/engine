#pragma once
#include <string>
#include <string_view>
#include <cstdint>
#include <exception>
#include <oxide/core/export.h>

namespace oxide::core {

enum class OXIDE_CORE_API ErrorCode : std::uint32_t
{
    // ── Oat-generic sentinel ────────────────────────────────────────────────
    Ok      = 0x00000000, // success, no error
    Unknown = 0xFFFFFFFF, // catch-all / uninitialised

    // ── POSIX / UNIX errno ──────────────────────────────────────────────────
    OperationNotPermitted                       =   1,  // EPERM
    NoSuchFileOrDirectory                       =   2,  // ENOENT
    NoSuchProcess                               =   3,  // ESRCH
    InterruptedSystemCall                       =   4,  // EINTR
    InputOutputError                            =   5,  // EIO
    NoSuchDeviceOrAddress                       =   6,  // ENXIO
    ArgumentListTooLong                         =   7,  // E2BIG
    ExecFormatError                             =   8,  // ENOEXEC
    BadFileDescriptor                           =   9,  // EBADF
    NoChildProcesses                            =  10,  // ECHILD
    ResourceTemporarilyUnavailable              =  11,  // EAGAIN
    CannotAllocateMemory                        =  12,  // ENOMEM
    PermissionDenied                            =  13,  // EACCES
    BadAddress                                  =  14,  // EFAULT
    BlockDeviceRequired                         =  15,  // ENOTBLK
    DeviceOrResourceBusy                        =  16,  // EBUSY
    FileExists                                  =  17,  // EEXIST
    InvalidCrossDeviceLink                      =  18,  // EXDEV
    NoSuchDevice                                =  19,  // ENODEV
    NotADirectory                               =  20,  // ENOTDIR
    IsADirectory                                =  21,  // EISDIR
    InvalidArgument                             =  22,  // EINVAL
    TooManyOpenFilesSystemWide                  =  23,  // ENFILE
    TooManyOpenFiles                            =  24,  // EMFILE
    InappropriateIoctlForDevice                 =  25,  // ENOTTY
    TextFileBusy                                =  26,  // ETXTBSY
    FileTooLarge                                =  27,  // EFBIG
    NoSpaceLeftOnDevice                         =  28,  // ENOSPC
    IllegalSeek                                 =  29,  // ESPIPE
    ReadOnlyFileSystem                          =  30,  // EROFS
    TooManyLinks                                =  31,  // EMLINK
    BrokenPipe                                  =  32,  // EPIPE
    NumericalArgumentOutOfDomain                =  33,  // EDOM
    NumericalResultOutOfRange                   =  34,  // ERANGE
    ResourceDeadlockAvoided                     =  35,  // EDEADLK
    FileNameTooLong                             =  36,  // ENAMETOOLONG
    NoLocksAvailable                            =  37,  // ENOLCK
    FunctionNotImplemented                      =  38,  // ENOSYS
    DirectoryNotEmpty                           =  39,  // ENOTEMPTY
    TooManyLevelsOfSymbolicLinks                =  40,  // ELOOP
    NoMessageOfDesiredType                      =  42,  // ENOMSG
    IdentifierRemoved                           =  43,  // EIDRM
    ChannelNumberOutOfRange                     =  44,  // ECHRNG
    Level2NotSynchronized                       =  45,  // EL2NSYNC
    Level3Halted                                =  46,  // EL3HLT
    Level3Reset                                 =  47,  // EL3RST
    LinkNumberOutOfRange                        =  48,  // ELNRNG
    ProtocolDriverNotAttached                   =  49,  // EUNATCH
    NoCSIStructureAvailable                     =  50,  // ENOCSI
    Level2Halted                                =  51,  // EL2HLT
    InvalidExchange                             =  52,  // EBADE
    InvalidRequestDescriptor                    =  53,  // EBADR
    ExchangeFull                                =  54,  // EXFULL
    NoAnode                                     =  55,  // ENOANO
    InvalidRequestCode                          =  56,  // EBADRQC
    InvalidSlot                                 =  57,  // EBADSLT
    BadFontFileFormat                           =  59,  // EBFONT
    DeviceNotAStream                            =  60,  // ENOSTR
    NoDataAvailable                             =  61,  // ENODATA
    TimerExpired                                =  62,  // ETIME
    OutOfStreamsResources                       =  63,  // ENOSR
    MachineIsNotOnNetwork                       =  64,  // ENONET
    PackageNotInstalled                         =  65,  // ENOPKG
    ObjectIsRemote                              =  66,  // EREMOTE
    LinkHasBeenSevered                          =  67,  // ENOLINK
    AdvertiseError                              =  68,  // EADV
    SrmountError                                =  69,  // ESRMNT
    CommunicationErrorOnSend                    =  70,  // ECOMM
    ProtocolError                               =  71,  // EPROTO
    MultihopAttempted                           =  72,  // EMULTIHOP
    RfsSpecificError                            =  73,  // EDOTDOT
    BadMessage                                  =  74,  // EBADMSG
    ValueTooLargeForDefinedDataType             =  75,  // EOVERFLOW
    NameNotUniqueOnNetwork                      =  76,  // ENOTUNIQ
    FileDescriptorInBadState                    =  77,  // EBADFD
    RemoteAddressChanged                        =  78,  // EREMCHG
    CannotAccessNeededSharedLibrary             =  79,  // ELIBACC
    AccessingCorruptedSharedLibrary             =  80,  // ELIBBAD
    LibSectionInAOutCorrupted                   =  81,  // ELIBSCN
    AttemptingToLinkInTooManySharedLibraries    =  82,  // ELIBMAX
    CannotExecSharedLibraryDirectly             =  83,  // ELIBEXEC
    InvalidOrIncompleteMultibyteOrWideCharacter =  84,  // EILSEQ
    InterruptedSystemCallShouldBeRestarted      =  85,  // ERESTART
    StreamsPipeError                            =  86,  // ESTRPIPE
    TooManyUsers                                =  87,  // EUSERS
    SocketOperationOnNonSocket                  =  88,  // ENOTSOCK
    DestinationAddressRequired                  =  89,  // EDESTADDRREQ
    MessageTooLong                              =  90,  // EMSGSIZE
    ProtocolWrongTypeForSocket                  =  91,  // EPROTOTYPE
    ProtocolNotAvailable                        =  92,  // ENOPROTOOPT
    ProtocolNotSupported                        =  93,  // EPROTONOSUPPORT
    SocketTypeNotSupported                      =  94,  // ESOCKTNOSUPPORT
    OperationNotSupported                       =  95,  // EOPNOTSUPP
    ProtocolFamilyNotSupported                  =  96,  // EPFNOSUPPORT
    AddressFamilyNotSupportedByProtocol         =  97,  // EAFNOSUPPORT
    AddressAlreadyInUse                         =  98,  // EADDRINUSE
    CannotAssignRequestedAddress                =  99,  // EADDRNOTAVAIL
    NetworkIsDown                               = 100,  // ENETDOWN
    NetworkIsUnreachable                        = 101,  // ENETUNREACH
    NetworkDroppedConnectionOnReset             = 102,  // ENETRESET
    SoftwareCausedConnectionAbort               = 103,  // ECONNABORTED
    ConnectionResetByPeer                       = 104,  // ECONNRESET
    NoBufferSpaceAvailable                      = 105,  // ENOBUFS
    TransportEndpointAlreadyConnected           = 106,  // EISCONN
    TransportEndpointNotConnected               = 107,  // ENOTCONN
    CannotSendAfterTransportEndpointShutdown    = 108,  // ESHUTDOWN
    TooManyReferencesCannotSplice               = 109,  // ETOOMANYREFS
    ConnectionTimedOut                          = 110,  // ETIMEDOUT
    ConnectionRefused                           = 111,  // ECONNREFUSED
    HostIsDown                                  = 112,  // EHOSTDOWN
    NoRouteToHost                               = 113,  // EHOSTUNREACH
    OperationAlreadyInProgress                  = 114,  // EALREADY
    OperationNowInProgress                      = 115,  // EINPROGRESS
    StaleFileHandle                             = 116,  // ESTALE
    StructureNeedsCleaning                      = 117,  // EUCLEAN
    NotAXenixNamedTypeFile                      = 118,  // ENOTNAM
    NoXenixSemaphoresAvailable                  = 119,  // ENAVAIL
    IsANamedTypeFile                            = 120,  // EISNAM
    RemoteIOError                               = 121,  // EREMOTEIO
    DiskQuotaExceeded                           = 122,  // EDQUOT
    NoMediumFound                               = 123,  // ENOMEDIUM
    WrongMediumType                             = 124,  // EMEDIUMTYPE
    OperationCanceled                           = 125,  // ECANCELED
    RequiredKeyNotAvailable                     = 126,  // ENOKEY
    KeyHasExpired                               = 127,  // EKEYEXPIRED
    KeyHasBeenRevoked                           = 128,  // EKEYREVOKED
    KeyWasRejectedByService                     = 129,  // EKEYREJECTED
    OwnerDied                                   = 130,  // EOWNERDEAD
    StateNotRecoverable                         = 131,  // ENOTRECOVERABLE
    OperationNotPossibleDueToRFKill             = 132,  // ERFKILL
    MemoryPageHasHardwareError                  = 133,  // EHWPOISON
    NotSupported                                = 134   // ENOTSUP

    // ── 1000-1999 reserved: render lib
    // ── 2000-2999 reserved: net lib
    // ── 3000-3999 reserved: asset lib
    // ── 4000-4999 reserved: audio lib
    // ── 5000-5999 reserved: crypto lib
    // ── 6000-6999 reserved: fs lib
    // ── 7000-7999 reserved: physics lib
    // ── 8000-8999 reserved: protocol lib
    // ── 9000-9999 reserved: ui lib
    // ── 10000-19999 reserved: client app
    // ── 20000-29999 reserved: zoned app
    // ── 30000-39999 reserved: unid app
};

// Forward declaration of to_string for ErrorCode
constexpr OXIDE_CORE_API std::string_view to_string(ErrorCode) noexcept;

struct OXIDE_CORE_API Error
{
    ErrorCode   code { ErrorCode::Unknown };
    std::string message;

    constexpr Error() noexcept = default;
    constexpr Error(ErrorCode c) noexcept          : code(c) {}
    Error(ErrorCode c, std::string_view m)         : code(c), message(m) {}
    Error(ErrorCode c, const std::exception& e) noexcept : code(c), message(e.what()) {}

    [[nodiscard("Check error state")]]
    constexpr bool ok() const noexcept { return code == ErrorCode::Ok; }

    [[nodiscard("Error message should be used")]]
    std::string_view what() const noexcept;

private:
    static std::string concat(ErrorCode code, const std::string& msg) noexcept {
        try {
            return std::string(to_string(code)) + ": " + msg;
        } catch (...) {
            return std::string(to_string(code));
        }
    }
};

} // namespace oxide::core