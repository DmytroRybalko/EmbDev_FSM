#ifdef TRAFFIC_TEST

#include "led.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

void app_main(void)
{
    traffic_init();

    int64_t start_us = esp_timer_get_time();
    bool switched_to_yellow = false;
    bool switched_back = false;

    while (true) {
        int64_t elapsed_ms = (esp_timer_get_time() - start_us) / 1000;

        if (!switched_to_yellow && elapsed_ms >= 10000) {
            traffic_set_mode(MODE_YELLOW);
            switched_to_yellow = true;
        }

        if (!switched_back && elapsed_ms >= 15000) {
            traffic_set_mode(MODE_TRAFFIC);
            switched_back = true;
        }

        traffic_update();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#endif