// Laborversuch Systemtechnik/Regelungstechnik 2: Motorboard
// (C) 2018 W.Lindermeir, W.Zimmermann
// Hochschule Esslingen

#include "main.h"

#pragma MESSAGE WARNING DISABLE C12056
#pragma MESSAGE WARNING DISABLE C5919

uint32 RtiPeriodNS;

static bool   rti_adc_started    = 0;           // has an AD-Conversion been started by myRTICallback()

uint16 startTime = 0;                           // time measurement (resolution is TIMER_RESOLUTION=0.667�sec/bit)
float  measuredTimeInUsec = 0;                  // measured time in usec

static enum c_state control_state = rpm;        // state machine for control toggling
static enum d_state display_state = disp_only;  // state machine for display and button interpretation toggling

// Manual control
float manual_u;                   // pwm value in manual mode 0..255
                                  
// rpm control                    
float set_rpm;                    // set rpm scaled -512..+511  = -7800 ...+7800 U/min
int16 measured_rpm_adc  = 0;      // speed rpm measured via ADC-reading -512..+511
int16 measured_rpm_ttl  = 0;      // speed measured via digital position sensor pulse period (scaling see ...rpm_adc)
int16 measured_rpm      = 0;      // measured speed (error corrected, scaling see ... rpm_adc)
int16 delta_measured_rpm= 0;	  // speed measurement error correction TTL - ADC)

int16 measured_rpm_filtered = 0;  // filtered values for LCD display

float e_rpm;                      // rpm control: control error 
float rpm_control_u;              // rpm control: control output 

const float max_rpm_control_output = +127;
const float min_rpm_control_output = -128;

// Position decoding              
int32 pos_tick;                   // actual position in ticks (400 ticks per rotation)
bool  direction_forward;          // true for positive speed; false for negative speed
bool  pac_position_decoding;      // true when in pac mode; false when in slow mode
                                 
// Position control               
float set_position;               // set position in ticks (400 ticks per rotation)
float e_position;                 // position control: control error 
const float max_pos_control_output =  MAX_RPM / CONV_ADC_RPM_UPMIN; // 7800/15.2737 = +511
const float min_pos_control_output = -MAX_RPM / CONV_ADC_RPM_UPMIN; //              = -511

// Motor overload
int16 measured_current;           // actual current ADC-readings -512..+511

// Error handling: error strings to be displayed on the LCD-display
const char * error_msg = NULL;
// Error string mask: 2 lines 16 chars each         "0123456789ABCDEF0123456789ABCDEF" 
const char err_txt_not_implemented_displaystate[] = "Not implemented display state";
const char err_txt_not_implemented_controlstate[] = "Not implemented control state";
const char err_txt_I2dt_overload[]                = "Motor Overload";

// Interrupt service callbacks
void myPortH74Callback(uint8 mask);
void myRtiCallback(void);
void myADC1Callback(void);

//*********************************************************************************************         
// Control Algorithms
//*********************************************************************************************

//#############################################################################################             
//#############################################################################################             
// ToDo: Bauen Sie den Code fuer den P--Positionsregler in die Funktion positionControl() ein
//       Die Funktion wird von der ISR myADC1Callback() in jeder Abtastperiode aufgerufen.
//
// Variablen, die Sie hierfuer verwenden sollen:
// *******************************************************************************************
// Groessen, die Sie lesen koennen. Diese Groessen sind an dieser Stelle bereits mit Werten belegt:
// *******************************************************************************************
// float set_position;                   // Sollwert in Ticks (400 Ticks pro Umdrehung)
// int32 pos_tick;                       // Istposition in Ticks
// *********************************************************************************
// Groessen, die Sie mit einem sinnvollen Wert belegen muessen: 
// *********************************************************************************
// const float kp_position               // Reglerparameter: Proportionalbeiwert
// *******************************************************
// Groessen, die Sie berechnen muessen; sind schon deklariert
// *******************************************************
// float e_position;                     // Regelfehler
// float set_rpm;                        // Stellgroesse des Positionsreglers bzw. Sollwert fuer den unterlagerten Drehzahlregler
//
void positionControl(void )
{
//ToDo: Einbau des Lagereglers/Positionsreglers
   const float kp_position = 0.3;                 // Position control: P gain ANPASSEN!!

   e_position = set_position - pos_tick;

   set_rpm = kp_position * e_position;
//#############################################################################################             
}


//#############################################################################################             
//#############################################################################################             
// ToDo: Bauen Sie den Code fuer den PI--Drehzahlregler mit Anti--Windup Massnahme ein in die
//       Funktion rpmControl() ein.
//       Die Funktion wird von der ISR myADC1Callback() in jeder Abtastperiode aufgerufen.
//
// Variablen, die Sie hierfuer verwenden sollen:
// *******************************************************************************************
// Groessen, die Sie lesen koennen. Diese Groessen sind an dieser Stelle bereits mit Werten belegt:
// *******************************************************************************************
// float set_rpm;                        // Drehzahl--Sollwert: passend skaliert zu measured_rpm
// int16 measured_rpm;                   // Gemessener Drehzahlwert vom ADC -512 .. + 511 --> -7800 ... +7800 U/min
// RtiPeriodNS                           // Abtastzeit = RTI Zykluszeit in ns
// const float max_rpm_control_output    // Stellgroessenbeschraenkung oben
// const float min_rpm_control_output    // Stellgroessenbeschraenkung unten
// *********************************************************************************
// Groessen, die Sie mit einem sinnvollen Wert belegen muessen: 
// *********************************************************************************
// const float kp_rpm                    // Reglerparameter: Proportionalbeiwert
// const float Tn_rpm                    // Reglerparameter: Nachstellzeit
// *******************************************************
// Groessen, die Sie berechnen muessen; sind schon deklariert
// *******************************************************
// float e_rpm;                          // Regelfehler
// float rpm_control_u;                  // Stellgroesse des Drehzahlreglers
// Sie muessen gegebenenfalls weitere Variablen lokal deklarieren
//
void rpmControl(void)
{

//ToDo: Reglerparameter des Drehzahlreglers sinnvoll vorgeben
//ToDo: und PI-Drehzahlregler mit Anti-Winup implementieren
    const float kp_rpm = 1.2; // ToDo       // rpm control: P gain
    const float Tn_rpm = 0.0511; // ToDo       // rpm control: reset time [usec]

    const float Ki = kp_rpm * RtiPeriodNS * 1e-9 / Tn_rpm;   //Integrationskoeff


    static float u_i = 0;

    float u, u_p;


    e_rpm = set_rpm - measured_rpm;

    u_p  = kp_rpm * e_rpm;
    u_i += Ki * e_rpm;

    u = u_p + u_i;

    if (u > max_rpm_control_output)
    {
        u = u_i = max_rpm_control_output;
    }
    else if (u < min_rpm_control_output)
    {
        u = u_i = min_rpm_control_output;
    }

    rpm_control_u = u;  // assigned scheduled value

//#############################################################################################
}


//#############################################################################################
//#############################################################################################

// ##### Called on every edge of one of the TTL position signals 
void myPortH74Callback(uint8 mask)
{ uint16 t0, t0H, t1, t1H;
  static uint16  dt0=0, dt1=0;
  uint8 myPTH7 = PTH & 0x80; // Read current value of Port H bit 7
  uint8 myPTH5 = PTH & 0x20; // Read current value of Port H bit 5

  // We are in slow mode
  pac_position_decoding = 0;
  // Position decoding in slow mode: update direction_forward 
  switch( mask & 0xF0 ){
       case 1*16:  // Interrupt due to falling edge on Port H(4) == H(5)
                   direction_forward = (myPTH7) ? 1 : 0;
                   break;
       case 2*16:  // Interrupt due to rising edge on Port H(5) == H(4)
                   direction_forward = (myPTH7) ? 0 : 1;
                   break;
       case 4*16:  // Interrupt due to falling edge on Port H(6) == H(7)
                   direction_forward = (myPTH5) ? 0 : 1;
                   break;
       case 8*16:  // Interrupt due to rising edge on Port H(7) == H(6)
                   direction_forward = (myPTH5) ? 1 : 0;
                   break;
       default:    // More than one interrupt pending -> we are too slow in processing the interrupts 
                   // Direction_forward cannot change
                   break;
  }
  
  // Reset and latch pac counters 
  t0 = TC0;                     // Dummy read to trigger latching of pac1 + pac0 into its holding register          
  t0H= TC0H;
  t1 = TC1;
  t1H= TC1H;
  if (t0 != t0H) dt0 = t0 - t0H;// Measure TTL signal periods for both TTL position signals
  if (t1 != t1H) dt1 = t1 - t1H;
    
  if(direction_forward ){
      pos_tick += PA1H;
      pos_tick += PA0H;
      
      if (dt0 > 0 && dt1 > 0) {   
          measured_rpm_ttl =  CONV_TTL_RPM / (dt0 + dt1);  // Calculate speed from TTL signal period measurement
          delta_measured_rpm = measured_rpm_ttl - measured_rpm_adc;
      }
  } else {
      pos_tick -= PA1H;
      pos_tick -= PA0H;
      
      if (dt0 > 0 && dt1 > 0) {   
          measured_rpm_ttl = - (int16) (CONV_TTL_RPM / (dt0 + dt1));
          delta_measured_rpm = measured_rpm_ttl - measured_rpm_adc;
      } 
  }

  // Do we need to switch postion decoding from slow-mode to pac-mode ??
  // Check whether we are currently too fast
  if( measured_rpm > MAX_ABS_RPM_POS_SLOW/CONV_ADC_RPM_UPMIN || measured_rpm < -MAX_ABS_RPM_POS_SLOW/CONV_ADC_RPM_UPMIN ){
      pac_position_decoding = 1;  // Switch to PAC position decoding
      PIEH &= 0x0F;               // Disable interrupts on port H7..4
  }
}


// ##### Called periodically by the RTI interrupt (every 768�s)
void myRtiCallback (void) 
{   static uint16 toggleCount = 0;

    // For debugging: Toggle LEDs on Port B6 and B7
    LedSet(0x40);                    //Turn on every RTI interrupt
    if (++toggleCount >= 1302U){     //Toggle once per second
       LedToggle(0x80);
       toggleCount=0;
    }

    Adc1Start(ADC_I_CHANNEL, 3);    //Start 3 AD-Conversions for current signal (motor overload check)
    rti_adc_started = 1;
}

// ##### Called when ADC conversion is completed
void myADC1Callback(void)
{   uint8 pwm_out;
    uint16 t0, t0H, t1, t1H;
    static uint16  dt0=0, dt1=0;

    static int16 wav_gen_ticks = WAV_GEN_PERIOD;        // Waveform generator time base

    if( rti_adc_started ){
         rti_adc_started = 0;

         // Read the three current values from ADC of current sensor and use median value
         measured_current = select_median((int16)Adc1Read(0), (int16)Adc1Read(1), (int16)Adc1Read(2));
         measured_current -= 512;        // Offset correction 2.5V <=> I = 0
         Adc1Start(ADC_RPM_CHANNEL, 3);  // Start three AD-Conversions for RPM Signal
    } else {
         // Read the three rpm values from ADC of tacho generator and use median value
         measured_rpm_adc  = select_median((int16)Adc1Read(0), (int16)Adc1Read(1), (int16)Adc1Read(2));
         measured_rpm_adc -= 512;       // Offset correction 2.5V <=> RPM = 0
         measured_rpm = measured_rpm_adc;

         // Check plausibility of rotation direction between analog speed signal and incremental position sensor
         if (measured_rpm > SPEED_CHECK_LIMIT) {  
            if (direction_forward == 0) {   
                direction_forward = 1;
                LedSet(0x08);     // Direction error --> check position sensor
            } else {   
                LedClear(0x08);   // Direction ok
            }
         } else if (measured_rpm < -SPEED_CHECK_LIMIT) {  
            if (direction_forward == 1) {   
                direction_forward = 0;
                LedSet(0x08);     // Direction error --> check position sensor
            } else {   
                LedClear(0x08);   // Direction ok
            }
         }

         // Do we need to switch position encoding from pac-mode to slow-mode ??
         // Check whether we are currently in pac-mode and RPM is too slow
         if(    pac_position_decoding 
             && measured_rpm < MIN_ABS_RPM_POS_PAC/CONV_ADC_RPM_UPMIN && measured_rpm > -MIN_ABS_RPM_POS_PAC/CONV_ADC_RPM_UPMIN ){
            PIEH |= 0xF0;         // Enable interrupts on port H7..4
         }
         
         // Position decoding: result will be available in pos_tick
         if( pac_position_decoding ){
             // Reset and latch pac counters 
             t0 = TC0;            // Read to trigger latching of pac0 and pac1 into its holding register
             t0H= TC0H;
             t1 = TC1;
             t1H= TC1H;
             if (t0 != t0H) dt0 = t0 - t0H;
             if (t1 != t1H) dt1 = t1 - t1H;
             
             if(direction_forward ){
                 pos_tick += PA1H;
                 pos_tick += PA0H;
                 
                 if (dt0 > 0 && dt1 > 0) {   
                     measured_rpm_ttl =    CONV_TTL_RPM / (dt0 + dt1);
                     delta_measured_rpm = measured_rpm_ttl - measured_rpm_adc;
                 }
             } else {
                 pos_tick -= PA1H;
                 pos_tick -= PA0H;
                 
                 if (dt0 > 0 && dt1 > 0) {   
                     measured_rpm_ttl = - (int16) (CONV_TTL_RPM / (dt0 + dt1));
                     delta_measured_rpm = measured_rpm_ttl - measured_rpm_adc;
                 }
             }
         }

         // Correct speed measurement error 
         measured_rpm = measured_rpm_adc + delta_measured_rpm;
         measured_rpm_filtered = measured_rpm / 4 + 3 * measured_rpm_filtered / 4;

         wav_gen_ticks = (wav_gen_ticks) ? wav_gen_ticks-1 : WAV_GEN_PERIOD; 

         LedSet(0x20);                                  // For debugging: Set B.5: start of control algo
         startTime = TimerGet();                        // Start time measurement for control algo

//*********************************************************************************************         
         // Position control
         if( control_state == position ){
             if( display_state == set_val_gen ){        // Automatic stimulus for position set value (position control mode)
                  set_position = (wav_gen_ticks < WAV_GEN_HIGH_COUNT) ? 
                                  WAV_GEN_HIGH_LEVEL_POS*CONV_ROTATIONS_POS : WAV_GEN_LOW_LEVEL_POS*CONV_ROTATIONS_POS;
              	  if (wav_gen_ticks < WAV_GEN_HIGH_COUNT) LedSet(0x02); else LedClear(0x02); 
             }
             positionControl();
         }
         // rpm control
         if( control_state == rpm && display_state == set_val_gen ){  // Automatic stimulus for rpm set value (rpm control mode)
              set_rpm = (wav_gen_ticks < WAV_GEN_HIGH_COUNT) ?  WAV_GEN_HIGH_LEVEL_RPM/CONV_ADC_RPM_UPMIN : WAV_GEN_LOW_LEVEL_RPM/CONV_ADC_RPM_UPMIN;
              if (wav_gen_ticks < WAV_GEN_HIGH_COUNT) 
                  LedSet(0x02); 
              else 
                  LedClear(0x02); 
         }
//*********************************************************************************************
    // Set speed limiter
    // set value limitation for position control
    set_rpm = (set_rpm > max_pos_control_output) ? max_pos_control_output : set_rpm;
    set_rpm = (set_rpm < min_pos_control_output) ? min_pos_control_output : set_rpm;
//*********************************************************************************************
 
         //ToDo: Implementierung des Drehzahlreglers
         //Der eigentliche Code fuer den Drehzahlregler gehoert in die Funktion rpmControl() (siehe oben)
         if (control_state == position || control_state == rpm) {  
             rpmControl();
         }

//*********************************************************************************************
         // Prepare PWM output signal
         if (rpm_control_u < -128) {  
            pwm_out = 0;
         } else if (rpm_control_u > +127) {  
            pwm_out = 255;
         } else {  
            pwm_out = ((uint8) rpm_control_u) + 128;	 // Offset for PWM control output
         }

//*********************************************************************************************

         LedClear(0x20);                                // For debugging: end of control algo
         measuredTimeInUsec = ((float) (TimerGet()-startTime)) * TIMER_RESOLUTION;  // Run-time since last call of startTime (resolution 0.667�sec)

         if( control_state == manual ){
              if( display_state == set_val_gen ){       // Automatic stimulus for PWM (manual mode)
                    manual_u = (wav_gen_ticks < WAV_GEN_HIGH_COUNT) ? WAV_GEN_HIGH_LEVEL_PWM : WAV_GEN_LOW_LEVEL_PWM;
                    if (wav_gen_ticks < WAV_GEN_HIGH_COUNT) LedSet(0x02); else LedClear(0x02); 
              }
              manual_u = (manual_u < UMIN) ? UMIN : manual_u;
              manual_u = (manual_u > UMAX) ? UMAX : manual_u;
              pwm_out = (uint8) manual_u;
         }

         // Check for overload
         if( overload_I2dt(measured_current*CONV_ADC_I_mA) || error_msg == err_txt_I2dt_overload ){
              error_msg = err_txt_I2dt_overload;
              pwm_out = 128;
         }

         PwmSetDuty(PWMChannel, pwm_out);

#ifdef USE_SOFTSCOPE						// Send data to Matlab via serial interface
        LedSet(0x10);
        if(control_state == position) {   
            softScope((int16) pos_tick, (int16) (measured_rpm * CONV_ADC_RPM_UPMIN));   
        } else if(control_state == rpm) {   
            softScope((int16) (set_rpm * CONV_ADC_RPM_UPMIN), (int16) (measured_rpm * CONV_ADC_RPM_UPMIN));  
        } else if(control_state == manual) {   
//          softScope((int16) (measured_rpm_adc), (int16) (measured_rpm_ttl));  
            softScope((int16) (measured_rpm_adc * CONV_ADC_RPM_UPMIN), (int16) (measured_rpm_ttl * CONV_ADC_RPM_UPMIN));  
//          softScope((int16) (measured_rpm_adc), (int16) (dt0+dt1));  
        }
#endif  
        LedClear(0x40);
    }
}


// ##### Main program
void main(void)
{   
    char lcd_line_0[32];
    char lcd_line_1[32];
    uint16 waitcount; 

    HalInit(CLK_8_MHZ);                                // Initialize the HAL (Dragon12 with 8 MHz crystal)

    DisableInterrupts;
    HPRIO = 0xcc;                                      // Raise interrupt priority of Port H ISR (position decoder)
    EnableInterrupts;

    LcdInit();                                         // Initialize LCD display

    Adc1Init();                                        // Initialize ADC
    Adc1RegisterCallback(&myADC1Callback);             // Register user callback routine for ADC
    Adc1EnableInterrupt(1);                            // Enable interrupts from ADC

    LedInit();                                         // Initialize LEDs (for relais only)
    LedSet(0x01);                                      // Set PB0 to 1 to switch on the relais
    SevenSegInit();

#ifdef USE_SOFTSCOPE
    softScopeInit();
#endif

    PwmInit(2, 0, 2, 0, 0);                            // Initialize PWM
    PwmSetPeriod(PWMChannel, 255);                     // Set PWM period to maximum
    PwmSetDuty(PWMChannel, 128);                       // Set PWM duty cycle to 50% -> uMotor = 0V
    PwmStart(PWMChannel);
    
    TimerInit(TIMER_DIVIDER);                          // Timer for runtime measurements

    // Init position decoder 
    pos_tick = 0L;                                     // Position starts at zero
    pac_position_decoding = 0;                         // We start in slow position decoding mode 
    SwitchInit();                                      // Initialize buttons and position encoder inputs
    SwitchAckInterrupt();                              // Clear any pending interrupt requests 
    Pac10InitEncoder();                                // Init pac1 and pac0 in 8 bit queue mode 
    SwitchRegisterCallback(myPortH74Callback);         // Register user callback routing for Port H(7..4) interrupt handling
    SwitchEnableInterrupt(0xA0, 0xA0);                 // Initialize Port H(7..4) for Encoder Interrupts

    RtiRegisterCallback(&myRtiCallback);               // Register user callback routine for RTI
    RtiPeriodNS = RtiStart(RTI_PERIOD_US);             // Start real time interrupt; 
    RtiEnableInterrupt(TRUE);

    manual_u      = 128;
    set_rpm       = 0;
    set_position  = 0;

    for(;;){
       // Control state machine -- display handling for line 0 and in display_state disp_only also for line 1 
       switch( control_state ){
            case rpm      : (void) sprintf(lcd_line_0, "R:w=%4d e=%4d", (int16)(set_rpm*CONV_ADC_RPM_UPMIN), (int16)(e_rpm*CONV_ADC_RPM_UPMIN));    
                            switch( display_state ){
                               case disp_only : (void) sprintf(lcd_line_1, "y=%4d u=%4d", (int16)(measured_rpm*CONV_ADC_RPM_UPMIN), (int16)(rpm_control_u));    
                            }
                            break;
            case position : (void) sprintf(lcd_line_0, "P:w=%2.2f e=%4d", (((float)set_position)/CONV_ROTATIONS_POS), (int16)e_position);    
                            switch( display_state ){                                                    
                               case disp_only : (void) sprintf(lcd_line_1, "y=%2.2f u=%4d", (((float)pos_tick)/CONV_ROTATIONS_POS), (int16)(set_rpm*CONV_ADC_RPM_UPMIN));    
                            }
                            break;
            case manual   : (void) sprintf(lcd_line_0, "M:u=%3.0f nA=%5d", manual_u, (int16)(measured_rpm_adc*CONV_ADC_RPM_UPMIN)); 
                            switch( display_state ){                                                    
                               case disp_only : (void) sprintf(lcd_line_1, "");
                            }
                            break;
            default       : error_msg = err_txt_not_implemented_controlstate;
                            break;
       }

       // Display state machine -- Handling of line 1 of LCD-Display
       switch( display_state ){
             case disp_only       : break;           // Individual outputs dependend on selected control -- handled above
             case set_val_gen     : (void) sprintf(lcd_line_1, "AutoStim %3.0fus", measuredTimeInUsec);
                                    break;
             case set_val_inc_dec : (void) sprintf(lcd_line_1, "SW4:- 5:+  %5d", (int16)(measured_rpm_filtered * CONV_ADC_RPM_UPMIN));    
//                                  (void) sprintf(lcd_line_1, "SW4: -- SW5: ++");    
                                    break;
             case set_val_0_max   : (void) sprintf(lcd_line_1, "SW4: 0 SW5: max");    
                                    break;
             case set_val_0_min   : (void) sprintf(lcd_line_1, "SW4:%c0 SW5: min", ( (control_state == position) ? '!' : ' ' ) );    
                                    break;
             default              : error_msg = err_txt_not_implemented_displaystate;
                                    break;
       }

       // Output to LCD display
       display_results(error_msg, lcd_line_0, lcd_line_1);

       // Handle relais
       if( control_state == position && display_state == set_val_0_min ){
          // Turn off relais such that the axis can be rotated by hand
          LedClear(0x01);
       } else {
          // Turn on relais to enable uC control
          LedSet(0x01);
       }

       // Handle SW2, SW3, SW4 and SW5 buttons
       sw_pressed_action(&control_state, &display_state, &set_rpm, &set_position, &manual_u);

       // Busy wait
       for(waitcount = 0; waitcount < 2000; waitcount++){
           ;
       }
    }
}

