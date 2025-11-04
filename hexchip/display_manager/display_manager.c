#include "display_manager_internal.h"
#include "display_manager.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <rtthread.h>
#include <lwp_shm.h>

#define DBG_TAG    "DISP_MGR_API"
#define DBG_LVL    DBG_INFO
#include <rtdbg.h>

typedef struct channel_msg_context channel_msg_context_t;
typedef void (*prepare_data_cb)(channel_msg_context_t *context, void *data);
typedef int (*extract_result_cb)(channel_msg_context_t *context, void *ret_data, void *result);


typedef struct screen_flush_arg {
    const display_manager_area_t *area;
    uint8_t *px_map;
} screen_flush_arg_t;


typedef struct channel_msg_context {
    display_manager_cmd_t cmd;
    void *cmd_arg;
    size_t data_size;
    prepare_data_cb prepare_data;
    extract_result_cb extract_result;
};

rt_inline int send_channel_msg(channel_msg_context_t *context, void *result) {
    int channel = rt_channel_open("display_manager", 0);

    if (channel == -1) {
        LOG_E("Error: could not find the display_manager channel!");
        return ENODEV;
    }

    size_t key = (size_t)(intptr_t) rt_thread_self();
    int shmid = lwp_shmget(key, context->data_size, 1);

    if (shmid == -1) {
        LOG_E("Fail to allocate a shared memory!");
        return -ENOMEM;
    }

    void *shm_vaddr = lwp_shmat(shmid, NULL);
    if (shm_vaddr == NULL) {
        LOG_E("invalid shm virtual address!");
        lwp_shmrm(shmid);
        return -EINVAL;
    }

    context->prepare_data(context, shm_vaddr);
   
    struct rt_channel_msg channel_msg;
    channel_msg.type = RT_CHANNEL_RAW;
    channel_msg.u.d = (void *)(intptr_t)shmid;

    rt_err_t rt_err = rt_channel_send_recv(channel, &channel_msg, &channel_msg);
    if (rt_err != RT_EOK) {
        LOG_E("rt_channel_send_recv failed!");
        return EIO;
    }

    int ret = context->extract_result(context, shm_vaddr, result);

    lwp_shmdt(shm_vaddr);
    lwp_shmrm(shmid);

    rt_channel_close(channel);

    return ret;
}

rt_inline void prepare_call_msg(channel_msg_context_t *context, void *data) {
    display_manager_msg_t *msg = data;
    msg->cmd = context->cmd;
    msg->error = 0;
    switch (msg->cmd) {
        case DISPLAY_MANAGER_GET_SCREEN_INFO:
            break;
        case DISPLAY_MANAGER_ACCESS_DISPLAY:
        case DISPLAY_MANAGER_LEAVE_DISPLAY:
            msg->arg.pid = getpid();
            break;
        case DISPLAY_MANAGER_FLUSH_SCREEN:
        {
            screen_flush_arg_t *arg = context->cmd_arg;
            int px_map_shmid = (((int *)arg->px_map) - 1)[0];
            msg->arg.screen_flush_arg.area = *arg->area;
            msg->arg.screen_flush_arg.px_map_shmid = px_map_shmid;
            break;
        }
        default:
            break;
    }
}

rt_inline int extract_call_result(channel_msg_context_t *context, void *ret_data, void *result) {
    display_manager_msg_t *msg = ret_data;

    switch (context->cmd) {
        case DISPLAY_MANAGER_GET_SCREEN_INFO:
        {
            if (msg->error == 0) {
                memcpy(result, &msg->ret.screen_info, sizeof(display_manager_screen_info_t));
            }
            break;
        }
        case DISPLAY_MANAGER_ACCESS_DISPLAY:
        case DISPLAY_MANAGER_LEAVE_DISPLAY:
        case DISPLAY_MANAGER_FLUSH_SCREEN:
            break;
        default:
            break;
    }
    return msg->error;
}

rt_inline void init_channel_msg_context(channel_msg_context_t *context, display_manager_cmd_t cmd, void *arg) {
    context->cmd = cmd;
    context->cmd_arg = arg;
    context->data_size = sizeof(display_manager_msg_t);
    context->prepare_data = prepare_call_msg;
    context->extract_result = extract_call_result;
}


int display_manager_get_screen_info(display_manager_screen_info_t *info) {
    channel_msg_context_t context;
    init_channel_msg_context(&context, DISPLAY_MANAGER_GET_SCREEN_INFO, NULL);

    return send_channel_msg(&context, info);
}

int display_manager_access_display() {
    channel_msg_context_t context;
    init_channel_msg_context(&context, DISPLAY_MANAGER_ACCESS_DISPLAY, NULL);

    return send_channel_msg(&context, NULL);
}

int display_manager_leave_display() {
    channel_msg_context_t context;
    init_channel_msg_context(&context, DISPLAY_MANAGER_LEAVE_DISPLAY, NULL);

    return send_channel_msg(&context, NULL);
}

void * display_manager_create_buffer(size_t key, size_t size) {
    int shmid = lwp_shmget(key, size + sizeof(int), 1);
    if (shmid == -1) {
        LOG_E("Fail to allocate a shared memory!");
        return NULL;
    }

    void *shm_vaddr = lwp_shmat(shmid, NULL);
    if (shm_vaddr == NULL) {
        LOG_E("invalid shm virtual address!");
        lwp_shmrm(shmid);
        return NULL;
    }

    // The shmid is stored at the head of the virtual memory.
    ((int *)shm_vaddr)[0] = shmid;

    // The buffer needs to be offset by the size of an int.
    return (int *)shm_vaddr + 1;
}

void display_manager_destroy_buffer(size_t key, void *buffer) {
    int shmid = lwp_shmget(key, 0, 0);
    if (shmid == -1) {
        LOG_E("Fail to get the share memory! key = %d", key);
        return;
    }

    lwp_shmdt(buffer);
    lwp_shmrm(shmid);
}

int display_manager_screen_flush(const display_manager_area_t *area, uint8_t *px_map) {
    channel_msg_context_t context;
    screen_flush_arg_t arg = {
        .area = area,
        .px_map = px_map
    };
    init_channel_msg_context(&context, DISPLAY_MANAGER_FLUSH_SCREEN, &arg);

    return send_channel_msg(&context, NULL);
}