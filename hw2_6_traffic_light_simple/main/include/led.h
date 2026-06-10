#pragma once

typedef enum {
    MODE_TRAFFIC,
    MODE_YELLOW
} system_mode_t;

void traffic_init(void);
void traffic_set_mode(system_mode_t mode);
void traffic_update(void);