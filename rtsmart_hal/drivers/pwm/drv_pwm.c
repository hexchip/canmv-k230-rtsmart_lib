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
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "canmv_misc.h"
#include "drv_pwm.h"

#define DRV_PWM_DEV "/dev/pwm"

#define KD_PWM_CMD_ENABLE   _IOW('P', 0, int)
#define KD_PWM_CMD_DISABLE  _IOW('P', 1, int)
#define KD_PWM_CMD_SET_CFG  _IOW('P', 2, int)
#define KD_PWM_CMD_GET_CFG  _IOW('P', 3, int)
#define KD_PWM_CMD_GET_STAT _IOW('P', 4, int)

#define PWM_CHECK_CHANNEL(chn)                                                                                                 \
    do {                                                                                                                       \
        if ((0 > (chn)) || (5 < (chn))) {                                                                                      \
            printf("[hal_pwm]: invalid channel %d\n", (chn));                                                                  \
            return -1;                                                                                                         \
        }                                                                                                                      \
    } while (0)

struct rt_pwm_configuration {
    uint32_t channel; /* 0-n */
    uint32_t period; /* unit:ns 1ns~4.29s:1Ghz~0.23hz */
    uint32_t pulse; /* unit:ns (pulse<=period) */
};

static int _drv_pwm_fd      = -1;
static int _drv_pwm_ref_cnt = 0;

static int pwm_ioctl(int cmd, void* arg)
{
    if (0 > _drv_pwm_fd) {
        if (0 > (_drv_pwm_fd = open(DRV_PWM_DEV, O_RDWR))) {
            printf("[hal_pwm]: open device failed: %s\n", strerror(errno));
            return -1;
        }
    }

    int ret = ioctl(_drv_pwm_fd, cmd, arg);
    if (ret < 0) {
        printf("[hal_pwm]: ioctl failed: %s\n", strerror(errno));
    }
    return ret;
}

static int drv_pwm_get_cfg(int channel, uint32_t* period, uint32_t* pulse)
{
    struct rt_pwm_configuration cfg = { .channel = channel, .period = 0, .pulse = 0 };

    if (0 != pwm_ioctl(KD_PWM_CMD_GET_CFG, &cfg)) {
        printf("[hal_pwm]: get cfg failed for channel %d\n", channel);
        return -1;
    }

    if (period) {
        *period = cfg.period;
    }

    if (pulse) {
        *pulse = cfg.pulse;
    }

    return 0;
}

int drv_pwm_set_freq(int channel, uint32_t freq)
{
    PWM_CHECK_CHANNEL(channel);

    if (0x00 == freq) {
        printf("[hal_pwm]: frequency cannot be 0\n");
        return -1;
    }

    uint32_t curr_pulse;

    struct rt_pwm_configuration cfg = { .channel = channel, .period = NSEC_PER_SEC / freq };

    // Get current pulse width to maintain duty cycle
    if (0 != drv_pwm_get_cfg(channel, NULL, &curr_pulse)) {
        return -1;
    }

    // Ensure pulse doesn't exceed new period
    cfg.pulse = (curr_pulse > cfg.period) ? cfg.period : curr_pulse;

    if (0 != pwm_ioctl(KD_PWM_CMD_SET_CFG, &cfg)) {
        printf("[hal_pwm]: set frequency failed for channel %d\n", channel);
        return -1;
    }

    return 0;
}

int drv_pwm_get_freq(int channel, uint32_t* freq)
{
    PWM_CHECK_CHANNEL(channel);

    uint32_t period;
    if (0 != drv_pwm_get_cfg(channel, &period, NULL)) {
        return -1;
    }

    if (0x00 == period) {
        if (freq) {
            *freq = 0; // Undefined frequency
        }
        return 0;
    }

    if (freq) {
        *freq = NSEC_PER_SEC / period;
    }

    return 0;
}

int drv_pwm_set_duty(int channel, uint32_t duty)
{
    PWM_CHECK_CHANNEL(channel);

    if (duty > 100) {
        printf("[hal_pwm]: duty cycle must be <= 100%%\n");
        return -1;
    }

    uint32_t period;
    if (0 != drv_pwm_get_cfg(channel, &period, NULL)) {
        return -1;
    }

    struct rt_pwm_configuration cfg = { .channel = channel, .period = period, .pulse = (period * duty) / 100 };

    if (0 != pwm_ioctl(KD_PWM_CMD_SET_CFG, &cfg)) {
        printf("[hal_pwm]: set duty cycle failed for channel %d\n", channel);
        return -1;
    }

    return 0;
}

int drv_pwm_get_duty(int channel, uint32_t* duty)
{
    PWM_CHECK_CHANNEL(channel);

    uint32_t period, pulse;
    if (0 != drv_pwm_get_cfg(channel, &period, &pulse)) {
        return -1;
    }

    if (period == 0) {
        if(duty) {
            *duty = 0;
        }
        return 0;
    }

    if(duty) {
        *duty = (pulse * 100) / period;
    }

    return 0;
}

int drv_pwm_enable(int channel)
{
    PWM_CHECK_CHANNEL(channel);

    struct rt_pwm_configuration cfg = { .channel = channel };

    if (0 != pwm_ioctl(KD_PWM_CMD_ENABLE, &cfg)) {
        printf("[hal_pwm]: enable failed for channel %d\n", channel);
        return -1;
    }

    _drv_pwm_ref_cnt++;
    return 0;
}

int drv_pwm_disable(int channel)
{
    PWM_CHECK_CHANNEL(channel);

    struct rt_pwm_configuration cfg = { .channel = channel };

    if (0 != pwm_ioctl(KD_PWM_CMD_DISABLE, &cfg)) {
        printf("[hal_pwm]: disable failed for channel %d\n", channel);
        return -1;
    }

    _drv_pwm_ref_cnt--;
    if (_drv_pwm_ref_cnt <= 0) {
        close(_drv_pwm_fd);
        _drv_pwm_fd      = -1;
        _drv_pwm_ref_cnt = 0;
    }
    return 0;
}

int drv_pwm_init(void)
{
    // Open device on first init
    if (_drv_pwm_fd < 0) {
        _drv_pwm_fd = open(DRV_PWM_DEV, O_RDWR);
        if (_drv_pwm_fd < 0) {
            printf("[hal_pwm]: init failed - could not open device\n");
            return -1;
        }
    }
    return 0;
}

int drv_pwm_deinit(void)
{
    if (_drv_pwm_fd >= 0) {
        close(_drv_pwm_fd);
        _drv_pwm_fd      = -1;
        _drv_pwm_ref_cnt = 0;
    }
    return 0;
}
