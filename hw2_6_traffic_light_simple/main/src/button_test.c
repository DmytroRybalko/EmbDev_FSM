
#ifdef BUTTON_TEST

#include "button.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "BTN_TEST"

void app_main(void)
{
    button_init();

    ESP_LOGI(TAG, "Diagnostic test started");
    ESP_LOGI(TAG, "GPIO18 with pull-up: released=1, pressed=0");

    while (1) {
        button_update();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

#endif
