#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "drv_rotary_encoder.h"

static volatile int running = 1;

/* 信号处理函数，用于优雅退出 */
void signal_handler(int sig)
{
    printf("\nReceived signal %d, exiting...\n", sig);
    running = 0;
}

int main(int argc, char *argv[])
{
    struct encoder_data data;
    int ret;
    int timeout_ms = 1000;  /* 默认超时1秒 */
    int64_t last_count = 0;
    int button_long_press_ms = 2000;  /* 长按2秒 */
    uint32_t button_press_start = 0;

    /* 解析命令行参数 */
    if (argc > 1) {
        timeout_ms = atoi(argv[1]);
        printf("Using timeout: %d ms\n", timeout_ms);
    }

    /* 设置信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 初始化编码器 */
    if (rotary_encoder_init() < 0) {
        printf("Failed to initialize encoder\n");
        return -1;
    }

    /* 配置GPIO引脚 */
    printf("Configuring encoder pins...\n");
    if (rotary_encoder_config(5, 42, 43) < 0) {
        printf("Failed to configure encoder\n");
        rotary_encoder_deinit();
        return -1;
    }

    printf("=== Rotary Encoder Blocking Demo ===\n");
    printf("- Rotate encoder to see count changes\n");
    printf("- Press button to see press events\n");
    printf("- Long press button (2s) to reset count\n");
    printf("- Press Ctrl+C to exit\n");
    printf("===================================\n\n");

    /* 主循环 - 使用阻塞等待 */
    while (running) {
        /* 阻塞等待编码器事件，带超时 */
        ret = rotary_encoder_wait_event(&data, timeout_ms);

        if (ret > 0) {
            /* 有数据可读 */

            /* 处理旋转事件 */
            if (data.delta != 0) {
                printf("[%u] Rotation: ", data.timestamp);
                printf("Delta=%+3d, Count=%5lld, Direction=%-3s, Speed=%.1f step/s\n",
                       data.delta, 
                       data.total_count,
                       data.direction == ENCODER_DIR_CW ? "CW" : "CCW",
                       (float)abs(data.delta) * 1000.0 / timeout_ms);

                last_count = data.total_count;
            }

            /* 处理按键事件 */
            if (data.button_state && button_press_start == 0) {
                /* 按键按下 */
                button_press_start = data.timestamp;
                printf("[%u] Button pressed\n", data.timestamp);
            } else if (!data.button_state && button_press_start > 0) {
                /* 按键释放 */
                uint32_t press_duration = data.timestamp - button_press_start;
                printf("[%u] Button released, duration: %u ms\n", 
                       data.timestamp, press_duration);

                /* 检查是否为长按 */
                if (press_duration >= button_long_press_ms) {
                    printf(">>> Long press detected! Resetting count...\n");
                    rotary_encoder_reset();
                    last_count = 0;
                }

                button_press_start = 0;
            }

        } else if (ret == 0) {
            /* 超时，没有新数据 */
            printf("[Timeout] No encoder activity for %d ms\n", timeout_ms);
        } else {
            /* 错误 */
            printf("Error waiting for encoder event\n");
            break;
        }
    }

    /* 清理资源 */
    printf("\nCleaning up...\n");

    /* 读取最终状态 */
    if (rotary_encoder_read(&data) == 0) {
        printf("Final count: %lld\n", data.total_count);
    }

    rotary_encoder_deinit();

    return 0;
}
