#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
} display_manager_area_t;

typedef struct display_manager_screen_info {
    uint32_t width;
    uint32_t height;
    uint32_t dpi;
} display_manager_screen_info_t;

int display_manager_get_screen_info(display_manager_screen_info_t *info);

int display_manager_access_display();

int display_manager_leave_display();

void * display_manager_create_buffer(size_t key, size_t size);

void display_manager_destroy_buffer(size_t key, void *buffer);

int display_manager_screen_flush(const display_manager_area_t *area, uint8_t *px_map);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // DISPLAY_MANAGER_H