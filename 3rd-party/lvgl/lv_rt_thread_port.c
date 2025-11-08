#ifdef __RTTHREAD__

#include "lv_rt_thread_port.h"

#include <pthread.h>

#include "lvgl.h"
#include "lv_disp_port.h"

#include <rtthread.h>

#define DBG_TAG    "LVGL"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>

#ifndef PKG_LVGL_THREAD_STACK_SIZE
    #define PKG_LVGL_THREAD_STACK_SIZE 4096
#endif /* PKG_LVGL_THREAD_STACK_SIZE */

#ifndef PKG_LVGL_THREAD_PRIO
    #define PKG_LVGL_THREAD_PRIO (RT_THREAD_PRIORITY_MAX*2/3)
#endif /* PKG_LVGL_THREAD_PRIO */

extern void lv_port_indev_init(void);

static pthread_t s_lvgl_thread = NULL;

static volatile bool s_is_lvgl_thread_running = false; 
static lvgl_gui_deinit_func s_gui_deinit = NULL;

#if LV_USE_LOG && LV_LOG_PRINTF == 0
static void lv_rt_log(lv_log_level_t level, const char * buf) {
    switch (level) {
        case LV_LOG_LEVEL_TRACE:
            LOG_D(buf);
            break;
        case LV_LOG_LEVEL_INFO:
        case LV_LOG_LEVEL_USER:
            LOG_I(buf);
            break;
        case LV_LOG_LEVEL_WARN:
            LOG_W(buf);
            break;
        case LV_LOG_LEVEL_ERROR:
            LOG_E(buf);
            break;
        default:
            break;
    }
}
#endif /* LV_USE_LOG */

static void* lvgl_thread_entry(void *parameter) {
    s_is_lvgl_thread_running = true;

    while(s_is_lvgl_thread_running) {
        uint32_t time_until_next = lv_timer_handler();
        rt_thread_mdelay(time_until_next);
    }

    return NULL;
}

int lvgl_thread_init(uint32_t stack_size, uint8_t priority, lvgl_gui_init_func gui_init, lvgl_gui_deinit_func gui_deinit) {
    if (stack_size == 0) {
        LV_LOG_ERROR("stack_size must > 0");
        return -EINVAL;
    }

    if (gui_init == NULL) {
        LV_LOG_ERROR("gui_init func is NULL");
        return -EINVAL;
    }

    s_gui_deinit = gui_deinit;

    lv_init();

    #if LV_USE_LOG && LV_LOG_PRINTF == 0
        lv_log_register_print_cb(lv_rt_log);
    #endif /* LV_USE_LOG */

    lv_tick_set_cb(&rt_tick_get_millisecond);

    int err = lv_port_disp_init();
    if (err) {
        LV_LOG_ERROR("lv_port_disp_init failed! err = %d", err);
        lvgl_thread_deint();
        return err;
    }

    lv_port_indev_init();

    err = gui_init();
    if (err) {
        LV_LOG_ERROR("lv_user_gui_init failed! err = %d", err);
        lvgl_thread_deint();
        return err;
    }

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    struct sched_param sched_param;
    sched_param.sched_priority = priority;
    pthread_attr_setschedparam(&attr, &sched_param);
    pthread_attr_setstacksize(&attr, stack_size);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);

    err = pthread_create(&s_lvgl_thread, &attr, lvgl_thread_entry, NULL);
    if (err) {
        LV_LOG_ERROR("Failed to create lvgl thread");
        return err;
    }

    return 0;
}

int lvgl_thread_deint() {
    if (s_is_lvgl_thread_running) {
        s_is_lvgl_thread_running = false;
        pthread_join(s_lvgl_thread, NULL);
        s_lvgl_thread = NULL;
        if (s_gui_deinit) {
            s_gui_deinit();
        }
        lv_port_disp_deinit();
    }
}

RT_WEAK char *rt_strcpy(char *dst, const char *src)
{
    char *dest = dst;

    while (*src != '\0')
    {
        *dst = *src;
        dst++;
        src++;
    }

    *dst = '\0';
    return dest;
}

RT_WEAK rt_tick_t rt_tick_get_millisecond(void)
{
#if 1000 % RT_TICK_PER_SECOND == 0u
    return rt_tick_get() * (1000u / RT_TICK_PER_SECOND);
#else
    #warning "rt-thread cannot provide a correct 1ms-based tick any longer,\
    please redefine this function in another file by using a high-precision hard-timer."
    return 0;
#endif /* 1000 % RT_TICK_PER_SECOND == 0u */
}

#endif /*__RTTHREAD__*/
