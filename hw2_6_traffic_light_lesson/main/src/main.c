#include "main.h"
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "TRAFFIC LIGHTS";

esp_timer_handle_t debounce_timer;
esp_timer_handle_t long_press_timer;

void hardware_init() {
    
    gpio_config_t io_conf = {
        .intr_type      = GPIO_INTR_ANYEDGE,
        .mode           = GPIO_MODE_INPUT,
        .pin_bit_mask   = (1ULL << BUTTON_PIN),
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_ENABLE
    };

    if (gpio_config(&io_conf) != ESP_OK) {
        ESP_LOGI(TAG, "Failed to configure GPIO");
        esp_restart();
    }

    const esp_timer_create_args_t debounce_args = {
        .callback   = get_short_timer_callback(),
        .name       = "debounce_timer"
    };

    esp_timer_create(&debounce_args, &debounce_timer);

    const esp_timer_create_args_t long_press_args = {
        .callback = get_long_timer_callback(),
        .name     = "long_press_timer"
    };

    esp_timer_create(&long_press_args, &long_press_timer);

    button_handler.state = IDLE;
    button_handler.debounce_timer = debounce_timer;
    button_handler.long_press_timer = long_press_timer;

    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, get_button_callback(), NULL);

    init_traffic_light(GPIO_NUM_35, GPIO_NUM_36, GPIO_NUM_37);
    start_traffic_light();

    ESP_LOGI(TAG, "System is ready. Waiting...");
}

void app_main(void) {
    hardware_init();

    while (1) {
        if (get_state() == SHORT_PRESS) {
            ESP_LOGW(TAG, "Дія: КОРОТКИЙ НАТИСК (SHORT PRESS)");
            enable_blinking_mode(false);
            reset_state();
        }
        if (get_state() == LONG_PRESS) {
            ESP_LOGE(TAG, "Дія: ДОВГИЙ НАТИСК (LONG PRESS)");
            enable_blinking_mode(true);
            reset_state();
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}