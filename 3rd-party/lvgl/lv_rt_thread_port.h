#ifndef LV_RT_THREAD_PORT_H
#define LV_RT_THREAD_PORT_H

#include <stdint.h>

typedef int (*lvgl_gui_init_func)();
typedef void (*lvgl_gui_deinit_func)();

int lvgl_thread_init(uint32_t stack_size, uint8_t priority, lvgl_gui_init_func gui_init, lvgl_gui_deinit_func gui_deinit);

int lvgl_thread_deint();

#endif // LV_RT_THREAD_PORT_H