#include "main.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

button_handler_t button_handler;

//void IRAM_ATTR long_press_cb(void *arg) {
//    button_handler.state = LONG_PRESS;
//}

void IRAM_ATTR long_press_cb(void *arg) {
    if (button_handler.state == HOLDING) {  // Only set if coming from HOLDING
        button_handler.state = LONG_PRESS;
    }
}

void IRAM_ATTR debounce_cb(void *arg) {
    int level = gpio_get_level(BUTTON_PIN);

    if (level == 0) {
        button_handler.state = HOLDING;
        esp_timer_start_once(button_handler.long_press_timer, 2000000);
    }
}

void IRAM_ATTR button_isr_handler(void* arg) {
    int level = gpio_get_level(BUTTON_PIN);

    esp_timer_stop(button_handler.debounce_timer);

    if (level == 1) {
        if (button_handler.state == HOLDING) {
            if (esp_timer_stop(button_handler.long_press_timer) == ESP_OK) {
                button_handler.state = SHORT_PRESS;
            }
        }
    }

    esp_timer_start_once(button_handler.debounce_timer, 50000);
}

button_callback_t get_long_timer_callback() {
    return long_press_cb;
}

button_callback_t get_short_timer_callback() {
    return debounce_cb;
}

button_callback_t get_button_callback() {
    return button_isr_handler;
}

void init_button_handler(button_handler_t btn) {
    button_handler = btn;
}

state_t get_state() {
    return button_handler.state;
}

void reset_state() {
    button_handler.state = IDLE;
}
