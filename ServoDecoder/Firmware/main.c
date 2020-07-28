// DCCServoDecoder, DCC Servor Driver Accessory for driving turnouts.
// Copyright (C) 2008, Frank Tkalcevic, www.franksworkshop.com

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <math.h> 

#include "common.h" 
#include "Serial.h"

#define MILLISECOND_COUNTER (256-78)

//#define DEBUG

//////////////////////////////////////////////////
// Port Config
//////////////////////////////////////////////////
static volatile uint8_t nSec;
#ifdef DEBUG
static volatile uint16_t nErrorBitTime;
static volatile byte nErrorTime;
static volatile uint16_t nErrorBufferFull;
static volatile uint16_t nErrorTooLong;
static volatile uint16_t nErrorXor;
#endif
static volatile byte nAckTimer;
static byte nLastCV1;
static byte nLastCV2;
static byte nLastCV3;
static byte nServoConfig;
static byte nServoConfigAddr;
static byte nConfigClosed;
static byte nConfigOpen;

#define MAX_SERVOS  8
#define SERVO_UPDATES_PER_TENTH       4     // Number of updates in 1/10 second - 2 = 20 per sec

typedef struct _SServo
{
    uint16_t nMin;
    uint16_t nMax;
    byte nTime;
    byte nConfig;
    uint16_t nCurrentPulse;
    byte nCurrentTime;
    bool bOpen;
    bool bDirection;
    byte filler[5]; // Pad to 16 bytes
} SServo;
static SServo Servo[MAX_SERVOS];
static volatile bool bUpdateServoPos = false;
static SServo *pCurrentServo;
static byte nCurrentBit;


#define SERVO_DIRECTION_REVERSE     0x01
#define SERVO_LIGHT_REVERSE         0x02
#define SERVO_LIGHT_TRISTATE        0x04

#define SCALE_SERVO_POS( n )                (7200 + ((((uint16_t)(n)) * 96)))

#define SHOW_MESSAGES
//#define BIT_CAPTURE
#ifdef BIT_CAPTURE
static volatile byte times[70];
static volatile byte timesbyte=0;
#endif

static void NextServo( void )
{
    SServo *pTemp = pCurrentServo + 1;
    if ( pTemp >= Servo + MAX_SERVOS )
        pCurrentServo = Servo;
    else
        pCurrentServo = pTemp;

    byte nTemp = nCurrentBit << 1;
    if ( nTemp == 0 )
        nCurrentBit = 1;
    else
        nCurrentBit = nTemp;
}

ISR (TIMER0_OVF_vect)	// Timer0 overflow interupt.  2.5 millisecond counter.
{
    TCNT0 = MILLISECOND_COUNTER;

    uint16_t nTime = pCurrentServo->nCurrentPulse;
    if ( nTime )
    {
        // Start timer 1
        TCNT1 = 0;
        OCR1A = nTime;
        PORTC |= nCurrentBit;
        TCCR1B = _BV(CS10);     // clk/1
    }
    else
    {
        NextServo();
    }

    static int nSecCount = 400;
    int nTemp = nSecCount - 1;
    if ( nTemp == 0 )
    {
        nSec++;
        nSecCount = 400;
    }
    else
        nSecCount = nTemp;

    static byte nServoPosCount = 40 / SERVO_UPDATES_PER_TENTH;       // 1/10th of a second ticks
    nTemp = nServoPosCount - 1;
    if ( nTemp == 0 )
    {
        bUpdateServoPos = true;
        nServoPosCount = 40 / SERVO_UPDATES_PER_TENTH;
    }
    else
        nServoPosCount = nTemp;

    if ( nAckTimer > 0 )
    {
        nAckTimer--;
        if ( nAckTimer == 0 )
            PORTD &= ~(_BV(PD3));
    }
}

#ifdef DEBUG
static uint16_t nOV1=0;
static uint16_t nOC1=0;
#endif

ISR (TIMER1_OVF_vect)	// Timer1 overflow interupt.
{
#ifdef DEBUG
    nOV1++;
#endif
    // Shouldn't get here.  Should have been stopped by oc
    PORTC = 0;
    TCCR1B = 0;
    NextServo();
}

ISR (TIMER1_COMPA_vect)	// Timer1 oc
{
#ifdef DEBUG
    nOC1++;
#endif
    PORTC = 0;
    TCCR1B = 0;
    NextServo();
}

enum _ESignalStates
{ 
    WAIT_FOR_PREAMBLE,
    READ_DATA,
    END_OF_BYTE
};
typedef enum _ESignalStates ESignalStates;

// EEPROM values
#define EE_CV513_LSB_ADDRESS        0
#define EE_CV521_MSB_ADDRESS        1
#define EE_CV541_CONFIGURATION      2
#define EE_CV545_SERVO_CONFIG       3
    #define EE_SERVO_MIN                0
    #define EE_SERVO_MAX                1
    #define EE_SERVO_SWITCH_TIME        2
    #define EE_SERVO_CONFIG             3
    #define EE_PER_SERVO_COUNT          4
#define EE_CV577_INTERACTIVE_ADDR   (EE_CV545_SERVO_CONFIG + EE_PER_SERVO_COUNT*MAX_SERVOS)

#define VERSION								(1)
#define MANUFACTURER						(13)

#define CV512_OFFSET								(512)
#define CV513_LSB_ADDRESS                           (513-1)
#define CV519_VERSION								(519-1)
#define CV520_MANUFACTURER							(520-1)
#define CV521_MSB_ADDRESS                           (521-1)
#define CV541_CONFIGURATION                         (541-1)
#define CV545_SERVO_CONFIG                          (545-1)
#define CV545_SERVO_CONFIG_MAX                      (CV545_SERVO_CONFIG + 4 * MAX_SERVOS)
#define CV577_SERVO_INTERACTIVE_CONFIG_ADDRESS      (577-1)
#define CV578_SERVO_INTERACTIVE_CONFIG_SERVO        (578-1)


// CV541
#define CONFIG_BI_COMMS             3
#define CONFIG_DECODER_TYPE         5
#define CONFIG_ADDRESSING_METHOD    6
#define CONFIG_ACCESSORY_DECODER    7

#define MIN_BITS_FOR_PREAMBLE   12
#define ONE_MIN                 30  //48
#define ONE_MAX                 75  //68
#define ZERO_MIN                80
#define ZERO_MAX                10000
#define MAX_PACKET_LEN          6

static volatile bool bNewPacket;
static volatile char nPacket[MAX_PACKET_LEN];
static volatile byte nPacketBytes;
static uint16_t nDecoderAddress;
static uint8_t nConfiguration;
static volatile byte nPacketTimerMSB;


ISR (TIMER2_OVF_vect)	// Timer2 overflow interupt.
{
    nPacketTimerMSB++;
}

ISR (INT0_vect)	// External interupt 0.
{
    static bool bFirst;
    static byte nLastOne;
    static byte nBits = 0;
    static byte nByte;
    static byte nErrorCheck;
    static ESignalStates eState = WAIT_FOR_PREAMBLE;

    byte nCount = TCNT2;
    TCNT2 = 0;
    //uint16_t nCount = (nPacketTimerMSB<<8) | nCountLSB;
    //byte nCount = nCountLSB;
    //nPacketTimerMSB = 0;

#ifdef BIT_CAPTURE
    if ( timesbyte < sizeof(times) )
        times[timesbyte++] = nCount;
    return;
#endif

    byte nOne;
    if ( nCount >= ONE_MIN && nCount <= ONE_MAX )
    {
        nOne = 1;
    }
    else if ( nCount >= ZERO_MIN && nCount <= ZERO_MAX )
    {
        nOne = 0;
    }
    else
    {
        // Error in the bit.
        bFirst = true;
#ifdef DEBUG
        nErrorBitTime++;
        nErrorTime = nCount;
#endif
        return;
    }

    if ( bFirst )
    {
        nLastOne = nOne;
        bFirst = false;
        return;
    }

    if ( nLastOne ^ nOne )
    {
        // Bit mismatch.  Treat 2nd part as first and try again.
        nLastOne = nOne;
        return;
    }
    bFirst = true;

    switch ( eState )
    {
    case WAIT_FOR_PREAMBLE:
        if ( !nOne )
        {
            if ( nBits >= MIN_BITS_FOR_PREAMBLE )
            {
                nBits = 0;
                nPacketBytes = 0;
                nErrorCheck = 0;
                eState = READ_DATA;
            }
            else
                nBits = 0;
        }
        else
            nBits++;
        break;

    case READ_DATA:
        if ( bNewPacket )
        {
            // Problem.  Already got a packet queued up.  Ignore next packet.
#ifdef DEBUG
            nErrorBufferFull++;
#endif
            eState = WAIT_FOR_PREAMBLE;
            nBits = 0;
        }
        nByte <<= 1;
        nByte |= nOne;
        nBits++;
        if ( nBits == 8 )
        {
            // store byte
            nPacket[nPacketBytes] = nByte;
            nErrorCheck ^= nByte;
            nPacketBytes++;
            eState = END_OF_BYTE;
        }
        break;

    case END_OF_BYTE:
        nBits = 0;
        if ( nOne )
        {
            // End of packet
            if ( nErrorCheck == 0 )
                bNewPacket = true;      // Error check passed, handle packet.
#ifdef DEBUG
            else
                nErrorXor++;
#endif
            eState = WAIT_FOR_PREAMBLE;
        }
        else
        {
            // End of byte
            if ( nPacketBytes == MAX_PACKET_LEN ) // Too long, restart
            {
#ifdef DEBUG
                nErrorTooLong++;
#endif
                eState = WAIT_FOR_PREAMBLE;
            }
            else
                eState = READ_DATA;
        }
    }
}

void ioinit( void )
{
    /////////////////////////
    //// Set IO registers
    /////////////////////////

    DDRA = OUTPUT(PA0) | OUTPUT(PA1) | OUTPUT(PA2) | OUTPUT(PA3) | OUTPUT(PA4) | OUTPUT(PA5) | OUTPUT(PA6) | OUTPUT(PA7);
    DDRB = OUTPUT(PB0) | OUTPUT(PB1) | OUTPUT(PB2) | OUTPUT(PB3) | OUTPUT(PB4) | OUTPUT(PB5) | OUTPUT(PB6) | OUTPUT(PB7);
    DDRC = OUTPUT(PC0) | OUTPUT(PC1) | OUTPUT(PC2) | OUTPUT(PC3) | OUTPUT(PC4) | OUTPUT(PC5) | OUTPUT(PC6) | OUTPUT(PC7);
    DDRD = OUTPUT(PD3);

    //   //////////////////////////
    //   ////// External interrupt
    //   /////////////////////////
    MCUCR = _BV(ISC00);     // any change
    GICR |= _BV(INT0);      

    /////////////////////////////
    //////// Set Timer
    /////////////////////////////
    // Use 8bit timer 2 to time incoming signal
    TCCR2 = _BV(CS21);     // clk/8 - 1 MHz
    TCNT2 = 0;
    TIMSK |= _BV(TOIE2);    // overflow interrupt for counting 16bit.

    ////// 1/4 msec timer
    TCCR0 = _BV(CS02);	// CLK/256
    TCNT0 = MILLISECOND_COUNTER;
    TIMSK |= _BV(TOIE0);

    // timer 1 is the servo pulse timer - clk/64 (125kHz), normal, 1 tick = 8us, 256 ticks = 2.048 ms
    TCCR1A = 0;     // normal
    TCCR1B = 0;     // clk stopped
    TIMSK |= _BV(OCIE1A) | _BV(TOIE1);    // output compare and overflow interrupts.
}


bool AddressMatch( byte *pMsg )
{
    // Addresses 10000000-10111111 (128-191)(inclusive):
    byte nAddressByte1 = *pMsg;
    if ( nAddressByte1 >= 128 && nAddressByte1 <= 191 )
    {
        byte nAddressByte2 = *(pMsg+1);
        uint16_t nAddress = nAddressByte1 & 0X3F;
        // Decoder address.
        if ( (nAddressByte2 & 0xC0) == 0x80 )     // BasicAddress
        {
            // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1 
            // {preamble} 0 10111111 0 1000CDDD 0 EEEEEEEE 1    - Broadcast
            nAddress |= ((~((uint16_t)nAddressByte2)) & 0x70) << 2;
        }
        else // Extended address
        {
            //nAddress |= ~((AddressByte2 >> 4 ) & 7);
        }
#ifdef DEBUG
        RS232Send_P( PSTR("Checking Address ") );
        RS232SendInt( nAddress );
        RS232Send_P( PSTR(" against ") );
        RS232SendInt( nDecoderAddress );
        RS232SendCRLF();
#endif

        #define ACCESSORY_BROADCAST_ADDRESS     0x1FF
        if ( nAddress == ACCESSORY_BROADCAST_ADDRESS || nAddress == nDecoderAddress )
            return true;
    }
    else if ( nServoConfig > 0 )        // Servo configuration command
    {
        uint16_t nAddress;
        bool bHasAddress = false;
        byte nAddressByte1 = *pMsg;
        if ( !(nAddressByte1 & 0x80) )      // 7 bit address
        {
            nAddress = nAddressByte1;
            bHasAddress = true;
        }
        else if ( (nAddressByte1 & 0xC0) == 0xC0 )      // 14 bit address
        {
            nAddress = ((((uint16_t)nAddressByte1) & 0x3F) << 8) | *(pMsg+1);
            bHasAddress = true;
        }

        if ( bHasAddress && nAddress == nServoConfigAddr )
            return true;
    }

    return false;
}

void SendAck( void )
{
    PORTD |= _BV(PD3);
    nAckTimer = 4;
}


byte GetCV( uint16_t nCV )
{
#ifdef DEBUG
    RS232Send_P( PSTR("Get CV=") );
    RS232SendInt( nCV+1 );
    RS232SendCRLF();
#endif
    nServoConfig = 0;
	if ( nCV < CV512_OFFSET )
		nCV += CV512_OFFSET;
    if ( nCV == CV513_LSB_ADDRESS )
    {
        return nDecoderAddress & 0xFF;
    }
    else if ( nCV == CV521_MSB_ADDRESS )
    {
        return (nDecoderAddress>>8) & 0xFF;
    }
    else if ( nCV == CV541_CONFIGURATION )
    {
        return nConfiguration;
    }
    else if ( nCV == CV519_VERSION )
    {
	    return VERSION;
    }
    else if ( nCV == CV520_MANUFACTURER )
    {
	    return MANUFACTURER;
    }
    else if ( nCV == CV577_SERVO_INTERACTIVE_CONFIG_ADDRESS )
    {
        return nServoConfigAddr;
    }
    else if ( nCV >= CV545_SERVO_CONFIG && nCV <= CV545_SERVO_CONFIG_MAX )
    {
        return eeprom_read_byte( (byte *)(EE_CV545_SERVO_CONFIG + (nCV - CV545_SERVO_CONFIG))  );
    }
    return 0;
}

void SetCV( uint16_t nCV, byte nValue )
{
#ifdef DEBUG
    RS232Send_P( PSTR("Set CV=") );
    RS232SendInt( nCV+1 );
    RS232Send_P( PSTR(" Val=") );
    RS232SendInt( nValue );    
    RS232SendCRLF();
#endif
    if ( nCV != CV578_SERVO_INTERACTIVE_CONFIG_SERVO )
        nServoConfig = 0;
	if ( nCV < CV512_OFFSET )
		nCV += CV512_OFFSET;
    if ( nCV == CV513_LSB_ADDRESS )
    {
        nDecoderAddress = (nDecoderAddress & 0xFF00) | nValue;
        eeprom_write_byte( (uint8_t *)EE_CV513_LSB_ADDRESS, nValue );
    }
    else if ( nCV == CV521_MSB_ADDRESS )
    {
        nDecoderAddress = (nDecoderAddress & 0x00FF) | (nValue<<8);
        eeprom_write_byte( (uint8_t *)EE_CV521_MSB_ADDRESS, nValue );
    }
    else if ( nCV == CV541_CONFIGURATION )
    {
        nConfiguration = nValue;
        eeprom_write_byte( (uint8_t *)EE_CV541_CONFIGURATION, nValue );
    }
    else if ( nCV == CV577_SERVO_INTERACTIVE_CONFIG_ADDRESS )
    {
        nServoConfigAddr = nValue;
        eeprom_write_byte( (uint8_t *)EE_CV577_INTERACTIVE_ADDR, nValue );
    }
    else if ( nCV == CV578_SERVO_INTERACTIVE_CONFIG_SERVO )
    {
        if ( nValue == 0 && nServoConfig > 0 && nServoConfig <= MAX_SERVOS )
        {
            // Save settings
            int nAddress = EE_CV545_SERVO_CONFIG + EE_PER_SERVO_COUNT * (nServoConfig-1);
            eeprom_write_byte( (uint8_t *)(nAddress + EE_SERVO_MIN), nConfigClosed );
            eeprom_write_byte( (uint8_t *)(nAddress + EE_SERVO_MAX), nConfigOpen );
            Servo[nServoConfig-1].nMin = SCALE_SERVO_POS( nConfigClosed );
            Servo[nServoConfig-1].nMax = SCALE_SERVO_POS( nConfigOpen );
#ifdef DEBUG
            RS232Send_P( PSTR("Saving config ") );
            RS232SendInt( nServoConfig );
            RS232Send_P( PSTR(" - ") );
            RS232SendInt( nConfigClosed );
            RS232Send_P( PSTR(", ") );
            RS232SendInt( nConfigOpen );
            RS232SendCRLF();
#endif        
        }
        nServoConfig = nValue;
        if ( nServoConfig >0 && nServoConfig <= MAX_SERVOS )
        {
            int nAddress = EE_CV545_SERVO_CONFIG + EE_PER_SERVO_COUNT * (nServoConfig-1);
            nConfigClosed = eeprom_read_byte( (uint8_t *)(nAddress + EE_SERVO_MIN) );
            nConfigOpen = eeprom_read_byte( (uint8_t *)(nAddress + EE_SERVO_MAX) );
#ifdef DEBUG
            RS232Send_P( PSTR("Configuring ") );
            RS232SendInt( nServoConfig );
            RS232Send_P( PSTR(" - ") );
            RS232SendInt( nConfigClosed );
            RS232Send_P( PSTR(", ") );
            RS232SendInt( nConfigOpen );
            RS232SendCRLF();
#endif
        }
    }
    else if ( nCV >= CV545_SERVO_CONFIG && nCV <= CV545_SERVO_CONFIG_MAX )
    {
        eeprom_write_byte( (byte *)(EE_CV545_SERVO_CONFIG + (nCV - CV545_SERVO_CONFIG)), nValue);

        byte nCmd = (nCV - CV545_SERVO_CONFIG);
        byte nServo = nCmd >> 2;                    // (There are 4 bytes of config for each servo - min, max, time, config
        byte nField = nCmd & 0x3;

        switch ( nField )
        {
            case 0:
                Servo[nServo].nMin = SCALE_SERVO_POS(nValue); break;
            case 1:
                Servo[nServo].nMax = SCALE_SERVO_POS(nValue); break;
            case 2:
                Servo[nServo].nTime = nValue * SERVO_UPDATES_PER_TENTH; break;
            case 3:
                Servo[nServo].nConfig = nValue; break;
        }
    }
}

void SetLights( byte nServo )
{
    byte nBits;
    if ( (Servo[nServo].nConfig & SERVO_LIGHT_TRISTATE) && Servo[nServo].nCurrentTime > 0 )
    {
        nBits = 3;
    }
    else
    {
        bool bOpen = ( (Servo[nServo].bOpen && !(Servo[nServo].nConfig & SERVO_LIGHT_REVERSE) )  ||
                    (!Servo[nServo].bOpen && (Servo[nServo].nConfig & SERVO_LIGHT_REVERSE) ) );

        if ( bOpen )
            nBits = 2;
        else
            nBits = 1;
    }

    if ( nServo < 4 )
    {
        byte nShift = ((3-nServo)*2);
        byte nMask = 3 << nShift;
        nBits <<= nShift;

        PORTB = (PORTB & ~nMask) | nBits;
    }
    else
    {
        byte nShift = ((nServo-4)*2);
        byte nMask = 3 << nShift;
        nBits <<= nShift;

        PORTA = (PORTA & ~nMask) | nBits;
    }
}

void UpdateServoPos( byte nServo )
{
    int16_t nNewPulse=0;
    SServo *pServo = Servo + nServo;
    if ( pServo->nCurrentTime == 0 )
    {
        //pServo->nCurrentPulse = 0;
        SetLights( nServo );
    }
    else
    {
        if ( pServo->bDirection )
        {
            // Going from min to max
            nNewPulse = ((int16_t)pServo->nMax) - ((int16_t)pServo->nMin);
            nNewPulse /= (int16_t)pServo->nTime;
            nNewPulse *= (int16_t)(pServo->nCurrentTime-1);
            nNewPulse += (int16_t)pServo->nMin;
            //uint16_t nNewPulse = Servo[nServo].nMin + (Servo[nServo].nMax - Servo[nServo].nMin) * (Servo[nServo].nCurrentTime-1) / Servo[nServo].nTime;
        }
        else
        {
            // Going from max to min
            nNewPulse = ((int16_t)pServo->nMax) - ((int16_t)pServo->nMin);
            nNewPulse /= (int16_t)pServo->nTime;
            nNewPulse *= (int16_t)(pServo->nTime - (pServo->nCurrentTime-1));
            nNewPulse += (int16_t)pServo->nMin;
            //uint16_t nNewPulse = Servo[nServo].nMin + (Servo[nServo].nMax - Servo[nServo].nMin) * (Servo[nServo].nTime - (Servo[nServo].nCurrentTime-1)) / Servo[nServo].nTime;
        }
        uint16_t *ptr = &(pServo->nCurrentPulse);
        cli();
        *ptr = nNewPulse;
        sei();
    }

#ifdef DEBUG
    RS232SendChar('P');
    RS232SendInt(nNewPulse);
    RS232SendChar('N');
    RS232SendInt(Servo[nServo].nMin);
    RS232SendChar('X');
    RS232SendInt(Servo[nServo].nMax);
    RS232SendCRLF();
#endif
}

void Switch( byte nServo, bool bOpen )
{
    if ( ( bOpen && !(Servo[nServo].nConfig & SERVO_DIRECTION_REVERSE) )  ||
         ( !bOpen && (Servo[nServo].nConfig & SERVO_DIRECTION_REVERSE) ) )
    {
        // Going from min to max
        Servo[nServo].bDirection = true;
    }
    else
    {
        // Going from max to min
        Servo[nServo].bDirection = false;
    }
    Servo[nServo].bOpen = bOpen;
    Servo[nServo].nCurrentTime = Servo[nServo].nTime+1;
    UpdateServoPos( nServo );
    SetLights( nServo );
}

void ProcessPacket( byte *pMsg, byte nLen )
{
    // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1             111CDAAA
    // {preamble} 0 10AAAAAA 0 1AAACDDD 0 (1110CCAA 0 AAAAAAAA 0 DDDDDDDD) 0 EEEEEEEE 1
    //              81         80          eA         0          e8          3
    if ( nLen == 6 && (*(pMsg+2) & 0xE0) == 0xE0 )
    {
        //RS232Send_P(PSTR("CV Instruction - "));
        // CV instruction
        #ifdef DEBUG
			byte nDevice;
			if ( *(pMsg+1) & 0xF )
				nDevice = *(pMsg+1) & 7;
			else
				nDevice = 8;
			RS232Send_P(PSTR("nDevice=")); RS232SendInt(nDevice);
		#endif

        byte nCmd = (*(pMsg+2) & 0x0C) >> 2;                 // 1 verify, 2 write, 3 bit
        uint16_t nCVAddress = *(pMsg+3) | ((*(pMsg+2) & 3) << 8);
        byte nData = *(pMsg+4);

        //RS232Send_P(PSTR(" nCmd=")); RS232SendInt(nCmd);
        //RS232Send_P(PSTR(" nCVAddress=")); RS232SendInt(nCVAddress);
        //RS232Send_P(PSTR(" nData=")); RS232SendInt(nData);
        //RS232SendCRLF();

        if ( nCmd == 1 ) // Verify Byte
        {
            // Verify Byte
            byte nCV = GetCV( nCVAddress );
            //RS232Send_P( PSTR("Verify CV="));
            //RS232SendInt( nCVAddress );
            //RS232Send_P( PSTR(" data="));
            //RS232SendInt( nData );
            //RS232Send_P( PSTR(" actual="));
            //RS232SendInt( nCV );
            //RS232SendCRLF();
            if ( nData == nCV )
                SendAck();
        }
        else if ( nCmd == 3 ) // Write Byte 
        {
            // Write Byte
            // We only write on the second packet
            //RS232Send_P( PSTR("CV Write Byte\r\n"));
            //RS232SendHex( nLastCV1 ); RS232SendHex( nLastCV2 ); RS232SendHex( nLastCV3 ); RS232SendCRLF();
            //RS232SendHex( *(pMsg+2) ); RS232SendHex( *(pMsg+3) ); RS232SendHex( *(pMsg+4) ); RS232SendCRLF();
            if ( nLastCV1 == *(pMsg+2) &&  nLastCV2 == *(pMsg+3) && nLastCV3 == *(pMsg+4) )
            {
                //RS232Send_P( PSTR("Set CV="));
                //RS232SendInt( nCVAddress );
                //RS232Send_P( PSTR(" data="));
                //RS232SendInt( nData );
                //RS232SendCRLF();
                SetCV( nCVAddress, nData );
                SendAck();
            }
        }
        else if ( nCmd == 2 ) // Bit Manipulation
        {
            byte nBit = *(pMsg+4) & 0x7;
            byte nValue = (*(pMsg+4)>>3) & 1;
            if ( *(pMsg+4) & 0x10 )
            {
                // We only write on the second packet
                if ( nLastCV1 == *(pMsg+2) &&  nLastCV2 == *(pMsg+3) && nLastCV3 == *(pMsg+4) )
                {
                    // Write Bit
                    //RS232Send_P( PSTR("Set CV="));
                    //RS232SendInt( nCVAddress );
                    //RS232Send_P( PSTR(" bit="));
                    //RS232SendInt( nBit );
                    //RS232Send_P( PSTR(" n="));
                    //RS232SendInt( nValue );
                    //RS232SendCRLF();
                    byte nCV = GetCV( nCVAddress );
                    if ( nBit )
                        nCV |= 1 << nBit;
                    else
                        nCV &= ~(1 << nBit);
                    SetCV( nCVAddress, nCV );
                    SendAck();
                }
            }
            else
            {
                // Verify Bit
                byte nCV = GetCV( nCVAddress );
                byte nCurrentValue = nCV & (1 << nBit);
                //RS232Send_P( PSTR("CV="));
                //RS232SendInt( nCV );
                //RS232Send_P( PSTR(", "));
                //RS232SendInt( nCurrentValue );
                //RS232Send_P( PSTR(" with "));
                //RS232SendInt( nValue );
                if ( ( nCurrentValue && nValue ) || (!nCurrentValue && !nValue) )
                {
                    // Bit is the same, ack it.
                    //RS232Send_P( PSTR("Match"));
                    SendAck();
                }
                //RS232SendCRLF();
            }
        }
        nLastCV1 = *(pMsg+2);
        nLastCV2 = *(pMsg+3);
        nLastCV3 = *(pMsg+4);

        // CVs
        // x 513      LSB of accessory decoder address
        //   514      Auxiliary activation of outputs
        //   515      Time On F1
        //   516      Time On F2
        //   517      Time On F3
        //   518      Time On F4
        // x 519      Version Number
        // x 520      Manufacturer Id       (13)
        // x 521      MSB of accessory decoder address
        //   540      Bi-Directional Communication Configuration
        // x 541      Accessory Decoder Configuration, similar to CV#29; for acc. decoders
        //                  Bit 3 = Bi-Directional Communications: "0" = Bi-Directional Communications disabled, "1" = Bi-Directional 385
        //                          Communications enabled. See RP-9.3.2 for more information.`
        //                  Bit 5 = Decoder Type: ‘0’ = Basic Accessory Decoder; ‘1’ = Extended Accessory Decoder
        //                  Bit 6 = Addressing Method: ‘0’= Decoder Address method; ‘1’ = Output Address method
        //                  Bit 7 = Accessory Decoder: = "0" Multifunction Decoder (See CV-29 for description of bit Assignments for 390
        //                          bits 0-6), "1" = Accessory Decoder. If bit 7 = 1, then the decoder may ignore the two mostsignificant
        //                          bits of the CV number in Service Mode only. Using this feature CV513 becomes CV1,
        //                          etc. Decoders which perform the translation must clearly document the feature in their manual.
        //   545-593  Manufacturer Unique
        //   624-640  Manufacturer Unique
    }
    else if ( nServoConfig > 0 && (*(pMsg) & 0x80) == 0 && nLen == 4 && (*(pMsg+1) == 0x3F) )      // 128 set, Speed cmd
    {
#ifdef DEBUG
        RS232Send_P(PSTR("Speed\r\n"));
#endif
        bool bForward = (*(pMsg+2) & 0x80) != 0;
        byte nSpeed = (*(pMsg+2) & 0x7F);
        if ( nSpeed > 100 )
            nSpeed = 100;

        if ( bForward ) // Set switch closed position
        {
            if ( nConfigClosed != nSpeed )
            {
                nConfigClosed = nSpeed;
                Servo[nServoConfig-1].nMin = SCALE_SERVO_POS(nSpeed);
                Servo[nServoConfig-1].nCurrentPulse = SCALE_SERVO_POS(nSpeed);
                Servo[nServoConfig-1].nCurrentTime = 1;
            }
        }
        else // !forward => reverse - set switch open position
        {
            if ( nConfigOpen != nSpeed )
            {
                nConfigOpen = nSpeed;
                Servo[nServoConfig-1].nMax = SCALE_SERVO_POS(nSpeed);
                Servo[nServoConfig-1].nCurrentPulse = SCALE_SERVO_POS(nSpeed);
                Servo[nServoConfig-1].nCurrentTime = 1;
            }
        }
    }
    else
    {
        // Normal instruction
        nServoConfig = 0;
        byte nDevice = *(pMsg+1) & 7;
        byte nState = *(pMsg+1) & 8;

        #ifdef DEBUG
            RS232Send_P(PSTR("Command - "));
            RS232Send_P(PSTR("nDevice=")); RS232SendInt(nDevice);
            RS232Send_P(PSTR(" nState=")); RS232SendInt(nState);
            RS232SendCRLF();
        #endif

        if ( ( nState && !Servo[nDevice].bOpen ) || ( !nState && Servo[nDevice].bOpen ) )
        {
            Switch( nDevice, nState );
        }

        nLastCV1 = 0;
        nLastCV2 = 0;
        nLastCV3 = 0;
    }
}


int main (void)
{
#ifdef DEBUG
    byte nStatus = MCUCSR;
#endif

    bNewPacket = false;
#ifdef DEBUG
    nErrorBitTime = 0;
    nErrorBufferFull = 0;
    nErrorTooLong = 0;
    nErrorXor = 0;
#endif
    nAckTimer = 0;

    pCurrentServo = Servo;
    nCurrentBit = 1;

    /*
    eeprom_write_byte( (uint8_t *)EE_CV513_LSB_ADDRESS, 1 );
    eeprom_write_byte( (uint8_t *)EE_CV521_MSB_ADDRESS, 0 );
    eeprom_write_byte( (uint8_t *)EE_CV541_CONFIGURATION, _BV(CONFIG_BI_COMMS) | _BV(CONFIG_ACCESSORY_DECODER) );
    for ( byte i = 0; i < 8; i++ )
    {
        int nAddress = EE_CV545_SERVO_CONFIG + EE_PER_SERVO_COUNT * i;
        eeprom_write_byte( (uint8_t *)(nAddress + EE_SERVO_MIN), 40 );
        eeprom_write_byte( (uint8_t *)(nAddress + EE_SERVO_MAX), 60 );
        eeprom_write_byte( (uint8_t *)(nAddress + EE_SERVO_SWITCH_TIME), 20 );
        eeprom_write_byte( (uint8_t *)(nAddress + EE_SERVO_CONFIG), 0 );
    }
    */

    // Read device config
    nDecoderAddress = eeprom_read_byte( (uint8_t *)EE_CV513_LSB_ADDRESS ) | ( eeprom_read_byte( (uint8_t *)EE_CV521_MSB_ADDRESS ) << 8 );
    nConfiguration = eeprom_read_byte( (uint8_t *)EE_CV541_CONFIGURATION );
    nServoConfig = 0;
    nServoConfigAddr = eeprom_read_byte( (uint8_t *)EE_CV577_INTERACTIVE_ADDR );

    // Read per servo config
    for ( byte i = 0; i < 8; i++ )
    {
        int nAddress = EE_CV545_SERVO_CONFIG + EE_PER_SERVO_COUNT * i;
        Servo[i].nMin = eeprom_read_byte( (uint8_t *)(nAddress + EE_SERVO_MIN) );
        Servo[i].nMax = eeprom_read_byte( (uint8_t *)(nAddress + EE_SERVO_MAX) );

        Servo[i].nMin = SCALE_SERVO_POS( Servo[i].nMin );
        Servo[i].nMax = SCALE_SERVO_POS( Servo[i].nMax );

        Servo[i].nTime = eeprom_read_byte( (uint8_t *)(nAddress + EE_SERVO_SWITCH_TIME) );
        Servo[i].nTime *= SERVO_UPDATES_PER_TENTH;
        Servo[i].nConfig = eeprom_read_byte( (uint8_t *)(nAddress + EE_SERVO_CONFIG) );
        Servo[i].nCurrentPulse = 0;
        Servo[i].nCurrentTime = 0;
        Servo[i].bOpen = false;
        Switch( i, true );
    }
    
    ioinit();

    sei();

#ifdef DEBUG
    InitRS232( 12, true, false );

    RS232Send_P( PSTR("Servo Accessory Decoder initialised\r\n") );     
    RS232Send_P( PSTR("MCUCSR=") );
    if ( nStatus & _BV(JTRF) ) RS232Send_P( PSTR("JTAG Reset ") );     
    if ( nStatus & _BV(WDRF) ) RS232Send_P( PSTR("Watchdog Reset ") );     
    if ( nStatus & _BV(BORF) ) RS232Send_P( PSTR("Brown-out Reset ") );     
    if ( nStatus & _BV(EXTRF) ) RS232Send_P( PSTR("External Reset ") );     
    if ( nStatus & _BV(PORF) ) RS232Send_P( PSTR("Power-on Reset ") );      
    RS232SendCRLF();

#endif

#ifdef DEBUG
    int nLastSec=0;
#endif
    while ( true )
    {
        if ( bUpdateServoPos && nServoConfig == 0 )     // If we are configuring a servo, don't do normal update.
        {
            bUpdateServoPos = false;
            for ( byte i = 0; i < MAX_SERVOS; i++ )
            {
                if ( Servo[i].nCurrentTime > 0 )
                {
                    Servo[i].nCurrentTime--;
                    UpdateServoPos( i );
                }
            }
        }
        if ( bNewPacket )
        {
            byte nBytes = nPacketBytes;
            char nTempPacket[MAX_PACKET_LEN];
            memcpy( nTempPacket, (void *)nPacket, nBytes );
            bNewPacket = false;

#ifdef DEBUG
#ifdef SHOW_MESSAGES
			for ( byte i = 0; i < nBytes; i++ )
			{
			    RS232SendHex( nTempPacket[i] );
			    RS232SendChar( ' ' );
			}
			RS232SendCRLF();
#endif
#endif

            if ( nTempPacket[0] != 0xFF ) // (idle Packet)
            {
                if ( AddressMatch( (byte *)nTempPacket ) )
                {
                    ProcessPacket( (byte *)nTempPacket, nBytes );
                }
            }
        }
#ifdef DEBUG
        if ( nSec != nLastSec )
        {
            nLastSec = nSec;
            RS232SendInt( nLastSec );
            RS232Send_P( PSTR(", BadBits=" ) );
            RS232SendInt( nErrorBitTime );
            RS232SendChar( '(' );
            RS232SendInt( nErrorTime );
            RS232Send_P( PSTR("), BufferFull=" ) );
            RS232SendInt( nErrorBufferFull );
            RS232Send_P( PSTR(", TooLong=" ) ); RS232SendInt( nErrorTooLong );
            RS232Send_P( PSTR(", CheckSum=" ) ); RS232SendInt( nErrorXor );
            RS232Send_P( PSTR(", OV=" ) ); RS232SendInt( nOV1 );
            RS232Send_P( PSTR(", OC=" ) ); RS232SendInt( nOC1 );
            RS232SendCRLF();
        }
#endif

        //if ( bLastSample )
        //{
        //    cli();
        //    uint16_t n = nLastSample;
        //    bLastSample = false;
        //    sei();

        //    static byte nAvgCount=0;
        //    static uint16_t nAverage=0;
        //    nAverage += n;
        //    nAvgCount++;
        //    if ( nAvgCount == 8 )
        //    {
        //        nAvgCount = 0;
        //        n /= 8;
        //        
        //        RS232Send_P( PSTR("Sample=") );
        //        RS232SendInt( n );
        //        RS232SendCRLF();
        //    }
        //}
        //if ( bLastSample )
        //{
        //    uint16_t s[4];
        //    cli();
        //    memcpy( s, nSampleHistory, sizeof(nSampleHistory) );
        //    bLastSample = false;
        //    sei();

        //    RS232Send_P( PSTR("Samples=") );
        //    RS232SendInt( s[0] ); RS232SendChar( ',' );
        //    RS232SendInt( s[1] ); RS232SendChar( ',' );
        //    RS232SendInt( s[2] ); RS232SendChar( ',' );
        //    RS232SendInt( s[3] ); RS232SendCRLF();
        //}
        //if ( bNewPacket )
        //    bNewPacket = false;
        //if ( bNewCount )
        //{
        //cli();
        //bNewCount = false;
        //if ( nLastCount == 1 )
        //{
        //    sei();
        //    RS232SendChar('1');
        //}
        //else if ( nLastCount == 0 )
        //{
        //    sei();
        //    RS232SendChar( '0' );
        //}
        //else
        //{
        //    sei();
        //    RS232SendChar( 'E' );
        //}
        //}

#ifdef BIT_CAPTURE
#ifdef DEBUG
        if ( timesbyte == sizeof(times) )
        {
            RS232Send_P( PSTR(" ** ") );
            for ( byte i = 0; i < sizeof(times); i++ )
            {
                if ( times[i] < ONE_MIN || ( times[i] > ONE_MAX && times[i] < ZERO_MIN ) )
                    RS232SendChar( 'E' );
                byte a = (times[i] & 0xF0) >> 4;
                if ( a < 10 )
                    RS232SendChar( '0' + a );
                else
                    RS232SendChar( 'A' + a - 10 );
                a = times[i] & 0xF;
                if ( a < 10 )
                    RS232SendChar( '0' + a );
                else
                    RS232SendChar( 'A' + a - 10 );
                RS232SendChar( ' ' );
            }
            timesbyte = 0;
        }
#endif
#endif
    }
}



/*
Make this an accessory decoder.
Movement from on to off, is gradual, from min to max.
Need matching digital I/Os

Config -
    Address Lo
    Address Hi
    Config
    For each port
        1) Min servo
        2) Max servo
        3) Switch Time
        3.1) Reverse Direction
        3.2) Reverse lights
        3.3) Tri colour


CV545     Closed     CV546     Open     CV547   Time        CV548   Config [0] Direction Reverse [1] Light reverse [2] Tri colour LED
CV549     Closed     CV550     Open     CV551   Time        CV552   Config
CV553     Closed     CV554     Open     CV555   Time        CV556   Config
CV557     Closed     CV558     Open     CV559   Time        CV560   Config
CV561     Closed     CV562     Open     CV563   Time        CV564   Config
CV565     Closed     CV566     Open     CV567   Time        CV568   Config
CV569     Closed     CV570     Open     CV571   Time        CV572   Config
CV573     Closed     CV574     Open     CV575   Time        CV576   Config

577     Interactive programming address
578     Interactive servo.
*/
