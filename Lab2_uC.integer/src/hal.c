
#include "hal.h"

// ############################################################
// BUTTONS
// ############################################################
// Initialize the GPIO for the Buttones
void pico_button_init(void){

    gpio_init(PICO_BUTTON_1);
    gpio_init(PICO_BUTTON_2);
    gpio_init(PICO_BUTTON_3);
    gpio_init(PICO_BUTTON_4);

    gpio_set_dir(PICO_BUTTON_1, GPIO_IN);
    gpio_set_dir(PICO_BUTTON_2, GPIO_IN);
    gpio_set_dir(PICO_BUTTON_3, GPIO_IN);
    gpio_set_dir(PICO_BUTTON_4, GPIO_IN);

    gpio_pull_up(PICO_BUTTON_1);
    gpio_pull_up(PICO_BUTTON_2);
    gpio_pull_up(PICO_BUTTON_3);
    gpio_pull_up(PICO_BUTTON_4);
}

// read the buttons
uint8_t pico_read_buttons(void){
   uint8_t ret = 0;
   ret = ret | (uint8_t)( (gpio_get(PICO_BUTTON_1)) ? 0x00 : 0x08 ); // SW07_1
   ret = ret | (uint8_t)( (gpio_get(PICO_BUTTON_2)) ? 0x00 : 0x04 ); // SW07_2
   ret = ret | (uint8_t)( (gpio_get(PICO_BUTTON_3)) ? 0x00 : 0x02 ); // SW07_3
   ret = ret | (uint8_t)( (gpio_get(PICO_BUTTON_4)) ? 0x00 : 0x01 ); // SW07_4
   return(ret);

}



// ############################################################
// Relay Pin
// ############################################################
// Initialize the GPIO for the relay
void pico_relay_init(void) {
    gpio_init(PIN_RELAY_MOTOR);
    gpio_set_dir(PIN_RELAY_MOTOR, GPIO_OUT);
}

// Turn the relay on or off
void pico_set_relay(bool relay_on) {
    gpio_put(PIN_RELAY_MOTOR, relay_on);
}

// ############################################################
// LCD display
// ############################################################

/* Quick helper function for single byte transfers */
void i2c_write_byte(uint8_t val) {
#ifdef i2c_default
    i2c_write_blocking(i2c_default, HAL_ADDR, &val, 1, false);
#endif
}

// Toggle enable pin on LCD display
// We cannot do this too quickly or things don't work
void lcd_toggle_enable(uint8_t val) {
#define DELAY_US 600
    sleep_us(DELAY_US);
    i2c_write_byte(val | LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
    i2c_write_byte(val & (uint8_t)~LCD_ENABLE_BIT);
    sleep_us(DELAY_US);
}

// The display is sent a byte as two separate nibble transfers
void lcd_send_byte(uint8_t val, int mode) {
    uint8_t high = (uint8_t)mode | (val & (uint8_t)0xF0) | LCD_BACKLIGHT;
    uint8_t low = (uint8_t)mode | ((val << 4) & 0xF0) | LCD_BACKLIGHT;

    i2c_write_byte(high);
    lcd_toggle_enable(high);
    i2c_write_byte(low);
    lcd_toggle_enable(low);
}

void lcd_clear(void) {
    lcd_send_byte(LCD_CLEARDISPLAY, LCD_COMMAND);
}

// go to location on LCD
void lcd_set_cursor(int line, int position) {
    int val = (line == 0) ? 0x80 + position : 0xC0 + position;
    lcd_send_byte((uint8_t)val, LCD_COMMAND);
}

static inline void lcd_char(char val) {
    lcd_send_byte((uint8_t)val, LCD_CHARACTER);
}

void lcd_string(const char *s) {
    int i = 0;
    while (*s) {
        lcd_char(*s++);
        i++;
    }
    while( i < 16 ){
        lcd_char(' ');
        i++;
    }
}

void lcd_init() {
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x03, LCD_COMMAND);
    lcd_send_byte(0x02, LCD_COMMAND);

    lcd_send_byte(LCD_ENTRYMODESET | LCD_ENTRYLEFT, LCD_COMMAND);
    lcd_send_byte(LCD_FUNCTIONSET | LCD_2LINE, LCD_COMMAND);
    lcd_send_byte(LCD_DISPLAYCONTROL | LCD_DISPLAYON, LCD_COMMAND);
    lcd_clear();
}


