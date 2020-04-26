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

#define MILLISECOND_COUNTER (256-125)

//#define DEBUG

//////////////////////////////////////////////////
// Port Config
//////////////////////////////////////////////////
static volatile uint8_t nSec;
static volatile uint16_t nErrorCount;
static volatile byte nAckTimer;
static byte nLastCV1;
static byte nLastCV2;
static byte nLastCV3;
static byte nDevice1Count;
static byte nDevice2Count;

//#define BIT_CAPTURE
#ifdef BIT_CAPTURE
static volatile byte times[70];
static volatile byte timesbyte=0;
#endif

SIGNAL (SIG_OVERFLOW0)	// Timer0 overflow interupt.
{
	TCNT0 = MILLISECOND_COUNTER;

    static int nSecCount = 0;
    nSecCount++;
    if ( nSecCount == 1000 )
    {
        nSec++;
        nSecCount=0;
    }

    static byte nTenthCount = 0;
    nTenthCount++;
    if ( nTenthCount == 100 )
    {
        nTenthCount=0;

        if ( nDevice1Count > 0 )
        {
            nDevice1Count--;
            if ( nDevice1Count == 0 )
            {
                PORTB &= ~(_BV(PB0) | _BV(PB1) | _BV(PB2));
                PORTD &= ~(_BV(PD5) | _BV(PD6) | _BV(PD7));
            }
        }

        if ( nDevice2Count > 0 )
        {
            nDevice2Count--;
            if ( nDevice2Count == 0 )
            {
                PORTC &= ~(_BV(PC3) | _BV(PC4) | _BV(PC5));
                PORTD &= ~(_BV(PD0) | _BV(PD1) | _BV(PD2));
            }
        }
    }


    if ( nAckTimer > 0 )
    {
        nAckTimer--;
        if ( nAckTimer == 0 )
            PORTD &= ~_BV(PD4);
    }
}

//SIGNAL (SIG_OVERFLOW1)	// Timer1 overflow interupt.
//{
//}

enum _ESignalStates
{ 
    WAIT_FOR_PREAMBLE,
    READ_DATA,
    END_OF_BYTE
};
typedef enum _ESignalStates ESignalStates;

// EEPROM values
#define EE_CV513_LSB_ADDRESS        0
#define EE_CV515_F1_ON              1
#define EE_CV516_F2_ON              2
#define EE_CV521_MSB_ADDRESS        3
#define EE_CV541_CONFIGURATION      4

// CV541
#define CONFIG_BI_COMMS             3
#define CONFIG_DECODER_TYPE         5
#define CONFIG_ADDRESSING_METHOD    6
#define CONFIG_ACCESSORY_DECODER    7

#define MIN_BITS_FOR_PREAMBLE   12
#define ONE_MIN                 48
#define ONE_MAX                 68
#define ZERO_MIN                90
#define ZERO_MAX                10000
#define MAX_PACKET_LEN          6

static volatile bool bNewPacket;
static volatile char nPacket[MAX_PACKET_LEN];
static volatile byte nPacketBytes;
static uint16_t nDecoderAddress;
static uint8_t nConfiguration;

SIGNAL (SIG_INTERRUPT1)	// External interupt 1.
{
    static bool bFirst;
    static byte nLastOne;
    static byte nBits = 0;
    static byte nByte;
    static byte nErrorCheck;
    static ESignalStates eState = WAIT_FOR_PREAMBLE;

    uint16_t nCount = TCNT1;
	TCNT1 = 0;

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
        nErrorCount++;
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
                nErrorCount++;
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
                eState = WAIT_FOR_PREAMBLE;
            }
            else
            {
                // End of byte
                if ( nPacketBytes == MAX_PACKET_LEN ) // Too long, restart
                {
                    nErrorCount++;
                    eState = WAIT_FOR_PREAMBLE;
                }
                else
                    eState = READ_DATA;
            }
    }
}

//SIGNAL (SIG_ADC)	// ADC conversion complete.
//{
//}

void ioinit( void )
{
	/////////////////////////
	//// Set IO registers
	/////////////////////////

	DDRB = OUTPUT(PB0) | OUTPUT(PB1) | OUTPUT(PB2) | OUTPUT(PB4);
	DDRC = OUTPUT(PC2) | OUTPUT(PC3) | OUTPUT(PC4) | OUTPUT(PC5);
	DDRD = OUTPUT(PD0) | OUTPUT(PD1) | OUTPUT(PD2) | OUTPUT(PD4) | OUTPUT(PD5) | OUTPUT(PD6) | OUTPUT(PD7);

    // INT1 - PD3 - input, pull up
    PORTD |= _BV(PD3);

 //   //////////////////////////
 //   ////// ADC Set up free run conversion of current sense port, with interrupt notification
 //   /////////////////////////
 //   ADMUX = CURRENT_SENSE_ADC;
 //   ADCSR = _BV(ADEN) | _BV(ADFR) | _BV(ADIE) | _BV(ADPS2);     // Prescale /16 => 937 S/s

 //   //////////////////////////
 //   ////// External interrupt
 //   /////////////////////////
    MCUCR = _BV(ISC10);     // any change
    GIMSK |= _BV(INT1);      

	/////////////////////////////
	//////// Set Timer
	/////////////////////////////
    // Use 16bit timer 1 to time incoming signal
    TCCR1B = _BV(CS11);     // clk/8 - 1 MHz
    TCNT1 = 0;
    //TIMSK |= _BV(TOIE1);

	////// msec timer
    TCCR0 = _BV(CS01) | _BV(CS00);	// CLK/64
	TCNT0 = MILLISECOND_COUNTER;
	TIMSK |= _BV(TOIE0);

}

void SwitchDevice1( byte nState )
{
    if ( nState )
    {
        // PD5-7 on, PB0-2 off
        PORTD |= _BV(PD5) | _BV(PD6) | _BV(PD7);
        PORTB &= ~(_BV(PB0) | _BV(PB1) | _BV(PB2));
        PORTB |= _BV(PB4);
    }
    else
    {
        // PD5-7 off, PB0-2 on
        PORTD &= ~(_BV(PD5) | _BV(PD6) | _BV(PD7));
        PORTB |= _BV(PB0) | _BV(PB1) | _BV(PB2);
        PORTB &= ~_BV(PB4);
    }
    nDevice1Count = eeprom_read_byte( (uint8_t *)EE_CV515_F1_ON );
}

void SwitchDevice2( byte nState )
{
    if ( nState )
    {
        // PD0-2 on, PBC3-5 off
        PORTD |= _BV(PD0) | _BV(PD1) | _BV(PD2);
        PORTC &= ~(_BV(PC3) | _BV(PC4) | _BV(PC5));
        PORTC |= _BV(PC2);
    }
    else
    {
        // PD0-2 off, PBC3-5 on
        PORTD &= ~(_BV(PD0) | _BV(PD1) | _BV(PD2));
        PORTC |= _BV(PC3) | _BV(PC4) | _BV(PC5);
        PORTC &= ~_BV(PC2);
    }
    nDevice2Count = eeprom_read_byte( (uint8_t *)EE_CV516_F2_ON );
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
        //RS232Send_P( PSTR("Checking Address ") );
        //RS232SendInt( nAddress );
        //RS232Send_P( PSTR(" against ") );
        //RS232SendInt( nDecoderAddress );
        //RS232SendCRLF();

        #define ACCESSORY_BROADCAST_ADDRESS     0x1FF
        if ( nAddress == ACCESSORY_BROADCAST_ADDRESS || nAddress == nDecoderAddress )
            return true;
    }
    return false;
}

void SendAck( void )
{
    PORTD |= _BV(PD4);
    nAckTimer = 10;
}

byte GetCV( uint16_t nCV )
{
    if ( nCV == 513-1 )
    {
        return nDecoderAddress & 0xFF;
    }
    else if ( nCV == 515-1 )
    {
        return eeprom_read_byte( (uint8_t *)EE_CV515_F1_ON );
    }
    else if ( nCV == 516-1 )
    {
        return eeprom_read_byte( (uint8_t *)EE_CV516_F2_ON );
    }
    else if ( nCV == 521-1 )
    {
        return (nDecoderAddress>>8) & 0xFF;
    }
    else if ( nCV == 541-1 )
    {
        return nConfiguration;
    }
    else
        return 0;
}

void SetCV( uint16_t nCV, byte nValue )
{
    if ( nCV == 513-1 )
    {
        nDecoderAddress = (nDecoderAddress & 0xFF00) | nValue;
        eeprom_write_byte( (uint8_t *)EE_CV513_LSB_ADDRESS, nValue );
    }
    else if ( nCV == 515-1 )
    {
        eeprom_write_byte( (uint8_t *)EE_CV515_F1_ON, nValue );
    }
    else if ( nCV == 516-1 )
    {
        eeprom_write_byte( (uint8_t *)EE_CV516_F2_ON, nValue );
    }
    else if ( nCV == 521-1 )
    {
        nDecoderAddress = (nDecoderAddress & 0x00FF) | (nValue<<8);
        eeprom_write_byte( (uint8_t *)EE_CV521_MSB_ADDRESS, nValue );
    }
    else if ( nCV == 541-1 )
    {
        nConfiguration = nValue;
        eeprom_write_byte( (uint8_t *)EE_CV541_CONFIGURATION, nValue );
    }
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
        byte nDevice;
        if ( *(pMsg+1) & 0xF )
            nDevice = *(pMsg+1) & 7;
        else
            nDevice = 8;
        //RS232Send_P(PSTR("nDevice=")); RS232SendInt(nDevice);

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
    else
    {
        // Normal instruction
        byte nDevice = *(pMsg+1) & 7;
        byte nState = *(pMsg+1) & 8;

        #ifdef DEBUG
            RS232Send_P(PSTR("Command - "));
            RS232Send_P(PSTR("nDevice=")); RS232SendInt(nDevice);
            RS232Send_P(PSTR(" nState=")); RS232SendInt(nState);
            RS232SendCRLF();
        #endif

        if ( nDevice == 0 )
            SwitchDevice1( nState );
        else if ( nDevice == 1 )
            SwitchDevice2( nState );

        nLastCV1 = 0;
        nLastCV2 = 0;
        nLastCV3 = 0;
    }
}

int main (void)
{
    bNewPacket = false;
    nErrorCount = 0;
    nAckTimer = 0;
    nDevice1Count = 0;
    nDevice2Count = 0;

    //eeprom_write_byte( (uint8_t *)EE_CV513_LSB_ADDRESS, 1 );
    //eeprom_write_byte( (uint8_t *)EE_CV515_F1_ON, 5 );
    //eeprom_write_byte( (uint8_t *)EE_CV516_F2_ON, 5 );
    //eeprom_write_byte( (uint8_t *)EE_CV521_MSB_ADDRESS, 0 );
    //eeprom_write_byte( (uint8_t *)EE_CV541_CONFIGURATION, _BV(CONFIG_BI_COMMS) | _BV(CONFIG_ACCESSORY_DECODER) );

    nDecoderAddress = eeprom_read_byte( (uint8_t *)EE_CV513_LSB_ADDRESS ) | ( eeprom_read_byte( (uint8_t *)EE_CV521_MSB_ADDRESS ) << 8 );
    nConfiguration = eeprom_read_byte( (uint8_t *)EE_CV541_CONFIGURATION );

    ioinit();
	sei();

#ifdef DEBUG
    InitRS232( 8, true, false );

    RS232Send_P( PSTR("Switch Decoder initialised\r\n") );
#endif

    SwitchDevice1( false );
    SwitchDevice2( false );

#ifdef DEBUG
    int nLastSec=0;
#endif
    while ( true )
    {
        if ( bNewPacket )
        {
            byte nBytes = nPacketBytes;
            char nTempPacket[MAX_PACKET_LEN];
            memcpy( nTempPacket, (void *)nPacket, nBytes );
            bNewPacket = false;

            #ifdef DEBUG
                RS232SendInt( nErrorCount );
                RS232SendChar( ' ' );
                for ( byte i = 0; i < nBytes; i++ )
                {
                    RS232SendHex( nTempPacket[i] );
                    RS232SendChar( ' ' );
                }
                RS232SendCRLF();
            #endif

            if ( AddressMatch( nTempPacket ) )
            {
                ProcessPacket( nTempPacket, nBytes );
            }
        }
        #ifdef DEBUG
            if ( nSec != nLastSec )
            {
                nLastSec = nSec;
                RS232SendInt( nLastSec );
                RS232SendCRLF();
            }
        #endif
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
    }
}

