/**
 * @file miniproj_fsm.h
 */
//includes
#include <stdlib.h>
#include <zephyr/smf.h>
#include "LED.h"
#include "BTN.h"

//defines and macros
 #ifndef MINIPROJ_FSM_H
 #define MINIPROJ_FSM_H
 
 #define MAX_STR_SIZE = 16;

 //function prototypes
 void state_machine_init();
 int state_machine_run();

 char* add_char(char *src, uint8_t new_char);
 void pwm_fade();

 static void state0_entry(void *o);
 static enum smf_state_result state0_run(void *o);

 static void state1_entry(void *o);
 static enum smf_state_result state1_run(void *o);

 static void state2_entry(void *o);
 static enum smf_state_result state2_run(void *o);

 static void state3_entry(void *o);
 static enum smf_state_result state3_run(void *o);

 //typedefs
 enum miniproj_states{
    NONE,
    STATE0,
    STATE1,
    STATE2,
    STATE3,
 };

 typedef struct {
    struct smf_ctx ctx;

    enum miniproj_states prev_state;
    uint8_t ascii_code;
    uint8_t bit_index;
    char* str;
    uint64_t time_held;
    uint8_t string_size;
    uint8_t duty_cycle;
    bool brighter;
 } miniproj_states_object_t;

 //local variables

 static const struct smf_state miniproj_states[] = {
    [STATE0] = SMF_CREATE_STATE(state0_entry, state0_run, NULL, NULL, NULL),
    [STATE1] = SMF_CREATE_STATE(state1_entry, state1_run, NULL, NULL, NULL),
    [STATE2] = SMF_CREATE_STATE(state2_entry, state2_run, NULL, NULL, NULL),
    [STATE3] = SMF_CREATE_STATE(state3_entry, state3_run, NULL, NULL, NULL)
 };

 static miniproj_states_object_t state_obj;

 #endif