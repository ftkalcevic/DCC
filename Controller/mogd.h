#ifndef _mogd_h_
#define _mogd_h_

extern void mogd_Init( void );
extern void mogd_ClearScreen( void );
void mogd_SendChar( byte b );
void mogd_EnterFlowControlMode( uint8_t nFull, uint8_t nEmpty );
void mogd_SetDrawingColour( uint8_t c );
void mogd_Drawline( uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2 );
void mogd_DrawOutlineRectangle( byte colour, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2 );
void mogd_DrawSolidRectangle( byte colour, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2 );
void mogd_DisplaySavedBitmap( uint8_t nImage, uint8_t x, uint8_t y );
void mogd_SetBacklight( bool bOn );
void mogd_SetCurrentFont( byte nFont );
void mogd_ClearDisplay( void );
void mogd_PositionCursor( byte col, byte row );
void mogd_PositionPixelCursor( byte x, byte y );
void mogd_SetFontMetrics( uint8_t left_margin, uint8_t top_margin, uint8_t x_space, uint8_t y_space, uint8_t scroll_row );
void mogd_Send_P( PGM_P s  );
void mogd_Send( char *str );
void mogd_SendInt( int n );

#endif
