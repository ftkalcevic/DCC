#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/delay.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/signal.h>
#include <avr/wdt.h>
#include <math.h> 

#include "../../AVRCommon/common.h"
#include "../../AVRCommon/Serial.h"

const int8_t MILLISECOND_COUNTER = 256-125;

//#define DEBUG

//////////////////////////////////////////////////
// Port Config
//////////////////////////////////////////////////
#define BEEPER_DDR      DDRD
#define BEEPER_PORT     PORTD
#define BEEPER_BIT      PD5

#define LED_NO_SIGNAL_DDR      DDRD
#define LED_NO_SIGNAL_PORT     PORTD
#define LED_NO_SIGNAL_BIT      PD7

#define LED_SIGNAL_DDR      DDRD
#define LED_SIGNAL_PORT     PORTD
#define LED_SIGNAL_BIT      PD6

#define LED_THERMAL_OVERLOAD_DDR      DDRB
#define LED_THERMAL_OVERLOAD_PORT     PORTB
#define LED_THERMAL_OVERLOAD_BIT      PB0

#define LED_CURRENT_OVERLOAD_DDR      DDRB
#define LED_CURRENT_OVERLOAD_PORT     PORTB
#define LED_CURRENT_OVERLOAD_BIT      PB1

#define PWM_OUT_DDR      DDRD
#define PWM_OUT_PORT     PORTD
#define PWM_OUT_BIT      PD4

#define DCC_IN_DDR      DDRD
#define DCC_IN_PORT     PORTD
#define DCC_IN_PIN      PIND
#define DCC_IN_BIT      PD2

#define THERMAL_OVEROAD_IN_DDR      DDRD
#define THERMAL_OVEROAD_IN_PORT     PORTD
#define THERMAL_OVEROAD_IN_PIN      PIND
#define THERMAL_OVEROAD_IN_BIT      PD3

#define OVERLOAD_RESET_DDR      DDRC
#define OVERLOAD_RESET_PORT     PORTC
#define OVERLOAD_RESET_PIN      PINC
#define OVERLOAD_RESET_BIT      PC4

#define AUTO_RESET_DDR      DDRC
#define AUTO_RESET_PORT     PORTC
#define AUTO_RESET_PIN      PINC
#define AUTO_RESET_BIT      PC3

#define CURRENT_SENSE_DDR      DDRD
#define CURRENT_SENSE_PORT     PORTD
#define CURRENT_SENSE_PIN      PINC
#define CURRENT_SENSE_BIT      PC5
#define CURRENT_SENSE_ADC      5


#define MAX_OVERFLOW_CURRENT        800     // ~ 2.5Amps
#define CURRENT_OVERFLOW_PERIOD     2       // number ADC cycles - if current is high for this time, overflow
#define AUTO_RESET_RETRY_PERIOD     500     // Retry signal after 500 ms

static volatile uint8_t nCurrentOverflowCount;
static volatile uint16_t nAutoResetCounter;

static volatile uint8_t nBeeper;
static volatile uint16_t nLastSignal;
static volatile uint16_t nLastSignalCount;
static volatile uint16_t nLastADC;
static volatile bool bNewSample;
static volatile uint16_t nMilliSecCount;
static volatile uint16_t nSecCount;
static uint16_t nADCSum;
static uint8_t nADCCount;

SIGNAL (SIG_OVERFLOW0)	// Timer0 overflow interupt.
{
    TCNT0 = MILLISECOND_COUNTER;

    nMilliSecCount++;
    if ( nMilliSecCount == 1000 )
    {
        nSecCount++;
        nMilliSecCount = 0;

        nLastSignal = nLastSignalCount;
        nLastSignalCount = 0;
    }

    if ( nBeeper != 0 )
    {
        nBeeper--;
        if ( nBeeper == 0 )
        {
            BEEPER_PORT &= ~_BV(BEEPER_BIT);
        }
    }

    if ( nCurrentOverflowCount > 1 )
    {
        nCurrentOverflowCount--;
    }

    if ( nAutoResetCounter > 1 )
    {
        nAutoResetCounter--;
    }
}

SIGNAL (SIG_INTERRUPT0)	// External interupt 0.
{
    nLastSignalCount++;
}


SIGNAL (SIG_ADC)	// ADC conversion complete.
{
    //nADCSum += ADCW;
    //nADCCount++;
    //if ( nADCCount == 8 )
    //{
    //    nLastADC = nADCSum/8;
    //    nADCCount=0;
    //    nADCSum = 0;
    //    bNewSample = true;
    //}
    nLastADC = ADCW;
    bNewSample = true;
}

void SetDCCSignal( bool bHasSignal )
{
    if ( bHasSignal )
    {
        LED_NO_SIGNAL_PORT &= ~_BV(LED_NO_SIGNAL_BIT);
        LED_SIGNAL_PORT |= _BV(LED_SIGNAL_BIT);
    }
    else
    {
        LED_NO_SIGNAL_PORT |= _BV(LED_NO_SIGNAL_BIT); 
        LED_SIGNAL_PORT &= ~_BV(LED_SIGNAL_BIT);
    }
}

void SetPWM( bool bEnable )
{
    if ( bEnable )
    {
        PWM_OUT_PORT |= _BV(PWM_OUT_BIT);
    }
    else
    {
        PWM_OUT_PORT &= ~_BV(PWM_OUT_BIT);
    }
}


void SetThermalOverload( bool bOverloaded )
{
    if ( bOverloaded )
    {
        LED_THERMAL_OVERLOAD_PORT |= _BV(LED_THERMAL_OVERLOAD_BIT);
    }
    else
    {
        LED_THERMAL_OVERLOAD_PORT &= ~_BV(LED_THERMAL_OVERLOAD_BIT);
    }
}

void SetCurrentOverload( bool bOverloaded )
{
    if ( bOverloaded )
    {
        LED_CURRENT_OVERLOAD_PORT |= _BV(LED_CURRENT_OVERLOAD_BIT);
    }
    else
    {
        LED_CURRENT_OVERLOAD_PORT &= ~_BV(LED_CURRENT_OVERLOAD_BIT);
    }
}

bool IsAutoReset( void )
{
    return (AUTO_RESET_PIN & _BV(AUTO_RESET_BIT)) != 0;
}

bool IsReset( void )
{
    return (OVERLOAD_RESET_PIN & _BV(OVERLOAD_RESET_BIT)) == 0;
}

void ioinit( void )
{
	/////////////////////////
	//// Set IO registers
	/////////////////////////

	DDRB = 0;
	DDRC = 0;
	DDRD = 0;

    BEEPER_DDR |= OUTPUT(BEEPER_BIT);
    BEEPER_PORT &= ~_BV(BEEPER_BIT);

    LED_NO_SIGNAL_DDR |= OUTPUT(LED_NO_SIGNAL_BIT);
    LED_NO_SIGNAL_PORT &= ~_BV(LED_NO_SIGNAL_BIT);

    LED_SIGNAL_DDR |= OUTPUT(LED_SIGNAL_BIT);
    LED_SIGNAL_PORT &= ~_BV(LED_SIGNAL_BIT);

    LED_THERMAL_OVERLOAD_DDR |= OUTPUT(LED_THERMAL_OVERLOAD_BIT);
    LED_THERMAL_OVERLOAD_PORT &= ~_BV(LED_THERMAL_OVERLOAD_BIT);

    LED_CURRENT_OVERLOAD_DDR |= OUTPUT(LED_CURRENT_OVERLOAD_BIT);
    LED_CURRENT_OVERLOAD_PORT &= ~_BV(LED_CURRENT_OVERLOAD_BIT);

    PWM_OUT_DDR |= OUTPUT(PWM_OUT_BIT);
    PWM_OUT_PORT &= ~_BV(PWM_OUT_BIT);

    DCC_IN_DDR |= INPUT(DCC_IN_BIT);
    DCC_IN_PORT |= _BV(DCC_IN_BIT);    // Hi-Z
    //DCC_IN_DDR |= OUTPUT(DCC_IN_BIT);
    //DCC_IN_PORT |= _BV(DCC_IN_BIT);

    OVERLOAD_RESET_DDR |= INPUT(OVERLOAD_RESET_BIT);
    OVERLOAD_RESET_PORT |= _BV(OVERLOAD_RESET_BIT);    // Pull-up

    AUTO_RESET_DDR |= INPUT(AUTO_RESET_BIT);
    AUTO_RESET_PORT |= _BV(AUTO_RESET_BIT);    // Pull-up

    CURRENT_SENSE_DDR |= INPUT(CURRENT_SENSE_BIT);
    CURRENT_SENSE_PORT |= _BV(CURRENT_SENSE_BIT);    // Pull-up

    THERMAL_OVEROAD_IN_DDR |= INPUT(THERMAL_OVEROAD_IN_BIT);
    THERMAL_OVEROAD_IN_PORT &= ~_BV(THERMAL_OVEROAD_IN_BIT);    // Hi-Z

    //////////////////////////
    ////// ADC Set up free run conversion of current sense port, with interrupt notification
    /////////////////////////
    ADMUX = CURRENT_SENSE_ADC;
    ADCSR = _BV(ADEN) | _BV(ADFR) | _BV(ADIE) | _BV(ADPS2);     // Prescale /16 => 937 S/s

    //////////////////////////
    ////// External interrupt
    /////////////////////////
    MCUCR = _BV(ISC00);
    GIMSK = _BV(INT0);

	///////////////////////////
	////// Set Timer
	///////////////////////////

	//// msec timer
    TCCR0 = _BV(CS01) | _BV(CS00);	// CLK/64
	TCNT0 = MILLISECOND_COUNTER;
	TIMSK |= _BV(TOIE0);

}

void Beep( uint8_t nLen )
{
    cli();
    nBeeper = nLen;
    sei();
    BEEPER_PORT |= _BV(BEEPER_BIT);
}

int main (void)
{
    nMilliSecCount = 0;
    nSecCount = 0;
    bNewSample = false;
    nADCSum=0;
    nADCCount=0;
    nCurrentOverflowCount = 0;
    nAutoResetCounter = 0;
    ioinit();

	sei();
    Beep( 1 );

#ifdef DEBUG
    InitRS232( 12, true, false );       // 38.4k
	RS232Send_P( PSTR("Booster intialised\r\n")  );
#endif

    // Sleep for 1 second
    for ( int i = 0; i < 1000; i++ )
    {
        _delay_us(CPU_CLK,1000);
    }

#ifdef DEBUG
	RS232Send_P( PSTR("Starting\r\n")  );
#endif
    // Assume No DCC signal
    bool bDCCStopped = true;
    SetDCCSignal( false );

    // Assume Thermal Overload
    bool bThermalOverloaded = false;
    SetThermalOverload( bThermalOverloaded );

    bool bCurrentOverload = false;
    SetCurrentOverload( bCurrentOverload );

    ADCSR |= _BV(ADSC); // Start ADC conversion
    int16_t nLastSec = nSecCount;

    int nLastMS = 0;
    for (;;)
    {
        // Update sounds
        cli();
        uint16_t nMS = nMilliSecCount;
        sei();
        nMS >>= 2;
        if ( nMS != nLastMS )
        {
            if ( bDCCStopped )
            {
                if ( (nSecCount & 0x3) == 0 )
                {
                    if ( nMS == 0 || nMS == 10 || nMS == 20  || nMS == 30   || nMS == 40 )
                    {
                        Beep( 10 );
                    }
                }
            }
            if ( bThermalOverloaded )
            {
                if ( (nSecCount & 0x3) == 2 )
                {
                    if ( nMS == 0 || nMS == 50 || nMS == 100  )
                    {
                        Beep( 50 );
                    }
                }
            }
            if ( bCurrentOverload )
            {
                if ( (nSecCount & 0x1) == 1 )
                {
                    if ( nMS < 5  )
                    {
                        Beep( 255 );
                    }
                }
            }
            nLastMS = nMS;
        }

        cli();
        int16_t nSignal = nLastSignal;
        sei();

        // Watch for DCC signal
        if ( nSignal == 0 && ! bDCCStopped )
        {
            bDCCStopped = true;
            SetPWM( false );
            SetDCCSignal( false );
            Beep( 50 );
        }
        else if ( nSignal != 0 && bDCCStopped )
        {
            bDCCStopped = false;
            SetPWM( true );
            SetDCCSignal( true );
        }

        // Watch for thermal overload
        bool bState = (THERMAL_OVEROAD_IN_PIN & _BV(THERMAL_OVEROAD_IN_BIT)) != 0;
        if ( !bState && !bThermalOverloaded )    // Overload when low
        {
            bThermalOverloaded = true;
            SetThermalOverload( true );
            SetPWM( false );
        }
        else if ( bState && bThermalOverloaded )    // Overload when low
        {
            bThermalOverloaded = false;
            SetThermalOverload( false );
            SetPWM( true );
        }

        // Check current overload
        if ( bNewSample )
        {
            int16_t nNextADC = nLastADC;
            bNewSample = false;

            // if the current is overflown, we wait n msec before shutting down.
            if ( nNextADC > MAX_OVERFLOW_CURRENT )
            {
                if ( !bCurrentOverload )
                {
                    nCurrentOverflowCount++;
                    if ( nCurrentOverflowCount >= CURRENT_OVERFLOW_PERIOD )
                    {
                        // End of count down.  Shutdown PWM.
                        SetPWM( false );
                        SetCurrentOverload( true );
                        bCurrentOverload = true;

                        Beep(250);
                        if ( IsAutoReset() )
                        {
                            nAutoResetCounter = AUTO_RESET_RETRY_PERIOD;
                        }
                    }
                }
            }
            else
            {
                nCurrentOverflowCount = 0;
            }
        }

        if ( bCurrentOverload )
        {
            if ( IsAutoReset() )
            {
                if ( nAutoResetCounter == 1 )
                {
#ifdef DEBUG        
                    RS232Send_P( PSTR("Auto reset")  );
#endif
                    nAutoResetCounter = 0;
                    SetCurrentOverload( false );
                    bCurrentOverload = false;
                    SetPWM( true );
                }
            }
            else
            {
                if ( IsReset() )
                {
#ifdef DEBUG        
                    RS232Send_P( PSTR("Manual reset")  );
#endif
                    SetCurrentOverload( false );
                    bCurrentOverload = false;
                    SetPWM( true );
                }
            }
        }

#ifdef DEBUG        
        if ( nSecCount != nLastSec )
        {
            cli();
            int16_t nNextADC = nLastADC;
            int16_t nNextSignal = nLastSignal;
            sei();
	        RS232Send_P( PSTR("Last ADC = ")  );
	        RS232SendInt( nNextADC );
	        RS232Send_P( PSTR(", DCC Signal = ")  );
	        RS232SendInt( nNextSignal );
	        RS232Send_P( PSTR(", Reset = ")  );
	        RS232SendInt( IsReset() );
	        RS232Send_P( PSTR(", AutoReset = ")  );
	        RS232SendInt( IsAutoReset() );
	        RS232SendCRLF();
            nLastSec = nSecCount;
        }
#endif
        //wdt_reset();
    }
}



/*
    DCC Signal
        - watch for real signal, 1s and 0s
            - or just changes on the line - noise?  Pull up?
        - if no signal for n msec, stop
        - if signal for n msec, start
            - stop
                - led
                - no pwm
                - beep
    Thermal Overload
        - stop?
            - led
            - pwm
            - beep
        - start timer
            - resume equivalent time after we have been overloaded
    Current Overload
        - Current high > 5 ms
            - stop PWM
            - light
            - beep
        - if auto mode,
            - wait 5 seconds, try again
        - if manual mode
            - try again after button push

    Each condition has a different beep
    When in a condition, constant beeping - every 5 seconds

0000000000111111111122222222223333333333444444444455555555556666666666
0123456789012345678901234567890123456789012345678901234567890123456789
00000000001111111111000000000011111111110000000000111111111100000000001111111111
1010100000000000000000000000000000000000101010000000000000000000000000
00000000000000000000111110000000000000000000000000000000000000000000001111100000
*/
