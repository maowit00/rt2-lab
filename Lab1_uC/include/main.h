#ifndef __MAIN__
#define __MAIN__

#include "pico/stdlib.h"
#include "hal.h"
#include "utils.h"

#define SAMPLINGTIME_US         1000L  // samplint time in [usec]

#define DEBOUNCE_COUNT_CYCLES      1   // number of main loop cycles the buttons are disabled when pressed or released

#define INCREMENT_RPM            100   // increment to ramp up or down set speed [U/min]
#define MAX_RPM                 7800   // max rpm speed [U/min]
#define MIN_RPM                -7800   // min rpm speed [U/min]
#define MAX_RPM_CONTROL_OUTPUT   127   // max Stellgröße Drehzahlregler
#define MIN_RPM_CONTROL_OUTPUT  -128   // min Stellgröße Drehzahlregler
                                                    
#define INCREMENT_POSITION         1   // increment to ramp up or down set position (in rotations)
#define MAX_POSITION              10   // max position (in rotations)
#define MIN_POSITION             -10   // min position (in rotations)
                           
#define INCREMENT_MANUAL           1   // increment PWM value in manual mode
#define MAX_PWM_VAL              255   // max PWM value
#define MIN_PWM_VAL                0   // min PWM value

// define conversion from U (rotations) to position ticks
#define CONV_ROTATIONS_POS       400

// constants for motor overload check
#define I_MAX_STAT               580              // maximum static current [mA]
#define I2_DT_MAX    (0.3 *1e12/SAMPLINGTIME_US)  // maximum value of integral I^2dt in [mA^2 us/us]
#define T_V_TH       (3.0 *1e6/SAMPLINGTIME_US)   // time constant for motor cooling scaled by SAMPLINGTIME_US

// defines for waveform generator -- used only in mode_auto for generation of stimuli
// common timing for all three square pulse generators
#define WAV_GEN_PERIOD          2000   // number of Sampling time ticks in period: Period of generator is SAMPLINGTIME_US*WAV_GEN_PERIOD
#define WAV_GEN_HIGH_COUNT      1000   // number of Sampling time ticks in high phase: Duty cycle is 50%

#define WAV_GEN_HIGH_LEVEL_POS    10   // high level for POS steering in rotations
#define WAV_GEN_LOW_LEVEL_POS      0   // low  level for POS steering in rotations

#define WAV_GEN_HIGH_LEVEL_RPM  3000   // high level for RPM steering in U/min 
#define WAV_GEN_LOW_LEVEL_RPM   1000   // low  level for RPM steering in U/min

#define WAV_GEN_HIGH_LEVEL_PWM   220   // high level for manual PWM steering [0 .. 255]
#define WAV_GEN_LOW_LEVEL_PWM    180   // low  level for manual PWM steering [0 .. 255]


void positionControl(void);
void rpmControl(void);
bool repeating_timer_callback(__unused struct repeating_timer *t);
int main(void);
#endif // #ifndef __MAIN__
