#pragma once
#include <stdbool.h>

typedef enum {
    BTN_IDLE,
    BTN_PRESSING,
    BTN_PRESSED,
    BTN_LONG_PRESSED,
    BTN_RELEASING
} btn_state_t;

typedef enum {
    BTN_EVENT_NONE,
    BTN_EVENT_SHORT_PRESS,
    BTN_EVENT_LONG_PRESS
} btn_event_t;

void button_init(void);
btn_event_t button_update(void);