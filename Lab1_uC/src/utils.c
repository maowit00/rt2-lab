#include "utils.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//Handle buttons SW07_1, SW07_2, SW07_3, SW07_4
void sw_pressed_action(enum c_state *control_state, enum d_state *display_state, 
                       float *set_rpm, float *set_position, float *manual_u )
{
  float max = 0., min = 0., inc = 0.;
  static uint8_t buttonStateOld = 0;
  uint8_t buttonState = pico_read_buttons() & 0x0F;
  static uint16_t debounceCounter = 0;
  
  if (debounceCounter > 0) { // button debouncing
      debounceCounter--; 
      return;
  } else if (buttonState != buttonStateOld) {   
      debounceCounter = DEBOUNCE_COUNT_CYCLES;
  }

  // check, if buttons SW07_1 or SW07_2 are pressed (edge sensitive only) => Change control_state
  if( (buttonState & 0x0C) && ((buttonState & 0x0C) != (buttonStateOld & 0x0C)) ) {  
     if( error_msg_l0 || error_msg_l1 ) {  
        error_msg_l0 = error_msg_l1 = NULL;  // clear any error state on SW07_1 or SW07_2 buttons pressed
     } else {  
        switch( buttonState & 0x0C ) {  
           case 0x08 : // button SW07_1 pressed -> cycle state of control state machine
                          (*control_state)++;
                          *control_state = (*control_state == end_cs) ? 0 : *control_state; 
                          // When control state changes reset the following values to 0
                          *set_position = 0;
                          *set_rpm  = 0;
                          *manual_u = 128; // manual u such that voltage to motor will be 0
                          current_substep_int64 = 0L;
                          break;
           case 0x04 : // button SW07_2 pressed -> cycle state of display state machine
                          (*display_state)++;
                          *display_state = (*display_state == end_ds) ? 0 : *display_state; 
                          break;
           default   : // simultanous button pressed - ignored 
                          break;
        }
     }
  }
  
  switch( *control_state ) { // define increment and min/max values for set values
       case rpm      : inc = INCREMENT_RPM;
                       max = MAX_RPM;
                       min = MIN_RPM;
                       break;
       case position : inc = INCREMENT_POSITION * CONV_ROTATIONS_POS;
                       max = MAX_POSITION       * CONV_ROTATIONS_POS;
                       min = MIN_POSITION       * CONV_ROTATIONS_POS;
                       break;
       case manual   : inc = INCREMENT_MANUAL;
                       max = MAX_PWM_VAL;
                       min = MIN_PWM_VAL;
                       break;
       default       : break;  
  }

  //check, if buttons SW07_3 or SW07_4 are pressed (static)
  switch ( buttonState & 0x03 ) {
       case 0x02 : // SW07_3 pressed
                   switch( *display_state ) {
                         case set_val_inc_dec : switch( *control_state ) {
                                                   case rpm      : *set_rpm -= inc;
                                                                   *set_rpm = (*set_rpm < min) ? min : *set_rpm;
                                                                   break;
                                                   case position : *set_position -= inc;
                                                                   *set_position = (*set_position < min) ? min : *set_position;
                                                                   break;
                                                   case manual   : *manual_u -= inc;
                                                                   *manual_u = (*manual_u < min) ? min : *manual_u;
                                                                   break;
                                                   default       : break;
                                                }
                                                break;
                         case set_val_0_max   : 
                         case set_val_0_min   : switch( *control_state ) {
                                                   case rpm      : *set_rpm = 0;
                                                                   break;
                                                   case position : *set_position = 0;
                                                                   //current_substep_int64 = 0L;
                                                                   break;
                                                   case manual   : *manual_u = 0;
                                                                   break;
                                                   default       : break;
                                                }
                                                break;
                         default              : break;   // buttons disabled in other states
                   }
                   break;
       case 0x01 : // SW07_4 pressed
                   switch( *display_state ) {
                         case set_val_inc_dec : switch( *control_state ) {
                                                   case rpm      : *set_rpm += inc;
                                                                   *set_rpm = (*set_rpm > max) ? max : *set_rpm;
                                                                   break;
                                                   case position : *set_position += inc;
                                                                   *set_position = (*set_position > max) ? max : *set_position;
                                                                   break;
                                                   case manual   : *manual_u += inc;
                                                                   *manual_u = (*manual_u > max) ? max : *manual_u;
                                                                   break;
                                                   default       : break;
                                                }
                                                break;
                         case set_val_0_max   : switch( *control_state ) {
                                                   case rpm      : *set_rpm = max;
                                                                   break;
                                                   case position : *set_position = max;
                                                                   break;
                                                   case manual   : *manual_u = max;
                                                                   break;
                                                   default       : break;
                                                }
                                                break;
                         case set_val_0_min   : switch( *control_state ) {
                                                   case rpm      : *set_rpm = min;
                                                                   break;
                                                   case position : *set_position = min;
                                                                   break;
                                                   case manual   : *manual_u = min;
                                                                   break;
                                                   default       : break;
                                                }
                                                break;
                         default              : break;   // buttons disabled in other states
                   }
                   break;
       default   : // simultanous button pressed - ignored 
                   break;
  }
  buttonStateOld = buttonState;
}


//################################################
// SoftScope
//################################################
     
#define SCIBUFSIZE 6 
#define SYNCMARK   0x58

// Send data via printf to Matlab
// Data format is  0x58+(sequenceCount in bit1 bit0)  channel1_MSB  channel1_LSB  channel2_MSB  channel2_LSB  checksumByte
void SoftScopeSend(int16_t channel1, int16_t channel2)
{   
    static uint8_t buf[SCIBUFSIZE];     // Buffer
    static uint8_t sequenceCount = 0;

    sequenceCount++;
    buf[0] = (SYNCMARK & 0xFC) | (sequenceCount & 0x03); // Sync pattern
    //we are on a Littel-Endian machine
    buf[1] = (uint8_t)((channel1 >> 8) & 0xff);
    buf[2] = (uint8_t)(channel1 & 0xff);
    buf[3] = (uint8_t)((channel2 >> 8) & 0xff);
    buf[4] = (uint8_t)(channel2 & 0xff);
    // Calculate checksum
    buf[5] = buf[0];
    for (uint8_t i =1; i < 5; i++) {
      buf[5] += buf[i];
    }
    stdio_put_string((char*)buf, SCIBUFSIZE, false, false);
    stdio_flush();
}

//////////////////////////////////////////////////////////////////////////////////////////////
uint16_t select_median(uint16_t a, uint16_t b, uint16_t c)
{   
 if      ( a > b && a > c){
     // a is max
     if( b > c ){
         return(b);
     } else {
         return(c); 
     }
 } else if( a < b && a < c){
     // a is min
     if( b > c ){
         return(c); 
     } else {
         return(b);
     }
 } else {
     // a is neither max nor min
     return(a);
 }
}

//////////////////////////////////////////////////////////////////////////////////////////////
bool overload_I2dt(float I_mA)
{
 static float int_I2dt = 0.;
 const float k = (2.*T_V_TH-1)/(2.*T_V_TH+1);
        
 if( I_mA < -I_MAX_STAT || I_mA > I_MAX_STAT){
    int_I2dt += I_mA * I_mA;         // time scaled by RTI_PERIOD_US and current scaled by mA considered in limit I2_DT_MAX
 } else {
    int_I2dt *= k;                   // cooling with time constant T_V_TH (scaled by RTI_PERIOD_US)
 }

 return( (int_I2dt > I2_DT_MAX) ? 1 : 0 );
}

