#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

#include "../../AVRCommon/Common.h"
#include "Serial.h"

#define ALMOST_EMPTY    0xFF
#define ALMOST_FULL     0xFE

#define ESCAPE_CHAR                 254

#define CLEAR_SCREEN                88
#define ENTER_FLOW_CONTROL_MODE     58
#define EXIT_FLOW_CONTROL_MODE      59
#define SET_TEXT_INSERTION_POINT    71
#define SET_TEXT_INSERTION_PIXEL    121
#define SET_CURRENT_FONT            49
#define BACKLIGHT_ON                66
#define BACKLIGHT_OFF               70
#define DRAW_LINE                   108
#define DRAW_OUTLINE_RECTANGLE      114
#define DRAW_SOLID_RECTANGLE        120
#define DISPLAY_SAVED_BITMAP        98
#define SET_DRAWING_COLOUR          99
#define SET_FONT_METRICS            50


static byte bFlowStop;

void mogd_SendChar( byte b )
{
    while ( bFlowStop || UART1_ReadDataAvailable() )
    {
        byte n = UART1_ReadByte();
        if ( n == ALMOST_EMPTY )
            bFlowStop = false;
        else if ( n == ALMOST_FULL )
            bFlowStop = true;
    }
    UART1_SendChar( b );
}

void mogd_ClearScreen( void )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( CLEAR_SCREEN );
}

void mogd_EnterFlowControlMode( uint8_t nFull, uint8_t nEmpty )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( ENTER_FLOW_CONTROL_MODE );
    mogd_SendChar( nFull );
    mogd_SendChar( nEmpty );
}


void mogd_SetDrawingColour( uint8_t c )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( SET_DRAWING_COLOUR );
    mogd_SendChar( c );
}

void mogd_DrawOutlineRectangle( byte colour, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2 )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( DRAW_OUTLINE_RECTANGLE );
    mogd_SendChar( colour );
    mogd_SendChar( x1 );
    mogd_SendChar( y1 );
    mogd_SendChar( x2 );
    mogd_SendChar( y2 );
}

void mogd_DrawSolidRectangle( byte colour, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2 )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( DRAW_SOLID_RECTANGLE );
    mogd_SendChar( colour );
    mogd_SendChar( x1 );
    mogd_SendChar( y1 );
    mogd_SendChar( x2 );
    mogd_SendChar( y2 );
}


void mogd_Drawline( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2 )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( DRAW_LINE );
    mogd_SendChar( x1 );
    mogd_SendChar( y1 );
    mogd_SendChar( x2 );
    mogd_SendChar( y2 );
}


void mogd_DisplaySavedBitmap( uint8_t nImage, uint8_t x, uint8_t y )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( DISPLAY_SAVED_BITMAP );
    mogd_SendChar( nImage );
    mogd_SendChar( x );
    mogd_SendChar( y );
}

void mogd_SetBacklight( bool bOn )
{
    mogd_SendChar( ESCAPE_CHAR );
    if ( bOn )
    {
        mogd_SendChar( BACKLIGHT_ON );
        mogd_SendChar( 0 );
    }
    else
    {
        mogd_SendChar( BACKLIGHT_OFF );
    }
}

void mogd_SetCurrentFont( byte nFont )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( SET_CURRENT_FONT );
    mogd_SendChar( nFont );
}

void mogd_ClearDisplay( void )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( CLEAR_SCREEN );
}

void mogd_PositionCursor( byte col, byte row )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( SET_TEXT_INSERTION_POINT );
    mogd_SendChar( col );
    mogd_SendChar( row );
}


void mogd_PositionPixelCursor( byte x, byte y )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( SET_TEXT_INSERTION_PIXEL );
    mogd_SendChar( x );
    mogd_SendChar( y );
}
void mogd_SetFontMetrics( uint8_t left_margin, uint8_t top_margin, uint8_t x_space, uint8_t y_space, uint8_t scroll_row )
{
    mogd_SendChar( ESCAPE_CHAR );
    mogd_SendChar( SET_FONT_METRICS );
    mogd_SendChar( left_margin );
    mogd_SendChar( top_margin );
    mogd_SendChar( x_space );
    mogd_SendChar( y_space );
    mogd_SendChar( scroll_row );
}

void mogd_Send_P( PGM_P s  )
{
	char b;
	goto ReadByte;
	while ( b != 0 )
	{
		mogd_SendChar( b );
		s++;
ReadByte:
		b = pgm_read_byte( s );
	}
}

void mogd_Send( char *str )
{
	while ( *str != 0 )
	{
		mogd_SendChar( *str );
		str++;
	}
}

void mogd_SendInt( int n )
{
	char s[7];
	itoa( n, s, 10 );
	mogd_Send( s );
}


void mogd_Init( void )
{
    bFlowStop = false;
    mogd_ClearScreen();
    mogd_EnterFlowControlMode( 10, 10 );
}

