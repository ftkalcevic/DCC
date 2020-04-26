
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/signal.h>

#include "../../AVRCommon/Common.h" 
#include "Serial.h"


/* UART Buffer Defines */
#ifdef UART0_RS232_TX
	#ifndef UART0_TX_BUFFER_SIZE
		#define UART0_TX_BUFFER_SIZE 128		/* 2,4,8,16,32,64,128 or 256 bytes */
	#endif
	#define UART0_TX_BUFFER_MASK ( UART0_TX_BUFFER_SIZE - 1 )
	#if ( UART0_TX_BUFFER_SIZE & UART0_TX_BUFFER_MASK )
		#error TX buffer size is not a power of 2
	#endif
 
	byte UART0_TxBuf[UART0_TX_BUFFER_SIZE];
	volatile byte UART0_TxHead;
	volatile byte UART0_TxTail;
#endif


#ifdef UART0_RS232_RX
	#ifndef UART0_RX_BUFFER_SIZE
		#define UART0_RX_BUFFER_SIZE 128		/* 2,4,8,16,32,64,128 or 256 bytes */
	#endif
	#define UART0_RX_BUFFER_MASK ( UART0_RX_BUFFER_SIZE - 1 )
	#if ( UART0_RX_BUFFER_SIZE & UART0_RX_BUFFER_MASK )
		#error RX buffer size is not a power of 2
	#endif

	byte UART0_RxBuf[UART0_RX_BUFFER_SIZE]; 
	volatile byte UART0_RxHead;
	volatile byte UART0_RxTail;
#endif


#ifdef UART1_RS232_TX
	#ifndef UART1_TX_BUFFER_SIZE
		#define UART1_TX_BUFFER_SIZE 128		/* 2,4,8,16,32,64,128 or 256 bytes */
	#endif
	#define UART1_TX_BUFFER_MASK ( UART1_TX_BUFFER_SIZE - 1 )
	#if ( UART1_TX_BUFFER_SIZE & UART1_TX_BUFFER_MASK )
		#error TX buffer size is not a power of 2
	#endif

	byte UART1_TxBuf[UART1_TX_BUFFER_SIZE];
	volatile byte UART1_TxHead;
	volatile byte UART1_TxTail;
#endif


#ifdef UART1_RS232_RX
	#ifndef UART1_RX_BUFFER_SIZE
		#define UART1_RX_BUFFER_SIZE 128		/* 2,4,8,16,32,64,128 or 256 bytes */
	#endif
	#define UART1_RX_BUFFER_MASK ( UART1_RX_BUFFER_SIZE - 1 )
	#if ( UART1_RX_BUFFER_SIZE & UART1_RX_BUFFER_MASK )
		#error RX buffer size is not a power of 2
	#endif

	byte UART1_RxBuf[UART1_RX_BUFFER_SIZE]; 
	volatile byte UART1_RxHead;
	volatile byte UART1_RxTail;
#endif


#ifdef UART0_RS232_TX

	SIGNAL (SIG_UART0_DATA)	// UART Data ready interrupt
	{ 
		byte tmptail;

		/* Check if all data is transmitted */
		if ( UART0_TxHead != UART0_TxTail )
		{
			/* Calculate buffer index */
			tmptail = ( UART0_TxTail + 1 ) & UART0_TX_BUFFER_MASK;
			UART0_TxTail = tmptail;      /* Store new index */
		
			UDR0 = UART0_TxBuf[tmptail];  /* Start transmition */
		}
		else
		{
			UCSR0B &= ~(1<<UDRIE);         /* Disable UDRE interrupt */
		}
	}

#endif

#ifdef UART0_RS232_RX

	SIGNAL (SIG_UART0_RECV)	// UART Data ready interrupt
	{ 
		byte data;
		byte tmphead;

		data = UDR0;                 /* Read the received data */
		/* Calculate buffer index */
		tmphead = ( UART0_RxHead + 1 ) & UART0_RX_BUFFER_MASK;
		UART0_RxHead = tmphead;      /* Store new index */

		if ( tmphead == UART0_RxTail )
		{
			/* ERROR! Receive buffer overflow */
		}
		
		UART0_RxBuf[tmphead] = data; /* Store received data in buffer */
	}

#endif

#ifdef UART0_RS232_TX
	void UART0_SendChar( byte data )
	{
		unsigned char tmphead;
		/* Calculate buffer index */
		tmphead = ( UART0_TxHead + 1 ) & UART0_TX_BUFFER_MASK; /* Wait for free space in buffer */
		while ( tmphead == UART0_TxTail );

		UART0_TxBuf[tmphead] = data;           /* Store data in buffer */
		UART0_TxHead = tmphead;                /* Store new index */

		UCSR0B |= (1<<UDRIE);                    /* Enable UDRE interrupt */
	}

	void UART0_Send( char *str )
	{
		while ( *str != 0 )
		{
			UART0_SendChar( *str );
			str++;
		}
	}

	void UART0_Send_P( PGM_P s  ) 
	{
		char b;
		goto ReadByte;
		while ( b != 0 )
		{
			UART0_SendChar(b );
			s++;
	ReadByte:
			b = pgm_read_byte( s );
		}
	}


	void UART0_SendInt( int v )
	{
		char s[7];
		itoa( v, s, 10 );
		UART0_Send( s );
	}

	void UART0_SendLong( long v )
	{
		char s[11];
		ltoa( v, s, 10 );
		UART0_Send( s );
	}

	void UART0_SendHex( int v )
	{
		char s[5];
		itoa( v, s, 16 );
		UART0_Send( s );
	}

	void UART0_SendCRLF( void )
	{
		UART0_SendChar( '\r' );
		UART0_SendChar( '\n' );
	}
#endif



#ifdef UART0_RS232_RX

	byte UART0_ReadByte( void )
	{
		byte tmptail;
		
		while ( UART0_RxHead == UART0_RxTail )  /* Wait for incomming data */
			;
		tmptail = ( UART0_RxTail + 1 ) & UART0_RX_BUFFER_MASK;/* Calculate buffer index */
		
		UART0_RxTail = tmptail;                /* Store new index */
		
		return UART0_RxBuf[tmptail];           /* Return data */
	}

	bool UART0_ReadDataAvailable( void )
	{
		return ( UART0_RxHead != UART0_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
	}

#endif

#if defined(UART0_RS232_RX) || defined(UART0_RS232_TX)

void UART0_Init( unsigned short nBaud )
{
#ifdef UART0_RS232_TX 
	UART0_TxTail = 0;
	UART0_TxHead = 0;
#endif

#ifdef UART0_RS232_RX
	UART0_RxTail = 0;
	UART0_RxHead = 0;
#endif

	UBRR0H = nBaud >> 8;
	UBRR0L = nBaud & 0xFF;
 
	UCSR0A = 0;
	UCSR0B = 0;

    // NOTE - URSEL is required by atmega32
	UCSR0C = _BV(UCSZ1) | _BV(UCSZ0);
#ifdef UART0_RS232_TX
	UCSR0B |=  _BV(TXEN);
#endif
#ifdef UART0_RS232_RX
	UCSR0B |=  _BV(RXCIE) | _BV(RXEN);
#endif
}

#endif


    
#ifdef UART1_RS232_TX

	SIGNAL (SIG_UART1_DATA)	// UART Data ready interrupt
	{ 
		byte tmptail;

		/* Check if all data is transmitted */
		if ( UART1_TxHead != UART1_TxTail )
		{
			/* Calculate buffer index */
			tmptail = ( UART1_TxTail + 1 ) & UART1_TX_BUFFER_MASK;
			UART1_TxTail = tmptail;      /* Store new index */
		
			UDR1 = UART1_TxBuf[tmptail];  /* Start transmition */
		}
		else
		{
			UCSR1B &= ~(1<<UDRIE);         /* Disable UDRE interrupt */
		}
	}

#endif

#ifdef UART1_RS232_RX

	SIGNAL (SIG_UART1_RECV)	// UART Data ready interrupt
	{ 
		byte data;
		byte tmphead;

		data = UDR1;                 /* Read the received data */
		/* Calculate buffer index */
		tmphead = ( UART1_RxHead + 1 ) & UART1_RX_BUFFER_MASK;
		UART1_RxHead = tmphead;      /* Store new index */

		if ( tmphead == UART1_RxTail )
		{
			/* ERROR! Receive buffer overflow */
		}
		
		UART1_RxBuf[tmphead] = data; /* Store received data in buffer */
	}

#endif

#ifdef UART1_RS232_TX
	void UART1_SendChar( byte data )
	{
		unsigned char tmphead;
		/* Calculate buffer index */
		tmphead = ( UART1_TxHead + 1 ) & UART1_TX_BUFFER_MASK; /* Wait for free space in buffer */
		while ( tmphead == UART1_TxTail );

		UART1_TxBuf[tmphead] = data;           /* Store data in buffer */
		UART1_TxHead = tmphead;                /* Store new index */

		UCSR1B |= (1<<UDRIE);                    /* Enable UDRE interrupt */
	}

	void UART1_Send( char *str )
	{
		while ( *str != 0 )
		{
			UART1_SendChar( *str );
			str++;
		}
	}

	void UART1_Send_P( PGM_P s  )
	{
		char b;
		goto ReadByte;
		while ( b != 0 )
		{
			UART1_SendChar(b );
			s++;
	ReadByte:
			b = pgm_read_byte( s );
		}
	}


	void UART1_SendInt( int v )
	{
		char s[7];
		itoa( v, s, 10 );
		UART1_Send( s );
	}

	void UART1_SendLong( long v )
	{
		char s[11];
		ltoa( v, s, 10 );
		UART1_Send( s );
	}

	void UART1_SendHex( int v )
	{
		char s[5];
		itoa( v, s, 16 );
		UART1_Send( s );
	}

	void UART1_SendCRLF( void )
	{
		UART1_SendChar( '\r' );
		UART1_SendChar( '\n' );
	}
#endif



#ifdef UART1_RS232_RX

	byte UART1_ReadByte( void )
	{
		byte tmptail;
		
		while ( UART1_RxHead == UART1_RxTail )  /* Wait for incomming data */
			;
		tmptail = ( UART1_RxTail + 1 ) & UART1_RX_BUFFER_MASK;/* Calculate buffer index */
		
		UART1_RxTail = tmptail;                /* Store new index */
		
		return UART1_RxBuf[tmptail];           /* Return data */
	}

	bool UART1_ReadDataAvailable( void )
	{
		return ( UART1_RxHead != UART1_RxTail ); /* Return 0 (FALSE) if the receive buffer is empty */
	}

#endif

#if defined( UART1_RS232_RX ) || defined( UART1_RS232_RX )

void UART1_Init( unsigned short nBaud )
{
#ifdef UART1_RS232_TX
	UART1_TxTail = 0;
	UART1_TxHead = 0;
#endif

#ifdef UART1_RS232_RX
	UART1_RxTail = 0;
	UART1_RxHead = 0;
#endif

	UBRR1H = nBaud >> 8;
	UBRR1L = nBaud & 0xFF;

	UCSR1A = 0;
	UCSR1B = 0;

	UCSR1C = _BV(UCSZ1) | _BV(UCSZ0);
#ifdef UART1_RS232_TX
	UCSR1B |=  _BV(TXEN);
#endif
#ifdef UART1_RS232_RX
	UCSR1B |=  _BV(RXCIE) | _BV(RXEN);
#endif
}

#endif

