/**
 * @file miniproj_fsm.c
 */

#include "miniproj_fsm.h"

//initialze and run
void state_machine_init(){
    state_obj.prev_state = NONE;
    state_obj.ascii_code = 0;
    state_obj.bit_index = 0;
    state_obj.str = NULL;
    state_obj.time_held = 0;
    state_obj.string_size = 0;

    smf_set_initial(SMF_CTX(&state_obj), &miniproj_states[STATE0]);
}

int state_machine_run(){
    return smf_run_state(SMF_CTX(&state_obj));
}

//helper functions
char* add_char(char* src, uint8_t new_char){
    uint8_t str_size = 0;
    while(src[str_size] != '\0'){
        str_size++;
    }

    char* dest = malloc(sizeof(char) * (str_size + 2));
    if (!dest) return NULL;

    uint8_t i = 0;
    while(src[i] != '\0'){
        dest[i] = src[i];
        i++;
    }

    dest[str_size] = (char)new_char;
    dest[str_size + 1] = '\0';
    free(src);
    return dest;
}

void pwm_fade(){
    if(state_obj.brighter){
        if(state_obj.duty_cycle >= 100){
            LED_pwm(LED0, state_obj.duty_cycle);
            LED_pwm(LED1, state_obj.duty_cycle);
            LED_pwm(LED2, state_obj.duty_cycle);
            LED_pwm(LED3, state_obj.duty_cycle);
            k_msleep(10);
            state_obj.brighter = false;
        }
        else{
            LED_pwm(LED0, state_obj.duty_cycle);
            LED_pwm(LED1, state_obj.duty_cycle);
            LED_pwm(LED2, state_obj.duty_cycle);
            LED_pwm(LED3, state_obj.duty_cycle);
            state_obj.duty_cycle++;
            k_msleep(10);
        }
    }
    if(!state_obj.brighter){
        if(state_obj.duty_cycle <= 0){
            LED_pwm(LED0, state_obj.duty_cycle);
            LED_pwm(LED1, state_obj.duty_cycle);
            LED_pwm(LED2, state_obj.duty_cycle);
            LED_pwm(LED3, state_obj.duty_cycle);
            state_obj.brighter = true;
            k_msleep(10);
        }
        else{
            LED_pwm(LED0, state_obj.duty_cycle);
            LED_pwm(LED1, state_obj.duty_cycle);
            LED_pwm(LED2, state_obj.duty_cycle);
            LED_pwm(LED3, state_obj.duty_cycle);
            state_obj.duty_cycle--;
            k_msleep(10);
        }
    }
}


// state entry, run and exit functions
static void state0_entry(void *o){
    
    LED_blink(LED3, LED_1HZ);
    state_obj.ascii_code = 0;
    state_obj.bit_index = 0;
    state_obj.str = NULL;
    state_obj.time_held = 0; 
}

static enum smf_state_result state0_run(void *o){

    if(BTN_is_pressed(BTN0)){
        state_obj.ascii_code = state_obj.ascii_code << 1;
        state_obj.bit_index++;
        LED_set(LED0, LED_ON);
        k_msleep(300);
        LED_set(LED0, LED_OFF);
        printk("%d", 0);
    }

    if(BTN_is_pressed(BTN1)){
        state_obj.ascii_code = state_obj.ascii_code << 1 | 1;
        state_obj.bit_index++;
        LED_set(LED1, LED_ON);
        k_msleep(300);
        LED_set(LED1, LED_OFF);
        printk("%d", 1);
    }

    if (BTN_is_pressed(BTN2)) {
        state_obj.ascii_code = 0;
        state_obj.bit_index = 0;
        state_obj.string_size = 0;
    }

    if(BTN_is_pressed(BTN3) && state_obj.bit_index >= 8){
        if (state_obj.str) free(state_obj.str);
        state_obj.str = malloc(2* sizeof(char));
        state_obj.str[0] = (char)state_obj.ascii_code;
        state_obj.str[1] = '\0';
        printk(" : %c", state_obj.ascii_code);
        printk("\n");
        printk("----SAVED----\n\n");
        smf_set_state(SMF_CTX(&state_obj), &miniproj_states[STATE1]);
        state_obj.prev_state = STATE0;
        state_obj.string_size++;
        state_obj.bit_index = 0;
        state_obj.ascii_code = 0;

    }

   
    if (BTN_is_pressed(BTN0) && BTN_is_pressed(BTN1)) {
        uint32_t start = (uint32_t)state_obj.time_held;
        uint32_t now = k_uptime_get_32();
        if (start == 0U) {
            state_obj.time_held = now; /* start timer */
        } else if ((uint32_t)(now - start) >= 2500U) {
            state_obj.prev_state = STATE0;
          
            smf_set_state(SMF_CTX(&state_obj), &miniproj_states[STATE3]);
            state_obj.time_held = 0; /* reset to avoid repeated transitions */
            printk("----STANDBY----\n");
        }
    } else {
        state_obj.time_held = 0;
    }
    return SMF_EVENT_HANDLED;

}

static void state1_entry(void *o){
    LED_blink(LED3, LED_4HZ);
    state_obj.time_held = 0;
    k_msleep(1000);
}

static enum smf_state_result state1_run(void *o){

    if(BTN_is_pressed(BTN0)){
        state_obj.ascii_code = state_obj.ascii_code << 1;
        state_obj.bit_index++;
        LED_set(LED0, LED_ON);
        k_msleep(300);
        LED_set(LED0, LED_OFF);
        printk("%d", 0);
    }

    if(BTN_is_pressed(BTN1)){
        state_obj.ascii_code = state_obj.ascii_code << 1 | 1;
        state_obj.bit_index++;
        LED_set(LED1, LED_ON);
        k_msleep(300);
        LED_set(LED1, LED_OFF);
        printk("%d", 1);
    }

    if (BTN_is_pressed(BTN2)) {
        state_obj.ascii_code = 0;
        state_obj.bit_index = 0;
        state_obj.string_size = 0;
        free(state_obj.str);
        smf_set_state(SMF_CTX(&state_obj), &miniproj_states[STATE0]);
    }

    if(state_obj.bit_index == 8){
        state_obj.str = add_char(state_obj.str, state_obj.ascii_code);
        printk(" : %c", state_obj.ascii_code);
        printk("\n");
        state_obj.bit_index = 0;
        state_obj.ascii_code = 0;
        state_obj.string_size++;

    }

    if(BTN_is_pressed(BTN3)){
        state_obj.prev_state = STATE1;
        printk("----SAVED_STRING----\n");
        smf_set_state(SMF_CTX(&state_obj), &miniproj_states[STATE2]);
    }


    if (BTN_is_pressed(BTN0) && BTN_is_pressed(BTN1)) {
        uint32_t start = (uint32_t)state_obj.time_held;
        uint32_t now = k_uptime_get_32();
        if (start == 0U) {
            state_obj.time_held = now; /* start timer */
        } else if ((uint32_t)(now - start) >= 2500U) {
            state_obj.prev_state = STATE1;
            smf_set_state(SMF_CTX(&state_obj), &miniproj_states[STATE3]);
            printk("----STANDBY----\n");
            state_obj.time_held = 0; /* reset to avoid repeated transitions */
        }
    } else {
        state_obj.time_held = 0;
    }

    return SMF_EVENT_HANDLED;
}

static void state2_entry(void *o){
    LED_blink(LED3, LED_16HZ);
    state_obj.time_held = 0; 
}

static enum smf_state_result state2_run(void *o){
    if(BTN_is_pressed(BTN3)){
        printk("%s\n", state_obj.str);
        k_msleep(500);
    }

    if(BTN_is_pressed(BTN2)){
        free(state_obj.str);
        state_obj.prev_state = STATE2;
        smf_set_state(SMF_CTX(&state_obj), &miniproj_states[STATE0]);
        printk("----CHAR ENTRY----\n");
    }

    // assuming delay is not exactly 1ms
   if (BTN_is_pressed(BTN0) && BTN_is_pressed(BTN1)) {
        uint32_t start = (uint32_t)state_obj.time_held;
        uint32_t now = k_uptime_get_32();
        if (start == 0U) {
            state_obj.time_held = now; /* start timer */
        } else if ((uint32_t)(now - start) >= 2500U) {
            state_obj.prev_state = STATE2;
            smf_set_state(SMF_CTX(&state_obj), &miniproj_states[STATE3]);
            state_obj.time_held = 0; /* reset to avoid repeated transitions */
            printk("----STANDBY----\n");
        }
    } else {
        state_obj.time_held = 0;
    }

    return SMF_EVENT_HANDLED;
}

static void state3_entry(void *o){
    state_obj.brighter = true;
    state_obj.duty_cycle = 0;
    LED_set(LED0, LED_OFF);
    LED_set(LED1, LED_OFF);
    LED_set(LED2, LED_OFF);
    LED_set(LED3, LED_OFF);
    k_msleep(1000);
}

static enum smf_state_result state3_run(void *o){
    if(BTN_is_pressed(BTN0) || BTN_is_pressed(BTN1)|| BTN_is_pressed(BTN2)|| BTN_is_pressed(BTN3)){
        LED_set(LED0, LED_OFF);
        LED_set(LED1, LED_OFF);
        LED_set(LED2, LED_OFF);
        LED_set(LED3, LED_OFF); 
        printk("%d", state_obj.prev_state);
        smf_set_state(SMF_CTX(&state_obj), &miniproj_states[state_obj.prev_state]);
    }
    else{
        pwm_fade();
    }

    return SMF_EVENT_HANDLED;
}
