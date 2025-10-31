/**
 * @file my_state_machine.c
 */

 #include <zephyr/smf.h>

 #include "LED.h"
 #include "BTN.h"
 #include "my_state_machine.h"

 static void s0_state_entry(void* o);
 static enum smf_state_result state_0(void* o);

 static void s1_state_entry(void* o);
 static enum smf_state_result state_1(void* o);

 static void s2_state_entry(void* o);
 static enum smf_state_result state_2(void* o);

 static void s3_state_entry(void* o);
 static enum smf_state_result state_3(void* o);

 static void s4_state_entry(void* o);
 static enum smf_state_result state_4(void* o);


 enum led_state_machine_states{
    S0,
    S1,
    S2,
    S3,
    S4
 };

 typedef struct{
    struct smf_ctx ctx;
    uint16_t count;
    enum btn_id_t btn;
 }led_state_object_t;

 static const struct smf_state led_states[] = {
    [S0] = SMF_CREATE_STATE(s0_state_entry, state_0, NULL, NULL, NULL),
    [S1] = SMF_CREATE_STATE(s1_state_entry, state_1, NULL, NULL, NULL),
    [S2] = SMF_CREATE_STATE(s2_state_entry, state_2, NULL, NULL, NULL),
    [S3] = SMF_CREATE_STATE(s3_state_entry, state_3, NULL, NULL, NULL),
    [S4] = SMF_CREATE_STATE(s4_state_entry, state_4, NULL, NULL, NULL)
 };

 static led_state_object_t led_state_object;

 void button_pressed(){
   if(BTN_is_pressed(BTN0)){
      led_state_object.btn = BTN0;
   }
   else if(BTN_is_pressed(BTN1)){
      led_state_object.btn = BTN1;
   }
   else if(BTN_is_pressed(BTN2)){
      led_state_object.btn = BTN2;
   }
   else if(BTN_is_pressed(BTN3)){
      led_state_object.btn = BTN3;
   }
 }

 void state_machine_init(){
   printk("state 0");
    led_state_object.count = 0;
    smf_set_initial(SMF_CTX(&led_state_object), &led_states[S0]);
 }

 int state_machine_run(){
    return smf_run_state(SMF_CTX(&led_state_object));
 }


// State 0 entry function and running function
 static void s0_state_entry(void* o){
   LED_set(LED0, LED_OFF);
   LED_set(LED1, LED_OFF);
   LED_set(LED2, LED_OFF);
   LED_set(LED3, LED_OFF);
 }
 
 static enum smf_state_result state_0(void* o){
      if(led_state_object.btn == BTN0){
         printk("state 1");
         led_state_object.count = 0;
         smf_set_state(SMF_CTX(&led_state_object), &led_states[S1]);
      }
      else{
         button_pressed();
         led_state_object.count++;
      }
      return SMF_EVENT_HANDLED;
 }

// State 1 entry function and running function
static void s1_state_entry(void *o){
   LED_blink(LED0, LED_4HZ);
}

 static enum smf_state_result state_1(void* o){
   if(led_state_object.btn == BTN2){
      printk("state 4");
      led_state_object.count = 0;
      smf_set_state(SMF_CTX(&led_state_object), &led_states[S4]); 
   }
   else if(led_state_object.btn == BTN1){
      printk("state 2");
      led_state_object.count = 0;
      smf_set_state(SMF_CTX(&led_state_object), &led_states[S2]); 
   }
   else if(led_state_object.btn == BTN3){
      printk("state 0");
      led_state_object.count = 0;
      smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]); 
   }
   else{
      button_pressed();
      led_state_object.count++;
   }
   return SMF_EVENT_HANDLED;
 }

 // State 2 entry function and running function
 static enum smf_state_result state_2(void* o){
   if(led_state_object.count > 1000){
      printk("state 3");
      led_state_object.count = 0;
      smf_set_state(SMF_CTX(&led_state_object), &led_states[S3]);
   }
   else if(led_state_object.btn == BTN3){
      printk("state 0");
      led_state_object.count = 0;
      smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
   }
   else{
      button_pressed();
      led_state_object.count++;
   }
   return SMF_EVENT_HANDLED;
}
 
 static void s2_state_entry(void *o){
   LED_set(LED0, LED_ON);
   LED_set(LED2, LED_ON);
   LED_set(LED3, LED_OFF);
   LED_set(LED1, LED_OFF);
}

// State 3 entry function and running function
static enum smf_state_result state_3(void* o){
   if(led_state_object.count > 2000){
      printk("state 2");
      led_state_object.count = 0;
      smf_set_state(SMF_CTX(&led_state_object), &led_states[S2]);
   }
   else if(led_state_object.btn == BTN3){
      printk("state 0");
      led_state_object.count = 0;
      smf_set_state(SMF_CTX(&led_state_object), &led_states[S0]);
   }
   else{
      button_pressed();
      led_state_object.count++;
   }
   return SMF_EVENT_HANDLED;

}

 static void s3_state_entry(void *o){
   LED_set(LED0, LED_OFF);
   LED_set(LED2, LED_OFF);
   LED_set(LED3, LED_ON);
   LED_set(LED1, LED_ON);
}

// State 4 entry function and running function

static enum smf_state_result state_4(void *o){
   if(led_state_object.btn == BTN3){
      printk("state 2");
      led_state_object.count = 0;
      smf_set_state(SMF_CTX(&led_state_object), &led_states[S2]); 
   }
   else{
      button_pressed();
      led_state_object.count++;
   }
   return SMF_EVENT_HANDLED;
}

static void s4_state_entry(void* o){
   LED_blink(LED0, LED_16HZ);
   LED_blink(LED1, LED_16HZ);
   LED_blink(LED2, LED_16HZ);
   LED_blink(LED3, LED_16HZ);
}