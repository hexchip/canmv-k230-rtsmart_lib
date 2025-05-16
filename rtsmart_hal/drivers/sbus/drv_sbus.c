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
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include "drv_sbus.h"

#define IOC_SET_BAUDRATE            _IOW('U', 0x40, int)

#define SBUS_CHANNEL_NUM 16
#define SBUS_FRAME_SIZE 25

#define SBUS_MIN 172
#define SBUS_MAX 1811
#define SBUS_NEUTRAL 1024

struct uart_configure
{
    uint32_t baud_rate;

    uint32_t data_bits               :4;
    uint32_t stop_bits               :2;
    uint32_t parity                  :2;
    uint32_t fifo_lenth              :2;
    uint32_t auto_flow               :1;
    uint32_t reserved                :21;
};

typedef enum _uart_parity
{
    UART_PARITY_NONE,
    UART_PARITY_ODD,
    UART_PARITY_EVEN
} uart_parity_t;

typedef enum _uart_receive_trigger
{
    UART_RECEIVE_FIFO_1,
    UART_RECEIVE_FIFO_8,
    UART_RECEIVE_FIFO_16,
    UART_RECEIVE_FIFO_30,
} uart_receive_trigger_t;

struct sbus_device
{
    bool debug;
    int fd;
    uint16_t channels[SBUS_CHANNEL_NUM];
    sbus_flag_t flags;
};

bool is_valid_uart_path(const char *path)
{
    const char *valid_paths[] =
    {
        "/dev/uart1",
        "/dev/uart2",
        "/dev/uart3",
        "/dev/uart4",
        NULL
    };

    for (int i = 0; valid_paths[i] != NULL; i++) {
        if (strcmp(path, valid_paths[i]) == 0) {
            return true;
        }
    }

    return false;
}

sbus_dev_t sbus_create(const char *uart)
{
    sbus_dev_t dev;
    struct uart_configure config;

    if (!is_valid_uart_path(uart)) {
        printf("pls use /dev/uart1 ~ /dev/uart4\n");
        goto err1;
    }

    dev = malloc(sizeof(struct sbus_device));
    if (!dev) {
        printf("sbus_create fail\n");
        goto err1;
    }

    memset(dev, 0, sizeof(struct sbus_device));

    dev->fd = open(uart, O_RDWR);
    if (dev->fd < 0) {
        printf("%s open failed!\n", uart);
        goto err2;
    }

    config.baud_rate = 100000;
    config.data_bits = 8;
    config.stop_bits = 2;
    config.parity = UART_PARITY_EVEN;
    config.fifo_lenth = UART_RECEIVE_FIFO_16;
    config.auto_flow = 0;

    if (ioctl(dev->fd, IOC_SET_BAUDRATE, &config)) {
        printf("%s set baudrate failed!\n", uart);
        goto err3;
    }

    return dev;

err3:
    close(dev->fd);
err2:
    free(dev);
err1:
    return NULL;

}

void sbus_destroy(sbus_dev_t dev)
{
    if (!dev) {
        printf("%s: pls ensure sbus_create called\n", __func__);
        return;
    } else {
        close(dev->fd);
        free(dev);
    }
}

int sbus_set_channel(sbus_dev_t dev, uint8_t channel_index, uint16_t value)
{
    int ret = 0;

    if (!dev) {
        printf("%s: pls ensure sbus_create called\n", __func__);
        ret = -1;
        goto out;
    }

    if (channel_index >= SBUS_CHANNEL_NUM) {
        printf("channel index is over range\n");
        ret = -1;
        goto out;
    }

#if 0
    if (value > 2047) {
        value = 2047;
    }

    if (value < 172) {
        value = 172;
    }
#endif

    dev->channels[channel_index] = value;

out:
    return ret;
}

int sbus_set_all_channels(sbus_dev_t dev, uint16_t *channels)
{
    int ret = 0;

    for (int i = 0; i < SBUS_CHANNEL_NUM; i++) {
        ret = sbus_set_channel(dev, i, channels[i]);
        if (ret != 0) {
            break;
        }
    }

    return ret;
}

void sbus_set_flags(sbus_dev_t dev, const sbus_flag_t *flags)
{
    if (!dev) {
        printf("%s: pls ensure sbus_create called\n", __func__);
        return;
    }

    if (flags) {
        dev->flags = *flags;
    }
}

void sbus_get_flags(sbus_dev_t dev, sbus_flag_t *flags_out)
{
    if (!dev) {
        printf("%s: pls ensure sbus_create called\n", __func__);
        return;
    }

    if (flags_out) {
        *flags_out = dev->flags;
    }
}

static void sbus_decode_frame(uint8_t *sbus_frame, uint16_t *ch, sbus_flag_t *flags)
{
    if (!sbus_frame || !ch || sbus_frame[0] != 0x0F || sbus_frame[24] != 0x00) {
        return;
    }

    for (int i = 0; i < SBUS_CHANNEL_NUM; i++) {
        int byte_pos = 1 + (i * 11) / 8;
        int bit_pos = (i * 11) % 8;

        ch[i] = (sbus_frame[byte_pos] >> bit_pos) |
            (sbus_frame[byte_pos + 1] << (8 - bit_pos));

        if (bit_pos > 5) {
            ch[i] |= (sbus_frame[byte_pos + 2] << (16 - bit_pos));
        }

        ch[i] &= 0x07FF;
    }

    if (flags) {
        uint8_t flag_byte = sbus_frame[23];
        flags->bit.ch17       = ((flag_byte >> 0) & 0x01) ? true : false;
        flags->bit.ch18       = ((flag_byte >> 1) & 0x01) ? true : false;
        flags->bit.frame_lost = ((flag_byte >> 2) & 0x01) ? true : false;
        flags->bit.failsafe   = ((flag_byte >> 3) & 0x01) ? true : false;
    }
}

void sbus_send_frame(sbus_dev_t dev)
{
    uint8_t flags = 0;
    uint8_t sbus_frame[SBUS_FRAME_SIZE];
    int len;

    if (!dev) {
        printf("pls ensure sbus_create called \n");
        return;
    }

    sbus_frame[0] = 0x0F;
    memset(&sbus_frame[1], 0, 22);

    for (int i = 0; i < SBUS_CHANNEL_NUM; i++) {
        uint16_t val = dev->channels[i];

        int byte_pos = 1 + (i * 11) / 8;
        int bit_pos = (i * 11) % 8;

        sbus_frame[byte_pos] |= (val << bit_pos) & 0xFF;
        sbus_frame[byte_pos + 1] |= (val >> (8 - bit_pos)) & 0xFF;
        if (bit_pos > 5) {
            sbus_frame[byte_pos + 2] |= (val >> (16 - bit_pos)) & 0xFF;
        }
    }

    if (dev->flags.bit.ch17) {
        flags |= (1 << 0);
    }
    if (dev->flags.bit.ch18) {
        flags |= (1 << 1);
    }
    if (dev->flags.bit.frame_lost) {
        flags |= (1 << 2);
    }
    if (dev->flags.bit.failsafe) {
        flags |= (1 << 3);
    }

    sbus_frame[23] = flags;
    sbus_frame[24] = 0x00;

    if (dev->debug) {
        sbus_flag_t flag;
        uint16_t ch[SBUS_CHANNEL_NUM];

        sbus_decode_frame(sbus_frame, ch, &flag);
        for (int i = 0; i < SBUS_CHANNEL_NUM; i++) {
            printf("CH[%02d] = 0x%-3x(%4d)\n", i, ch[i], ch[i]);
        }
        printf("ch17 = %d, ch18 = %d, frame_lost = %d, failsafe = %d\n",
               flag.bit.ch17, flag.bit.ch18, flag.bit.frame_lost, flag.bit.failsafe);
    }

    len = write(dev->fd, sbus_frame, SBUS_FRAME_SIZE);
    if (len != SBUS_FRAME_SIZE) {
        printf("%s short write , len = %d\n", __func__, len);
    }
}

void sbus_set_debug(sbus_dev_t dev, bool val)
{
    if (!dev) {
        printf("%s: pls ensure sbus_create called\n", __func__);
        return;
    }

    dev->debug = val;
}
