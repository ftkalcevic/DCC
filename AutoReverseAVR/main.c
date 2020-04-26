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

const int8_t MILLISECOND_COUNTER = 256-62;      // ~ 1kHz

//#define DEBUG

//////////////////////////////////////////////////
// Port Config
//////////////////////////////////////////////////
#define RELAY_PORT      PORTC
#define SENSE_PIN       PIND
#define WEST_LED_PORT   PORTD
#define EAST_LED_PORT   PORTB
#define CENTER_LED_PORT PORTB

#define WEST_POWER      PC1
#define WEST_SENSE      PC0
#define EAST_POWER      PC5
#define EAST_SENSE      PC4
#define CENTER_SET      PC3
#define CENTER_RESET    PC2

#define WEST_LED        PD7
#define EAST_LED        PB0
#define CENTER_ON_LED   PB1
#define CENTER_OFF_LED  PB2

#define SENSE_WEST      PD2
#define SENSE_EAST      PD3


#define RELAY_ON_TIME  5      // 5 ms switch time.

static volatile byte nRelayTimer;
static volatile uint16_t nMilliSecCount;
static volatile byte nSecCount;

SIGNAL (SIG_OVERFLOW0)	// Timer0 overflow interupt.
{
    TCNT0 = MILLISECOND_COUNTER;

    nMilliSecCount++;
    if ( nMilliSecCount == 1000 )
    {
        nSecCount++;
        nMilliSecCount = 0;
    }

    if ( nRelayTimer > 0 )
    {
        nRelayTimer--;
        if ( nRelayTimer == 0 )
        {
            RELAY_PORT = 0;
        }
    }
}

void ioinit( void )
{
    /////////////////////////
    //// Set IO registers
    /////////////////////////
    DDRC = OUTPUT(WEST_POWER) | OUTPUT(WEST_SENSE) | OUTPUT(EAST_POWER) | OUTPUT(EAST_SENSE) | OUTPUT(CENTER_SET) | OUTPUT(CENTER_RESET);
    DDRD = INPUT(SENSE_WEST) | INPUT(SENSE_EAST) | OUTPUT(WEST_LED);
    DDRB = OUTPUT(EAST_LED) | OUTPUT(CENTER_ON_LED) | OUTPUT(CENTER_OFF_LED);

    ///////////////////////////
    ////// Set Timer
    ///////////////////////////

    //// msec timer
    TCCR0 = _BV(CS01) | _BV(CS00);	// CLK/64
    TCNT0 = MILLISECOND_COUNTER;
    TIMSK |= _BV(TOIE0);

}

int main (void)
{
    nMilliSecCount = 0;
    nSecCount = 0;
    nRelayTimer = 0;
    ioinit();

    sei();

    // Detection on the west portal.  Switch West on, East Off, Centre On.
    RELAY_PORT = _BV(WEST_SENSE) | _BV(EAST_SENSE) | _BV(CENTER_SET);
    WEST_LED_PORT &= ~_BV(WEST_LED);
    EAST_LED_PORT &= ~_BV(EAST_LED);
    CENTER_LED_PORT = (CENTER_LED_PORT & ~(_BV(CENTER_ON_LED) | _BV(CENTER_OFF_LED))) | _BV(CENTER_ON_LED);
    nRelayTimer = RELAY_ON_TIME;

    bool bEastSense = true;
    bool bWestSense = true;

    for (;;)
    {
        // Check the sense ports
        if ( bWestSense && !(SENSE_PIN & _BV(SENSE_WEST)) )
        {
            // Detection on the west portal.  Switch West on, East Off, Centre On.
            RELAY_PORT = _BV(WEST_POWER) | _BV(EAST_SENSE) | _BV(CENTER_SET);
            bWestSense = false;
            bEastSense = true;
            WEST_LED_PORT |= _BV(WEST_LED);
            EAST_LED_PORT &= ~_BV(EAST_LED);
            CENTER_LED_PORT = (CENTER_LED_PORT & ~(_BV(CENTER_ON_LED) | _BV(CENTER_OFF_LED))) | _BV(CENTER_ON_LED);
            nRelayTimer = RELAY_ON_TIME;
        }
        
        if ( bEastSense && !(SENSE_PIN & _BV(SENSE_EAST)) )
        {
            // Detection on the east portal.  Switch West off, East On, Centre Off.
            RELAY_PORT = _BV(WEST_SENSE) | _BV(EAST_POWER) | _BV(CENTER_RESET);
            bWestSense = true;
            bEastSense = false;
            WEST_LED_PORT &= ~_BV(WEST_LED);
            EAST_LED_PORT |= _BV(EAST_LED);
            CENTER_LED_PORT = (CENTER_LED_PORT & ~(_BV(CENTER_ON_LED) | _BV(CENTER_OFF_LED))) | _BV(CENTER_OFF_LED);
            nRelayTimer = RELAY_ON_TIME;
        }
    }
}
