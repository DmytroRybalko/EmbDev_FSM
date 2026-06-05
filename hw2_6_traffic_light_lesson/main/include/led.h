#ifndef LIGHTS_H
#define LIGHTS_H

#include "config.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"

#ifndef TIME_RED
#define TIME_RED            5000
#endif

#ifndef TIME_RED_YELLOW
#define TIME_RED_YELLOW     1000
#endif

#ifndef TIME_GREEN
#define TIME_GREEN          5000
#endif

#ifndef TIME_YELLOW
#define TIME_YELLOW         2000
#endif

#ifndef TIME_BLINK
#define TIME_BLINK          500
#endif

typedef enum {
    STATE_RED,
    STATE_RED_YELLOW,
    STATE_GREEN,
    STATE_YELLOW,
    STATE_BLINKING_ON,
    STATE_BLINKING_OFF
} traffic_state_t;

void enable_blinking_mode(bool state);
void start_traffic_light(void);
esp_err_t init_traffic_light(gpio_num_t red, gpio_num_t yellow, gpio_num_t green);

#endif // LIGHTS_H