// Laborversuch Systemtechnik/Regelungstechnik 2: Motorboard
// (C) 2018 W.Lindermeir, W.Zimmermann
// Hochschule Esslingen

#ifndef __UTILS__
#define __UTILS__

#include "main.h"


#define USE_SOFTSCOPE                             // include code for software oszi (serial interface to Matlab)

#ifdef USE_SOFTSCOPE
  void softScopeInit(void);
  void softScope(int16 channel1, int16 channel2);
#endif


#define USE_OSZI                                // include code for in memory logging
//#undef USE_OSZI                               // uncomment to remove oszi code
                                                
#ifdef USE_OSZI                                 
  #define OSZI_CHANNELS               2         // number of channels to use; \in {1,2}
  #define OSZI_DOWN_SR                1         // down sample rate; every OSZI_DOWN_SR sample is logged; 
                                                //     1 for logging each sample; \in {1, .., 255}
  #define OSZI_TRIG_CH                1         // Trigger on channel x; possible values {0,1,2}; 
                                                //     0 is no trigger at all; free running scope
  #define OSZI_TRIG_POS               1         // trigger position: 0: left; 1: center; 2: right
  #define OSZI_TRIG_EDGE_R            1         // trigger on rising edge ?? boolean
  #define OSZI_TRIG_LEVEL       (5*400)         // trigger level (casted to int16)
  #define OSZI_TRIG_MARGIN           10         // trigger safety margin for trigger edge detection
  #if OSZI_CHANNELS == 1                        
     #define OSZI_MEM_DEPTH        1000         // number of samples to be logged (>2)
     #define OSZI_DATA_CH1 measured_rpm         // casted to int16
  #else                                         
     #define OSZI_MEM_DEPTH         800         // number of samples to be logged (>2)
     //#define OSZI_DATA_CH1 set_position       // casted to int16
     #define OSZI_DATA_CH1     pos_tick         // casted to int16
     //#define OSZI_DATA_CH1 (measured_current * CONV_ADC_I_mA)     // casted to int16
     #define OSZI_DATA_CH2 (measured_rpm * CONV_ADC_RPM_UPMIN)      // casted to int16
  #endif // OSZI_CHANNELS == 1

  // prototype
  void oszi(void);
#endif // USE_OSZI


// local prototypes
int16 select_median(int16 a, int16 b, int16 c);

bool overload_I2dt(float I_mA);

void display_results(const char* error_msg, const char* lcd_line_0, const char* lcd_line_1);

void sw_pressed_action(enum c_state *control_state, enum d_state *display_state, 
                  float *set_rpm, float *set_position, float *manual_u);


//#################################################################################################
// HAL extensions
//#################################################################################################

void   Pac10InitEncoder(void);

uint32 RtiStart(uint32 periodInUsec);

void   Adc1Init(void);
void   Adc1Start(uint8 channel, uint8 n_conversions);
void   Adc1SeqStart(uint8 channel, uint8 n_channels);
bool   Adc1Eoc(void);
uint16 Adc1Read(int reg_i);
void   Adc1EnableInterrupt(bool enable);
void   Adc1RegisterCallback(void userAdc1Callback(void));

#endif // __UTILS__


