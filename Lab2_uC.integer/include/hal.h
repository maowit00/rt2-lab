#ifndef __HAL__
#define __HAL__ 1

#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include <pico/divider.h>
#include "quadrature_encoder_substep.pio.h"
#include "quadrature_encoder_substep.h"


// ############################################################
// BUTTONS
// ############################################################
#define PICO_BUTTON_1 21 // SW07_1
#define PICO_BUTTON_2 20 // SW07_2
#define PICO_BUTTON_3 15 // SW07_3
#define PICO_BUTTON_4 14 // SW07_4

// Initialize the GPIO for the Buttones
void pico_button_init(void);

// read the buttons
uint8_t pico_read_buttons(void);


// ############################################################
// ADC
// ############################################################
#define ADC_NUM_CURRENT 1
#define ADC_NUM_Tacho 0
#define ADC_PIN_I (26 + ADC_NUM_CURRENT)
#define ADC_PIN_N (26 + ADC_NUM_Tacho)
// Skalierung für Integer Rechnung:
// Skalierung mit 2^18 bedeutet:
// Schrittspannung exakt E0 = 3.3 / (2^12 - 1) = 8.0586e-04
// Schrittspannung int   E0int = floor( E0 * 2^18) = 211
// Schrittspannung abs Fehler  = E0int / 2^18 - E0 = -9.5968e-07
// Schrittspannung rel Fehler  = abs Fehler / E0 = 0.12%
// max reading des ADC 2^12 -> max Wert in int = E0int * 2^12 = 864256
// max reading log_2 = 20 -> passt in int32
#define BITSCALEADC 18
#define ADC_VREF (double)3.3
#define ADC_RANGE (1 << 12) // wir haben einen 12-bit ADC
#define ADC_E0_INT (int32_t)( ADC_VREF / (double)(ADC_RANGE - 1) * (1<<BITSCALEADC) )
#define ADC_MAX_READING_INT (int32_t)(ADC_E0_INT * ADC_RANGE)

#define BITSCALECONDITIONING 18
// Anpassschaltung: Verstärkung -R2/R1 mit R2 = 5.6 kOhm und R1 = 34.3 kOhm
// float Wert (betrag) = 0.0016
// Skalierung mit 2^18 macht Fehler 0.0016 statt richtigem Wert 0.0016 -> 0.23% rel Fehler
#define ADCREADINGTOCURRENT (int32_t)((float)((-5.6/34.3) * 1. / 0.1 / 1000.) * (float)(1 << BITSCALECONDITIONING))
// Anpassschaltung: Verstärkung -R2/R1 mit R2 = 5.6 kOhm und R1 = 53 kOhm
// float Wert = 2.1132e-04
// Skalierung mit 2^18 macht 2.0981-04 statt richtigem Wert 2.1132e-04 -> 0.72% Fehler
#define ADCREADINGTORPM     (int32_t)((float)((5.6/53.) * 2. / 1000.) * (1 << BITSCALECONDITIONING))

// ############################################################
// PWM
// ############################################################
#define PICO_PWM_OUT 18

// ############################################################
// substep encoder
// ############################################################
#define ENCODER_PIN_A 0 // pin B needs to be the next pin, i.e. 1

// ############################################################
// PIN used for turning on/off the relais
// ############################################################
#ifndef PIN_RELAY_MOTOR
#define PIN_RELAY_MOTOR 19  // 15 
#endif
// Initialize the GPIO for the relay
void pico_relay_init(void);

// Turn the relay on or off
void pico_set_relay(bool relay_on);

// ############################################################
// LCD display
// ############################################################
/* Example code to drive a 16x2 LCD panel via a I2C bridge chip (e.g. PCF8574)

   NOTE: The panel must be capable of being driven at 3.3v NOT 5v. The Pico
   GPIO (and therefore I2C) cannot be used at 5v.

   You will need to use a level shifter on the I2C lines if you want to run the board at 5v.

   Connections on Raspberry Pi Pico board, other boards may vary.

   GPIO 4 (pin 6)-> SDA on LCD bridge board
   GPIO 5 (pin 7)-> SCL on LCD bridge board
*/

#define I2C_SDA_PIN 16 
#define I2C_SCL_PIN 17 

// Modes for lcd_send_byte
#define LCD_CHARACTER  1
#define LCD_COMMAND    0

#define MAX_LINES      2
#define MAX_CHARS      16

// commands
#define LCD_CLEARDISPLAY     (int)0x01
#define LCD_RETURNHOME       (int)0x02
#define LCD_ENTRYMODESET     (int)0x04
#define LCD_DISPLAYCONTROL   (int)0x08
#define LCD_CURSORSHIFT      (int)0x10
#define LCD_FUNCTIONSET      (int)0x20
#define LCD_SETCGRAMADDR     (int)0x40
#define LCD_SETDDRAMADDR     (int)0x80

// flags for display entry mode
#define LCD_ENTRYSHIFTINCREMENT  (int)0x01
#define LCD_ENTRYLEFT            (int)0x02

// flags for display and cursor control
#define LCD_BLINKON          (int)0x01
#define LCD_CURSORON         (int)0x02
#define LCD_DISPLAYON        (int)0x04

// flags for display and cursor shift
#define LCD_MOVERIGHT        (int)0x04
#define LCD_DISPLAYMOVE      (int)0x08

// flags for function set
#define LCD_5x10DOTS         (int)0x04
#define LCD_2LINE            (int)0x08
#define LCD_8BITMODE         (int)0x10

// flag for backlight control
#define LCD_BACKLIGHT        (int)0x08

#define LCD_ENABLE_BIT       (int)0x04

// By default these LCD display drivers are on bus address 0x27
//#define HAL_ADDR   (int)0x3F
#define HAL_ADDR   (int)0x27

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t val);

// Toggle enable pin on LCD display
// We cannot do this too quickly or things don't work
void lcd_toggle_enable(uint8_t val);

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode);

void lcd_clear(void);

// go to location on LCD
void lcd_set_cursor(int line, int position);

static inline void lcd_char(char val);

void lcd_string(const char *s);

void lcd_init(void);

#endif  // #ifndef __HAL__
