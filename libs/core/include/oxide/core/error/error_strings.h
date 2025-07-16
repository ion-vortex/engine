#pragma once

#include <string_view>

#include "error.h"

namespace oxide::core {

    constexpr std::string_view to_string(ErrorCode c) noexcept
    {
        using E = ErrorCode;
        switch (c)
        {
            case E::Ok:                                   return "Ok";
            case E::Unknown:                              return "Unknown error";

            // POSIX / errno — long-form names
            case E::OperationNotPermitted:                return "Operation not permitted";
            case E::NoSuchFileOrDirectory:                return "No such file or directory";
            case E::NoSuchProcess:                        return "No such process";
            case E::InterruptedSystemCall:                return "Interrupted system call";
            case E::InputOutputError:                     return "Input/output error";
            case E::NoSuchDeviceOrAddress:                return "No such device or address";
            case E::ArgumentListTooLong:                  return "Argument list too long";
            case E::ExecFormatError:                      return "Exec format error";
            case E::BadFileDescriptor:                    return "Bad file descriptor";
            case E::NoChildProcesses:                     return "No child processes";
            case E::ResourceTemporarilyUnavailable:       return "Resource temporarily unavailable";
            case E::CannotAllocateMemory:                 return "Cannot allocate memory";
            case E::PermissionDenied:                     return "Permission denied";
            case E::BadAddress:                           return "Bad address";
            case E::BlockDeviceRequired:                  return "Block device required";
            case E::DeviceOrResourceBusy:                 return "Device or resource busy";
            case E::FileExists:                           return "File exists";
            case E::InvalidCrossDeviceLink:               return "Invalid cross-device link";
            case E::NoSuchDevice:                         return "No such device";
            case E::NotADirectory:                        return "Not a directory";
            case E::IsADirectory:                         return "Is a directory";
            case E::InvalidArgument:                      return "Invalid argument";
            case E::TooManyOpenFilesSystemWide:           return "Too many open files (system-wide)";
            case E::TooManyOpenFiles:                     return "Too many open files";
            case E::InappropriateIoctlForDevice:          return "Inappropriate ioctl for device";
            case E::TextFileBusy:                         return "Text file busy";
            case E::FileTooLarge:                         return "File too large";
            case E::NoSpaceLeftOnDevice:                  return "No space left on device";
            case E::IllegalSeek:                          return "Illegal seek";
            case E::ReadOnlyFileSystem:                   return "Read-only file system";
            case E::TooManyLinks:                         return "Too many links";
            case E::BrokenPipe:                           return "Broken pipe";
            case E::NumericalArgumentOutOfDomain:         return "Numerical argument out of domain";
            case E::NumericalResultOutOfRange:            return "Numerical result out of range";
            case E::ResourceDeadlockAvoided:              return "Resource deadlock avoided";
            case E::FileNameTooLong:                      return "File name too long";
            case E::NoLocksAvailable:                     return "No locks available";
            case E::FunctionNotImplemented:               return "Function not implemented";
            case E::DirectoryNotEmpty:                    return "Directory not empty";
            case E::TooManyLevelsOfSymbolicLinks:         return "Too many levels of symbolic links";
            case E::NoMessageOfDesiredType:               return "No message of desired type";
            case E::IdentifierRemoved:                    return "Identifier removed";
            case E::ChannelNumberOutOfRange:              return "Channel number out of range";
            case E::Level2NotSynchronized:                return "Level-2 not synchronized";
            case E::Level3Halted:                         return "Level-3 halted";
            case E::Level3Reset:                          return "Level-3 reset";
            case E::LinkNumberOutOfRange:                 return "Link number out of range";
            case E::ProtocolDriverNotAttached:            return "Protocol driver not attached";
            case E::NoCSIStructureAvailable:              return "No CSI structure available";
            case E::Level2Halted:                         return "Level-2 halted";
            case E::InvalidExchange:                      return "Invalid exchange";
            case E::InvalidRequestDescriptor:             return "Invalid request descriptor";
            case E::ExchangeFull:                         return "Exchange full";
            case E::NoAnode:                              return "No anode";
            case E::InvalidRequestCode:                   return "Invalid request code";
            case E::InvalidSlot:                          return "Invalid slot";
            case E::BadFontFileFormat:                    return "Bad font file format";
            case E::DeviceNotAStream:                     return "Device not a stream";
            case E::NoDataAvailable:                      return "No data available";
            case E::TimerExpired:                         return "Timer expired";
            case E::OutOfStreamsResources:                return "Out of streams resources";
            case E::MachineIsNotOnNetwork:                return "Machine is not on network";
            case E::PackageNotInstalled:                  return "Package not installed";
            case E::ObjectIsRemote:                       return "Object is remote";
            case E::LinkHasBeenSevered:                   return "Link has been severed";
            case E::AdvertiseError:                       return "Advertise error";
            case E::SrmountError:                         return "SRMOUNT error";
            case E::CommunicationErrorOnSend:             return "Communication error on send";
            case E::ProtocolError:                        return "Protocol error";
            case E::MultihopAttempted:                    return "Multihop attempted";
            case E::RfsSpecificError:                     return "RFS specific error";
            case E::BadMessage:                           return "Bad message";
            case E::ValueTooLargeForDefinedDataType:      return "Value too large for defined data type";
            case E::NameNotUniqueOnNetwork:               return "Name not unique on network";
            case E::FileDescriptorInBadState:             return "File descriptor in bad state";
            case E::RemoteAddressChanged:                 return "Remote address changed";
            case E::CannotAccessNeededSharedLibrary:      return "Cannot access needed shared library";
            case E::AccessingCorruptedSharedLibrary:      return "Accessing corrupted shared library";
            case E::LibSectionInAOutCorrupted:            return ".lib section in a.out corrupted";
            case E::AttemptingToLinkInTooManySharedLibraries: return "Attempting to link in too many shared libraries";
            case E::CannotExecSharedLibraryDirectly:      return "Cannot exec a shared library directly";
            case E::InvalidOrIncompleteMultibyteOrWideCharacter: return "Invalid or incomplete multibyte/wide character";
            case E::InterruptedSystemCallShouldBeRestarted: return "Interrupted system call should be restarted";
            case E::StreamsPipeError:                     return "Streams pipe error";
            case E::TooManyUsers:                         return "Too many users";
            case E::SocketOperationOnNonSocket:           return "Socket operation on non-socket";
            case E::DestinationAddressRequired:           return "Destination address required";
            case E::MessageTooLong:                       return "Message too long";
            case E::ProtocolWrongTypeForSocket:           return "Protocol wrong type for socket";
            case E::ProtocolNotAvailable:                 return "Protocol not available";
            case E::ProtocolNotSupported:                 return "Protocol not supported";
            case E::SocketTypeNotSupported:               return "Socket type not supported";
            case E::OperationNotSupported:                return "Operation not supported";
            case E::ProtocolFamilyNotSupported:           return "Protocol family not supported";
            case E::AddressFamilyNotSupportedByProtocol:  return "Address family not supported by protocol";
            case E::AddressAlreadyInUse:                  return "Address already in use";
            case E::CannotAssignRequestedAddress:         return "Cannot assign requested address";
            case E::NetworkIsDown:                        return "Network is down";
            case E::NetworkIsUnreachable:                 return "Network is unreachable";
            case E::NetworkDroppedConnectionOnReset:      return "Network dropped connection on reset";
            case E::SoftwareCausedConnectionAbort:        return "Software caused connection abort";
            case E::ConnectionResetByPeer:                return "Connection reset by peer";
            case E::NoBufferSpaceAvailable:               return "No buffer space available";
            case E::TransportEndpointAlreadyConnected:    return "Transport endpoint already connected";
            case E::TransportEndpointNotConnected:        return "Transport endpoint not connected";
            case E::CannotSendAfterTransportEndpointShutdown: return "Cannot send after transport endpoint shutdown";
            case E::TooManyReferencesCannotSplice:        return "Too many references – cannot splice";
            case E::ConnectionTimedOut:                   return "Connection timed out";
            case E::ConnectionRefused:                    return "Connection refused";
            case E::HostIsDown:                           return "Host is down";
            case E::NoRouteToHost:                        return "No route to host";
            case E::OperationAlreadyInProgress:           return "Operation already in progress";
            case E::OperationNowInProgress:               return "Operation now in progress";
            case E::StaleFileHandle:                      return "Stale file handle";
            case E::StructureNeedsCleaning:               return "Structure needs cleaning";
            case E::NotAXenixNamedTypeFile:               return "Not a XENIX named-type file";
            case E::NoXenixSemaphoresAvailable:           return "No XENIX semaphores available";
            case E::IsANamedTypeFile:                     return "Is a named-type file";
            case E::RemoteIOError:                        return "Remote I/O error";
            case E::DiskQuotaExceeded:                    return "Disk quota exceeded";
            case E::NoMediumFound:                        return "No medium found";
            case E::WrongMediumType:                      return "Wrong medium type";
            case E::OperationCanceled:                    return "Operation canceled";
            case E::RequiredKeyNotAvailable:              return "Required key not available";
            case E::KeyHasExpired:                        return "Key has expired";
            case E::KeyHasBeenRevoked:                    return "Key has been revoked";
            case E::KeyWasRejectedByService:              return "Key was rejected by service";
            case E::OwnerDied:                            return "Owner died";
            case E::StateNotRecoverable:                  return "State not recoverable";
            case E::OperationNotPossibleDueToRFKill:      return "Operation not possible due to RF-kill";
            case E::MemoryPageHasHardwareError:           return "Memory page has hardware error";
            case E::NotSupported:                         return "Not supported";

            // fallback
            default: return "Unrecognised error";
        }
    }

    inline std::string_view Error::what() const noexcept
    {
        if (!message.empty())
            return message;
        return to_string(code);
    }
}