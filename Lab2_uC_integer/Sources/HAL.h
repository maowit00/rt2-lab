/*
;   Hardware Abstraction Layer HAL for HCS12 Evaluation Board Dragon12
;
;   Computerarchitektur - Systemtechnik - Regelungstechnik
;   (C) 2007-2018 W. Zimmermann
;   Hochschule Esslingen - University of Applied Sciences, Germany
;
;   Author:  	W.Zimmermann, July  05, 2007
;   Modified:	W.Zimmermann, April 15, 2008
;				W.Zimmermann, March 23, 2011
;				W.Zimmermann, Sept  12, 2013
;   Include this header file into HCS12 C programs which use the HAL
;
;   Please note: This program is subject to LPGL licensing terms.
*/

#ifndef __HAL__
#define __HAL__

//! Uncomment "#define DRAGON12" for Dragon12 evaluation boards and "#define EVB9S12XF512E" for the S12XF starter kit evaluation board
#define DRAGON12
//#define EVB9S12XF512E

#pragma MESSAGE DISABLE C1420

#include <hidef.h>                      //Common defines

#ifdef DRAGON12
#include <mc9s12dp256.h>                //CPU specific defines
#endif
#ifdef EVB9S12XF512E
#include <mc9s12xf512.h>
#endif

//! Type definitions used in the HAL
typedef unsigned char           uint8;
typedef unsigned short int      uint16;
typedef unsigned long           uint32;
typedef char                    int8;
typedef short int               int16;
typedef long                    int32;
typedef uint8                   bool;
#ifndef TRUE
  #define TRUE        1
  #define FALSE       0
#endif

//#############################################################################
//! \defgroup HAL ***** HAL **********************************************************
//! @{

//! Crystal frequency (in kHz), see HalInit() for details.
typedef enum {CLK_4_MHZ=4000, CLK_8_MHZ=8000} FOSC;

//! Initializes the HAL, should be called before any other HAL function. Calls HalPllInit(), if PLL has not been initialized by the serial monitor.
/*! @param fOsc  use CLK_4_MHZ for 4 MHz crystal (Dragon12, EVB9S12XF512E), CLK_8_MHZ for  8MHz crystal (Dragon12 Plus)
*/
void HalInit(FOSC fOsc);

//! Get the frequency of the crystal oscillator in Hz (4MHz on Dragon12/EVB9S12XF512E, 8MHz on Dragon12 Plus)
uint32 HalGetOscclk(void);

//! Get the output frequency of the PLL in Hz (default 24MHz, when PLL was initialized)
uint32 HalGetBusclk(void);

//! Initialize the PLL. Normally called automatically by HalInit().
/*! Explicit call not required on Dragon boards, when the serial monitor is used..
*/
void HalPllInit(void);

//! Get HAL version info
/*! @return version info as 16bit integer (upper 8bits: major version, lower 8bits: minor version)
*/
int HalGetVersion(void);
//! @}


//#############################################################################
//! \defgroup LED ***** LEDs (Dragon12: Port B and J.1, EVB9S12XF512E: D29-D22)************************
//! @{

//! Initialize the 8 LEDs, must be called before any of the Led...() functions is used
/*! Note: On Dragon12 the LEDs do share data port B with the seven segment displays. I.e. if the seven segment display
    is used, the LEDs will display the same data as the active digit of the seven segment display.
    If several digits of the seven segment display are multiplexed, the LED signals will be multiplexed
    too and thus do not make much sense. So in this case, the LEDs should not be used.
*/
void  LedInit(void);

//! Turn on one or more of the LEDs
/*! @param mask		bit mask, set bits to one for those LEDs you want to turn on, e.g. 0x91 to turn on LEDs 7, 4 and 0.
			LEDs not specified in the mask, will not change their state.
*/
void  LedSet(uint8 mask);

//! Turn off one or more of the LEDs
/*! @param mask		bit mask, set bits to one for those LEDs you want to turn off, e.g. 0x91 to turn on LEDs 7, 4 and 0.
			LEDs not specified in the mask, will not change their state.
*/
void  LedClear(uint8 mask);

//! Directly turn LEDs on and off
/*! @param value	turns on (1) or off (0) all 8 LEDs.
*/
void  LedWrite(uint8 value);

//! Toggle one or more of the LEDs
/*! @param mask		bit mask, set bits to one for those LEDs you want to toggle, e.g. 0x91 to toggle LEDs 7, 4 and 0.
			LEDs not specified in the mask, will not change their state.
*/
void LedToggle(uint8 mask);

//! Get the LED state
/*! @return		read the state of all 8 LEDs
*/
uint8 LedGet(void);
//! @}

//#############################################################################
//! \defgroup DIP ***** DIP switch and push buttons (Port H), not available on EVB9S12XF512E *********
//! @{

//! Init the DIP switches and toggle buttons.
/*! Must be called before any of the Switch...() functions is used.
*/
void  SwitchInit(void);

//! Get the state of any of the switches or buttons
/*! @return	bit mask, where a 1 bit indicates that the respective switch is set or button is pressed
*/
uint8 SwitchGet(void);

#ifndef USER_BUTTON_ISR
//! Register a callback function, which will be called when the button interrupt is triggered
/*! @param userButtonCallback 	name of the callback function

    First register the callback function, then enable the interrupt via SwitchEnableInterrupt().

    Note: The HAL does implement Interrupt Service Routines (ISR) for the RTI interrupt 25. So
    the user may use a normal function instead of an ISR. If the user wants to provide his own ISR, he/she
    should insert \#define USER_BUTTON_ISR before line \#include "HAL.h"
*/
    void SwitchRegisterCallback(void userButtonCallback(uint8 mask));
#endif

//! Enable interrupt, when a switch or button is pressed or released
/*! @param mask		bit mask, defines which switches or buttons do trigger an interrupt
    @param positiveEdge	bit mask, set bit to 1, if a positive edge (0->1) shall trigger the interrupt
*/
void  SwitchEnableInterrupt(uint8 mask, uint8 positiveEdge);

//! Acknowledge the interrupt flag.
/*! Only needed it the user provides his own ISR rather than using the callback function.
*/
void  SwitchAckInterrupt(void);

//! @}

//#############################################################################
//! \defgroup SevenSeg ***** Seven Segment Display (Port B and P.3 ... P.0), not available on EVB9S12XF512E *********
//! @{

//! Seven Segment Display: Ones Digit
#define SEVENSEG_ONES           0x07
//! Seven Segment Display: Tens Digit
#define SEVENSEG_TENS		0x0B
//! Seven Segment Display: Hundreds Digit
#define SEVENSEG_HUNDREDS       0x0D
//! Seven Segment Display: Thousands Digit
#define SEVENSEG_THOUSANDS      0x0E
//! Seven Segment Display: Turn display off
#define SEVENSEG_OFF            0x0F

//! Init the 4 seven segment displays. Note: These displays must be multiplexed. They also do share PORT B together with the LEDs.
/*! -
*/
void SevenSegInit(void);

//! Set the seven segments of all enabled displays
/*!  @param mask    choose digit, one of SEVENSEG_ONES to SEVENSEG_THOUSANDS
     @param value   bit mask for the seven segments of all enabled displays
*/
void SevenSegWrite(uint8 mask, uint8 value);
//! @}

//#############################################################################
//! \defgroup RTI ***** Real Time Interrupt ***************************************************
//! @{

//! Initialize RTI and set RTI interrupt period to 2^(9+x) * (y+1) / OSCCLK (4MHz on Dragon12, 8MHz on Dragon12 Plus).
/*! @param x	1...7  clock prescaler, note: x=0 effectively disables RTI
    @param y	0...15 clock prescaler
*/
void RtiInit(uint8 x, uint8 y);

//! Enable/disable interrupt
/*! @param enable	TRUE to enable, FALSE to disable
*/
void RtiEnableInterrupt(bool enable);

//! Acknowledge the interrupt flag.
/*! Only needed it the user provides his own ISR rather than using the callback function.
*/
void RtiAckInterrupt(void);

#ifndef USER_RTI_ISR
//! Register a callback function, which will be called when the RTI interrupt is triggered
/*! @param userRtiCallback 	name of the callback function

    Note: The HAL does implement Interrupt Service Routines (ISR) for the RTI interrupt 7. So
    the user may use a normal function instead of an ISR. If the user wants to provide his own ISR, he/she
    should insert \#define USER_RTI_ISR before line \#include "HAL.h"
*/
     void RtiRegisterCallback(void userRtiCallback(void));
#endif

//! @}


//#############################################################################
//! \defgroup WDG/COP ***** Watchdog/Computer Operating Properly **************
//! @{

//! Initialize Watchdog
/*! @param x	0...7  watchdog time period select (number of OSCCLK cylces)

    Note: Once you start the watchdog, you must periodically call WdgRetrigger()
    within the watchdog time period. Otherwise the CPU will reset. There is
    no way to stop the watchdog, once initialized.
*/
void WdgInit(uint8 x);

//! Retrigger Watchdog
void WdgRetrigger(void);

//! @}



//#############################################################################
//! \defgroup LCD ***** LCD Display (Port K), not available on EVB9S12XF512E *********
//! @{

//! Initialize the LCD, must be called before using any of the other Lcd...() functions
void LcdInit(void);

//! Display a text string on the LCD
/*! @param row     0...1, choose upper or lower row on display
    @param string  text, max. 16 characters, rest of line filled with blanks
    @return        true if ok, false if string was truncated
 */
bool LcdWriteLine(int row, const char *string);
//! @}

//#############################################################################
//! \defgroup PWM ***** PWM (Port P, shared with Seven Segment Display), not yet supported on EVB9S12XF512E *********
//! @{

//! Initialize the PWM unit. Must be called before using any of the other Pwm...() functions
/*! @param xA	0...7, clock divisors for channels 0, 1, 4, 5
    @param yA   1...256
    @param xB	0...7, clock divisors for channels 2, 3, 6, 7
    @param yB   1...256
    @param mask  set to 1 for those channels, which shall use the slow clock signal

    Note:
    The clock tick of the PWM signal is either 	fast clock tick T  = 2^x / BUSCLK
    or the 					slow clock tick TS = 2^(x+1) * y /BUSCLK

    Channels 0,1,4,5 use clock divider A, channesl 2,3,6,7 use clock divider B.
    With mask you can choose, if a channel shall use the fast or the slow clock.

    All channels start with the HIGH phase of the PWM signal. Use PwmSetPolarity() to change.
*/
void PwmInit(uint8 xA, uint8 yA, uint8 xB, uint8 yB, uint8 mask);

//! Set polarity, i.e. if the PWM signal period starts with logic HIGH, followed by LOW or inverted (start with LOW followed by HIGH)
/*! @param channel         number (0...7) of the PWM channel, which shall be used
    @param startWithHigh   if TRUE period starts with logic HIGH (default after PwmInit()), if FALSE start with LOW
*/
void PwmSetPolarity(uint8 channel, bool startWithHigh);

//! Set the period of the PWM signal
/*! @param channel   number (0...7) of the PWM channel, which shall be used
    @param periodCnt number (0 ... 255) of PWM ticks for one PWM period, set to 255 for best (8bit) resolution
*/
void PwmSetPeriod(uint8 channel, uint8 periodCnt);

//! Set the duty cycle of the PWM signal
/*! @param channel   number (0...7) of the PWM channel, which shall be used
    @param dutyCnt   number (0 ... periodCnt) of PWM ticks of the first phase (HIGH or LOW as defined by PwmSetPolarity()) of a signal period
*/
void PwmSetDuty(uint8 channel, uint8 dutyCnt);

//! Start the PWM signal
/*! @param channel   number (0...7) of the PWM channel, which shall be used
*/
void PwmStart(uint8 channel);

//! Stop the PWM signal
/*! @param channel   number (0...7) of the PWM channel, which shall be used
*/
void PwmStop(uint8 channel);
//! @}

//#############################################################################
//! \defgroup ADC ***** ADC (Port AD), not yet supported on EVB9S12XF512E *********
//! @{

//! Initialize ADC interface.
/*! Must be called before using any of the other Adc...() functions.
*/
void   AdcInit(void);

//! Start a conversion
/*! @param  channel  number of the ADC channel to start a conversion (0 ... 7)
    @param multipleChannels 0 (reserved for future functional enhancements)
*/
void   AdcStart(uint8 channel, int multipleChannels);

//! Get conversion status
/*! @return	TRUE, if End of Conversion, i.e. if conversion is complete
*/
bool   AdcEoc(void);

//! Read converted value
/*! @param  channelSequenceNr   0 (reserved for future functional enhancements)
    @return analog signal as 10bit right justified value, i.e. 1023 = 5V ... 0 = 0V
*/
uint16 AdcGet(int channelSequenceNr);

#ifndef USER_ADC_ISR
//! Register a callback function, which will be called when a conversion is complete.
/*! @param userAdcCallback 	name of the callback function

    First register the callback, then enable the interrupt via AdcEnableInterrupts().

    Note: The HAL does implement an Interrupt Service Routine (ISR) for the ADC interrupts 22. So
    the user may use a normal function instead of an ISR. If the user wants to provide his own ISR, he/she
    should insert \#define USER_ADC_ISR before line \#include "HAL.h"
*/
     void AdcRegisterCallback(void userAdcCallback(void));
#endif

//! Enable ADC interface End of Conversion interrupt/callback
/*! @param enable 	TRUE to trigger an interrupt/callback, when a conversion is complete
*/
void   AdcEnableInterrupt(bool enable);

//! @}

//#############################################################################
//! \defgroup Timer ***** Timer *****************************************************************
//! @{

//! Output and input events for timer channels, see TimerChannelMode()
typedef enum {NOEVENT, OUTTOGGLE_INEDGEPOS, OUTZERO_INEDGENEG, OUTONE_INEDGEBOTH} TIMERPORTEVENT;

//! Initialize the timer module
/*! @param x	set timer tick to T_TCNT = 2^x / BUSCLK with x=0...7 (on Dragon12: BUSCLK = 24MHz)
		timer period is 2^16 * T_TCNT
*/
void   TimerInit(uint8 x);

//! Read the timer
/*! @return  current timer tick, see TimerInit()
*/
uint16 TimerGet(void);

//! Set the operation mode of a timer channel
/*! @param channel 	0..7 number of the channel
    @param isOutput	TRUE for output compare, FALSE for input capture
    @param event	one of TIMERPORTEVENT
*/
void TimerChannelMode(uint8 channel, bool isOutput, TIMERPORTEVENT event);

//! Set time of next event in tick units, see TimerInit()
/*! @param channel 	0..7 number of the channel
    @param tick         time of next event in ticks
    @param isAbsolute   TRUE if ticks is an absolute value, FALSE if ticks is relative to the last event
*/
void TimerSetNextEvent(uint8 channel, uint16 tick, bool isAbsolute);

//! Get time of next event in tick units, see TimerInit()
/*! @param channel 	0..7 number of the channel
    @return             time of last event in tick units
*/
uint16 TimerGetLastEvent(uint8 channel);

#ifndef USER_TIMER_ISR
//! Register callback functions, which will be called on timer events
/*! @param channel  	0...7 number of channel
    @param userTimerCallback 	name of the callback function

    First register the callback function, then enable interrupts via TimerEnableInterrupts().

    Note: The HAL does implement Interrupt Service Routines (ISR) for the Timer interrupts 8 ... 15. So
    the user may use a normal function instead of an ISR. If the user wants to provide his own ISR, he/she
    should insert \#define USER_TIMER_ISR before line \#include "HAL.h"
*/
     void TimerRegisterCallback(uint8 channel, void userTimerCallback(void));
#endif

//! Enable/disable interrupt
/*! @param channel 	0..7 number of the channel
    @param enable	TRUE to enable, FALSE to disable
*/
void TimerChannelInterruptEnable(uint8 channel, bool enable);

//! Acknowledge interrupt
/*! @param channel 	0..7 number of the channel
*/
void TimerChannelAckInterrupt(uint8 channel);

//! @}

//#############################################################################
//! \defgroup SCI ***** Serial Interface (SCI, UART) ***********************************************
//! @{

//! Initialize serial interface. Must be called before using any of the other Sci...() functions.
/*! @param interfaceNr  0 for first, 1 for second serial interface (UART)
    @param baudRate     baud rate, e.g. 2400, 4800, 9600, 19200, 38400, 57600, 115200 (min. 184bit/s, max. 1,5Mbit/s)

    Uses 8N1 (8bit characters, no parity, 1 stop bit).
*/
void SciInit(uint8 interfaceNr, uint32 baudRate);

//! Initialize serial interface. Must be called before using any of the other Sci...() functions.
/*! @param interfaceNr  0 for first, 1 for second serial interface (UART)
    @param baudRate     baud rate, e.g. 2400, 4800, 9600, 19200, 38400, 57600, 115200 (min. 184bit/s, max. 1,5Mbit/s)
    @param loopbackMode true, if using loopback mode

    Uses 8N1 (8bit characters, no parity, 1 stop bit).
*/
void SciInitEx(uint8 interfaceNr, uint32 baudRate, bool loopbackMode);

//! Read a byte from the serial interface (blocking, i.e. waits until the receive buffer is full)
/*! @param interfaceNr  0 for first, 1 for second serial interface (UART)
    @return last byte received
*/
uint8 SciGetc(uint8 interfaceNr);

//! Send a character to the serial interface (blocking, i.e. waits, until the transmit buffer is empty)
/*! @param interfaceNr  0 for first, 1 for second serial interface (UART)
    @param data		character (byte) to send
*/
void SciPutc(uint8 interfaceNr, uint8 data);

//! Check, if a character has been received, i.e. if the receive buffer is full. Call first, if you do not want SciGetc() or SciGets() to block.
/*! @param interfaceNr  0 for first, 1 for second serial interface (UART)
    @return TRUE, if the receive buffer is full
*/
bool SciInStatus(uint8 interfaceNr);

//! Check, if a character can be send, i.e. if the transmit buffer is empty. Call first, if you do not want SciPutc() or SciPuts() to block.
/*! @param interfaceNr  0 for first, 1 for second serial interface (UART)
    @return TRUE, if the transmit buffer is empty
*/
bool SciOutStatus(uint8 interfaceNr);

//! Get status of serial interface
/*! @param interfaceNr  0 for first, 1 for second serial interface (UART)
    @return bit7 = transmit register empty, bit5 = receive register full, other bits don't care
*/
uint8 SciStatus(uint8 interfaceNr);


//! Send a character string (calls SciPutc() repeatedly, until the string has been sent).
/*! @param interfaceNr  0 for first, 1 for second serial interface (UART)
    @param p            pointer to ASCIIZ string
*/
void SciPuts(uint8 interfaceNr, char *p);

#ifndef USER_SCI_ISR
//! Register a callback function, which will be called on character reception or transmission.
/*! @param interfaceNr  	0 for first, 1 for second serial interface (UART)
    @param userSciCallback 	name of the callback function

    First register the callback function, then enable interrupts via SciEnableInterrupts().

    In case, the callback function is used for both receive and transmit events, use SciInStatus() and
    SciOutStatus() to find out, if the interrupt was called by reception or transmission.

    Note: The HAL does implement Interrupt Service Routines (ISR) for the SCI interrupts 20 and 21. So
    the user may use a normal function instead of an ISR. If the user wants to provide his own ISR, he/she
    should insert \#define USER_SCI_ISR before line \#include "HAL.h"
*/
     void SciRegisterCallback(uint8 interfaceNr, void userSciCallback(void));
#endif

//! Enable serial interface receive or transmit interrupts/callback functions
/*! @param interfaceNr  	0 for first, 1 for second serial interface (UART)
    @param receiveInterrupt 	TRUE to trigger an interrupt/callback, when a character has been received
    @param transmitInterrupt	TRUE to trigger an interrupt/callback, when a character has been sent
*/
void SciEnableInterrupts(uint8 interfaceNr, bool receiveInterrupt, bool transmitInterrupt);

#ifdef _HCS12_SERIALMON
//! Standard output - With serial monitor SCI1 is used
   #define STDOUT       1
//! Standard input  - With serial monitor SCI1 is used
   #define STDIN	1
#else
//! Standard output - With simulation, SCI0 is used
   #define STDOUT       0
//! Standard input  - With simulation, SCI0 is used
   #define STDIN	0
#endif

//! @}

//#############################################################################
//! \defgroup CAN ***** CAN *****************************************************************
//! @{

//! Data structure for CAN messages
typedef struct
{
//! CanId	11 bit or 29 bit CAN identifier (Note: 29bit CAN identifiers must be/will be marked by MSB=1)
    uint32 CanId;
//! DataLength	Number of data bytes
    uint8  DataLength;
//! Data[8]	CAN message data (max. 8 byte)
    uint8  Data[8];
} CANMESSAGE;

//! Enumeration for CAN bitrates (in kbit/s)
typedef enum {CAN_1000, CAN_500, CAN_250, CAN_100} CANBITRATE;

//! Enumertion for different CAN operating modes
typedef enum {CAN_NORMAL, CAN_LISTENONLY, CAN_LOOPBACK} CANMODE;

//! Length of CAN ID acceptance filters
typedef enum {CAN_8BIT_FILTER} CANIDFILTER;

//! In the debugger CAN is simulated via serial interface
#define CANviaSCIx	0

//! Initialize the CAN interface
/*! @param canBitrate  	set bitrate, use one of CANBITRATE
    @param canMode	one of CANMODE, sets normal operation, listen only or loopback mode

    Note: In listen only mode, messages can be received, but not sent. In loopback mode,
    the controller is decoupled from the real bus, but receives its own send messages.

    This function resets the CAN ID acceptance registers to default, i.e. receiving all
    CAN messages. Use CanSetAcceptanceFilter(), if necessary.
*/
void CanInit(CANBITRATE canBitrate, CANMODE canMode);

//! Set CAN ID receive acceptance filter
/*! @param filterType   currently, only CAN_8BIT_FILTER is defined, i.e. upper 8bit of CAN ID are filtered
    @param filterIndex  index of the filter, with CAN_8BIT_FILTER: 0 ... 7
    @param acceptMask   upper 8bits of the CAN ID(s) which shall be accepted
    @param ignoreMask   set to 1 for don't care bits in the CAN ID (upper 8bits only)

    Note: Setting the filter interrupts CAN bus operation of the local node. Use the same
    setting for filterType in all calls. Behaviour when changing filterType between calls is
    undefined.
    
    Note: The Codewarrior debugger does not simulate CAN, however the HAL implements a
    CAN simulation via serial interface SCI0. So when calling CanInit() in the simulator
    you cannot use SCI0 in a user program. The simulation does only have one transmit and
    one receive buffer, does only allow the RX, but not the TX interrupt and does not
    support acceptance filtering and loopback mode.
*/
void CanSetAcceptanceFilter(CANIDFILTER filterType, uint8 filterIndex, uint8 acceptMask, uint8 ignoreMask);


//! Read a CAN message (blocks, until a message was received)
/*! @param pMsg		pointer to a buffer, into which a CAN message will be copied

    Note: 29bit IDs will be marked by the MSB bit in CanId set.

    If not used within the CAN receive interrupt service routine, CanIsReceiveBufferFull()
    should be called before. When trying to read an empty receive buffer, no data is read
    and pMsg->CanId is set to 0xFFFFFFFF.
*/
void CanReadMessage(CANMESSAGE *pMsg);

//! Send a CAN message
/*! @param pMsg		pointer to a buffer containing the message to be sent

    Note: 29bit IDs must be marked by setting the MSB in CanId.

    This function call may block, if the CAN controller does not have a free transmit buffer.
    If blocking is not allowed, check for free buffers with CanIsTransmitBufferFree() first.
*/
void CanSendMessage(CANMESSAGE *pMsg);

//! Query, if a call to CanSendMessage() can be done without blocking.
bool CanIsTransmitBufferFree(void);

//! Query, if a message is available for reading
bool CanIsReceiveBufferFull(void);               //Check if a message has been received

#ifndef USER_CAN_ISR
//! Register a callback function, which will be called when a CAN message was received
/*! @param userCanRxCallbackFcn  name of the callback function

    First register the callback function, then enable the interrupt via CanEnableInterrupt().
    It is assumed, that the callback function uses CanReadMessage() to read the message
    from the CAN's receive FIFO.

    Note: The HAL does implement an Interrupt Service Routine (ISR) for the CAN interrupt 38. So
    the user may use a normal function instead of an ISR. If the user wants to provide his own ISR, he/she
    should insert \#define USER_CAN_ISR before line \#include "HAL.h" and recompile the HAL library.
*/
    void CanRegisterRxCallback(void userCanRxCallbackFcn(void));

//! Register a callback function, which will be called when a CAN message was transmitted
/*! @param userCanTxCallbackFcn  name of the callback function

    First register the callback function, then enable the interrupt via CanEnableInterrupt().

    Note: The HAL does implement an Interrupt Service Routine (ISR) for the CAN interrupt 39. So
    the user may use a normal function instead of an ISR. If the user wants to provide his own ISR, he/she
    should insert \#define USER_CAN_ISR before line \#include "HAL.h" and recompile the HAL library.

    NOTE: CAN TX interrupt has NOT yet been fully tested!
*/
    void CanRegisterTxCallback(void userCanTxCallbackFcn(void));

//! Enable the CAN receive (RX) interrupt
/*! @param enable 	set to TRUE to enable, FALSE to disable CAN receive interrupts
*/
    void CanEnableRxInterrupt(bool enable);

//! Enable the CAN transmit (TX) interrupt
/*! @param enable 	set to TRUE to enable, FALSE to disable CAN receive interrupts

    NOTE: CAN TX interrupt has NOT yet been fully tested!
*/
    void CanEnableTxInterrupt(bool enable);

#endif
//! @}

//#############################################################################
//! \defgroup SPI ***** SPI ***************************************************
//! @{

//! Initialize SPI interface.
/*! Must be called before using any of the other Spi...() functions.
    @param master       true for master mode, false for slave mode
    @param x            x=0...7, bit rate is BUSCLK (typ. 24MHz) / { (x+1)*2^(y+1) }, e.g. x=2, y=1 for 2MHz
    @param y            y=0...7, see x
    @param lsbFirst     true to send LSB first, false to send MSB first
    @param useSSout     automatically activate /SS during data transmission (master only)
    @param clkMode      clkMode=0...3 sets CPOL and CPHA (see HCS12 manual) for clock polarity selection

    Note: Normal mode (not bidirectional, no interrupts).
*/
void SpiInit(bool master, int8 x, int8 y, bool lsbFirst, bool useSSout, int8 clkMode);

//! Send one byte (blocks until data sent)
/*! @param   c  character to send
*/
void SpiPutc(int8 c);

//! Read one byte (blocks until data read)
/*! @return byte read

    Note: To generate a SPI clock signal, SPI masters must call SpiPutc() before.
*/
int8 SpiGetc(void);

//! Send block of bytes (blocks until data sent)
/*! @param   pData  pointer to block of data
    @param   n      number of bytes
*/
void SpiPutData(int8 *pData, uint16 n);

//! Read block of bytes (reads up to nmax bytes or till an 'eot' character is received, whichever occurs first), blocks until all data read
/*! @param   pData pointer to buffer into which data will be copied
    @param   nmax  maximum number of bytes
    @param   eot   end of data character, e.g. 0x00 for character strings
    @return  number of bytes read (including eot)

    Note: This function does not generate a SPI clock signal, so it should be used with SPI slaves only.
    SPI masters should use a loop with SpiPutc and SpiGetc to generate a clock signal before reading a byte.
*/
uint16 SpiGetData(int8 *pData, int16 nmax, int8 eot);

//! Check, if a byte has been received, i.e. if the data register has data. Call first, if you do not want SpiGetc() to block.
/*! @return     True, if a byte is in the receive data register
*/
bool SpiInStatus(void);

//! Check, if the send data register is free. Call first, if you do not want SpiPutc() to block.
/*! @return     True, if the send data register is free
*/
bool SpiOutStatus(void);


//! Enable the SPI receive (RX) and/or transmit (TX) interrupt
/*! @param enableRXinterrupt 	set to TRUE to enable, FALSE to disable SPI receive interrupts
    @param enableTXinterrupt 	set to TRUE to enable, FALSE to disable SPI transmit interrupts

    NOTE: SPI interrupts NOT fully tested!
*/
void SpiEnableInterrupt(bool enableRXinterrupt, bool enableTXinterrupt);

#ifndef USER_SPI_ISR
//! Register a callback function, which will be called on character reception or transmission.
/*! @param userSpiCallback 	name of the callback function

    First register the callback function, then enable interrupts via SpiEnableInterrupt().
    In the callback function, SpiInStatus()/SpiGetc() should be called, when data is received.
    When using transmit interrupts, call SpiOutStatus() in the callback function and SpiPutc(), if
    more data needs to be sent.

    Note: The HAL does implement an Interrupt Service Routine (ISR) for the SPI interrupt. So
    the user may use a normal function instead of an ISR. If the user wants to provide his own ISR, he/she
    should insert \#define USER_SPI_ISR before line \#include "HAL.h"
*/
     void SpiRegisterCallback(void userSpiCallback(void));
#endif


//! @}

//#############################################################################
//! \defgroup I2C ***** I2C ***************************************************
//! @{

//! I2C bitrate, see I2cInit()
typedef enum {I2C_100_kHz, I2C_200_kHz, I2C_400_kHz} I2C_BITRATE;

//! Initialize I2C interface.
/*! Must be called before using any of the other I2c...() functions.
    @param localSlaveAddress 	NULL for bus master mode, local slave address for bus slave mode
    @param bitrate      	I2C_100_kHz, I2C_200_kHz or I2C_400_kHz
*/
void I2cInit(uint8 localSlaveAddress, I2C_BITRATE bitrate);


//! Send a data block from an I2C master to an I2C slave
/*! @param slaveAddress 	address of I2C slave
    @param *data                pointer to an array of data bytes
    @param dataLen              number of data bytes
*/
void I2cMasterSend(uint8 slaveAddress, uint8 *data, uint8 dataLen);


//! Read a data block from an I2C slave into an I2C master
/*! @param slaveAddress 	address of I2C slave
    @param *data                pointer to an array of bytes, to where the data will be copied
    @param dataLen              number of data bytes to be read
*/
void I2cMasterRead(uint8 slaveAddress, uint8 *data, uint8 dataLen);

//! @}

//#############################################################################
//! \defgroup FlexRay ***** FlexRay *******************************************
//! @{

//! Initialize FlexRay (EVB9S12XF512E only)
void FrInit(void);

//! @}

//#############################################################################
//! \defgroup Memory ***** Memory *********************************************
//! @{

//! Change Flash ROM memory page
/*! This function changes the Flash ROM block, which is mapped into address range
    0x4000-0x7FFF. The DP256 has 16 blocks of 16KB, the DP512 has 32 blocks of 16KB each.

    @param newPage 	number of memory page to set, range 0x30 ... 0x3D for DP256
    @return         number of previous page

    Note:
    0x3E is a mirror of memory locations 0x4000-0xBFFF, 0x3F is a mirror of
    locations 0xC000-0xFFFF.

    To move a function to a mapped page, declare the function as:<br>
      \#pragma CODE_SEG OTHER_ROM <br>
        void __far functionName(...)  { ...} <br>
      \#pragma CODE_SEG_DEFAULT <br>
    When calling the function, the compiler will automatically generate the required
    code to change the PPAGE register.

    To move data constant to a mapped page, declare the constant as:<br>
      \#pragma DATA_SEG OTHER_ROM<br>
        const int x = ...;<br>
      \#pragma DATA_SEG OTHER_ROM<br>
    Before you access this constant, you have to switch the memory page manually
    using MemSetPage().

    OTHER_ROM must have been declared in the .prm locater configuration file.
*/
uint8 MemSetPage(uint8 newPage);


//! Initialize the Flash-ROM for reprogramming
void MemFlashRomInitProgramming(void);

//! Erase a Flash-ROM sector (sector size is 512 byte)
/*! Erase a memory sector of 512 byte
    @param  flashBlock number of the Flash block 0...3
    @param  page      number of memory page
    @param startAddress      startAddress (address bits 8...0 will be ignored)
    @return 0 for success, !=0 indicating failure
*/
int8 MemFlashRomSectorErase(uint8 flashBlock, uint8 page, uint16 startAddress);


//! Program Flash memory
/*! @param  flashBlock number of the Flash block 0...3
    @param  page    number of memory page
    @param  startAddress   startAddress
    @param  data     data to be programmed
    @param  dataLength length of data in words (1 word = 2 byte = 16 bit)
    @return 0 for success, !=0 indicating failure

    Note 1: Only erased memory can be programmed, so use MemFlashRomSectorErase() before.

    Note 2: Program data and dataLength are in Word rather than Byte, because the HCS12
    controller can only reprogram 16bit values.

    Note 3: Memory in the current Flash block from which the program is executing cannot be reprogrammed!
*/
int8 MemFlashRomProgram(uint8 flashBlock, uint8 page, uint16 startAddress, uint16 data[], uint16 dataLength);

//! @}


//*****************************************************************************
#endif //__HAL__
