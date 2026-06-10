#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

// SETUP LED PINS
#define LED_RED   GPIO_NUM_35
#define LED_YEL   GPIO_NUM_36
#define LED_GRE   GPIO_NUM_37

// SETUP TIME RANGES
#define TIMER_RED           5000
#define TIMER_RED_YEL       3000
#define TIMER_GRE           5000
#define TIMER_YEL           3000
#define TIMER_GREEN_BLINK   3000
#define BLINK_INTERVAL      500

typedef enum {
    STATE_RED,
    STATE_RED_YELLOW,
    STATE_YELLOW,
    STATE_GREEN,
    STATE_GREEN_BLINK
} traffic_state_t;

static traffic_state_t ledState = STATE_RED;
static uint64_t stateTimer      = 0;

static void set_traffic_lights(uint8_t r, uint8_t y, uint8_t g) {
    gpio_set_level(LED_RED, r);
    gpio_set_level(LED_YEL, y);
    gpio_set_level(LED_GRE, g);
}

static void enter_state(traffic_state_t newState)
{
    ledState   = newState;
    stateTimer = esp_timer_get_time();

    // Встановлюємо лeds при вході в стан, а не в циклі
    switch (newState) {
        case STATE_RED:          set_traffic_lights(1, 0, 0); break;
        case STATE_RED_YELLOW:   set_traffic_lights(1, 1, 0); break;
        case STATE_GREEN:        set_traffic_lights(0, 0, 1); break;
        case STATE_GREEN_BLINK:  set_traffic_lights(0, 0, 1); break;
        case STATE_YELLOW:       set_traffic_lights(0, 1, 0); break;
    }

    ESP_LOGI("TRAFFIC", "→ %d", newState);
}

void run_traffic_lights(void)
{
    int64_t now     = esp_timer_get_time();
    int64_t elapsed = (now - stateTimer) / 1000; 
    
    switch (ledState) {
        // Red led lights for 5 sec then switch to STATE_RED_YELLOW state
        case STATE_RED:
            if (elapsed >= TIMER_RED) {
                enter_state(STATE_RED_YELLOW);
            }
            break;
        
        // Yellow start lights for 3 sec while RED is steel lighting
        // Switch to STATE GREEN
        case STATE_RED_YELLOW:
            if (elapsed >= TIMER_RED_YEL) {
                enter_state(STATE_GREEN);
            }
            break;

        // Yellow start lights for 3 second after green is off
        // Switch to STATE_RED
        case STATE_YELLOW:
            if (elapsed >= TIMER_YEL) {
                enter_state(STATE_RED);
            }
            break;
        
        // 1. Green led lights for 5 sec
        // 2. Green led starts blinking for 3 sec
        // 3. Green led turn off
        // 4. Switch to STATE_YELLOW
        case STATE_GREEN:
            if (elapsed >= TIMER_GRE) {
                enter_state(STATE_GREEN_BLINK);
            }
            break;

        case STATE_GREEN_BLINK:
            gpio_set_level(LED_GRE, (elapsed / BLINK_INTERVAL) % 2);
            if (elapsed >= TIMER_GREEN_BLINK)
                enter_state(STATE_YELLOW);
            break;
        }
    }

void app_main(void)
{
    // LEDs SETUP
    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_YEL, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GRE, GPIO_MODE_OUTPUT);
    
    while(true)
    {
        run_traffic_lights();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
