#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "button.h"
#include "led.h"

void app_main(void)
{
    traffic_init();
    button_init();
    
    while(true)
    {
        btn_event_t event = button_update();

        if (event == BTN_EVENT_SHORT_PRESS) {
            traffic_set_mode(MODE_TRAFFIC);
        } else if (event == BTN_EVENT_LONG_PRESS) {
            traffic_set_mode(MODE_YELLOW);
        }

        traffic_update();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
