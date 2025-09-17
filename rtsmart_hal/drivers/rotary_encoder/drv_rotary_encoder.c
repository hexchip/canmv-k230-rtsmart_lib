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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "drv_rotary_encoder.h"

/* IOCTL commands */
#define ENCODER_CMD_GET_DATA   _IOR('E', 1, struct encoder_data*)
#define ENCODER_CMD_RESET      _IO('E', 2)
#define ENCODER_CMD_SET_COUNT  _IOW('E', 3, int64_t*)
#define ENCODER_CMD_CONFIG     _IOW('E', 4, struct encoder_config*)
#define ENCODER_CMD_WAIT_DATA  _IOW('E', 5, int32_t*)

struct encoder_config {
    int clk_pin;     /* Clock/A phase pin */
    int dt_pin;      /* Data/B phase pin */
    int sw_pin;      /* Switch/button pin (use -1 if not connected) */
};

static int encoder_fd = -1;

int rotary_encoder_init(void)
{
    if (encoder_fd >= 0) {
        return 0; /* Already initialized */
    }

    encoder_fd = open("/dev/encoder", O_RDWR);
    if (encoder_fd < 0) {
        printf("[hal_encoder] Failed to open device: %d\n", errno);
        return -1;
    }

    return 0;
}

int rotary_encoder_deinit(void)
{
    if (encoder_fd >= 0) {
        close(encoder_fd);
        encoder_fd = -1;
    }
    return 0;
}

int rotary_encoder_config(int clk_pin, int dt_pin, int sw_pin)
{
    struct encoder_config config;

    if (encoder_fd < 0) {
        if (rotary_encoder_init() < 0) {
            return -1;
        }
    }

    config.clk_pin = clk_pin;
    config.dt_pin = dt_pin;
    config.sw_pin = sw_pin;

    if (ioctl(encoder_fd, ENCODER_CMD_CONFIG, &config) < 0) {
        printf("[hal_encoder] Failed to configure encoder: %d\n", errno);
        return -1;
    }

    return 0;
}

int rotary_encoder_read(struct encoder_data *data)
{
    if (encoder_fd < 0 || !data) {
        return -1;
    }

    if (ioctl(encoder_fd, ENCODER_CMD_GET_DATA, data) < 0) {
        printf("[hal_encoder] Failed to read encoder data: %d\n", errno);
        return -1;
    }

    return 0;
}

int rotary_encoder_wait_event(struct encoder_data *data, int timeout_ms)
{
    if (encoder_fd < 0 || !data) {
        return -1;
    }

    /* Wait for data available */
    if (ioctl(encoder_fd, ENCODER_CMD_WAIT_DATA, &timeout_ms) < 0) {
        if (errno == 2) {
            return 0; /* Timeout, no data */
        }
        printf("[hal_encoder] Failed to wait for data: %d\n", errno);
        return -1;
    }

    /* Read the data */
    return (rotary_encoder_read(data) == 0) ? 1 : -1;
}

int rotary_encoder_reset(void)
{
    if (encoder_fd < 0) {
        return -1;
    }

    if (ioctl(encoder_fd, ENCODER_CMD_RESET, NULL) < 0) {
        printf("[hal_encoder] Failed to reset encoder: %d\n", errno);
        return -1;
    }

    return 0;
}

int rotary_encoder_set_count(int64_t count)
{
    if (encoder_fd < 0) {
        return -1;
    }

    if (ioctl(encoder_fd, ENCODER_CMD_SET_COUNT, &count) < 0) {
        printf("[hal_encoder] Failed to set encoder count: %d\n", errno);
        return -1;
    }

    return 0;
}

/* 便捷函数：获取当前计数值 */
int64_t rotary_encoder_get_count(void)
{
    struct encoder_data data;

    if (rotary_encoder_read(&data) < 0) {
        return 0;
    }

    return data.total_count;
}

/* 便捷函数：获取并清除增量 */
int32_t rotary_encoder_get_delta(void)
{
    struct encoder_data data;

    if (rotary_encoder_read(&data) < 0) {
        return 0;
    }

    return data.delta;
}
