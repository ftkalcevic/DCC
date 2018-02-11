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

#ifndef __AVR_SERIAL_H__
#define __AVR_SERIAL_H__
 
enum EParity
{
    eNone,
    eOdd,
    eEven
} ;

extern void InitRS232( unsigned short nBaud, bool bTransmit, bool bReceive );
extern void RS232SetParity( enum EParity eParity );
extern void RS232SetStopBits( byte nBits );
extern void RS232SetCharSize( byte nBits );

#ifdef RS232_TX
	extern void RS232SendChar( byte data );
	extern void RS232Send( char *str );
	extern void RS232Send_P( PGM_P s  );
	extern void RS232SendInt( int v );
	extern void RS232SendLong( long v );
	extern void RS232SendHex( int v );
	extern void RS232SendCRLF( void ); 
#endif
#ifdef RS232_RX
	extern byte RS232ReadByte( void );
	extern bool RS232ReadDataAvailable( void );
#endif


#endif
