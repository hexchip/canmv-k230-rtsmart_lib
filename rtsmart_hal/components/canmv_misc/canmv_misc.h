/* Copyright (c) 2025, Canaan Bright Sight Co., Ltd
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define MISC_DEV_CMD_READ_HEAP              (0x1024 + 0)
#define MISC_DEV_CMD_READ_PAGE              (0x1024 + 1)
#define MISC_DEV_CMD_GET_MEMORY_SIZE        (0x1024 + 2)
#define MISC_DEV_CMD_CPU_USAGE              (0x1024 + 3)
#define MISC_DEV_CMD_CREATE_SOFT_I2C        (0x1024 + 4)
#define MISC_DEV_CMD_DELETE_SOFT_I2C        (0x1024 + 5)
#define MISC_DEV_CMD_GET_FS_STAT            (0x1024 + 6)
#define MISC_DEV_CMD_NTP_SYNC               (0x1024 + 7)
#define MISC_DEV_CMD_GET_UTC_TIMESTAMP      (0x1024 + 8)
#define MISC_DEV_CMD_SET_UTC_TIMESTAMP      (0x1024 + 9)
#define MISC_DEV_CMD_GET_LOCAL_TIME         (0x1024 + 10)
#define MISC_DEV_CMD_SET_TIMEZONE           (0x1024 + 11)
#define MISC_DEV_CMD_GET_TIMEZONE           (0x1024 + 12)
#define MISC_DEV_CMD_SET_AUTO_EXEC_PY_STAGE (0x1024 + 13)

// MISC_DEV_CMD_GET_FS_STAT
#define FS_STAT_PATH_LENGTH 32

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// MISC_DEV_CMD_READ_HEAP
// MISC_DEV_CMD_READ_PAGE
struct canmv_misc_dev_meminfo_t {
    size_t total_size;
    size_t free_size;
    size_t used_size;
};

// MISC_DEV_CMD_CREATE_SOFT_I2C
struct soft_i2c_configure {
    uint32_t bus_num;
    uint32_t pin_scl;
    uint32_t pin_sda;
    uint32_t freq;
    uint32_t timeout_ms;
};

// MISC_DEV_CMD_GET_FS_STAT
struct dfs_statfs {
    size_t f_bsize; /* block size */
    size_t f_blocks; /* total data blocks in file system */
    size_t f_bfree; /* free blocks in file system */
};

// MISC_DEV_CMD_GET_FS_STAT
struct statfs_wrap {
    char              path[FS_STAT_PATH_LENGTH];
    struct dfs_statfs sb;
};

// MISC_DEV_CMD_SET_AUTO_EXEC_PY_STAGE
enum {
    STAGE_NORMAL = 1,
    STAGE_BOOTPY_START,
    STAGE_BOOTPY_END,
    STAGE_MAINPY_START,
    STAGE_MAINPY_END,
    STAGE_MAX,
};

static __inline __attribute__((__always_inline__)) int canmv_misc_dev_ioctl(int cmd, void* args)
{
    int result = 0, misc_dev_fd = -1;

    if (0 > (misc_dev_fd = open("/dev/canmv_misc", O_RDWR))) {
        printf("can not misc device");
        return -1;
    }

    if (0x00 != (result = ioctl(misc_dev_fd, cmd, args))) {
        printf("ioctl misc device failed, cmd %x\n", cmd);
    }

    if (0 <= misc_dev_fd) {
        close(misc_dev_fd);
    }

    return result;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */
