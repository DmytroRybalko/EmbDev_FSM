#include "led.h"
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

static traffic_state_t ledState    = STATE_RED;
static system_mode_t   currentMode = MODE_TRAFFIC;
static uint64_t        stateTimer  = 0;

static void set_traffic_lights(uint8_t r, uint8_t y, uint8_t g) {
    gpio_set_level(LED_RED, r);
    gpio_set_level(LED_YEL, y);
    gpio_set_level(LED_GRE, g);
}

static void enter_state(traffic_state_t newState)
{
    ledState   = newState;
    stateTimer = esp_timer_get_time();

    switch (newState) {
        case STATE_RED:          set_traffic_lights(1, 0, 0); break;
        case STATE_RED_YELLOW:   set_traffic_lights(1, 1, 0); break;
        case STATE_GREEN:        set_traffic_lights(0, 0, 1); break;
        case STATE_GREEN_BLINK:  set_traffic_lights(0, 0, 1); break;
        case STATE_YELLOW:       set_traffic_lights(0, 1, 0); break;
    }

    ESP_LOGI("TRAFFIC", "→ %d", newState);
}

void traffic_init(void)
{
    gpio_set_direction(LED_RED, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_YEL, GPIO_MODE_OUTPUT);
    gpio_set_direction(LED_GRE, GPIO_MODE_OUTPUT);

    //enter_state(STATE_RED);
}

void traffic_set_mode(system_mode_t mode)
{
    if (currentMode == mode) return;
    currentMode = mode;

    if (mode == MODE_TRAFFIC) {
        enter_state(STATE_RED); // start from the beginning
    } else {
        set_traffic_lights(0, 0, 0); // turn off all leds before yellow blinking
    }
}

void traffic_update(void)
{
    int64_t now     = esp_timer_get_time();
    int64_t elapsed = (now - stateTimer) / 1000; 
    
    if (currentMode == MODE_YELLOW) {
        gpio_set_level(LED_YEL, (now / (BLINK_INTERVAL * 1000LL)) %2);
        return;
    }
    
    switch (ledState) {
        case STATE_RED:
            if (elapsed >= TIMER_RED) {
                enter_state(STATE_RED_YELLOW);
            }
            break;
        
        case STATE_RED_YELLOW:
            if (elapsed >= TIMER_RED_YEL) {
                enter_state(STATE_GREEN);
            }
            break;

        case STATE_YELLOW:
            if (elapsed >= TIMER_YEL) {
                enter_state(STATE_RED);
            }
            break;
        
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