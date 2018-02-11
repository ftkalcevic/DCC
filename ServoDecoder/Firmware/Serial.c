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


// The original source isn't mine.  If it is yours, let me know and I'll 
// add attribution.

#include "Common.h"
#include "Serial.h"
#include <stdlib.h>


/* UART Buffer Defines */
#ifdef RS232_TX
 
	#ifndef UART_TX_BUFFER_SIZE
		#define UART_TX_BUFFER_SIZE 128		/* 2,4,8,16,32,64,128 or 256 bytes */ 
	#endif
	#define UART_TX_BUFFER_MASK ( UART_TX_BUFFER_SIZE - 1 )
	#if ( UART_TX_BUFFER_SIZE & UART_TX_BUFFER_MASK )
		#error TX buffer size is not a power of 2
	#endif

	byte UART_TxBuf[UART_TX_BUFFER_SIZE];
	volatile byte UART_TxHead;
	volatile byte UART_TxTail;

#endif

#ifdef RS232_RX

	#ifndef UART_RX_BUFFER_SIZE
		#define UART_RX_BUFFER_SIZE 128		/* 2,4,8,16,32,64,128 or 256 bytes */
	#endif
	#define UART_RX_BUFFER_MASK ( UART_RX_BUFFER_SIZE - 1 )
	#if ( UART_RX_BUFFER_SIZE & UART_RX_BUFFER_MASK )
		#error RX buffer size is not a power of 2
	#endif

	byte UART_RxBuf[UART_RX_BUFFER_SIZE]; 
	volatile byte UART_RxHead;
	volatile byte UART_RxTail;

#endif

#ifdef USE_UART1
	#define UDR		UDR1
	#define UCSRA	UCSR1A
	#define UCSRB	UCSR1B
	#define UCSRC	UCSR1C
	#define UBRRH	UBRR1H
	#define UBRRL	UBRR1L
	#define SIG_UART_DATA	SIG_UART1_DATA
	#define SIG_UART_RECV	SIG_UART1_RECV
#endif

#if MCU == at90s4433
    #if !defined(UBRRL)
        #define UBRRL   UBRR
    #endif
#endif

#ifdef RS232_TX

	ISR (USART_UDRE_vect)	// UART Data ready interrupt
	{ 
		byte tmptail;

		/* Check if all data is transmitted */
		if ( UART_TxHead != UART_TxTail )
		{
			/* Calculate buffer index */
			tmptail = ( UART_TxTail + 1 ) & UART_TX_BUFFER_MASK;
			UART_TxTail = tmptail;      /* Store new index */
		
			UDR = UART_TxBuf[tmptail];  /* Start transmition */
		}
		else
		{
			UCSRB &= ~(1<<UDRIE);         /* Disable UDRE interrupt */
		}
	}

#endif

#ifdef RS232_RX

	ISR (USART_RXC_vect)	// UART Data ready interrupt
	{ 
		byte data;
		byte tmphead;

		data = UDR;                 /* Read the received data */
		/* Calculate buffer index */
		tmphead = ( UART_RxHead + 1 ) & UART_RX_BUFFER_MASK;
		UART_RxHead = tmphead;      /* Store new index */

		if ( tmphead == UART_RxTail )
		{
			/* ERROR! Receive buffer overflow */
		}
		
		UART_RxBuf[tmphead] = data; /* Store received data in buffer */
	}

#endif

void InitRS232( unsigned short nBaud, bool bTransmit, bool bReceive )
{
#ifdef RS232_TX
	UART_TxTail = 0;
	UART_TxHead = 0;
#endif

#ifdef RS232_RX
	UART_RxTail = 0;
	UART_RxHead = 0;
#endif

	UBRRH = nBaud >> 8;
	UBRRL = nBaud & 0xFF;

	UCSRA = 0;
	UCSRB = 0;

#if MCU != at90s4433
    // NOTE - URSEL is required by atmega32
	UCSRC = 
#if MCU == atmega32
        _BV(URSEL) | 
#endif
        _BV(UCSZ1) | _BV(UCSZ0);
#endif

#ifdef RS232_TX
	if ( bTransmit )
		UCSRB |=  _BV(TXEN);
#endif
#ifdef RS232_RX
	if ( bReceive )
		UCSRB |=  _BV(RXCIE) | _BV(RXEN);
#endif
}

void RS232SetParity( enum EParity eParity )
{
    switch ( eParity )
    {
        case eOdd:
            UCSRC = UCSRC | _BV(UPM1) | _BV(UPM0);
            break;
        case eEven:
            UCSRC = (UCSRC & ~ (_BV(UPM0))) | _BV(UPM1);
            break;
        case eNone:
        default:
            UCSRC = UCSRC & ~ (_BV(UPM1) | _BV(UPM0));
            break;
    }
}

void RS232SetStopBits( byte nBits )
{
    switch ( nBits )
    {
        case 2:
            UCSRC = UCSRC | _BV(USBS);
            break;
        case 1:
        default:
            UCSRC = UCSRC & ~ (_BV(USBS));
            break;
    }
}

void RS232SetCharSize( byte nBits )
{
    switch ( nBits )
    {
        case 5:
            UCSRC = UCSRC & ~(_BV(UCSZ1) | _BV(UCSZ0));
            UCSRB = UCSRB & ~(_BV(UCSZ2));
            break;
        case 6:
            UCSRC = (UCSRC & ~(_BV(UCSZ1)))  | _BV(UCSZ0);
            UCSRB = UCSRB & ~(_BV(UCSZ2));
            break;
        case 7:
            UCSRC = (UCSRC & ~(_BV(UCSZ0))) | _BV(UCSZ1);
            UCSRB = UCSRB & ~(_BV(UCSZ2));
            break;
        case 8:
        default:
            UCSRC = UCSRC | _BV(UCSZ1) | _BV(UCSZ0);
            UCSRB = UCSRB & ~(_BV(UCSZ2));
            break;
        case 9:
            UCSRC = UCSRC | _BV(UCSZ1) | _BV(UCSZ0);
            UCSRB = UCSRB | _BV(UCSZ2);
            break;
    }
}

#ifdef RS232_TX
	void RS232SendChar( byte data )
	{
		unsigned char tmphead;
		/* Calculate buffer index */
		tmphead = ( UART_TxHead + 1 ) & UART_TX_BUFFER_MASK; /* Wait for free space in buffer */
		while ( tmphead == UART_TxTail );

		UART_TxBuf[tmphead] = data;           /* Store data in buffer */
		UART_TxHead = tmphead;                /* Store new index */

		UCSRB |= (1<<UDRIE);                    /* Enable UDRE interrupt */
	}

	void RS232Send( char *str )
	{
		while ( *str != 0 )
		{
			RS232SendChar( *str );
			str++;
		}
	}

	void RS232Send_P( PGM_P s  )
	{
		char b;
		goto ReadByte;
		while ( b != 0 )
		{
			RS232SendChar(b );
			s++;
	ReadByte:
			b = pgm_read_byte( s );
		}
	}


	void RS232SendInt( int v )
	{
		char s[7];
		itoa( v, s, 10 );
		RS232Send( s );
	}

	void RS232SendLong( long v )
	{
		char s[11];
		ltoa( v, s, 10 );
		RS232Send( s );
	}

	void RS232SendHex( int v )
	{
		char s[5];
		itoa( v, s, 16 );
		RS232Send( s );
	}

	void RS232SendCRLF( void )
	{
		RS232SendChar( '\r' );
		RS232SendChar( '\n' );
	}
#endif



#ifdef RS232_RX

	byte RS232ReadByte( void )
	{
		byte tmptail;
		
		while ( UART_RxHead == UART_RxTail )  /* Wait for incomming data */
			;
		tmptail = ( UART_RxTail + 1 ) & UART_RX_BUFFER_MASK;/* Calculate buffer index */
		
		UART_RxTail = tmptail;                /* Store new index */
		
		return UART_RxBuf[tmptail];           /* Return data */
	}

	bool RS232ReadDataAvailable( void )
	{
		return ( UART_RxHead != UART_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
	}

#endif
