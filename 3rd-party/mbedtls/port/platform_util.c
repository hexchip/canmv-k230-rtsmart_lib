/*
 * Common and shared functions used by multiple modules in the Mbed TLS
 * library.
 *
 *  Copyright The Mbed TLS Contributors
 *  SPDX-License-Identifier: Apache-2.0 OR GPL-2.0-or-later
 */

#include "mbedtls/build_info.h"

#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"

#include "hal_utils.h"

#if defined(MBEDTLS_HAVE_TIME)

#if defined(MBEDTLS_PLATFORM_MS_TIME_ALT)

mbedtls_ms_time_t mbedtls_ms_time(void) { return utils_cpu_ticks_ms(); }

#endif /* MBEDTLS_PLATFORM_MS_TIME_ALT */

#if defined(MBEDTLS_PLATFORM_TIME_ALT)

#include "canmv_misc.h"

mbedtls_time_t mbedtls_time_wrap(mbedtls_time_t* timer)
{
    mbedtls_time_t tm;

    if (0x00 != canmv_misc_dev_ioctl(MISC_DEV_CMD_GET_UTC_TIMESTAMP, &tm)) {
        tm = 0;
        mbedtls_printf("rtc get timestamp failed\n");
    }

    if (timer) {
        *timer = tm;
    }

    return tm;
}
#endif /* MBEDTLS_PLATFORM_TIME_ALT */

#endif /* MBEDTLS_HAVE_TIME */
