// Laborversuch Systemtechnik/Regelungstechnik 2: Motorboard
// (C) 2018 W.Lindermeir, W.Zimmermann
// Hochschule Esslingen

#ifndef __MAIN__
#define __MAIN__

#include <hidef.h>              // common defines and macros
#include <mc9s12dp256.h>        // derivative information
#include <time.h>
#pragma LINK_INFO DERIVATIVE "mc9s12dp256b"
#include <ctype.h>
#include <stdio.h>
#include <limits.h>
#include "hal.h"
#include "utils.h"

#define PWMChannel                 0   // PWM channel for actuator signal
#define ADC_RPM_CHANNEL            0   // ADC channel for analog RPM signal
#define ADC_I_CHANNEL              1   // ADC channel for Current/Torque signal
                                       
#define RTI_PERIOD_US          1536L   // periodic RT-Interrupt in [usec]

#define DEBOUNCE_COUNT_CYCLES 5        // number of main loop cycles the buttons are disabled when pressed or released

#define UMAX                     255   // max PWM control output
#define UMIN                       0   // min PWM control output
                                       
#define INCREMENT_RPM            100   // increment to ramp up or down set speed [U/min]
#define MAX_RPM                 7800   // max speed
#define MIN_RPM                -7800   // min speed
                                       
#define INCREMENT_POSITION         1   // increment to ramp up or down set position (in rotations)
#define MAX_POSITION              10   // max position (in rotations)
#define MIN_POSITION             -10   // min position (in rotations)
                                       
#define INCREMENT_MANUAL           1   // increment PWM value in manual mode
#define MAX_MANUAL              UMAX   // max PWM value in manual mode
#define MIN_MANUAL              UMIN   // min PWM value in manual mode

// define conversion from U (rotations) to position ticks
#define CONV_ROTATIONS_POS       400

// define conversions from ADC-readings (-512..+511) to rpm [U/min]
#define CONV_ADC_RPM_UPMIN       ((float)(1000*5/(1023*2*0.16)))      // 15,2737
/* Internal RPM values set_rpm and measured_rpm = RPM in U/min / CONV_ADC_RPM_UPMIN
                                   e.g. 4000 U/min -->  262
                                        7800 Z/min -->  511
*/
#define SPEED_CHECK_LIMIT        100   // Limit for position signal / speed signal plausibility check

// define conversions from ADC-readings (-512..+511) to current [mA]
#define CONV_ADC_I_mA            ((float)(0.1*4*5/1023*1000))         // 1,955e-6

// define switching between slow position decoding and fast postion decoding 
#define MAX_ABS_RPM_POS_SLOW    2000   // max rpm speed for slow position decoding
#define MIN_ABS_RPM_POS_PAC     1800   // min rpm speed for fast position decoding

// constants for motor overload check
#define I_MAX_STAT               580            // maximum static current [mA]
#define I2_DT_MAX    (0.2 *1e12/RTI_PERIOD_US)  // maximum value of integral I^2dt in [mA^2 us/us]
#define T_V_TH       (3.0 *1e6/RTI_PERIOD_US)   // time constant for motor cooling scaled by RTI_PERIOD_US

// defines for waveform generator -- used only in mode_auto for generation of stimuli
// common timing for all three square pulse generators
#define WAV_GEN_PERIOD          2000   // number of RTI ticks in period
#define WAV_GEN_HIGH_COUNT      1000   // number of RTI ticks in high phase

#define WAV_GEN_HIGH_LEVEL_POS    10   // high level for POS steering in rotations
#define WAV_GEN_LOW_LEVEL_POS      0   // low  level for POS steering in rotations

#define WAV_GEN_HIGH_LEVEL_RPM  3000   // high level for RPM steering [-8000 .. +8000]
#define WAV_GEN_LOW_LEVEL_RPM   1000   // low  level for RPM steering [-8000 .. +8000]

#define WAV_GEN_HIGH_LEVEL_PWM   220   // high level for PWM steering [0 .. 255]
#define WAV_GEN_LOW_LEVEL_PWM    180   // low  level for PWM steering [0 .. 255]


// ECT timer constants for measuring run times and motor speed via TTL position signal
#define TIMER_DIVIDER     0x04                  
#define TIMER_RESOLUTION  ((float) 0.667) // in usec  (24 MHz / 2^TIMER_DIVIDER)

// Constant to convert TTL signal period to speed in rpm (with ...rpm_adc scaling see above)
// 16=2^TIMER_DIVIDER to be changed when timer resolution set by TimerInit(TIMER_DIVIDER) changes.
#define CONV_TTL_RPM      ((uint16) (60.0 / 100.0 * 24e6 / 16 / CONV_ADC_RPM_UPMIN))  


// define some types
enum c_state {rpm = 0, position, manual, end_cs};   // state machine for control toggling
enum d_state {disp_only = 0,                        // state machine for display, button, setvalue interpretation
              set_val_gen,                          // automatic waveform generator mode
              set_val_inc_dec, set_val_0_max, set_val_0_min, end_ds}; 

// may be used in oszi
extern float manual_u;     
extern int16 measured_current;     
extern int16 measured_rpm;
extern int32 pos_tick;
extern float set_position;
extern uint8 direction_forward;
extern bool  pac_position_decoding;
extern uint32 RtiPeriodNS;
extern const char *error_msg;

#endif // __MAIN__


