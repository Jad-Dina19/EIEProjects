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

    smf_set_initial(SMF_CTX(&state_obj), &miniproj_states[STATE0]);
}

void state_machine_run(){
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
    return dest
}


// state entry, run and exit functions
static void state0_entry(void *o){
    
    LED_blink(LED3, LED_1HZ)
    state_obj.ascii_code = 0;
    state_obj.bit_index = 0;
    state_obj.str = NULL;
}

static enum smf_state_result state0_run(void *o){

    if(BTN_is_pressed(BTN0)){
        state_obj.ascii_code = state_obj.ascii_code << 1;
        state_obj.bit_index++;
        LED_set(LED0, LED_ON);
        k_msleep(100);
        LED_set(LED0, LED_OFF);
    }

    if(BTN_is_pressed(BTN0)){
        state_obj.ascii_code = state_obj.ascii_code << 1 | 1;
        state_obj.bit_index++;
        LED_set(LED1, LED_ON);
        k_msleep(100);
        LED_set(LED1, LED_OFF);
    }

    if (BTN2_pressed()) {
        obj->ascii_code = 0;
        obj->bit_index = 0;
    }

    if(BTN_is_pressed(BTN3) && state_obj.bit_index >= 8){
        state_obj.str = malloc(2* sizeof(char));
        state_obj.str[0] = (char)state_obj.ascii_code;
        state_obj.str[1] = '\0';
        smf_set_state(SMF_CTX(&state_obj), &miniproj_state[STATE1]);
        state_obj.prev_state = STATE0;

    }

    // assuming delay is not exactly 1ms
    if(BTN_is_pressed(BTN0) && BTN_is_pressed(BTN1)){
        if(state_obj.time_held >= 2500){
            state_obj.prev_state = STATE0;
            smf_set_state(SMF_CTX(state_obj), &miniproj_states(STATE3));
        }

        state_obj.time_held++;
    }
    else{
        state_obj.time_held = 0; 
    }

}

static void state1_entry(void *o){
    LED_blink(LED3, LED_4HZ);
}

static enum smf_state_result state1_run(void *o){

    if(BTN_is_pressed(BTN0)){
        state_obj.ascii_code = state_obj.ascii_code << 1;
        state_obj.bit_index++;
        LED_set(LED0, LED_ON);
        k_msleep(100);
        LED_set(LED0, LED_OFF);
    }

    if(BTN_is_pressed(BTN0)){
        state_obj.ascii_code = state_obj.ascii_code << 1 | 1;
        state_obj.bit_index++;
        LED_set(LED1, LED_ON);
        k_msleep(100);
        LED_set(LED1, LED_OFF);
    }

    if (BTN2_pressed()) {
        state_obj.ascii_code = 0;
        state_obj.bit_index = 0;
        free(state_obj.str);
        smf_set_state(SMF_CTX(&state_obj), &miniproj_state[STATE0]);
    }

    if(state_obj.bit_index >= 8){
        state_obj.str = malloc(2* sizeof(char));
        state_obj.str[0] = (char)state_obj.ascii_code;
        state_obj.str[1] = '\0';
        smf_set_state(SMF_CTX(&state_obj), &miniproj_state[STATE1]);
        state_obj.prev_state = STATE0;

    }

    // assuming delay is not exactly 1ms
    if(BTN_is_pressed(BTN0) && BTN_is_pressed(BTN1)){
        if(state_obj.time_held >= 2500){
            state_obj.prev_state = STATE0;
            smf_set_state(SMF_CTX(state_obj), &miniproj_states(STATE3));
        }

        state_obj.time_held++;
    }
    else{
        state_obj.time_held = 0; 
    }

}