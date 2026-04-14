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
;				W.Zimmermann, Apr   20, 2015
;
;   Please note: This program is subject to LPGL licensing terms.
*/

#define HALVERSION 0x0007               //Current version V0.7

#include "hal.h"
#include <string.h>

#pragma MESSAGE DISABLE C4301	        //Turn off information about function inlining
#pragma MESSAGE DISABLE C1420	        //Turn off information about result of function call not used
#pragma MESSAGE DISABLE C4002			//Turn off warning about function result not used

uint32 OSCCLK =  4000000;               //Crystal frequency (default: 4MHz for Dragon12, EVB9SXF512E)

#ifdef DRAGON12
uint32 BUSCLK = 24000000;               //PLL output frequency (default: 24MHz on Dragon12/Dragon12 Plus)
#endif
#ifdef EVB9S12XF512E
uint32 BUSCLK = 40000000;               //40MHz on EVB9S12XF512E
#endif

//***** HAL *******************************************************************
void HalInit(FOSC fOsc)
{   OSCCLK = (uint32) fOsc * 1000UL;
#ifdef _HCS12_SERIALMON                 //Reserved for serial monitor operation
    if ((CLKSEL_PLLSEL!=1)|| (PLLCTL!=0xF1) || (CRGFLG_LOCK!=1))
    {   HalPllInit();                   //If PLL has not yet been initialized, initialize
    }
#endif    
}

#pragma INLINE
uint32 HalGetOscclk(void)               //Read crystal clock OSCCLK
{   return OSCCLK;
}

#pragma INLINE
uint32 HalGetBusclk(void)               //Read bus clock (PLL output) BUSCLK
{   return BUSCLK;
}


void HalPllInit(void)                   //Initialize PLL
{
#ifdef DRAGON12
    CLKSEL_PLLSEL = 0;			//Disconnect CPU clock from PLL, just in case
    switch (OSCCLK)
    {   case 8000000:
            SYNR  = 2;	                //Set for 24MHz
	    REFDV = 0;			//... with 8MHz Quartz (Dragon12 Plus)
    	    break;
    	default:
            SYNR  = 5;	                //Set for 24MHz SYSCLK=MCLK
	    REFDV = 0;			//... with 4MHz Quartz (Dragon12)
    	    break;
    }
    PLLCTL= 0xF1;			//Turn on PLL and clock monitor
#endif
#ifdef EVB9S12XF512E
    PLLCTL_PLLON = 0;                   // Disable PLL
    SYNR  = 0x49;
    REFDV = 0x40;
    PLLCTL_PLLON = 1;                   // Enable PLL

#endif
    asm NOP
    asm NOP
    while (CRGFLG_LOCK!=1);	        //Wait, till PLL has locked
    CLKSEL_PLLSEL=1;                    //Switch CPU clock to PLL clock
}

int HalGetVersion(void)
{   return HALVERSION;
}

//***** 8 LEDs ****************************************************************
void LedInit(void)
{
#ifdef DRAGON12
    DDRJ_DDRJ1 	= 1;	                //Data Direction Register Port J: bit 1 as output
    PTJ_PTJ1 	= 0;              	//Bit 1 = 0 turns supply voltage for LEDs on

    DDRB    = 0xFF;                 	//Data Direction Register Port B: Port B as output
    PORTB   = 0x00;			//All LEDs off
#endif
#ifdef EVB9S12XF512E
    PT1AD  = 0x0F;                      // Initial output state Port AD1
    RDR1AD = 0xFF;                      // Reduced drive
    DDR1AD = 0x0F;                      // Port AD as output LED D22-D25

    PTT    = 0xFF;                      // Initial output state Port T
    RDRT   = 0xFF;                      // Reduced drive
    DDRT   = 0x0F;                      // Lower nibble as outputs LED D26-D29
#endif
}

#pragma INLINE
void LedSet(uint8 mask)
{
#ifdef DRAGON12
    PORTB = PORTB | mask;
#endif
#ifdef EVB9S12XF512E
    PT1AD = PT1AD & (~mask | 0xF0);
    PTT   = PTT   & (~(mask>>4));
#endif
}

#pragma INLINE
void LedClear(uint8 mask)
{
#ifdef DRAGON12
     PORTB = PORTB & (~mask);
#endif
#ifdef EVB9S12XF512E
    PT1AD = PT1AD | (mask & 0x0F);
    PTT   = PTT   | (mask >> 4);
#endif
}

#pragma INLINE
void LedWrite(uint8 value)
{
#ifdef DRAGON12
    PORTB = value;
#endif
#ifdef EVB9S12XF512E
    PT1AD =  value & 0x0F;
    PTT   =  value >> 4;
#endif
}

#pragma INLINE
uint8 LedGet(void)
{
#ifdef DRAGON12
    return PORTB;
#endif
#ifdef EVB9S12XF512E
    return (PTT << 4) | (PT1AD & 0x0F);
#endif
}

#pragma INLINE
void LedToggle(uint8 mask)
{
#ifdef DRAGON12
   PORTB ^= mask;
#endif
#ifdef EVB9S12XF512E
    PT1AD ^= (mask & 0x0F);
    PTT   ^= (mask >> 4);
#endif
}

//***** DIP Switch and buttons ************************************************
// Note: Respective DIP switch must be set to one, otherwise associated button does not work!
#ifdef DRAGON12
void SwitchInit(void)
{ DDRH = 0x00;       //Port H as inputs
  PIEH = 0x00;       //No interrupts on Port H
}

#pragma INLINE
uint8 SwitchGet(void)
{
  #ifdef _HCS12_SERIALMON
    //HW implementation
    return ~PTH;	        //PTIH instead of PTH ???
  #else
    //Simulation implementation
    return PTH;
  #endif
}

#pragma INLINE
void SwitchAckInterrupt(void)
{   PIFH = PIFH;                        //'1' clears the interrupt flag
}

void SwitchEnableInterrupt(uint8 enableMask, uint8 positiveSlope)
{   PPSH = positiveSlope;               //'1' triggers interrupt on positive slope
    PIEH = enableMask;			//'1' enables interrupt for the respective input
}


#ifndef USER_BUTTON_ISR

void (*UserButtonCallbackFcn)(uint8 mask) = NULL;

interrupt 25 void ButtonISR (void)
{   uint8 c = PIFH;
    SwitchAckInterrupt();
    if (UserButtonCallbackFcn)
        UserButtonCallbackFcn(c);    	//Call the user callback, '1' in mask signals interrupt source
}

void SwitchRegisterCallback(void userButtonCallback(uint8 mask))
{   UserButtonCallbackFcn = userButtonCallback;
}
#endif //USER_BUTTON_ISR
#endif //DRAGON12

//***** Seven Segment Display *************************************************
#ifdef DRAGON12

#define SEVENSEG_ONES           0x07
#define SEVENSEG_TENS		0x0B
#define SEVENSEG_HUNDREDS       0x0D
#define SEVENSEG_THOUSANDS      0x0E
#define SEVENSEG_OFF            0x0F

void SevenSegInit(void)
{   DDRP =  DDRP      | 0x0F;       //Digit selection lines on Port P as output
    DDRB = 0xFF;                    //Data Direction Register Port B: Port B as output
    PTP_PTP0 = 1;                   //Turn all 4 digits off
    PTP_PTP1 = 1;
    PTP_PTP2 = 1;
    PTP_PTP3 = 1;
}

#pragma INLINE
void SevenSegWrite(uint8 mask, uint8 value)
{   PTP  = (PTP&0xF0) | mask;
    LedWrite(value);
}
#endif //DRAGON12

//***** Real Time Interrupt generator *****************************************
#ifndef USER_RTI_ISR
void (*UserRtiCallbackFcn)(void) = NULL;

interrupt 7 void RtiISR (void)
{   RtiAckInterrupt();
    if (UserRtiCallbackFcn)
        UserRtiCallbackFcn();
}

void RtiRegisterCallback(void userRtiCallback(void))
{   UserRtiCallbackFcn = userRtiCallback;
}
#endif //PROVIDE_USER_RTI_ISR

#pragma INLINE
void RtiInit(uint8 x, uint8 y)
{   RTICTL = (x & 0x07)<<4 | (y & 0x0F);	//RTI interrupt period is (x+1) * 2^(y+9) / OSCCLK (4MHz on Dragon12, 8MHz on Dragon12 Plus)
}

#pragma INLINE
void RtiEnableInterrupt(bool enable)
{   if (enable)
	CRGINT = CRGINT | 0x80;     		//Set RTIE bit in CRGINT --> Enable RTIE interrupts
    else
	CRGINT = CRGINT & ~0x80;
}

//! Reset flag which indicates the occurence of an RTI interrupt (must be called in the ISR, otherwise no further interrupt will be generated)
#pragma INLINE
void RtiAckInterrupt(void)
{   CRGFLG = CRGFLG | 0x80;             //Reset Interrupt Flag (bit 7 in CRGFLG)
}

//***** Watchdog **************************************************************
#pragma INLINE
void WdgInit(uint8 x)
{    COPCTL = 0b01000000 | (x & 0x07); 
        
}

#pragma INLINE
void WdgRetrigger(void)
{    ARMCOP = 0x55;
     ARMCOP = 0xAA;
}


//***** LCD Display ***********************************************************
#ifdef DRAGON12
/* Magic delay constants, based on 24MHz CPU clock */
#define ENBIT      (0x02)
#define DELAY40US  (200L)
#define DELAY4_1MS (22000L)
#define DELAY100US (5000L)
#define DELAY4_1S  (22000000L)
#define LCDWIDTH   (16)

//! Delay routine (uses busy wait)
void Delay(uint32 constant)
{   volatile uint32 counter;
    for(counter = constant; counter > 0; counter--);
}

#ifdef _HCS12_SERIALMON
/*
 * Write LCD module in 8-bit mode
 * Inputs:
 *  data: to be written, lower 4 bits are ignored
 *  rs: register select, only bit 0 is significant
 * Handles the shifting into place and the EN pulsing
 * This is only used at the start of the init sequence
 *
 */
static void LcdWrite8(unsigned char data)
{   unsigned char temp;

    temp = (data >> 2);                 //Prepare data, rs is always 0
    PORTK = temp;                       //Do write with EN=0 */
    PORTK = temp | ENBIT;               //         with EN=1 pulse write enable
    PORTK = temp;                       //         with EN=0
    Delay(DELAY40US);                   //Pause for display to complete processing
}

/*
 * Write LCD module in 4-bit mode
 * Inputs:
 *  data: to be written, 8 bits are significant
 *  rs: register select, only bit 0 is significant
 * Does two consecutive writes, high nibble, then low
 * Handles the shifting into place and the EN pulsing
 * This is can be used at any time (init and display)
 *
 */
static void LcdWrite4(unsigned char data, unsigned char rs)
{   unsigned char hi, lo;

    hi = ((data & 0xf0) >> 2) | (rs & 0x01);    //Split byte into 2 nibbles
    lo = ((data & 0x0f) << 2) | (rs & 0x01);

    PORTK = hi;                         //Do write upper nibble with EN=0
    PORTK = hi | ENBIT;                 //                      with EN=1 pulse write enable
    PORTK = hi;                         //                      with EN=0
    PORTK = lo;                         //Do write lower nibble with EN=0
    PORTK = lo | ENBIT;                 //                      with EN=1 pulse write enable
    PORTK = lo;                         //                      with EN=0
    Delay(DELAY40US);                   //Pause for display to complete processing
}

//! Initialize LCD module, must be called before using LCD display
void LcdInit(void)
{   DDRK = 0xFF;                        //Set port K as output

    LcdWrite8(0x30);                    //Tell LCD once
    Delay(DELAY4_1MS);
    LcdWrite8(0x30);                    //Tell LCD twice
    Delay(DELAY100US);
    LcdWrite8(0x30);                    //Tell LCD thrice
    LcdWrite8(0x20);                    //Last write in 8-bit mode sets bus to 4 bit mode
                                        //Now we are in 4 bit mode, write upper/lower nibble
    LcdWrite4(0x28, 0); // last function set: 4-bit mode, 2 lines, 5x7 matrix
    LcdWrite4(0x0c, 0); // display on, cursor off, blink off									*/
    LcdWrite4(0x01, 0); // display clear																			*/
    LcdWrite4(0x06, 0); // cursor auto-increment, disable display shift
}

//! Write a line of max. 16 ASCII characters to the LCD display
/* Write a line to the LCD.
 * Inputs:
 *   string: is a pointer to a null terminated array of char to be sent.
 *   line: determines which line to display (0=top line, 1=bottom line).
 * If the string is less than 16 characters long the rest of the line
 * is filled with blanks.
 *
 * Strings longer than 16 characters (excluding the terminating \0)
 * get truncated.
 *
*/
bool LcdWriteLine(int line, const char *string)
{   int8 currentChar;
    char endOfLine;
    uint8 instruction;

    if (line == 1)                      //Set address in LCD module
	instruction = 0xc0;             //-- bottom line
    else
	instruction = 0x80;             //-- top line
    LcdWrite4(instruction, 0);          // rs=0 means command

     endOfLine = 0;			//Send 16 characters to LDC display
     for (currentChar = 0; currentChar < LCDWIDTH; ++currentChar)
     {  if (string[currentChar] == 0)
        {   endOfLine = 1;
	}
	if (endOfLine == 0)
	{   LcdWrite4(string[currentChar], 1); // rs=1 means data
	} else
	{   LcdWrite4(' ', 1);
	}
     }
     if (strlen(string) > LCDWIDTH)
     {	return FALSE;
     } else
     {	return TRUE;
     }

}

#else
#define LCDDATA (*(unsigned char*) 0x32)
#define LCDCTRL (*(unsigned char*) 0x33)

void SLcdWriteCmd(unsigned char data)
{   LCDCTRL = 0b00000100;
    LCDDATA = data;
    LCDCTRL = 0b00000000;
}

void SLcdWriteDat(unsigned char data)
{   LCDCTRL = 0b00000101;
    LCDDATA = data;
    LCDCTRL = 0b00000001;
}

bool LcdWriteLine(int line, const char *string)
{   int8 currentChar;
    char endOfLine;

    if (line == 1)                      //Set address in LCD module
    	SLcdWriteCmd(0xC0);            	//-- bottom line
    else
    	SLcdWriteCmd(0x80);             //-- top line

     endOfLine = 0;			//Send 16 characters to LDC display
     for (currentChar = 0; currentChar < LCDWIDTH; ++currentChar)
     {  if (string[currentChar] == 0)
        {   endOfLine = 1;
	}
	if (endOfLine == 0)
	{   SLcdWriteDat(string[currentChar]);
	} else
	{   SLcdWriteDat(' ');
	}
     }

     if (strlen(string) > LCDWIDTH)
     {	return FALSE;
     } else
     {	return TRUE;
     }
}

void LcdInit(void)
{   SLcdWriteCmd(0x30);
    SLcdWriteCmd(0x30);
    SLcdWriteCmd(0x30);
    SLcdWriteCmd(0x38);
    SLcdWriteCmd(0x0E);
    SLcdWriteCmd(0x01);
    SLcdWriteCmd(0x06);
}
#endif
#endif //DRAGON12

//***** PWM *******************************************************************
#ifdef DRAGON12
void PwmInit(uint8 xA, uint8 yA, uint8 xB, uint8 yB, uint8 mask)
{   PWMCLK = mask;			//Select slow or fast clock
    PWMPRCLK = ( xB << 4) | xA;		//Clock divisors
    PWMSCLA = yA;
    PWMSCLB = yB;
    PWMPOL = 0xFF;			//By default all PWM signals start with logic HIGH
}

#pragma INLINE
void PwmSetPeriod(uint8 channel, uint8 periodCnt)
{   uint8 *p = &PWMPER0;
    p[channel] = periodCnt;
}

#pragma INLINE
void PwmSetDuty(uint8 channel, uint8 highCnt)
{   uint8 *p = &PWMDTY0;
    p[channel] = highCnt;
}

#pragma INLINE
void PwmSetPolarity(uint8 channel, bool startWithHigh)
{   if (startWithHigh)
	PWMPOL = PWMPOL |  (1<<channel);
    else
	PWMPOL = PWMPOL & ~(1<<channel);
}

#pragma INLINE
void PwmStart(uint8 channel)
{   PWME   = PWME  | (1 << channel);
}

#pragma INLINE
void PwmStop(uint8 channel)
{   PWME   = PWME  & ~(1 << channel);
}
#endif //DRAGON12

//***** ADC *******************************************************************
#ifdef DRAGON12
void AdcInit(void)
{   ATD0CTL2=0xC0;                      //Enable ATD, fast clear, no interrupt
    Delay(DELAY40US);                   //10µs would be sufficient
    ATD0CTL3=0x08;		        //1 conversion only
    ATD0CTL4=0x45;                      //8 cycle sampling, ATD frequency 2MHz (24MHz/12), 10bit resolution
}

void AdcStart(uint8 channel, int multipleChannels)
{   uint8 ctrl5=0x80;
    uint8 ctrl3=ATD0CTL3 & 0x87;

    if (multipleChannels)
    {   ctrl3 = ctrl3 | (multipleChannels<<3);
        ctrl5 = ctrl5 | 0x10;
    } else
    {   ctrl3 = ctrl3 | (0x1 <<3);		// Was soll das, ctrl3 wird gar nicht verwendet ???

    }

    ATD0CTL5=ctrl5 + channel;		//Start conversion for selected channel
}

#pragma INLINE
bool AdcEoc(void)
{   return ATD0STAT0 & ATD0STAT0_SCF_MASK;//Read End of Conversion bit
}

uint16 AdcGet(int channelSequenceNr)  //Read conversion result
{   switch (channelSequenceNr)
    {   case 0: return ATD0DR0;
        case 1: return ATD0DR1;
        case 2: return ATD0DR2;
        case 3: return ATD0DR3;
        case 4: return ATD0DR4;
        case 5: return ATD0DR5;
        case 6: return ATD0DR6;
        case 7: return ATD0DR7;
    }

}

void AdcEnableInterrupt(bool enable)   //Enable/disable ADC conversion complete interrupt
{   if (enable)
        ATD0CTL2 =  ATD0CTL2 |  ((uint8) ATD0CTL2_ASCIE_MASK);
    else
        ATD0CTL2 =  ATD0CTL2 & ~((uint8) ATD0CTL2_ASCIE_MASK);
}

#ifndef USER_ADC_ISR
void (*UserAdcCallbackFcn)(void) = NULL;

void AdcRegisterCallback(void userAdcCallback(void))
{   UserAdcCallbackFcn = userAdcCallback;
}

interrupt 22 void adcISR (void)
{   if (UserAdcCallbackFcn)
        UserAdcCallbackFcn();
}

#endif //USER_ADC_ISR
#endif //DRAGON12

//***** Timer *****************************************************************
#pragma INLINE
void TimerInit(uint8 x)                    	//x=0...3 clock divider
{   TSCR1=0x80;					//Turn on timer module
    TSCR2=x & 0x07;    				//Timer clock is fTCNT = BUSCLK / 2^x = 24MHz / 2^x on Dragon12
    						//Timer period TP = 2^16 / fTCNT
}

#pragma INLINE
uint16 TimerGet(void)
{   return TCNT;
}

void TimerChannelMode(uint8 channel, bool isOutput, TIMERPORTEVENT event)
{   if (isOutput)
    { 	TIOS = TIOS |  (1 << channel);
	if (channel<4)
	{   TCTL2 = TCTL2 & ~(0b11  << (channel<<1));
	    TCTL2 = TCTL2 |  (event << (channel<<1));
	} else
	{   TCTL1 = TCTL1 & ~(0b11  << ((channel-4)<<1));
	    TCTL1 = TCTL1 |  (event << ((channel-4)<<1));
	}
    } else
    { 	TIOS = TIOS &  ~(1 << channel);
	if (channel<4)
	{   TCTL4 = TCTL4 & ~(0b11  << (channel<<1));
	    TCTL4 = TCTL4 |  (event << (channel<<1));
	} else
	{   TCTL3 = TCTL3 & ~(0b11  << ((channel-4)<<1));
	    TCTL3 = TCTL3 |  (event << ((channel-4)<<1));
	}
    }
}

#pragma INLINE
void TimerSetNextEvent(uint8 channel, uint16 tick, bool isAbsolute)
{   uint16 *pTC = (uint16 *) &TC0;
    if (isAbsolute)
    	pTC[channel]  = tick;
    else
    	pTC[channel] += tick;
}

#pragma INLINE
uint16 TimerGetLastEvent(uint8 channel)
{   uint16 *pTC = (uint16 *) &TC0;
    return pTC[channel];
}

#ifndef USER_TIMER_ISR
void (*UserTimerCallbackFcn[8])(void) = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

interrupt 8 void TimerISR0(void)
{   TimerChannelAckInterrupt(0);
    if (UserTimerCallbackFcn[0])
	UserTimerCallbackFcn[0]();
}
interrupt 9 void TimerISR1(void)
{   TimerChannelAckInterrupt(1);
    if (UserTimerCallbackFcn[1])
	UserTimerCallbackFcn[1]();
}
interrupt 10 void TimerISR2(void)
{   TimerChannelAckInterrupt(2);
    if (UserTimerCallbackFcn[2])
	UserTimerCallbackFcn[2]();
}
interrupt 11 void TimerISR3(void)
{   TimerChannelAckInterrupt(3);
    if (UserTimerCallbackFcn[3])
	UserTimerCallbackFcn[3]();
}
interrupt 12 void TimerISR4(void)
{   TimerChannelAckInterrupt(4);
    if (UserTimerCallbackFcn[4])
	UserTimerCallbackFcn[4]();
}
interrupt 13 void TimerISR5(void)
{   TimerChannelAckInterrupt(5);
    if (UserTimerCallbackFcn[5])
	UserTimerCallbackFcn[5]();
}
interrupt 14 void TimerISR6(void)
{   TimerChannelAckInterrupt(6);
    if (UserTimerCallbackFcn[6])
	UserTimerCallbackFcn[6]();
}
interrupt 15 void TimerISR7(void)
{   TimerChannelAckInterrupt(7);
    if (UserTimerCallbackFcn[7])
	UserTimerCallbackFcn[7]();
}

void TimerRegisterCallback(uint8 channel, void userTimerCallback(void))
{   UserTimerCallbackFcn[channel] = userTimerCallback;
}
#endif

#pragma INLINE
void TimerChannelInterruptEnable(uint8 channel, bool enable)
{   if (enable)
	TIE = TIE |   (1 << channel);
    else
    	TIE = TIE &  ~(1 << channel);
}

#pragma INLINE
void TimerChannelAckInterrupt(uint8 channel)
{   TFLG1 = TFLG1 | (1 << channel);
}

//***** Serial Interface (UART) ***********************************************
//Note: If you use interfaceNr=0 (SCI0), the debugger does not work anymore,
//      because the debugger uses this interface for its own communication,
//      when debugging the Dragon12 hardware board
void SciInit(uint8 interfaceNr, uint32 baudRate)
{   SciInitEx(interfaceNr, baudRate, FALSE);
}

void SciInitEx(uint8 interfaceNr, uint32 baudRate, bool loopbackMode)
{   uint16 divisor = (uint16)(BUSCLK /(baudRate<<4)) & 0x1FFF;	// divisor=MCLK/(16*baudRate), assume MCLK=BUSCLK=24MHz

/* bit value meaning in SCIxCR1
    7   0    LOOPS, no looping, normal
    6   0    WOMS, normal high/low outputs
    5   0    RSRC, not appliable with LOOPS=0
    4   0    M, 1 start, 8 data, 1 stop
    3   0    WAKE, wake by idle (not applicable)
    2   0    ILT, short idle time (not applicable)
    1   0    PE, no parity
    0   0    PT, parity type (not applicable with PE=0) */
/* bit value meaning in SCIxCR2
    7   0    TIE, no transmit interrupts on TDRE
    6   0    TCIE, no transmit interrupts on TC
    5   0    RIE, no receive interrupts on RDRF
    4   0    ILIE, no interrupts on idle
    3   1    TE, enable transmitter
    2   1    RE, enable receiver
    1   0    RWU, no receiver wakeup
    0   0    SBK, no send break */
  switch (interfaceNr)
  {   case 1:   SCI1BD  = divisor;
                if (loopbackMode)
                {   SCI1CR1 = 0xA4;
                } else
                {   SCI1CR1 = 0x04;
                }
                SCI1CR2 = 0x0C;
                break;
      case 0:
      default:
                SCI0BD  = divisor;
                if (loopbackMode)
                {   SCI0CR1 = 0xA4;
                } else
                {   SCI0CR1 = 0x04;
                }
                SCI0CR2 = 0x0C;
                break;
  }
}

#ifndef SCI0CR2_SCTIE_MASK
#define SCI0CR2_SCTIE_MASK 128
#endif
#ifndef SCI1CR2_SCTIE_MASK
#define SCI1CR2_SCTIE_MASK 128
#endif

void SciEnableInterrupts(uint8 interfaceNr, bool receiveInterrupt, bool transmitInterrupt)
{   if (interfaceNr==1)
    {   if (receiveInterrupt)
            SCI1CR2 = SCI1CR2 |  SCI1CR2_RIE_MASK;
        else
            SCI1CR2 = SCI1CR2 &  ~((uint8) SCI1CR2_RIE_MASK);
        if (transmitInterrupt)
            SCI1CR2 = SCI1CR2 |  SCI1CR2_SCTIE_MASK;
        else
            SCI1CR2 = SCI1CR2 &  ~((uint8) SCI1CR2_SCTIE_MASK);
    } else
    {   if (receiveInterrupt)
            SCI0CR2 = SCI0CR2 |  SCI0CR2_RIE_MASK;
        else
            SCI0CR2 = SCI0CR2 & ~((uint8) SCI0CR2_RIE_MASK);
        if (transmitInterrupt)
            SCI0CR2 = SCI0CR2 |  SCI0CR2_SCTIE_MASK;
        else
            SCI0CR2 = SCI0CR2 & ~((uint8) SCI0CR2_SCTIE_MASK);
    }
}

// Checks if new input is ready, TRUE if new input is ready
// Input: none
// Output: TRUE if a call to InChar will return right away with data
//         FALSE if a call to InChar will wait for input
bool SciInStatus(uint8 interfaceNr)
{    if (interfaceNr==1)
        return (SCI1SR1 & SCI1SR1_RDRF_MASK);
     else
        return (SCI0SR1 & SCI0SR1_RDRF_MASK);
}

// Checks if output data buffer is empty, TRUE if empty
// Input: none
// Output: TRUE if a call to OutChar will output and return right away
//         FALSE if a call to OutChar will wait for output to be ready
bool SciOutStatus(uint8 interfaceNr)
{    if (interfaceNr==1)
     {  return (SCI1SR1 & SCI1SR1_TDRE_MASK);
     } else
     {  return (SCI0SR1 & SCI0SR1_TDRE_MASK);
     }
}

uint8 SciStatus(uint8 interfaceNr)
{    if (interfaceNr==1)
     {  return SCI1SR1;
     } else
     {  return SCI0SR1;
     }
}

// Wait for new serial port input, busy-waiting synchronization
uint8 SciGetc(uint8 interfaceNr)
{   while(!SciInStatus(interfaceNr));   //Busy wait, till data is available
    if (interfaceNr==1)
        return SCI1DRL;
    else
        return SCI0DRL;
}

// Wait for buffer to be empty, output 8-bit to serial port
// busy-waiting synchronization
// Input: 8-bit data to be transferred
// Output: none
void SciPutc(uint8 interfaceNr, uint8 data)
{   while(!SciOutStatus(interfaceNr));	//Busy wait, till send register is empty
    if (interfaceNr==1)
        SCI1DRL = data;
    else
        SCI0DRL = data;
}

// Output String (NULL termination), busy-waiting synchronization
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void SciPuts(uint8 interfaceNr, char *p)
{   while(*p)
    {   SciPutc(interfaceNr, *p);
        p++;
    }
}

#pragma INLINE
void TERMIO_PutChar(char *c)            //Glue function to make stdio functions putc(), puts(), printf() work
{   SciPutc(STDOUT, (uint8) c);         //Serial interface must be initialized by SciInit() before
}

#pragma INLINE
char TERMIO_GetChar(void)               //Glue function to make stdio functions getc(), gets(), scanf() work
{   return SciGetc(STDIN);              //Serial interface must be initialized by SciInit() before
}


#ifndef USER_SCI_ISR
void (*UserSci0CallbackFcn)(void) = NULL;
void (*UserSci1CallbackFcn)(void) = NULL;

void SciRegisterCallback(uint8 interfaceNr, void userSciCallback(void))
{  if (interfaceNr==1)
        UserSci1CallbackFcn = userSciCallback;
   else
        UserSci0CallbackFcn = userSciCallback;
}

#ifndef _HCS12_SERIALMON                //Reserved for serial monitor operation
interrupt 20 void sci0ISR (void)
{   if (UserSci0CallbackFcn)
        UserSci0CallbackFcn();
}
#endif

interrupt 21 void sci1ISR (void)
{   if (UserSci1CallbackFcn)
        UserSci1CallbackFcn();
}
#endif //USER_SCI_ISR


//***** CAN *******************************************************************
#ifndef USER_CAN_ISR
void (*UserCanRxCallbackFcn)(void) = NULL;
void (*UserCanTxCallbackFcn)(void) = NULL;
#endif

#ifndef _HCS12_SERIALMON

#define CANSTX	0x02
#define CANETX  0x03
 
CANMESSAGE  canTXbuffer, canRXbuffer;
int8  canTXindex = -1,   canRXindex = -1;
bool  canRXbufferFull = FALSE, canTXbufferFree = TRUE;
bool  canRXintEnable  = FALSE, canTXintEnable  = FALSE;


void CanViaSciCallback(void)
{   volatile uint8 s=0xFF, c=0xFF; 

    s = SciStatus(CANviaSCIx);
    if (s & 0x20)			//Receiving
    {   c = SciGetc(CANviaSCIx);
    
       if (canRXindex == -1)
       {  if (c == CANSTX)
       	  {  canRXindex++;
       	  }
       } else if (canRXindex == 0)
       {  canRXbuffer.CanId = ((long) c) << 8;
          canRXindex++;
       } else if (canRXindex == 1)
       {  canRXbuffer.CanId = canTXbuffer.CanId + c;
          canRXindex++;
       } else if (canRXindex == 2)
       {  canRXbuffer.DataLength = c;
          canRXindex++;
       } else if ((canRXindex >= 3) && (canRXindex < 11) && (canRXindex-3 < canRXbuffer.DataLength))
       {  canRXbuffer.Data[canRXindex-3] = c;;
          canRXindex++;
       } else
       {  canRXindex = -1;
          canRXbufferFull = TRUE;
//        if c== CANETX call CAN callback, set buffer full
	  if ((canRXintEnable==TRUE)  && (UserCanRxCallbackFcn!=NULL))
          {   UserCanRxCallbackFcn();
          }
       }
    } else			 	//Sending
    {
       if (canTXindex == 0)
       {  SciPutc(CANviaSCIx, (uint8) ((canTXbuffer.CanId>>8) & 0xFF));
          canTXindex++;
       } else if (canTXindex == 1)
       {  SciPutc(CANviaSCIx, (uint8) (canTXbuffer.CanId & 0xFF));
          canTXindex++;
       } else if (canTXindex == 2)
       {  SciPutc(CANviaSCIx, canTXbuffer.DataLength);
          canTXindex++;
       } else if ((canTXindex >= 3) && (canTXindex < 11) && (canTXindex-3 < canTXbuffer.DataLength))
       {  SciPutc(CANviaSCIx, canTXbuffer.Data[canTXindex-3]);
          canTXindex++;
       } else
       {  SciEnableInterrupts(CANviaSCIx, TRUE, FALSE);
          SciPutc(CANviaSCIx, CANETX);    
          canTXindex = -1;
     	  canTXbufferFree = TRUE;
       }
    }
}
#endif

//---------------------------------------------------------------------------------
void CanReadMessage(CANMESSAGE *pMsg)
{   
#ifdef _HCS12_SERIALMON                 	//CAN Hardware only available on the Dragon12 board
    uint32 temp;

    if (CanIsReceiveBufferFull()==FALSE)        //Return, if the receive FIFO is empty
    {   pMsg->CanId = 0xFFFFFFFF;
        return;
    }

    if (CAN0RXIDR1 & 0x08)              	//Test if it is a 29bit CAN ID
    {   temp = CAN0RXIDR0;
        pMsg->CanId = temp << 21;

        temp = CAN0RXIDR1;
        pMsg->CanId = pMsg->CanId + ((temp & 0xE0) << 13) + ((temp & 0x07) << 15);

        temp = CAN0RXIDR2;
        pMsg->CanId = pMsg->CanId + (temp << 7);

        temp = CAN0RXIDR3;
        pMsg->CanId = pMsg->CanId + (temp >> 1);
        pMsg->CanId |= 0x80000000;              //Mark as 29bit CAN ID

    } else					//It's an 11bit CAN ID
    {   pMsg->CanId = CAN0RXIDR0;
        pMsg->CanId = (pMsg->CanId << 3) + (CAN0RXIDR1 >> 5);
    }

    (void) memcpy(pMsg->Data, &CAN0RXDSR0, CAN0RXDLR);
    pMsg->DataLength = CAN0RXDLR;

    CAN0RFLG = CAN0RFLG | 0x01;       	        // Clear receive flag
#else
    memcpy(pMsg, &canRXbuffer, sizeof(CANMESSAGE));
    canRXbufferFull = FALSE;
#endif
}

//----------------------------------------------------------------------------------
void CanSendMessage(CANMESSAGE *pMsg)
{   
#ifdef _HCS12_SERIALMON                 	//CAN Hardware only available on the Dragon12 board
    volatile uint8 txBuf;

    while ((CAN0TFLG & 0x07) == 0) {};     	//Wait for a free transmit buffer
    txBuf = CAN0TFLG & 0x07;               	//Select the transmit buffer
    CAN0TBSEL = txBuf;
    txBuf = CAN0TBSEL & 0x07;

    if (pMsg->CanId & 0x80000000)               //29bit CAN IDs
    {   CAN0TXIDR0 = (unsigned char) (pMsg->CanId >> 21);//Copy message to transmit buffer
        CAN0TXIDR1 = (unsigned char) (((pMsg->CanId >> 13) & 0xE0) | 0x18 | ((pMsg->CanId & 0x07) >> 7));
        CAN0TXIDR2 = (unsigned char) (pMsg->CanId >>  7);
        CAN0TXIDR3 = (unsigned char) (pMsg->CanId <<  1);
    } else                                   	//11bit CAN IDs
    {   CAN0TXIDR0 = (unsigned char) (pMsg->CanId >> 3); //Copy message to transmit buffer
        CAN0TXIDR1 = (unsigned char) (pMsg->CanId << 5);
        CAN0TXIDR2 = 0x00;
        CAN0TXIDR3 = 0x00;
    }

    (void) memcpy (&CAN0TXDSR0, pMsg->Data, pMsg->DataLength);

    CAN0TXDLR  = pMsg->DataLength;        	//Data length
    CAN0TXTBPR = CAN0TXIDR0;              	//Send priority (here set to same as upper 8bits of canId)

    CAN0TFLG = txBuf;                     	//Send the data
#else
     memcpy(&canTXbuffer, pMsg, sizeof(CANMESSAGE));
     canTXindex = 0;
     canTXbufferFree = FALSE;
     SciPutc(CANviaSCIx, CANSTX);
     SciEnableInterrupts(CANviaSCIx, FALSE, TRUE);
#endif    
}

#pragma INLINE
bool CanIsTransmitBufferFree(void)		//Check if at least one of the transmit buffers is free
{
#ifdef _HCS12_SERIALMON                 	//CAN Hardware only available on the Dragon12 board
    if (CAN0TFLG)
        return TRUE;
    else
        return FALSE;
#else    
    return canTXbufferFree;
#endif   
}

#pragma INLINE
bool CanIsReceiveBufferFull(void)               //Check if a message has been received
{
#ifdef _HCS12_SERIALMON                 	//CAN Hardware only available on the Dragon12 board
    if (CAN0RFLG & 0x01)
        return TRUE;
    else
        return FALSE;
#else    
    return canRXbufferFull;
#endif   
}

void CanInit(CANBITRATE canBitrate, CANMODE canMode)
{
#ifdef _HCS12_SERIALMON                 	//CAN Hardware only available on the Dragon12 board
    CAN0CTL0 = CAN0CTL0 | 0x01;	          	//Request initialization
    while ((CAN0CTL1 & 0x01) ==0) {};    	//Wait for acknowledge

    if (canMode==CAN_LOOPBACK)
    	CAN0CTL1 = 0xA0;		  	//CAN enable, loopback mode, listen off, use OSCCLK
    else if (canMode==CAN_LISTENONLY)
    	CAN0CTL1 = 0x90;		  	//CAN enable, no loopback, listen on, use OSCCLK
    else
    	CAN0CTL1 = 0x80;		  	//CAN enable, no loopback, listen off, use OSCCLK

    switch (canBitrate)			 	//Calculation for CAN bit timing see Freescale MSCAN block guide chapter 4.3.2
    {	case CAN_1000:
    	    if (OSCCLK == 8000000)
    	    {   CAN0BTR0 = 0x00;         	//1000 kbit/s at fOSCCLK = 8MHz, TBit = 8 Tq, Prescale=1, Sync Jump Width= 1
    	    } else
            {   CAN0CTL1 = CAN0CTL1 | 0x40;	//Must use fBUSCLK=24MHz from PLL and Prescale=3 (not recommended, but only solution)
    		CAN0BTR0 = 0x02;		//1000 kbit/s at fBUSCLK = 28MHz, TBit = 8 Tq, Prescale=3, Sync Jump Width= 1
            }
    	    CAN0BTR1 = 0x14;		 	//Time segment 1 = 5Tq, Time Segment 2 = 2Tq
    	    break;
    	case CAN_500:
    	    if (OSCCLK == 8000000)
    	    {   CAN0BTR0 = 0x41;           	//500 kbit/s at fOSCCLK = 8MHz, TBit = 8 Tq, Prescale=2, Sync Jump Width= 2
    	    } else
            {   CAN0BTR0 = 0x40;          	//500 kbit/s at fOSCCLK = 4MHz, TBit = 8 Tq, Prescale=1, Sync Jump Width= 2
            }
    	    CAN0BTR1 = 0x14;		   	//Time segment 1 = 5Tq, Time Segment 2 = 2Tq
    	    break;
    	case CAN_250:
    	    if (OSCCLK == 8000000)
    	    {   CAN0BTR0 = 0xC1;               	//250 kbit/s at fOSCCLK = 8MHz, TBit = 16 Tq, Prescale=2, Sync Jump Width= 4
    	    } else
            {   CAN0BTR0 = 0xC0;         	//250 kbit/s at fOSCCLK = 4MHz, TBit = 16 Tq, Prescale=1, Sync Jump Width= 4
            }
    	    CAN0BTR1 = 0x58;			//Time segment 1 = 9Tq, Time Segment 2 = 6Tq
    	    break;
    	case CAN_100:
    	    if (OSCCLK == 8000000)
    	    {   CAN0BTR0 = 0xC3;             	//100 kbit/s at fOSCCLK = 8MHz, TBit = 20 Tq, Prescale=4, Sync Jump Width= 4
    	    } else
            {   CAN0BTR0 = 0xC1;            	//100 kbit/s at fOSCCLK = 4MHz, TBit = 20 Tq, Prescale=2, Sync Jump Width= 4
            }
    	    CAN0BTR1 = 0x7A;			//Time segment 1 =11Tq, Time Segment 2 = 8Tq
    	    break;
    }
    CAN0IDAC  = 0x20;                       	//8bit acceptance filter

    CAN0IDAR0 = 0x00;                       	//Specify, which CAN identifiers shall be accepted (only 8 most significant bits used)
    CAN0IDAR1 = 0x01;			    	//Details see MSCAN user guide chapter 3.3.2
    CAN0IDAR2 = 0x02;
    CAN0IDAR3 = 0x03;
    CAN0IDAR4 = 0x04;
    CAN0IDAR5 = 0x05;
    CAN0IDAR6 = 0x06;
    CAN0IDAR7 = 0x07;

    CAN0IDMR0 = 0xFF;                       	//Specify, which of the CAN identifier bits shall be ignored
    CAN0IDMR1 = 0x00;                     	//(Here the filter is set to ignore all bits, thus receiving any CAN message)
    CAN0IDMR2 = 0x00;
    CAN0IDMR3 = 0x00;
    CAN0IDMR4 = 0x00;
    CAN0IDMR5 = 0x00;
    CAN0IDMR6 = 0x00;
    CAN0IDMR7 = 0x00;

    CAN0CTL0 = CAN0CTL0 & 0xFE;           	//End initialization
    while ((CAN0CTL1 & 0x01) == 1)  {};   	//Wait for end of initalization

    CAN0RFLG_RXF = 1;       			//Clear receiver flags

#else						//Simulate CAN over serial line SCI0
    SciInitEx(CANviaSCIx, 115200, FALSE);
    SciRegisterCallback(CANviaSCIx, CanViaSciCallback);
    SciEnableInterrupts(CANviaSCIx, TRUE, FALSE);//Enable receive interrupt
    
    canRXbufferFull = FALSE;
    canTXbufferFree = TRUE;
    canRXintEnable  = FALSE;
    canTXintEnable  = FALSE;
    canTXindex      = -1;
    canRXindex      = -1;
#endif   
}


void CanSetAcceptanceFilter(CANIDFILTER filterType, uint8 filterIndex, uint8 acceptMask, uint8 ignoreMask)
{   
#ifdef _HCS12_SERIALMON                 	//CAN Hardware only available on the Dragon12 board
    CAN0CTL0 = CAN0CTL0 | 0x01;	          	//Request initialization
    while ((CAN0CTL1 & 0x01) ==0) {};    	//Wait for acknowledge

    switch (filterType)
    {   case CAN_8BIT_FILTER:
            CAN0IDAC  = 0x20;                   //8 x 8bit CAN ID acceptance filters (filter upper 8bit of CAN ID)
            switch (filterIndex)
            {   case 0: CAN0IDAR0 = (uint8) acceptMask; CAN0IDMR0 = (uint8) ignoreMask; break;
                case 1: CAN0IDAR1 = (uint8) acceptMask; CAN0IDMR1 = (uint8) ignoreMask; break;
                case 2: CAN0IDAR2 = (uint8) acceptMask; CAN0IDMR2 = (uint8) ignoreMask; break;
                case 3: CAN0IDAR3 = (uint8) acceptMask; CAN0IDMR3 = (uint8) ignoreMask; break;
                case 4: CAN0IDAR4 = (uint8) acceptMask; CAN0IDMR4 = (uint8) ignoreMask; break;
                case 5: CAN0IDAR5 = (uint8) acceptMask; CAN0IDMR5 = (uint8) ignoreMask; break;
                case 6: CAN0IDAR6 = (uint8) acceptMask; CAN0IDMR6 = (uint8) ignoreMask; break;
                case 7: CAN0IDAR7 = (uint8) acceptMask; CAN0IDMR7 = (uint8) ignoreMask; break;
            }
            break;
    }

    CAN0CTL0 = CAN0CTL0 & 0xFE;           	//End initialization
    while ((CAN0CTL1 & 0x01) == 1)  {};   	//Wait for end of initalization
#else

#endif
};

void CanEnableRxInterrupt(bool enable)
{   
#ifdef _HCS12_SERIALMON                 	//CAN Hardware only available on the Dragon12 board
    CAN0RFLG_RXF = 1;       			//Clear receiver flags
    if (enable)
    	CAN0RIER = CAN0RIER | 0x01;           	//Receive interrupt enable
    else
    	CAN0RIER = CAN0RIER & ~0x01;           	//Receive interrupt disable
#else
    canRXintEnable = enable;
#endif
}

void CanEnableTxInterrupt(bool enable)
{   
#ifdef _HCS12_SERIALMON                 	//CAN Hardware only available on the Dragon12 board
    //CAN0TFLG_TXE = 0x07;     			 //Clear transmit flags
    if (enable)
    	CAN0TIER = CAN0TIER | 0x07;           	//Transmit interrupt enable
    else
    	CAN0TIER = CAN0TIER & ~0x07;           	//Transmit interrupt disable
#else
        canTXintEnable = enable;
#endif
}

#ifndef USER_CAN_ISR
//void (*UserCanRxCallbackFcn)(void) = NULL;
//void (*UserCanTxCallbackFcn)(void) = NULL;

void interrupt 38 CanReceiveISR(void)
{   if (UserCanRxCallbackFcn)
    {   UserCanRxCallbackFcn();
    }
    CAN0RFLG = CAN0RFLG | 0x01;       	        // Clear receive flag
}

void interrupt 39 CanTransmitISR(void)
{   if (UserCanTxCallbackFcn)
    {   UserCanTxCallbackFcn();
    }
//???    CAN0TFLG = CAN0TFLG | 0x07;       	        // Clear transmit flag
}

void CanRegisterRxCallback(void userCanRxCallbackFcn(void))
{   UserCanRxCallbackFcn = userCanRxCallbackFcn;
}

void CanRegisterTxCallback(void userCanTxCallbackFcn(void))
{   UserCanTxCallbackFcn = userCanTxCallbackFcn;
}

#endif


//***** SPI *******************************************************************
void SpiInit(bool master, int8 x, int8 y, bool lsbFirst, bool useSSout, int8 clkMode)
{
#ifdef EVB9S12XF512E
    PTS =  PTS  & 0x0F;
    DDRS = DDRS | 0xF0;
    RDRS = DDRS | 0xF0;
#endif
    SPI0BR = ((x & 0x07) << 4) | (y & 0x07);    //Bit rate is BUSCLK (typ. 24MHz) / {(x+1)*2^(y+1)}

    SPI0CR1 = 0x40 | (master ? 0x10 : 0) | ((clkMode & 0x03)<<2) | (lsbFirst ? 0x01 : 0) | (useSSout ? 0x02 : 0);
    SPI0CR2 = (useSSout ? 0x10 : 0);

    (void) SPI0SR;                              //Read the status register to clear
#ifdef DRAGON12
    (void) SPI0DR;                              //Read the data register to clear
#endif
#ifdef EVB9S12XF512E
    (void) SPI0DRL;                             //Read the data register to clear
#endif
}

#pragma INLINE
bool SpiInStatus(void)
{   return SPI0SR & SPI0SR_SPIF_MASK;
}

#pragma INLINE
bool SpiOutStatus(void)
{   return SPI0SR & SPI0SR_SPTEF_MASK;
}

#pragma INLINE
void SpiPutc(int8 c)
{   while (!SpiOutStatus());                    //Wait till data register free
#ifdef DRAGON12
    SPI0DR = c;                          	    //Write byte to data register
#endif
#ifdef EVB9S12XF512E
    SPI0DRL = c;                          	    //Write byte to data register
#endif
}

#pragma INLINE
int8 SpiGetc(void)
{   while (!SpiInStatus());                     //Wait for data in data register
#ifdef DRAGON12
    return (int8) SPI0DR;                       //Read data register
#endif
#ifdef EVB9S12XF512E
    return (int8) SPI0DRL;                      //Read data register
#endif
}

void SpiPutData(int8 *pData, uint16 n)
{  uint16 i;
   for (i=0; i < n; i++)
   {   SpiPutc(pData[i]);
   }
}

uint16 SpiGetData(int8 *pData, int16 nmax, int8 eot)
{  uint16 i;

   for (i=0; i < nmax; )
   {    pData[i++] = SpiGetc();
        if (pData[i-1]==eot)
            break;
   }
   return i;
}

void SpiEnableInterrupt(bool enableRXinterrupt, bool enableTXinterrupt)
{   if (enableRXinterrupt)
    {  SPI0CR1 = SPI0CR1 | SPI0CR1_SPIE_MASK;
    } else
    {  SPI0CR1 = SPI0CR1 &  ~ (uint8) SPI0CR1_SPIE_MASK;
    }
    if (enableTXinterrupt)
    {  SPI0CR1 = SPI0CR1 | SPI0CR1_SPTIE_MASK;
    } else
    {  SPI0CR1 = SPI0CR1 & ~ (uint8) SPI0CR1_SPTIE_MASK;
    }
}


#ifndef USER_SPI_ISR
void (*UserSpiCallbackFcn)(void) = NULL;

void SpiRegisterCallback(void userSpiCallback(void))
{   UserSpiCallbackFcn = userSpiCallback;
}

interrupt 19 void spiISR (void)
{   if (UserSpiCallbackFcn)
        UserSpiCallbackFcn();
}
#endif //USER_SPI_ISR



//***** I2C ********************************************************************
void I2cInit(uint8 localSlaveAddress, I2C_BITRATE bitrate)
{   if (bitrate==I2C_400_kHz)
    {	IBFD = 0x45;
    } else if (bitrate==I2C_200_kHz)
    {	IBFD = 0x85;
    } else if (bitrate==I2C_100_kHz)
    {	IBFD = 0x1F;
    }
    IBAD = localSlaveAddress;
    IBCR = IBCR_IBEN_MASK;	        			//I2C module on, interrupts off, slave mode, ACK on
    IBSR = IBSR | IBSR_IBIF_MASK;			    	//Clear interrupt flag
}

void I2cMasterSend(uint8 slaveAddress, uint8 *data, uint8 dataLen)
{   uint8 i;

    while (IBSR_IBB);                    // Wait until bus is free
    IBCR_TX_RX = 1;                      // Switch to transmit mode
    IBCR_MS_SL = 1;                      // Switch to master mode, generates a START condition on the bus
 // IBCR_TXAK  = 0;                      // Generate ACK bits, when receiving data

    IBDR = slaveAddress & 0xFE;          // Send address to I2C slaves (bit 0 indicates Master Send)
    while (!IBSR_IBIF);                  // Wait until address transmission is complete
    IBSR_IBIF = 1;                       // Clear transmission complete flag
    while(IBSR_RXAK);                    // Wait for the slave's ACK --> NOTE: Program hangs, if slave does not respond ???

    for (i=0; i < dataLen; i++)
    {   IBDR = data[i];                  // Sending a data byte
        while (!IBSR_IBIF);              // Wait until byte transmission is complete
        IBSR_IBIF = 1;                   // Clear transmission complete flag
        while(IBSR_RXAK);                // Wait for the slave's ACK --> NOTE: Program hangs, if slave does not respond ???
    }

    IBCR_MS_SL = 0;                      // Switch to slave mode, generates a STOP condition on the bus
    IBCR_TX_RX = 0;                      // Switch to receive mode

    for (i=0;i<120;i++){}                // Delay between before next START condition min. 5µs --> NOTE: Loop not calibrated
}

void I2cMasterRead(uint8 slaveAddress, uint8 *data, uint8 dataLen)
{   uint8 dummy;
    uint8 i;

    while (IBSR_IBB);                    // Wait until bus is free
    IBCR_TX_RX = 1;                      // Switch to transmit mode (for address transmission)
    IBCR_MS_SL = 1;                      // Switch to master mode, generates a START condition on the bus
    IBCR_TXAK  = 0;                      // Generate ACK bits, when receiving data

    IBDR = slaveAddress | 0x01;          // Send address to I2C slaves (bit 0 indicates Master Read)
    while (!IBSR_IBIF);                  // Wait until address transmission is complete
    IBSR_IBIF = 1;                       // Clear transmission complete flag
    while(IBSR_RXAK);                    // Wait for the slave's ACK --> NOTE: Program hangs, if slave does not respond ???

    IBCR_TX_RX = 0;                      // Switch to read mode
    IBCR_TXAK  = 0;                      // Generate ACK bits, when receiving data

    for(i=dataLen-1; i!=0; i--)
    //for(i=0; i < dataLen - 1;i++)        // Read data byte by byte (not including the last byte)
    {   dummy = IBDR;                    // Dummy read -> generates I2C clock signal to read a byte
        while(!IBSR_IBIF);               // Wait until byte transmission is complete
        IBSR_IBIF = 1;                   // Clear transmission complete flag
        data[i]= IBDR;                   // Save data byte
    }
    IBCR_TXAK  = 1;                      // Before reading the last byte turn off ACK (signals end of read to slave)
    dummy = IBDR;                        // Dummy read -> generates I2C clock signal to read a byte
    while(!IBSR_IBIF);                   // Wait until byte is complete
    IBSR_IBIF = 1;                       // Clear transmission complete flag

    IBCR_MS_SL = 0;                      // Switch to slave mode, generates a STOP condition on the bus
    data[i]= IBDR;                       // Save last data byte

    for (i=0;i<120;i++){}                // Delay between before next START condition min. 5µs --> NOTE: Loop not calibrated
}

//***** FlexRay ****************************************************************
#ifdef EVB9S12XF512E
void FrPllInit(void)
{   CGMSYNR  = 0x49;
//  CGMSYN_SYN = 0x09;                          //Fvco = 2*OSCCLK*(SYNR+1)/(REFDV+1) = 8MHz*(9+1)/1 = 80MHz -> VCOFRQ = 01b
//  CGMSYN_VCOFRQ1 = 0;
//  CGMSYN_VCOFRQ0 = 1;

    CGMREFDV = 0x40;
//  CGMREF_REFDIV = 0x00;
//  CGMREF_REFFRQ1 = 0;                         //Fref = OSCCLK/(REFDIV+1) = 4MHz/1 = 4MHz -> REFFRQ = 01b
//  CGMREF_REFFRQ0 = 1;

    CGMCTL_DIV2 = 0;                            //DIV2=0 -> IPLL = Fvco
    CGMCTL_PLLON=1;                             //IPLL enabled

    while(!CGMFLG_LOCK);                        //Wait until the IPLL has settled
}

void FrInit(void)
{   FrPllInit();                                //Initialize FlexRay PLL for 10 Mbit/s

    DDRH = DDRH | 0x88;                         //Enable FlexRay Transceivers on PTH.3 and PTH.7
    PERH = 0;                                   //No pullups
    RDRH = 0xFF;                                //Reduced drive
    PTH  = 0x88;

    PTJ = 0;
    DDRJ = DDRJ & (~0x60);                      //Enable RXEN inputs on PTJ.5 and PTJ.6
    PERJ = 0;                                   //No pullups
}
#endif //EVB9S12XF512E

//***** Memory ****************************************************************
uint8 MemSetPage(uint8 newPage)
{   uint8 oldPage = PPAGE;
    PPAGE = newPage;
    return oldPage;
}

void MemFlashRomInitProgramming(void)           //Setting for OSCCLK=4MHz (Dragon12)
{   if (FCLKDIV & 0x80)
    {   return;                                 //Already initialized
    }

    if (OSCCLK == 4000000)
    {   FCLKDIV = 0x13;
    } else if (OSCCLK == 8000000)               //Setting for OSCCLK=8MHz (Dragon12Plus)
    {   FCLKDIV = 0x27;
    }
}

int8 MemFlashRomProgram(uint8 flashBlock, uint8 page, uint16 startAddress, uint16 data[], uint16 dataLength)
{   uint16 *pAddress = (uint16*) startAddress;
    int8 rc = 0;
    uint16 i;

    if ((startAddress&0x0001)==1)
    {   return -1;                              //Make sure, address is word aligned and within EEPROM range
    }

    while ((FSTAT & 0x80)==0);                  //Wait, till command buffer is free

    FCNFG = (FCNFG & ~0x03)| (flashBlock & 0x03); //Select Flash block
    MemSetPage(page);                       //Select memory page

    for (i=0; i < dataLength; i++)
    {   FSTAT = 0x30;                           //Reset error flags

        *pAddress = data[i];                    //Set address and data word
        FCMD = 0x20;                            //Word programming command
        FSTAT = FSTAT | 0x80;                   //Start the word programming operation by resetting CBEIF

        if (FSTAT & 0x30)                       //Check for errors after programming operation
        {   FSTAT = 0x30;                       //Reset error flags
            return rc = -1;
        }
        while ((FSTAT & 0x80)==0);              //Wait, till command buffer is free, i.e. CBEIF set
        pAddress++;
    }
    while ((FSTAT & 0x40)==0);                  //Wait, till all progamming completed, i.e. CCIF set
    return rc;
}

int8 MemFlashRomSectorErase(uint8 flashBlock, uint8 page, uint16 startAddress)
{   uint16 *pAddress = (uint16*) startAddress;  //Arbitrary address within EEPROM block
    int8 rc = 0;

    while ((FSTAT & 0x80)==0);              //Wait, till command buffer is free

    FCNFG = (FCNFG & ~0x03)| (flashBlock & 0x03); //Select Flash block
    MemSetPage(page);                       //Select memory page

    FSTAT = 0x30;                           //Reset error flags

    *pAddress = 0;                          //Set address and data word
    FCMD  = 0x40;                           //Mass erase command
    FSTAT = ESTAT | 0x80;                   //Start the word programming operation by resetting CBEIF

    if (FSTAT & 0x30)                       //Check for errors after programming operation
    {   FSTAT = 0x30;                       //Reset error flags
        return rc = -1;
    }
    while ((FSTAT & 0x40)==0);              //Wait, till all progamming completed, i.e. CCIF set

    return rc;
}
