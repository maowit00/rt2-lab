// Laborversuch Systemtechnik/Regelungstechnik 2: Motorboard
// (C) 2026 W.Lindermeir, W.Zimmermann
// Hochschule Esslingen

#include "main.h"
#if defined(__linux__) || defined(__FreeBSD__)   /* Linux & FreeBSD */
#else  /* windows */
//#include <cstdint>
#include "quadrature_encoder_substep.pio.h"
#endif


uint64_t time_us_in_repeating_timer_callback;       // for logging: time in repeating timer callback function in us

static enum c_state control_state = manual;     // state machine for control toggling
static enum d_state display_state = disp_only;  // state machine for display and button interpretation toggling

// Manual control
float manual_u;        // pwm value in manual mode 0..255

// rpm control                    
float set_rpm;         // set rpm \in approx. -7800 ...+7800 U/min
float e_rpm;           // rpm control: control error
float rpm_control_u;   // rpm control: control output

// rpm measurements
float measured_rpm_tacho = 0;             // rpm speed measured via Tacho ADC-reading in U/min

// substep encoder
substep_state_t encoder_state;
int last_position = 0;
int64_t current_substep_int64 = 0;      // current position of shaft; accumulated from deltas of substep-encoder
float rpm_substepencoder = 0;           // rpm speed measured via substepencoder pulse period in U/min

// Position control               
float set_position;                     // set position in steps (400, i.e. CONV_ROTATIONS_POS steps per rotation)
float e_position;                       // position control: control error signal in steps

// Motor overload
float measured_current_mA;      // current ADC-reading in mA

// Error handling: error strings to be displayed on the LCD-display
const char * error_msg_l0 = NULL;
const char * error_msg_l1 = NULL;
// Error string mask: 2 lines 16 chars each           "0123456789ABCDEF" 
const char *err_txt_not_implemented_displaystate_l0 = "Not implemented ";
const char *err_txt_not_implemented_displaystate_l1 = "display state   ";
const char *err_txt_not_implemented_controlstate_l0 = "Not implemented ";
const char *err_txt_not_implemented_controlstate_l1 = "control state   ";
const char *err_txt_I2dt_overload_l0                = "Motor Overload  ";
const char *err_txt_I2dt_overload_l1                = "                ";

uint slice_num;  // slice number for PWM output

//*********************************************************************************************         
// Control Algorithms
//*********************************************************************************************

//#############################################################################################             
//#############################################################################################             
// ToDo: Bauen Sie den Code fuer den P--Positionsregler in die Funktion positionControl() ein
//       Die Funktion wird von repeating_timer_callback() in jeder Abtastperiode aufgerufen.
//
// Variablen, die Sie hierfuer verwenden sollen:
// *******************************************************************************************
// Groessen, die Sie lesen koennen. Diese Groessen sind an dieser Stelle bereits mit Werten belegt:
// *******************************************************************************************
// float set_position;         // Sollwert in Ticks (400, i.e. CONV_ROTATIONS_POS Ticks pro Umdrehung)
// int64_t current_substep_int64; // Istposition in Ticks
// MAX_RPM                     // Stellgroessenbeschraenkung oben 7800 [U/min]
// MIN_RPM                     // Stellgroessenbeschraenkung unten -7800 [U/min]
// *********************************************************************************
// Groessen, die Sie mit einem sinnvollen Wert belegen muessen: 
// *********************************************************************************
// const float kp_position     // Reglerparameter: Proportionalbeiwert
// *******************************************************
// Groessen, die Sie berechnen muessen; sind schon deklariert
// *******************************************************
// float e_position;           // Regelfehler in ticks (400, i.e. CONV_ROTATIONS_POS Ticks pro Umdrehung)
// float set_rpm;              // Stellgroesse des Positionsreglers und Sollwert fuer den unterlagerten 
//                             // Drehzahlregler in U/min
//
void positionControl(void)
{
//#############################################################################################
//ToDo: Einbau des Lagereglers/Positionsreglers und Begrenzung der Stellgröße
   const float kp_position = 1;   // Position control: P gain ANPASSEN!!

   e_position = 1;
   set_rpm = 1;

//#############################################################################################             
}


//#############################################################################################             
//#############################################################################################             
// ToDo: Bauen Sie den Code fuer den PI--Drehzahlregler mit Anti--Windup Massnahme in die
//       Funktion rpmControl() ein.
//       Die Funktion wird von repeating_timer_callback() in jeder Abtastperiode aufgerufen.
//
// Variablen, die Sie hierfuer verwenden sollen:
// *******************************************************************************************
// Groessen, die Sie lesen koennen. Diese Groessen sind bereits mit Werten belegt:
// *******************************************************************************************
// float set_rpm;                      // Drehzahl--Sollwert in U/min
// float rpm_substepencoder;           // Ist-Drehzahlwert in U/min
// SAMPLINGTIME_US                     // Abtastzeit des Reglers in us
// MAX_RPM_CONTROL_OUTPUT              // Stellgroessenbeschraenkung oben (127)
// MIN_RPM_CONTROL_OUTPUT              // Stellgroessenbeschraenkung unten (-128)
// *********************************************************************************
// Groessen, die Sie mit einem sinnvollen Wert belegen muessen: 
// *********************************************************************************
// const float kp_rpm                    // Reglerparameter: Proportionalbeiwert
// const float Tn_rpm                    // Reglerparameter: Nachstellzeit
// *******************************************************
// Groessen, die Sie berechnen muessen; sind schon deklariert
// *******************************************************
// float e_rpm;                          // Regelfehler in U/min
// float rpm_control_u;                  // Stellgroesse des Drehzahlreglers
// Sie muessen gegebenenfalls weitere Variablen lokal deklarieren
//
void rpmControl(void)
{
//ToDo: Reglerparameter des Drehzahlreglers sinnvoll vorgeben
//ToDo: und PI-Drehzahlregler mit Anti-Winup implementieren
    const float kp_rpm = 1; // ToDo       // rpm control: P gain
    const float Tn_rpm = 1; // ToDo       // rpm control: reset time [usec]
    
    e_rpm = 1;
    rpm_control_u = 1;
//#############################################################################################
}

//////////////////////////////////////////////////////////////////////////////////////////////
// Timer and repeating timer
//////////////////////////////////////////////////////////////////////////////////////////////
bool repeating_timer_callback(__unused struct repeating_timer *t) 
{
    uint64_t timer_start_time = time_us_64();
    uint8_t pwm_out = 128;  // output value for pwm signal \in [0;255]
    uint16_t adc_read_1, adc_read_2, adc_read_3;
    uint16_t adc_reading_median;
    float adc_reading_voltage;

    // Waveform generator time base
    static int16_t wav_gen_ticks = WAV_GEN_PERIOD;        
    wav_gen_ticks = (wav_gen_ticks) ? wav_gen_ticks-1 : WAV_GEN_PERIOD;
   
    // Read the three values from ADC of current sensor and use median value
    adc_select_input(ADC_NUM_CURRENT);
    adc_read_1 = adc_read();
    adc_read_2 = adc_read();
    adc_read_3 = adc_read();
    adc_reading_median = select_median(adc_read_1, adc_read_2, adc_read_3);
    adc_reading_voltage = adc_reading_median * ADC_CONVERT_VOLTAGE;
    // Anpassschaltung: Verstärkung -R2/R1 mit R2 = 5.6 kOhm und R1 = 34.3 kOhm
    measured_current_mA = (adc_reading_voltage - ADC_VREF/2.) / (-5.6/34.3) / 1 * 0.1 * 1000;
    //printf("Measured Current: %8.2f\t\t Voltage %8.2f\t\t\t", measured_current_mA, adc_reading_voltage);
   
    // Read three values from ADC of tacho generator and use median value
    adc_select_input(ADC_NUM_Tacho);
    adc_read_1 = adc_read();
    adc_read_2 = adc_read();
    adc_read_3 = adc_read();
    adc_reading_median = select_median(adc_read_1, adc_read_2, adc_read_3);
    adc_reading_voltage = adc_reading_median * ADC_CONVERT_VOLTAGE;
    // Anpassschaltung: Verstärkung -R2/R1 mit R2 = 5.6 kOhm und R1 = 53 kOhm
    measured_rpm_tacho = (adc_reading_voltage - ADC_VREF/2.) / (5.6/53) / 2 * 1000;
    //printf("Measured rpm: %.2f\t\t Voltage %.2f\n", measured_rpm_tacho, adc_reading_voltage);

    // read the substepencoder PIO 
    substep_update(&encoder_state);
    // uint encoder_state.position : position in substeps
    // uint encoder_state.raw_step : position in steps
    // int  encoder_state.speed    : rpm in substeps per second
    current_substep_int64 += (int)encoder_state.raw_step;
    current_substep_int64 -= last_position;
    last_position = (int)encoder_state.raw_step; 
    rpm_substepencoder = (float)encoder_state.speed / 64. / 400. * 60.; // in U/min; 64 substeps per Tick; 400 Ticks per Umdrehung

    // Position control
    if( control_state == position ){
        if( display_state == set_val_gen ){
             // Automatic stimulus for position set value (position control mode)
             set_position = (wav_gen_ticks < WAV_GEN_HIGH_COUNT) ? WAV_GEN_HIGH_LEVEL_POS*CONV_ROTATIONS_POS : WAV_GEN_LOW_LEVEL_POS*CONV_ROTATIONS_POS;
        }
        // evaluate position-controller
        positionControl();
    }
    // rpm control
    if( control_state == rpm && display_state == set_val_gen ){
         // Automatic stimulus for rpm set value (rpm control mode)
         set_rpm = (wav_gen_ticks < WAV_GEN_HIGH_COUNT) ? WAV_GEN_HIGH_LEVEL_RPM : WAV_GEN_LOW_LEVEL_RPM;
    }

    if (control_state == position || control_state == rpm) {
        // set value limitation for rpm control
        set_rpm = (set_rpm > MAX_RPM) ? MAX_RPM : set_rpm;
        set_rpm = (set_rpm < MIN_RPM) ? MIN_RPM : set_rpm;
        // evaluate rpm-controller
        rpmControl();

        // Prepare PWM output signal: convert from signed [-128:+127] to unsigned pwm value [0:255] and limit pwm value
        pwm_out = rpm_control_u + 128;    // Offset for PWM control output
        pwm_out = (pwm_out < MIN_PWM_VAL) ? MIN_PWM_VAL : pwm_out;
        pwm_out = (pwm_out > MAX_PWM_VAL) ? MAX_PWM_VAL : pwm_out;
    }

    if( control_state == manual ){
        if( display_state == set_val_gen ){       
              // Automatic stimulus for PWM (manual mode)
              manual_u = (wav_gen_ticks < WAV_GEN_HIGH_COUNT) ? WAV_GEN_HIGH_LEVEL_PWM : WAV_GEN_LOW_LEVEL_PWM;
        }
        manual_u = (manual_u < MIN_PWM_VAL) ? MIN_PWM_VAL : manual_u;
        manual_u = (manual_u > MAX_PWM_VAL) ? MAX_PWM_VAL : manual_u;
        pwm_out = (uint8_t)manual_u;
    }

   // Send data to Matlab softscope via serial interface
   if(control_state == position) {
       SoftScopeSend((int16_t) current_substep_int64, (int16_t)rpm_substepencoder);
   } else if(control_state == rpm) {
       SoftScopeSend((int16_t)set_rpm, (int16_t)rpm_substepencoder);
   } else if(control_state == manual) {
       SoftScopeSend((int16_t)measured_rpm_tacho, (int16_t)rpm_substepencoder);
   }
 
   // Check for overload
   if( overload_I2dt(measured_current_mA) || error_msg_l0 == err_txt_I2dt_overload_l0 ){
        error_msg_l0 = err_txt_I2dt_overload_l0;
        error_msg_l1 = err_txt_I2dt_overload_l1;
        pwm_out = 128;
   }

   // update PWM output
   pwm_set_gpio_level(PICO_PWM_OUT, pwm_out);

   time_us_in_repeating_timer_callback = time_us_64() - timer_start_time; 
   return true;
}

int main(void)
{
  stdio_init_all();
  uint idx_time_us_vec = 0;
  uint64_t time_us_vec[8] = {0,0,0,0,0,0,0,0};;         // logging of 8 last times in repeating timer callback function in us
  uint64_t max_time_us_in_repeating_timer_callback = 0; // maximum value of last 8 times in time_us_vec
  
  // initialize buttons for transitioning state machines
  pico_button_init();

  // initialize i2c and LCD 16x2 Display
  char lcd_line_0[17];
  char lcd_line_1[17];
  i2c_init(i2c_default, 100 * 1000);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SDA_PIN);
  gpio_pull_up(I2C_SCL_PIN);
  // Make the I2C pins available to picotool
  bi_decl(bi_2pins_with_func(I2C_SDA_PIN, I2C_SCL_PIN, GPIO_FUNC_I2C));
  lcd_init();

  // Init ADC
  adc_init();
  adc_gpio_init(ADC_PIN_I);
  adc_gpio_init(ADC_PIN_N);

  // Init set values
  manual_u      = 128;
  set_rpm       = 0;
  set_position  = 0;

  // Init PWM: output pin is PICO_PWM_OUT
  gpio_set_function(PICO_PWM_OUT, GPIO_FUNC_PWM);
  // Find out which PWM slice is connected to GPIO PICO_PWM_OUT
  slice_num = pwm_gpio_to_slice_num(PICO_PWM_OUT);
  // Set period of 255 cycles (0 to 254 inclusive)
  // allowes for constant output 0 and 1
  pwm_set_wrap(slice_num, 254);
  pwm_set_gpio_level(PICO_PWM_OUT, manual_u);
  // Set the PWM running
  pwm_set_enabled(slice_num, true);

  // initialize output pin for switching the relais
  pico_relay_init();  
  pico_set_relay(true); // turn on relais

  // Init substep encoder
  PIO pio_substep = pio0;
  const uint sm_substep = 0;
  pio_add_program(pio_substep, &quadrature_encoder_substep_program);
  // the two encoder pins are ENCODER_PIN_A and ENCODER_PIN_A+1
  substep_init_state(pio_substep, sm_substep, (uint)ENCODER_PIN_A, &encoder_state);

  //    // example calibration code, uncomment to calibrate the encoder:
  //    if(true){ 
  //       // todo
  //       // - turn on a DC motor at 50% PWM
  //       // - wait for the motor to reach a reasonably stable speed
  //       pwm_set_gpio_level(PICO_PWM_OUT, 190);
  //       sleep_ms(2000);
  //       // - run the phase size calibration code
  //       substep_calibrate_phases(pio_substep, sm_substep);
  //       // - stop the motor
  //       pwm_set_gpio_level(PICO_PWM_OUT, 128);
  //       sleep_ms(2000000);
  //    }
  // default: replace this with the output of the calibration function
  // substep_set_calibration_data(&encoder_state, 64, 128, 192);
  // Result for Board G07
  substep_set_calibration_data(&encoder_state, 63, 127, 196);

  // initialize repeating timer
  struct repeating_timer timer;
  add_repeating_timer_us(-SAMPLINGTIME_US, repeating_timer_callback, NULL, &timer);
  //call to stop repeating timer: cancel_repeating_timer(&timer);
  //negative time refers to timer restart at start of call

  while (1) {
     time_us_vec[idx_time_us_vec] = time_us_in_repeating_timer_callback;
     idx_time_us_vec++;
     idx_time_us_vec &= 0x7;
     max_time_us_in_repeating_timer_callback = time_us_vec[0];
     for(int i = 1; i < 8; i++)
        max_time_us_in_repeating_timer_callback =   // select max value in time_us_vec[]
          (max_time_us_in_repeating_timer_callback < time_us_vec[i]) ? time_us_vec[i] : max_time_us_in_repeating_timer_callback;



     // Control state machine -- display handling for line 0 
     switch( control_state ){
          case rpm      : (void) snprintf(lcd_line_0, 17, "R:w=%4d e=%4d", (int16_t)set_rpm, (int16_t)e_rpm);
                          break;
          case position : (void) snprintf(lcd_line_0, 17, "P:w=%4d e=%4d", (int16_t)set_position, (int16_t)e_position);
                          break;
          case manual   : (void) snprintf(lcd_line_0, 17, "M:u=%3d nT=%4d", (uint8_t)manual_u, (int16_t)measured_rpm_tacho);
                          break;
          default       : error_msg_l0 = err_txt_not_implemented_controlstate_l0;
                          error_msg_l1 = err_txt_not_implemented_controlstate_l1;
                          break;
     }

     // Display state machine -- Handling of line 1 of LCD-Display
     switch( display_state ){
           case disp_only       : (void) snprintf(lcd_line_1, 17, "nE=%4d         ", (int16_t)rpm_substepencoder);
                                  break;
           case set_val_gen     : (void) snprintf(lcd_line_1, 17, "AutoStim %4d us",  (uint16_t)max_time_us_in_repeating_timer_callback);
                                  break;
           case set_val_inc_dec : (void) snprintf(lcd_line_1, 17, "SW3: -- 4: ++");
                                  break;
           case set_val_0_max   : (void) snprintf(lcd_line_1, 17, "SW3: 0 SW4: max");
                                  break;
           case set_val_0_min   : (void) snprintf(lcd_line_1, 17, "SW3: 0 SW4: min");
                                  break;
           default              : error_msg_l0 = err_txt_not_implemented_displaystate_l0;
                                  error_msg_l1 = err_txt_not_implemented_displaystate_l1;
                                  break;
     }
         
     // Output to LCD display
     if( error_msg_l0 ) {
        lcd_set_cursor(0, 0);
        lcd_string(error_msg_l0);
        lcd_set_cursor(1, 0);
        lcd_string(error_msg_l1);
     }else{
        lcd_set_cursor(0, 0);
        lcd_string(lcd_line_0);
        lcd_set_cursor(1, 0);
        lcd_string(lcd_line_1);
     }

     // Handle SW07_1, SW07_2, SW07_3 and SW07_4 buttons
     sw_pressed_action(&control_state, &display_state, &set_rpm, &set_position, &manual_u);

     // busy wait
     sleep_us(10);
  }
}
