#include "button.h"
#include "freertos/FreeRTOS.h"  
#include "freertos/task.h"  
#include "driver/gpio.h"  
#include "esp_timer.h"  
#include "esp_log.h"  
	
#define BUTTON_PIN GPIO_NUM_4
	
static const char *TAG = "FSM_TIMERS";  
	
static esp_timer_handle_t debounce_timer;  
static esp_timer_handle_t long_press_timer;  
	
static volatile bool short_press_event = false;  
static volatile bool long_press_event  = false;  
volatile bool is_holding = false;  
	
static void long_press_cb(void *arg) {  
	long_press_event = true;  
	is_holding = false;  
}  
	
static void debounce_cb(void *arg) {  
	int level = gpio_get_level(BUTTON_PIN);  
	
	if (level == 0) {  
		is_holding = true;  
	esp_timer_start_once(long_press_timer, 2000000);  
	}  
}  
	
static void IRAM_ATTR button_isr_handler(void* arg) {  
	int level = gpio_get_level(BUTTON_PIN);  
	
	esp_timer_stop(debounce_timer);  
	
	if (level == 1) {  
		if (is_holding) {  
			if (esp_timer_stop(long_press_timer) == ESP_OK) {  
				short_press_event = true;  
			}  
			is_holding = false;  
		} 
	}  
	
	esp_timer_start_once(debounce_timer, 50000);  
}  
	
void button_init(void) {  
	gpio_config_t io_conf = {  
		.intr_type = GPIO_INTR_ANYEDGE,  
		.mode = GPIO_MODE_INPUT,  
		.pin_bit_mask = (1ULL << BUTTON_PIN),  
		.pull_down_en = GPIO_PULLDOWN_DISABLE,  
		.pull_up_en = GPIO_PULLUP_ENABLE  
	};  
	
	gpio_config(&io_conf);  
    
    const esp_timer_create_args_t debounce_args = {  
        .callback = &debounce_cb,  
        .name = "debounce_timer"  
    };  
    esp_timer_create(&debounce_args, &debounce_timer);  
    
    const esp_timer_create_args_t long_press_args = {  
        .callback = &long_press_cb,  
        .name = "long_press_timer"  
    };  
    esp_timer_create(&long_press_args, &long_press_timer);  
    
    gpio_install_isr_service(0);  
    gpio_isr_handler_add(BUTTON_PIN, button_isr_handler, NULL);  
}

btn_event_t button_update(void)
{
    if (short_press_event) {  
		ESP_LOGW(TAG, "Дія: КОРОТКИЙ НАТИСК (SHORT PRESS)");  
		short_press_event = false;  
		return BTN_EVENT_SHORT_PRESS;
	}  
	
	if (long_press_event) {  
		ESP_LOGE(TAG, "Дія: ДОВГИЙ НАТИСК (LONG PRESS)");  
		long_press_event = false;
		return BTN_EVENT_LONG_PRESS;
	}
	
	return BTN_EVENT_NONE;
}
