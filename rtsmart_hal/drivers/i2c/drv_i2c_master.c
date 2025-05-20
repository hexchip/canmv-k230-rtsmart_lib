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
#include "drv_i2c.h"

#define RT_I2C_DEV_CTRL_10BIT (8 * 0x100 + 0x01)
// #define RT_I2C_DEV_CTRL_ADDR    (8 * 0x100 + 0x02)
#define RT_I2C_DEV_CTRL_TIMEOUT (8 * 0x100 + 0x03)
#define RT_I2C_DEV_CTRL_RW      (8 * 0x100 + 0x04)
#define RT_I2C_DEV_CTRL_CLK     (8 * 0x100 + 0x05)
#define RT_I2C_DEV_CTRL_7BIT    (8 * 0x100 + 0x06)

static const int i2c_master_inst_type = 0;

static int hard_i2c_in_use[KD_HARD_I2C_MAX_NUM];

static int drv_i2c_ioctl(drv_i2c_inst_t* inst, int cmd, void* arg)
{
    if ((NULL == inst) || (0x00 > inst->fd)) {
        printf("timer not open\n");
        return -1;
    }

    return ioctl(inst->fd, cmd, arg);
}

int drv_i2c_inst_create(int id, uint32_t freq, uint32_t timeout_ms, drv_i2c_inst_t** inst)
{
    return drv_i2c_inst_create_ex(id, freq, timeout_ms, 0xFF, 0xFF, inst);
}

int drv_i2c_inst_create_ex(int id, uint32_t freq, uint32_t timeout_ms, uint8_t scl, uint8_t sda, drv_i2c_inst_t** inst)
{
    int  fd = -1;
    char dev_name[64];
    int  dev_type = DRV_I2C_TYPE_HARD;

    uint8_t soft_scl = scl, soft_sda = sda;

    if (KD_HARD_I2C_MAX_NUM > id) {
        if (hard_i2c_in_use[id]) {
            printf("i2c%d maybe in use\n", id);
        }
        dev_type = DRV_I2C_TYPE_HARD;
    } else {
        if ((64 <= soft_scl) || (64 <= soft_sda)) {
            printf("invalid scl(%d), sda(%d) for soft i2c\n", soft_scl, soft_sda);
            return -1;
        }

        struct soft_i2c_configure cfg;

        cfg.bus_num    = id;
        cfg.pin_scl    = soft_scl;
        cfg.pin_sda    = soft_sda;
        cfg.freq       = freq;
        cfg.timeout_ms = timeout_ms;

        if (0x00 != canmv_misc_dev_ioctl(MISC_DEV_CMD_CREATE_SOFT_I2C, &cfg)) {
            printf("can't create soft i2c device");

            return -1;
        }
        dev_type = DRV_I2C_TYPE_SOFT;
    }
    snprintf(dev_name, sizeof(dev_name), "/dev/i2c%d", id);

    if (0 > (fd = open(dev_name, O_RDWR))) {
        printf("open %s failed\n", dev_name);
        return -1;
    }

    if (*inst) {
        drv_i2c_inst_destroy(inst);
        *inst = NULL;
    }

    *inst = malloc(sizeof(drv_i2c_inst_t));
    if (NULL == *inst) {
        printf("malloc failed");

        close(fd);
        return -1;
    }
    memset(*inst, 0x00, sizeof(drv_i2c_inst_t));

    (*inst)->base         = (void*)&i2c_master_inst_type;
    (*inst)->type         = dev_type;
    (*inst)->id           = id;
    (*inst)->fd           = fd;
    (*inst)->soft.pin_scl = soft_scl;
    (*inst)->soft.pin_sda = soft_sda;
    (*inst)->freq         = freq;
    (*inst)->timeout_ms   = timeout_ms;

    if (DRV_I2C_TYPE_HARD == dev_type) {
        hard_i2c_in_use[id] = 1;
    }

    return 0;
}

void drv_i2c_inst_destroy(drv_i2c_inst_t** inst)
{
    int id, type, fd;
    if (NULL == (*inst)) {
        return;
    }

    type = (*inst)->type;
    fd   = (*inst)->fd;
    id   = (*inst)->id;

    free(*inst);
    *inst = NULL;

    close(fd);

    if (DRV_I2C_TYPE_SOFT == type) {
        uint32_t bus_num = id;

        if (0x00 != canmv_misc_dev_ioctl(MISC_DEV_CMD_DELETE_SOFT_I2C, &bus_num)) {
            printf("can't delete soft i2c device");
        }
    } else {
        hard_i2c_in_use[id] = 0;
    }
}

int drv_i2c_set_7b_addr(drv_i2c_inst_t* inst)
{
    if (!inst) {
        return -1;
    }

    return drv_i2c_ioctl(inst, RT_I2C_DEV_CTRL_7BIT, NULL);
}

int drv_i2c_set_10b_addr(drv_i2c_inst_t* inst)
{
    if (!inst) {
        return -1;
    }

    return drv_i2c_ioctl(inst, RT_I2C_DEV_CTRL_10BIT, NULL);
}

int drv_i2c_set_freq(drv_i2c_inst_t* inst, uint32_t freq)
{
    uint32_t _freq = freq;

    if (!inst) {
        return -1;
    }

    if (_freq == inst->freq) {
        return 0;
    }

    if (0x00 != drv_i2c_ioctl(inst, RT_I2C_DEV_CTRL_CLK, &_freq)) {
        printf("i2c set clock failed\n");
        return -1;
    }

    inst->freq = _freq;

    return 0;
}

int drv_i2c_set_timeout(drv_i2c_inst_t* inst, uint32_t timeout_ms)
{
    uint32_t _timeout_ms = timeout_ms;

    if (!inst) {
        return -1;
    }

    if (_timeout_ms == inst->timeout_ms) {
        return 0;
    }

    if (0x00 != drv_i2c_ioctl(inst, RT_I2C_DEV_CTRL_TIMEOUT, &_timeout_ms)) {
        printf("i2c set timeout failed\n");
        return -1;
    }

    inst->timeout_ms = _timeout_ms;

    return 0;
}

int drv_i2c_transfer(drv_i2c_inst_t* inst, i2c_msg_t* msgs, int msg_cnt)
{
    struct rt_i2c_priv_data {
        i2c_msg_t* msgs;
        size_t     number;
    } priv;

    if (!inst) {
        return -1;
    }

    priv.msgs   = msgs;
    priv.number = msg_cnt;

    return drv_i2c_ioctl(inst, RT_I2C_DEV_CTRL_RW, &priv);
}
