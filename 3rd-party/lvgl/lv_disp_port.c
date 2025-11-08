#include "lv_disp_port.h"

#include <stddef.h>
#include <errno.h>

#define DBG_TAG    "LVGL_PORT"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>

#include "lvgl.h"
#include "display_manager.h"
#include "rtdef.h"

static char *BUFFER_KEY = "lvgl_buf_key";
static lv_disp_t *s_disp = NULL;
static void *s_draw_buffer = NULL;

static void flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map) {
    if(!lv_display_flush_is_last(disp)) return;

    display_manager_area_t dm_area = {
        .x1 = area->x1,
        .x2 = area->x2,
        .y1 = area->y1,
        .y2 = area->y2
    };

    display_manager_screen_flush(&dm_area, px_map);

    lv_display_flush_ready(disp);
}

static void flush_wait_cb(lv_display_t * disp) {

}

int lv_port_disp_init() {
    display_manager_screen_info_t screen_info;
    int err = display_manager_get_screen_info(&screen_info);

    if (err) {
        LOG_E("display_manager_get_screen_info failed err = %d", err);
        return err;
    }

    uint32_t width = screen_info.width;
    uint32_t height = screen_info.height;

    s_disp = lv_display_create(width, height);

    if (s_disp == NULL) {
        return -ENOMEM;
    }

    lv_display_set_dpi(s_disp, screen_info.dpi);

    lv_display_set_flush_cb(s_disp, flush);
    lv_display_set_flush_wait_cb(s_disp, flush_wait_cb);

    lv_color_format_t color_format = lv_display_get_color_format(s_disp);
    uint32_t stride = lv_draw_buf_width_to_stride(width, color_format);
    size_t buffer_size = stride * height;
    s_draw_buffer = display_manager_create_buffer((size_t)&BUFFER_KEY, buffer_size);
    if (s_draw_buffer == NULL) {
        lv_port_disp_deinit();
        return -ENOMEM;
    }

    lv_display_set_buffers(s_disp, s_draw_buffer, NULL, buffer_size, LV_DISPLAY_RENDER_MODE_DIRECT);

    err = display_manager_access_display();
    if (err) {
        LOG_E("display_manager_access_display failed err = %d", err);
        return err;
    }
    
    return 0;
}

void lv_port_disp_deinit() {
    if (s_draw_buffer != NULL) {
        display_manager_destroy_buffer((size_t)&BUFFER_KEY, s_draw_buffer);
    }

    if (s_disp != NULL) {
        lv_display_delete(s_disp);
    }

    display_manager_leave_display();
}