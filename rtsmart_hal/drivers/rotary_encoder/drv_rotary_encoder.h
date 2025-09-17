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
#ifndef __HAL_ROTARY_ENCODER_H__
#define __HAL_ROTARY_ENCODER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Encoder direction definitions */
#define ENCODER_DIR_CW   1    /* Clockwise */
#define ENCODER_DIR_CCW  0xFF /* Counter-clockwise */
#define ENCODER_DIR_NONE 0    /* No movement */

/* Data structures */
struct encoder_data {
    int32_t  delta;        /* Change since last read */
    int64_t  total_count;  /* Total count */
    uint8_t  direction;    /* Last direction */
    uint8_t  button_state; /* Button pressed state */
    uint32_t timestamp;    /* Tick timestamp */
};
/**
 * Initialize rotary encoder device
 * @return 0 on success, -1 on error
 */
int rotary_encoder_init(void);

/**
 * Deinitialize rotary encoder device
 * @return 0 on success, -1 on error
 */
int rotary_encoder_deinit(void);

/**
 * Configure rotary encoder GPIO pins
 * @param clk_pin  CLK/A phase pin number
 * @param dt_pin   DT/B phase pin number
 * @param sw_pin   Switch/button pin number (use -1 if not connected)
 * @return 0 on success, -1 on error
 */
int rotary_encoder_config(int clk_pin, int dt_pin, int sw_pin);

/**
 * Read encoder data (non-blocking)
 * @param data  Pointer to store encoder data
 * @return 0 on success, -1 on error
 */
int rotary_encoder_read(struct encoder_data *data);

/**
 * Wait for encoder event and read data
 * @param data       Pointer to store encoder data
 * @param timeout_ms Timeout in milliseconds (0 = wait forever)
 * @return 1 if data available, 0 on timeout, -1 on error
 */
int rotary_encoder_wait_event(struct encoder_data *data, int timeout_ms);

/**
 * Reset encoder count to zero
 * @return 0 on success, -1 on error
 */
int rotary_encoder_reset(void);

/**
 * Set encoder count to specific value
 * @param count  New count value
 * @return 0 on success, -1 on error
 */
int rotary_encoder_set_count(int64_t count);

/**
 * Get current encoder count
 * @return Current count value
 */
int64_t rotary_encoder_get_count(void);

/**
 * Get and clear encoder delta
 * @return Delta value since last read
 */
int32_t rotary_encoder_get_delta(void);

#ifdef __cplusplus
}
#endif

#endif /* __HAL_ROTARY_ENCODER_H__ */
