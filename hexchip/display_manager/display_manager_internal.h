#ifndef DISPLAY_MANAGER_INTERNAL_H
#define DISPLAY_MANAGER_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include "display_manager.h"

typedef enum display_manager_cmd {
    DISPLAY_MANAGER_UNKNOWN,
    DISPLAY_MANAGER_GET_SCREEN_INFO,
    DISPLAY_MANAGER_ACCESS_DISPLAY,
    DISPLAY_MANAGER_LEAVE_DISPLAY,
    DISPLAY_MANAGER_FLUSH_SCREEN,
} display_manager_cmd_t;

typedef struct display_manager_screen_flush_cmd_arg {
    pid_t pid;
    display_manager_area_t area;
    int px_map_shmid;
} display_manager_screen_flush_cmd_arg_t;

typedef struct display_manager_msg {
    display_manager_cmd_t cmd;
    union {
        pid_t pid;
        display_manager_screen_flush_cmd_arg_t screen_flush_arg;
    } arg;
    union {
        display_manager_screen_info_t screen_info;
    } ret;
    int error;
} display_manager_msg_t;

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif // DISPLAY_MANAGER_INTERNAL_H