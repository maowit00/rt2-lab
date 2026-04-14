// Laborversuch Systemtechnik/Regelungstechnik 2: Motorboard
// (C) 2018 W.Lindermeir, W.Zimmermann
// Hochschule Esslingen

#include "utils.h"

#pragma MESSAGE DISABLE C5905           //Turn off information about multiplication by one

#ifdef USE_SOFTSCOPE
    #define SCIBUFSIZE 8 
    uint8   sciBuffer[SCIBUFSIZE];     // Buffer
    uint16  iSci = 0;                  // Index to the next byte
    uint16  iDummy = 0;
    uint16  iCount = 0;
#endif


//////////////////////////////////////////////////////////////////////////////////////////////
int16 select_median(int16 a, int16 b, int16 c)
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
bool overload_I2dt(int32 I_mA)
{
 static int32 int_I2dt = 0.;
 const int32  k = ((2.*T_V_TH-1)/(2.*T_V_TH+1) * (1<<14));
 // k         \approx 16380    -> 14 Bit
 // I2_DT_MAX \approx 1e8      -> 27 bit
 
 if( I_mA < -I_MAX_STAT || I_mA > I_MAX_STAT){
    // time scaled by RTI_PERIOD_US and current scaled by mA considered in limit I2_DT_MAX
    int_I2dt = sat_add_int32( int_I2dt, I_mA * I_mA); 
 }else{
    int_I2dt >>= 14;
    int_I2dt *= k;         // cooling with Time Kontant T_V_TH (scaled by RTI_PERIOD_US)
 }

 return( (int_I2dt > I2_DT_MAX) ? 1 : 0 );
}

//////////////////////////////////////////////////////////////////////////////////////////////
void display_results(const char* error_msg, const char* lcd_line_0, const char* lcd_line_1)
{
  if(error_msg){
      // here we have an error string to be output on the LCD
      if( ! LcdWriteLine(0, error_msg) ){
             (void)LcdWriteLine(1, error_msg+16);
      } else {
             (void)LcdWriteLine(1, "");
      }
  } else {
      (void)LcdWriteLine(0, lcd_line_0);
      (void)LcdWriteLine(1, lcd_line_1);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//Handle buttons SW2, SW3, SW4, SW5
void sw_pressed_action(enum c_state *control_state, enum d_state *display_state, 
                  int32 *set_rpm, int32 *set_position, int32 *manual_u)
{
  int32 *target_set_value = NULL;
  int32 max, min, inc;
  static uint8 buttonStateOld = 0;
  uint8 buttonState = SwitchGet() & 0x0F;
  static uint16 debounceCounter = 0;
  
  if (debounceCounter > 0) // button debouncing
  {   debounceCounter--; 
      return;
  } else if (buttonState != buttonStateOld)
  {   debounceCounter = DEBOUNCE_COUNT_CYCLES;
  }
  
  switch( *control_state ) // define increment and min/max values for manual set values
  {    case rpm      : target_set_value = set_rpm;
                       inc = ((INCREMENT_RPM * (int32)(1<<CONV_ADC_N_SHIFT) ) / CONV_ADC_RPM_UPMIN);
                       max = ((MAX_RPM       * (int32)(1<<CONV_ADC_N_SHIFT) ) / CONV_ADC_RPM_UPMIN);
                       min = ((MIN_RPM       * (int32)(1<<CONV_ADC_N_SHIFT) ) / CONV_ADC_RPM_UPMIN);
                       break;
       case position : target_set_value = set_position;
                       inc = INCREMENT_POSITION * CONV_ROTATIONS_POS;
                       max = MAX_POSITION       * CONV_ROTATIONS_POS;
                       min = MIN_POSITION       * CONV_ROTATIONS_POS;
                       break;
       case manual   : target_set_value = manual_u;
                       inc = INCREMENT_MANUAL;
                       max = MAX_MANUAL;
                       min = MIN_MANUAL;
                       break;
       default       : break;  
  }

  if ( target_set_value != NULL )
  {  //check, if buttons SW4 and SW5 are pressed (static)
     switch ( buttonState & 0x03 )
     {    case 0x02 : // SW4 pressed
                      switch( *display_state )
                      {     case set_val_inc_dec : *target_set_value -= inc;
                                                   *target_set_value = (*target_set_value < min) ? min : *target_set_value;
                                                   break;
                            case set_val_0_max   : *target_set_value  = 0;   break;
                            case set_val_0_min   : if( *control_state == position )
                                                   {  pos_tick = 0L;
                                                   } else
                                                   {  *target_set_value  = 0;
                                                   }
                                                   break;
                            default              : break;   // buttons disabled in other states
                      }
                      break;
          case 0x01 : // SW5 pressed
                      switch( *display_state )
                      {     case set_val_inc_dec : *target_set_value += inc;
                                                   *target_set_value = (*target_set_value > max) ? max : *target_set_value;
                                                   break;
                            case set_val_0_max   : *target_set_value  = max; break;
                            case set_val_0_min   : *target_set_value  = min; break;
                            default              : break;   // buttons disabled in other states
                      }
                      break;
     }
  }
  
  // check, if buttons SW2 and SW3 are pressed (edge sensitive only)  
  if( (buttonState & 0x0C) && ((buttonState & 0x0C) != (buttonStateOld & 0x0C)) )
  {  if( error_msg )
     {  error_msg = NULL;        // clear any error state on SW2 or SW3 buttons pressed
     } else 
     {  switch( buttonState & 0x0C )   
        {  case 0x08 : // button SW2 pressed -> cycle state of control state machine
                          (*control_state)++;
                          *control_state = (*control_state == end_cs) ? 0 : *control_state; 
                          
                          // When control state changes reset all set values to 0
                          *set_position = 0;
                          *set_rpm  = 0;
                          *manual_u = 128;
                          pos_tick = 0L;
                          break;
           case 0x04 : // button SW3 pressed -> cycle state of display state machine
                          (*display_state)++;
                          *display_state = (*display_state == end_ds) ? 0 : *display_state; 
                          break;
           default   : // simultanous button pressed - ignored 
                          break;
        }
     }
  }
  buttonStateOld = buttonState;
}
     
//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef USE_OSZI
  #if OSZI_TRIG_CH == 1
     #define OSZI_TRIG_CH_CUR (int16)OSZI_DATA_CH1  // data on which to trigger
     #define OSZI_TRIG_CH_LOG        oszi_ch_1      // data on which to trigger
  #elif OSZI_TRIG_CH == 2
     #define OSZI_TRIG_CH_CUR (int16)OSZI_DATA_CH2  // data on which to trigger
     #define OSZI_TRIG_CH_LOG        oszi_ch_2      // data on which to trigger
  #endif
  int16 oszi_ch_1[OSZI_MEM_DEPTH];
  #if OSZI_CHANNELS == 2
  int16 oszi_ch_2[OSZI_MEM_DEPTH];
  #endif

  // after completion oszi_ch_?[0] holds the index of the leftmost sample
  // and              oszi_ch_?[1] holds the sample time in [usec]
  //                  oszi_ch_?[2..OSZI_MEM_DEPTH-1] hold the sampled data 

  void oszi(void)
  {
    static bool   oszi_triggered = 0; // has the trigger condition already evaluated to true ??
    static bool   oszi_init_done = 0; // is the oszi memory already once fully written? Required in trigger position right/center
    static uint8  oszi_t         = 0; // tick counter for oszi samples; incremented mod OSZI_DOWN_SR in each oszi() call
    static uint16 oszi_i         = 2; // index in oszi samples; after completion of oszi logging this is the index for the leftmost sample
    static uint16 oszi_v         = 0; // counter of valid samples logged after trigger condition evaluated to true
  
  #if OSZI_TRIG_CH  != 0
    int16 trig_oszi_i_1;              // data type needs to be identical to type of logged data
    int16 trig_oszi_i_2;              // data type needs to be identical to type of logged data
  
    if( oszi_init_done && !oszi_triggered ){
      // locate the two most recent samples
      if( oszi_i == 2 ){
         trig_oszi_i_1 = OSZI_TRIG_CH_LOG[OSZI_MEM_DEPTH-1];
         trig_oszi_i_2 = OSZI_TRIG_CH_LOG[OSZI_MEM_DEPTH-2];
      } else if( oszi_i == 3 ){
         trig_oszi_i_1 = OSZI_TRIG_CH_LOG[2];
         trig_oszi_i_2 = OSZI_TRIG_CH_LOG[OSZI_MEM_DEPTH-1];
      } else {
         trig_oszi_i_1 = OSZI_TRIG_CH_LOG[oszi_i-1];
         trig_oszi_i_2 = OSZI_TRIG_CH_LOG[oszi_i-2];
      }
      // evaluate trigger condition
  #if OSZI_TRIG_EDGE_R == 1
      if(      OSZI_TRIG_CH_CUR >= (int16)OSZI_TRIG_LEVEL
           && ( trig_oszi_i_1 < (int16)OSZI_TRIG_LEVEL-OSZI_TRIG_MARGIN && trig_oszi_i_2 < (int16)OSZI_TRIG_LEVEL-OSZI_TRIG_MARGIN)){
            oszi_triggered = 1;
      }
  #else
      if(      OSZI_TRIG_CH_CUR <= (int16)OSZI_TRIG_LEVEL
           && ( trig_oszi_i_1 > (int16)OSZI_TRIG_LEVEL+OSZI_TRIG_MARGIN && trig_oszi_i_2 > (int16)OSZI_TRIG_LEVEL+OSZI_TRIG_MARGIN )){
            oszi_triggered = 1;
      }
  #endif
    }
  #endif  //OSZI_TRIG_CH  != 0
    oszi_t++;
    if( oszi_t >= OSZI_DOWN_SR ){
      oszi_t = 0;
      // log data
      oszi_ch_1[oszi_i] = (int16)OSZI_DATA_CH1;                         // log application data channel 1 here
  #if OSZI_CHANNELS == 2
      oszi_ch_2[oszi_i] = (int16)OSZI_DATA_CH2;                         // log application data channel 2 here
  #endif
      // is the first sample run completed ??
      if( oszi_i == OSZI_MEM_DEPTH-1){
        oszi_init_done = 1;
      }
      oszi_i = (oszi_i < OSZI_MEM_DEPTH-1) ? oszi_i+1 : 2 ;
  #if OSZI_TRIG_CH  != 0        // we are running in triggered mode
             // check whether we are done
             oszi_v = (oszi_triggered) ? oszi_v+1 : oszi_v;             // number of valid oszi samples read after trigger
         #if   OSZI_TRIG_POS == 0 // trigger position left
             if( oszi_v >= OSZI_MEM_DEPTH-3 )
         #elif OSZI_TRIG_POS == 1 // trigger position center
             if( oszi_v >= OSZI_MEM_DEPTH/2-1 )
         #elif OSZI_TRIG_POS == 2 // trigger position right
             if( oszi_v >= 1 )
         #endif
             {     // the index of the leftmost sample is oszi_i which we write to the first element of the sampled data
                   // the sample time [usec] is written to the second element of the sampled data
                   oszi_ch_1[0] = oszi_i;
                   oszi_ch_1[1] = (int16)(OSZI_DOWN_SR * RtiPeriodNS / 1000); 
         #if OSZI_CHANNELS == 2
                   oszi_ch_2[0] = oszi_i;
                   oszi_ch_2[1] = (int16)(OSZI_DOWN_SR * RtiPeriodNS / 1000); 
         #endif
//#############################################################################################
                   // set breakpoint here (if not in free running mode) to read oszi data; 
                   // we reset the scope to prepare for the next frame
//#############################################################################################
                   oszi_triggered = 0;
                   oszi_init_done = 0;
                   oszi_t         = 0;
                   oszi_i         = 2;
                   oszi_v         = 0;
             }
  #else      // free-running mode
             // the index of the leftmost sample is oszi_i which we write to the first element of the sampled data
             // the sample time [usec] is written to the second element of the sampled data
             if( oszi_init_done ){
                   oszi_ch_1[0] = oszi_i;
                   oszi_ch_1[1] = (int16)(OSZI_DOWN_SR * RtiPeriodNS / 1000); 
         #if OSZI_CHANNELS == 2
                   oszi_ch_2[0] = oszi_i;
                   oszi_ch_2[1] = (int16)(OSZI_DOWN_SR * RtiPeriodNS / 1000); 
         #endif
                   // set breakpoint here if in free-running mode
             }
  #endif // OSZI_TRIG_CH  != 0
    }
  }
#endif // USE_OSZI



// assuming: typedef char int8;
int8 sat_add_int8( int8 a, int8 b)
{
 int8 s;
 s = a + b;
 if( a > 0 && b > 0 && s < 0){
     s = SCHAR_MAX;
 }else if( a < 0 && b < 0 && ( s > 0 || s == SCHAR_MIN) ){
     s = -SCHAR_MAX;
 }
 return(s);
}

//////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _HCS12_SERIALMON       
  #define SOFTSCOPE_SCI 1
#else
  #define SOFTSCOPE_SCI 0
#endif

#ifdef USE_SOFTSCOPE
/* Note: This is a software oscilloscope with transmits data via the serial interface to
         Matlab function SoftScope
         
         The transmission takes approx. 500µs (6 bytes @ 115,2 kbit/sec).
         The RTI interrupt period must be set such, that it is longer than the
         runtime of myRtiCallback plus myADC1Callback plus the transmission time.
         The sum of the runtimes is approx. 600 us (float algo), thus T_RTI > 500us+600us = 1100us.
         
         For the Simulink Autocode the sum of the runtimes is approx. 750ms.
         
         If the RTI interrupt period is too short, transmission errors will occur.
*/

#define SEQUENCECOUNTER

#ifdef SEQUENCECOUNTER
   #define SYNCMARK     0x58
   uint8 sequenceCount = 0;
#else
   #define SYNCMARK     0x5A
#endif

// Send data via serial interface to Matlab
// Data format is        0x58+(sequenceCount in bit1 bit0)  channel1_MSB  channel1_LSB  channel2_MSB  channel2_LSB  checksumByte
void softScope(int16 channel1, int16 channel2)
{   
#ifdef USE_OSZI
    oszi();						// Local in memory log
#endif

    if (iSci==0)
    {   iDummy = iDummy+63;
#ifdef SEQUENCECOUNTER    
        sequenceCount++;
        sciBuffer[0]              = (SYNCMARK & 0xFC) | (sequenceCount & 0x03); // Sync pattern
#else
        sciBuffer[0]              = SYNCMARK;           // Sync pattern
#endif
        *((int16*) &sciBuffer[1]) = (int16)  channel1;  // channel 1 MSB/LSB
        *((int16*) &sciBuffer[3]) = (int16)  channel2;  // channel 2 MSB/LSB
        sciBuffer[5]              = sciBuffer[0];       // Calculate checksum
        for (iSci=1; iSci < 5; iSci++)
        {  sciBuffer[5] = sciBuffer[5] + sciBuffer[iSci];
        }
                
        iCount = 0;                                     
        iSci   = 0;
        SciEnableInterrupts(SOFTSCOPE_SCI, FALSE, TRUE);// Enable transmission interrupt
        SciPutc(SOFTSCOPE_SCI,sciBuffer[iSci++]);       // Send first byte
    }
}

void softScopeCallback(void)                            // SCI Transmission interrupt
{   if (iSci < 6)                                       // Send bytes
        SciPutc(SOFTSCOPE_SCI,sciBuffer[iSci++]);
    if (iSci >= 6)
    {   SciEnableInterrupts(SOFTSCOPE_SCI, FALSE, FALSE);   
        iSci = 0;                                       // If last byte, turn of SCI transmission interrupt
        LedClear(0x10);
    }
}

void softScopeInit(void)                                 // Initialize serial interface to Matlab
{   SciInit(SOFTSCOPE_SCI, 115200);
    SciRegisterCallback(SOFTSCOPE_SCI, softScopeCallback);
}
#endif // USE_SOFTSCOPE


// assuming: typedef int int16;
int16 sat_add_int16( int16 a, int16 b)
{
 int16 s;
 s = a + b;
 if( a > 0 && b > 0 && s < 0){
     s = INT_MAX;
 }else if( a < 0 && b < 0 && ( s > 0 || s == INT_MIN) ){
     s = -INT_MAX;
 }
 return(s);
}

// assuming: typedef long int32;
int32 sat_add_int32( int32 a, int32 b)
{
 int32 s;
 s = a + b;
 if( a > 0 && b > 0 && s < 0){
     s = LONG_MAX;
 }else if( a < 0 && b < 0 && ( s > 0 || s == LONG_MIN) ){
     s = -LONG_MAX;
 }
 return(s);
}

//#################################################################################################
// HAL extensions
//#################################################################################################

// dummy read TC1H and TC0H input capture registers
// this will transfer the corresponding pac counters into their respective holding registers and reset them;
// then read the PA1H and PA0H pac holding registers
//***** PAC *******************************************************************
void Pac10InitEncoder(void)
{ // initialize pac1 und pac0 in queue mode
  TIOS  &= ~(0x03); // Channel 1 and 0 as input capture used as pac1 and pac0
  TCTL4 |= 0x0F;    // Channel 1 and 0 sensitive on both edges
  TIE   &= ~(0x03); // Channel 1 and 0 generate no interrupt on capture
  ICSYS  = 0x06;    // no sharing of inputs; max count (should never be reached anyway); holding registers enabled /queue mode
  ICPAR |= 0x03;    // Enable pac1 and pac0
  PBCTL  = 0x00;    // PACB disabled; no interrupts on pac1 wrap-around
  DLYCT  = 0x01;    // enable delay counter to reject noisy edges; DLY = 256/24MHz < 11us;
                    // pulse edges arrive at 8000U/min a rate of 400/U * 8000U/min > 18 us;
}

//#################################################################################################
// Note: This function assumes a crystal with 8 MHz (blue PCB Dragon12)!
uint32 RtiStart(uint32 periodInUsec)
{   uint32 x;
    uint8  y;

    periodInUsec *= 2;      // for Dragon 12 Plus Rev. D Board (blue  PCB)
//  periodInUsec *= 1;      // for Dragon 12      Rev. E Board (green PCB)

    // Calculate RTI clock divider settings
    if (periodInUsec < 256)
    {   y=0; x=1;
    }   else if (periodInUsec >= 262144L)
    {   y=0xF; x=0x7;
    } else
    {   for (x=1; x<8; x++)
        {     y = (periodInUsec>>((uint8)(x+7))) ;
              if (y<=16)
              {  y--;
                break;
              }
        }
    }
    
    RtiInit((uint8) x,y);
    
    return((y+1) * (((uint32)1)<<(uint8)(x+9))*125); //Assume quartz crystal frequency 8 MHz = 1/125ns
}


//#################################################################################################
#define DELAY40US  (200L)
extern void Delay(uint32 constant);

//***** ADC1 *******************************************************************
void Adc1Init(void)
{   ATD1CTL2=0xE0;      //Enable ATD, fast clear, no interrupt
    ATD1CTL3=0x08;      //1 conversion only (default); no FIFO mode; continue conversions in BDM mode
    ATD1CTL4=0x65;      //10bit resolution, 16 cycle sampling, ATD frequency 2MHz (24MHz/12),
    Delay(DELAY40US);   //10µs would be sufficient
    ATD1DIEN=0x00;      //all analog input pins used as analog inputs
}

void Adc1Start(uint8 channel, uint8 n_conversions)
{   ATD1CTL3 = (ATD1CTL3 & 0x87) | (n_conversions<<3);  //set number of conversions
    ATD1CTL5 = 0x80 + channel;                          //right justified data, unsigned representation
}

void Adc1SeqStart(uint8 channel, uint8 n_channels)
{   ATD1CTL3 = (ATD1CTL3 & 0x87) | (n_channels<<3);    //set number of channels to convert
    ATD1CTL5 = 0x90 + channel;                         //right justified data, unsigned representation, multichannel conversion
}

#pragma INLINE
bool Adc1Eoc(void)
{   return(ATD1STAT0 & ATD1STAT0_SCF_MASK);   //Read End of Conversion bit
}

uint16 Adc1Read(int reg_i)  //Read conversion result from ATD1DR[reg_i]
{
  return(*((uint16*)(&ATD1DR0) + reg_i));
}

void Adc1EnableInterrupt(bool enable)   //Enable/disable ADC conversion complete interrupt
{   if (enable)
        ATD1CTL2 |=  ATD1CTL2_ASCIE_MASK;
    else
        ATD1CTL2 &= ~(byte)ATD1CTL2_ASCIE_MASK;
}

#ifndef USER_ADC1_ISR
void (*UserAdc1CallbackFcn)(void) = NULL;

void Adc1RegisterCallback(void userAdc1Callback(void))
{   UserAdc1CallbackFcn = userAdc1Callback;
}

interrupt 23 void adc1ISR (void)
{   if (UserAdc1CallbackFcn)
        UserAdc1CallbackFcn();
}

#endif //USER_ADC1_ISR

//#################################################################################################


