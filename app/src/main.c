/*
* main.c
*/

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)
#define LED1_NODE DT_ALIAS(led1)
#define LED2_NODE DT_ALIAS(led2)
#define LED3_NODE DT_ALIAS(led3)

static const struct gpio_dt_spec leds[] = {
    GPIO_DT_SPEC_GET(LED0_NODE, gpios),
    GPIO_DT_SPEC_GET(LED1_NODE, gpios),
    GPIO_DT_SPEC_GET(LED2_NODE, gpios),
    GPIO_DT_SPEC_GET(LED3_NODE, gpios),
};


int main(void){
    uint8_t ret;
    
    for(uint8_t i = 0; i < ARRAY_SIZE(leds); i++){
        
        if(!gpio_is_ready_dt(&leds[1])){
            return -1;
        }    
        ret = gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_ACTIVE); 
        if (ret < 0){
            return ret;
        }
    } 
    
    while(1){
        for(uint8_t i = 0; i < ARRAY_SIZE(leds); i++){ 
            gpio_pin_toggle_dt(&leds[i]);
            k_msleep(500); 
        }
    }
        
    


    return 0;
}