// SPDX-License-Identifier: GPL-3.0-only

// Provide some potentially useful POSIX errno values for error handling.
//
// TODO: Make a project-specific type like UEFI Status or coreboot cb_err.

#ifndef _COMMON_ERRNO_H
#define _COMMON_ERRNO_H

#define EPERM           1       // Operation not permitted
#define EIO             5       // Input/output error
#define EAGAIN          11      // Resource temporarily unavailable
#define EACCES          13      // Permission denied
#define EFAULT          14      // Bad address
#define EBUSY           16      // Device or resource busy
#define ENODEV          19      // No such device
#define EINVAL          22      // Invalid argument
#define EPROTO          72      // Protocol error
#define ETIMEDOUT       110     // Connection timed out
#define ECANCELED       125     // Operation canceled

#endif // _COMMON_ERRNO_H
