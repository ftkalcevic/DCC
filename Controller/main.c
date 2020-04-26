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
#include <math.h> 

#include "../../AVRCommon/common.h"
#include "Serial.h"
#include "mogd.h"

const int8_t MILLISECOND_COUNTER = 256-250;
#define ONE_BIT_TIME        (58 * 2)        // PWM clock at 2 MHz, therefore, 58us = 58*2 ticks
#define ZERO_BIT_TIME       (100 * 2)



//////////////////////////////////////////////////
// Port Config
//////////////////////////////////////////////////
#define BEEPER_DDR      DDRB
#define BEEPER_PORT     PORTB
#define BEEPER_BIT      PB6

#define DCC_DDR      DDRB
#define DCC_PORT     PORTB
#define DCC_BIT      PB7

#define KNOB_DDR        DDRG
#define KNOB_PORT       PORTG
#define KNOB_PIN        PING
#define KNOB_A_BIT      PG3
#define KNOB_B_BIT      PG4

#define DECODER_ACK_DDR     DDRA
#define DECODER_ACK_PORT    PORTA
#define DECODER_ACK_PIN     PINA
#define DECODER_ACK_BIT     PA0



#define DCC_DECODER_CONSIST_CTL  0x00        // 000 Decoder and Consist Control Instruction
#define DCC_ADVANCED_OPER_INS    0x20        // 001 Advanced Operation Instructions
#define DCC_SPEED_DIR_REV        0x40        // 010 Speed and Direction Instruction for reverse operation
#define DCC_SPEED_DIR_FWD        0x60        // 011 Speed and Direction Instruction for forward operation
#define DCC_FUNCTION_GROUP_ONE   0x80        // 100 Function Group One Instruction
#define DCC_FUNCTION_GROUP_TWO   0xA0        // 101 Function Group Two Instruction
                                             // 110 Future Expansion
#define DCC_CONFIG_VAR_ACCESS    0xE0        // 111 Configuration Variable Access Instruction


//////////////////////////////////////////////////
// EEPROM layout
//////////////////////////////////////////////////
#define EE_LOCODB_COUNT         0
#define EE_ACCDB_COUNT          1
#define EE_LOCO_DATA_START      2
#define EE_ACC_DATA_START       0   // Immediately after last loco

//////////////////////////////////////////////////
// Structures
//////////////////////////////////////////////////
#define SPEED_14_FORWARD        0x60
#define SPEED_14_REVERSE        0x40
#define SPEED_28_FORWARD        0x40
#define SPEED_28_REVERSE        0x60
#define SPEED_128               0x3F
#define FUNCTION_GROUP_1        0x80
#define FUNCTION_GROUP_2        0xA0
#define FUNCTION_GROUP_SHIFT    0x10

struct SLocoMapEntry
{
    uint8_t nId;
    uint8_t sSpeed[5];
    uint8_t nSpeedRepeats;
    uint8_t sFunctionBlock1[4];
    uint8_t nFunctionBlock1Repeats;
    uint8_t sFunctionBlock2[4];
    uint8_t nFunctionBlock2Repeats;
    uint8_t sFunctionBlock3[4];
    uint8_t nFunctionBlock3Repeats;
    uint8_t nLastSendTime;
    uint8_t nCurrentDirection;
    uint8_t nCurrentSpeed;
    uint8_t nMaxSpeed;
    uint8_t nFunctions;
    uint16_t nFunctionsState;
    uint8_t nLastSent;
    uint8_t nLastSentMax;
    bool bLights;
};
enum ELastSent 
{
    LAST_SENT_SPEED=0,
    LAST_FUNCTION_BLOCK1,
    LAST_FUNCTION_BLOCK2,
    LAST_FUNCTION_BLOCK3,
    LAST_MAX
};

struct EnumMenu
{
	uint8_t nValue;
	PGM_P sName;
};

struct Menu
{
	PGM_P sName;
	byte eType;
	int nVal1;
	int nVal2;
	byte nEEPROMAddress;
};

enum EMenuItemTypes
{
	MENU_TYPE_ENUM,
	MENU_TYPE_SUBMENU,
	MENU_TYPE_INT,
	MENU_TYPE_FUNC
};

typedef void (* MENU_FUNC_PTR)( void );

enum ESpeedSteps
{
    SPEED_STEPS_14,
    SPEED_STEPS_28,
    SPEED_STEPS_128,
};

enum EFunctionType
{
    FUNCTION_TOGGLE,
    FUNCTION_MOMENTARY
};

enum EDecoderType
{
    DECODER_TYPE_LOCO,
    DECODER_TYPE_ACC,
};

enum EAccessoryType
{
    ACC_TYPE_SINGLE,
    ACC_TYPE_PAIR
};

#define FUNCTION_NAME_LEN   9
struct SFunctions
{
    uint8_t nFunctionId;
    char sName1[FUNCTION_NAME_LEN+1];
    char sName2[FUNCTION_NAME_LEN+1];
    enum EFunctionType eType;
};

#define LOCO_NAME_LEN       28
#define MAX_FUNCTIONS       12
struct SLoco
{
    char sName[LOCO_NAME_LEN+1];
    uint16_t nAddress;
    enum ESpeedSteps eSpeedSteps;
    uint8_t nFunctions;
    struct SFunctions funcs[MAX_FUNCTIONS];
};


#define ACC_NAME_LEN       16
struct SAcc
{
    char sName[ACC_NAME_LEN+1];
    uint16_t nAddress;
    uint8_t nSubAddress;
    uint8_t nType;
};



//////////////////////////////////////////////////
// Variables
//////////////////////////////////////////////////


static const char sMomentary[] PROGMEM = "Momentary";
static const char sToggle[] PROGMEM =    "Toggle   ";

static struct EnumMenu LocoFunctionTypesEnum[] PROGMEM =
{
    { FUNCTION_TOGGLE, sToggle },
    { FUNCTION_MOMENTARY, sMomentary }
};


static const char sDecoderTypeLoco[] PROGMEM = "Locomotive";
static const char sDecoderTypeAcc[] PROGMEM =    "Accessory ";

static struct EnumMenu DecoderTypesEnum[] PROGMEM =
{
    { DECODER_TYPE_LOCO, sDecoderTypeLoco },
    { DECODER_TYPE_ACC, sDecoderTypeAcc }
};


static const char sAccTypeSingle[] PROGMEM = "Single";
static const char sAccTypePair[] PROGMEM =   "Pair  ";

static struct EnumMenu AccessoryTypesEnum[] PROGMEM =
{
    { ACC_TYPE_SINGLE, sAccTypeSingle },
    { ACC_TYPE_PAIR, sAccTypePair }
};



static const char sSS14[] PROGMEM = "14 ";
static const char sSS28[] PROGMEM = "28 ";
static const char sSS128[] PROGMEM = "128";


static struct EnumMenu SpeedStepsEnum[] PROGMEM =
{
    { SPEED_STEPS_14, sSS14 },
    { SPEED_STEPS_28, sSS28 },
    { SPEED_STEPS_128, sSS128 },
};


static const char sLocoDB[] PROGMEM = "Locomotive Database";
static const char sAccessoryDB[] PROGMEM = "Accessory Database";

extern void HandleLocoDB( void );
extern void HandleAccessoryDB( void );

static struct Menu SetupMenu[] PROGMEM =
{
};

static const char sSetupMenu[] PROGMEM = "Setup";
static const char sFunctionProgram[] PROGMEM = "Program CVs";

extern void HandleProgram( void );

static struct Menu MainMenu[] PROGMEM =
{
	{ sFunctionProgram, MENU_TYPE_FUNC,  (int)&HandleProgram, 0, 0 },
	{ sLocoDB, MENU_TYPE_FUNC, (int)&HandleLocoDB, 0, 0 },
	{ sAccessoryDB, MENU_TYPE_FUNC, (int)&HandleAccessoryDB, 0, 0 },
	{ sSetupMenu, MENU_TYPE_SUBMENU, (int)SetupMenu, sizeof(SetupMenu)/sizeof(SetupMenu[0]), 0 },
};

static int16_t nSpeedCount;
static byte nLastQuadValue;

static volatile uint8_t nBeeper;
static uint16_t msec;
static volatile uint16_t sec;
static uint8_t tenths;
static volatile uint8_t nCharTimer;
static volatile bool bCharTimerExpired;
static volatile uint8_t nCursorFlashTimer;
static bool bCursorInOnState;
static bool bCursorVisible;
static bool bInvertedCursor;

static uint8_t nLocoMapCount;
static uint8_t nLastTransmitLoco;
static uint8_t nCurrentLoco;
static uint8_t nFunctionShift; 
static struct SLocoMapEntry LocoMap[10];
static volatile bool bFunctionsChanged;
static volatile bool bSpeedChanged;
static volatile bool bDirectionChanged;
static volatile bool bLocoChanged;
static volatile bool nSpecialRepeat;
static byte DCC_Special[6];
static byte nOldDisplaySpeed;
static byte nCurrentAccessoryId;

#define CHAR_REPEAT_TIMER       10     // 1 second till auto move to next character
#define CURSOR_FLASH_TIMER      5

#define DCC_BUFFER_SIZE 64 
#define DCC_BUFFER_MASK ( DCC_BUFFER_SIZE - 1 )
#if ( DCC_BUFFER_SIZE & DCC_BUFFER_MASK )
	#error DCC buffer size is not a power of 2
#endif

byte DCC_Buf[DCC_BUFFER_SIZE];
volatile byte DCC_Head;
volatile byte DCC_Tail;

#define KEY_FORWARD         'f'
#define KEY_BRAKE           'b'
#define KEY_REVERSE         'r'
#define KEY_EMERGENCY_STOP  'e'
#define KEY_STOP_ALL        'a'
#define KEY_MENU            'm'
#define KEY_TOGGLE_LOCO     't'
#define KEY_SWITCH_SELECT   'w'
#define KEY_SELECT_LOCO     's'
#define KEY_FUNCTION        'n'
#define KEY_1               '1'
#define KEY_2               '2'
#define KEY_3               '3'
#define KEY_4               '4'
#define KEY_5               '5'
#define KEY_6               '6'
#define KEY_7               '7'
#define KEY_8               '8'
#define KEY_9               '9'
#define KEY_0               '0'
#define KEY_ASTERISK        '*'
#define KEY_UPPER           '+'
#define KEY_ENTER           13
#define KEY_CANCEL          27
#define KEY_UP              'u'
#define KEY_DOWN            'd'
//#define KEY_RIGHT           'l'
//#define KEY_LEFT            'h'
#define KEY_F1              '!'
#define KEY_F2              '@'
#define KEY_F3              '#'
#define KEY_F4              '$'
#define KEY_FL              'l'

// Pseudo Keys
#define KEY_TIMER_EXIRED_EVENT  0xFF
#define KEY_SPEED_CHANGE        0xFE

extern void RunMenu( const struct Menu * PROGMEM pMenu, byte nCount, byte nLine, byte nEEPROMOffset );
extern byte get_char( void );
extern void FlashCursor( uint8_t x, uint8_t y );
extern void DrawLocos( uint8_t nLocos, uint8_t nItem, uint8_t nBlockOffset, bool bExtraLine );
extern byte EditInt( byte col, byte row, int *nValue, int nMin, int nMax );
extern byte EditButton( uint8_t pixel_x, uint8_t pixel_y, PGM_P sButton );
extern byte EditEnum( byte col, byte row, byte *nOrigValue, const struct EnumMenu * PROGMEM pEnum, byte nCount );
extern void DoMsg( PGM_P sMsg );
extern void DumpMsg( char *sBuffer );
void Function( byte nFunc );
void FunctionShift( void );
void ToggleLoco( void );
void DisplayFunctionKeys( void );
void DriveLoco( uint8_t nId );
void SendIdlePacket( void );
void FunctionLights(void);


static struct SLoco CurrentLoco;



SIGNAL (SIG_OVERFLOW0)	// Timer0 overflow interupt.
{
    TCNT0 = MILLISECOND_COUNTER;

    msec++;
    if ( msec == 1000 )
    {
        sec++;
        msec = 0;
    }

    tenths++;
    if ( tenths == 100 )
    {
        tenths = 0;
        if ( nCharTimer > 0 )
        {
            nCharTimer--;
            if ( nCharTimer == 0 )
                bCharTimerExpired = true;
        }
        if ( nCursorFlashTimer > 0 )
            nCursorFlashTimer--;
    }

    if ( nBeeper != 0 )
    {
        nBeeper--;
        if ( nBeeper == 0 )
            BEEPER_PORT &= ~_BV(BEEPER_BIT);
    }
}

static void SetNextBit( byte bBit )
{
    if ( bBit != 0 )
    {
        OCR1A = ONE_BIT_TIME * 2;
        OCR1C = ONE_BIT_TIME;
    }
    else
    {
        OCR1A = ZERO_BIT_TIME * 2;
        OCR1C = ZERO_BIT_TIME;
    }
}

enum 
{
    IDLE = 0,
    SEND_PREAMBLE,
    SEND_DATA
} DCC_State;

#define PREAMBLE_BITS           14
#define END_OF_PREAMBLE_BIT     0
#define END_OF_BYTE_BIT         0
#define END_OF_PACKET_BIT       1

extern void SendDCCMsg( char *sBuffer );

SIGNAL (SIG_OVERFLOW1)	// Timer1 overflow interupt.  counter back to BOTTOM
{
    static byte nDCCSendBytes;
    static byte nDCCByte;
    static byte nDCCBit;
    static byte nDCCError;
    static byte nPreambleCount;
    static bool bErrorByteSent;

    // Idle
    if ( DCC_State == IDLE )
    {
        SetNextBit( 1 );

        // New data?
        if ( DCC_Head != DCC_Tail )
        {
            // We could short cut this, because idle time is the same as a preamble, but
            // doing it this way protects us from bytes not being in the queue in time for an interrupt.
            DCC_State = SEND_PREAMBLE;
            nPreambleCount = PREAMBLE_BITS;
        }
        else
        {
            if ( nSpecialRepeat > 0 )
            {   
                nSpecialRepeat--;
                SendDCCMsg( DCC_Special );
            }
            else  if ( nLocoMapCount > 0 )
            {
                // Work through the LocoMap to determine which one to send next
                nLastTransmitLoco++;
                if ( nLastTransmitLoco >= nLocoMapCount )
                    nLastTransmitLoco = 0;

                // If there is a repeat count, do that first
                if ( LocoMap[nLastTransmitLoco].nSpeedRepeats > 0 )
                {
                    LocoMap[nLastTransmitLoco].nSpeedRepeats--;
                    SendDCCMsg( LocoMap[nLastTransmitLoco].sSpeed );
                }
                else if ( LocoMap[nLastTransmitLoco].nFunctionBlock1Repeats > 0 )
                {
                    LocoMap[nLastTransmitLoco].nFunctionBlock1Repeats--;
                    SendDCCMsg( LocoMap[nLastTransmitLoco].sFunctionBlock1 );
                }
                else if ( LocoMap[nLastTransmitLoco].nFunctionBlock2Repeats > 0 )
                {
                    LocoMap[nLastTransmitLoco].nFunctionBlock2Repeats--;
                    SendDCCMsg( LocoMap[nLastTransmitLoco].sFunctionBlock2 );
                }
                else if ( LocoMap[nLastTransmitLoco].nFunctionBlock3Repeats > 0 )
                {
                    LocoMap[nLastTransmitLoco].nFunctionBlock3Repeats--;
                    SendDCCMsg( LocoMap[nLastTransmitLoco].sFunctionBlock3 );
                }
                else
                {
                    switch ( LocoMap[nLastTransmitLoco].nLastSent )
                    {
                        default:
                        case LAST_SENT_SPEED:
                            SendDCCMsg( LocoMap[nLastTransmitLoco].sSpeed ); break;
                        case LAST_FUNCTION_BLOCK1:
                            if( LocoMap[nLastTransmitLoco].nFunctions > 0 )
                                SendDCCMsg( LocoMap[nLastTransmitLoco].sFunctionBlock1 ); 
                            break;
                        case LAST_FUNCTION_BLOCK2:
                            if( LocoMap[nLastTransmitLoco].nFunctions > 4 )
                                SendDCCMsg( LocoMap[nLastTransmitLoco].sFunctionBlock2 ); 
                            break;
                        case LAST_FUNCTION_BLOCK3:
                            if( LocoMap[nLastTransmitLoco].nFunctions > 8 )
                                SendDCCMsg( LocoMap[nLastTransmitLoco].sFunctionBlock3 ); 
                            break;
                    }

                    LocoMap[nLastTransmitLoco].nLastSent++;
                    if ( LocoMap[nLastTransmitLoco].nLastSent == LocoMap[nLastTransmitLoco].nLastSentMax )
                        LocoMap[nLastTransmitLoco].nLastSent = LAST_SENT_SPEED;
                }
            }
            else
            {
                SendIdlePacket();
            }
        }
    }
    // Still sending the preamble
    else if ( DCC_State == SEND_PREAMBLE )
    {
        nPreambleCount--;
        if ( nPreambleCount == 0 )
        {
            SetNextBit( END_OF_PREAMBLE_BIT );
            DCC_State = SEND_DATA;

            // Length of data to send
		    byte tmptail = ( DCC_Tail + 1 ) & DCC_BUFFER_MASK;
		    nDCCSendBytes = DCC_Buf[tmptail] - 1;
            bErrorByteSent = false;

            // First byte to send
		    tmptail = ( tmptail + 1 ) & DCC_BUFFER_MASK;
            nDCCByte = DCC_Buf[tmptail];
		    DCC_Tail = tmptail;      /* Store new index */
		    nDCCError = nDCCByte;
            nDCCBit = 8;
        }
        else
        {
            SetNextBit( 1 );
        }
    }
    else if ( DCC_State == SEND_DATA )
    {
        // End of byte. ?
        if ( nDCCBit == 0 )
        {
            // if we are finshed, send an END_OF_PACKET bit, else, END_OF_BYTE bit
            if ( nDCCSendBytes == 0 && bErrorByteSent )
            {
                SetNextBit( END_OF_PACKET_BIT );
                DCC_State = IDLE;
            }
            else
            {
                SetNextBit( END_OF_BYTE_BIT );
                DCC_State = SEND_DATA;

                if ( nDCCSendBytes == 0 )   
                {
                    // Sent all data, send error byte
		            nDCCByte = nDCCError;
                    bErrorByteSent = true;
                }
                else
                {
                    // regular data
		            byte tmptail = ( DCC_Tail + 1 ) & DCC_BUFFER_MASK;
		            DCC_Tail = tmptail;      /* Store new index */
		            nDCCByte = DCC_Buf[tmptail];
                    nDCCError ^= nDCCByte;
                    nDCCSendBytes--;
                }
                nDCCBit = 8;
            }
        }
        else
        {
            nDCCBit--;
            if ( nDCCByte & (1<<nDCCBit) )
            {
                SetNextBit( 1 );
            }
            else
            {
                SetNextBit( 0 );
            }
        }
    }
}

void SetFontNormal( void )
{
    mogd_SetCurrentFont( 1 );
}

void SetFontInverse( void )
{
    mogd_SetCurrentFont( 2 );
}

void EraseLine( byte nLine )
{
    mogd_ClearDisplay();
}


void DCC_SendChar( byte data )
{
	unsigned char tmphead;
	/* Calculate buffer index */
	tmphead = ( DCC_Head + 1 ) & DCC_BUFFER_MASK; /* Wait for free space in buffer */
	while ( tmphead == DCC_Tail )
        {
            byte a = DCC_Head;
            byte b = DCC_Tail;
            sei();
            UART0_Send_P( PSTR("DCC buffer full") );
            UART0_Send_P( PSTR(" Head ") );
            UART0_SendInt( a );
            UART0_Send_P( PSTR(" Tail ") );
            UART0_SendInt( b );
            UART0_Send_P( PSTR(" Tmp head ") );
            UART0_SendInt( tmphead );
a: goto a;
        }

	DCC_Buf[tmphead] = data;           /* Store data in buffer */
	DCC_Head = tmphead;                /* Store new index */
}

void SendEmergencyStopAll( void )
{
    DCC_Special[0] = 2;
    DCC_Special[1] = 0;
    DCC_Special[2] = 1;
    nSpecialRepeat = 5;
}

void SendStopAll( void )
{
    DCC_Special[0] = 2;
    DCC_Special[1] = 0;
    DCC_Special[2] = 0x40;
    nSpecialRepeat = 5;
}

void SendIdlePacket( void )
{
    DCC_SendChar( 2 );
    DCC_SendChar( 0xFF );
    DCC_SendChar( 0 );
}

void SendDCCMsg( char *sBuffer )
{
    uint8_t nLen = sBuffer[0]+1;
    for ( uint8_t i = 0; i < nLen; i++ )
        DCC_SendChar( sBuffer[i] );
}

void Beep( byte ms )
{
    cli();
    nBeeper = ms;
    BEEPER_PORT |= _BV(BEEPER_BIT);
    sei();
}

void DrawItem( const struct Menu * PROGMEM pMenuItem, byte nLine )
{
	EraseLine(nLine);
    SetFontNormal();
	mogd_PositionCursor( 1, nLine );
	PGM_P s = (PGM_P)pgm_read_word( &(pMenuItem->sName) );
	mogd_Send_P( s );
}

void DrawItemValue( byte nType, byte nEEProm, byte nLine, int nVal1, int nVal2 )
{
	switch ( nType )
	{
		//case MENU_TYPE_ENUM:
		//	{
		//		byte nCount = (byte)(uint16_t)nVal2;
		//		byte nValue = eeprom_read_byte( (uint8_t *)(int)nEEProm );
		//		if ( nValue >= nCount )
		//			nValue = 0;
		//		const struct EnumMenu * PROGMEM pEnum = (const struct EnumMenu * PROGMEM)nVal1;
		//		DrawItemValueEnum( pEnum+nValue, nLine );
		//		break;
		//	}

		case MENU_TYPE_SUBMENU:
			{
	            //EraseLine( nLine );
	            //mogd_PositionCursor( 1, nLine );
				break;
			}

//		case MENU_TYPE_INT:
//			{
//	            EraseLine( nLine );
//	            mogd_PositionCursor( 1, nLine );
//				int nValue = (int)eeprom_read_word( (uint16_t *)(int)nEEProm );
//DEBUG_START;
//	RS232Send_P( PSTR("DrawItemValue eeprom is " ) ); RS232SendInt(nEEProm); RS232SendCRLF();
//	RS232Send_P( PSTR("DrawItemValue Int from eeprom is " ) ); RS232SendInt(nValue); RS232SendCRLF();
//DEBUG_END;	
//				if ( nValue < nVal1 || nValue > nVal2 )
//					nValue = nVal1;
//DEBUG_START;
//	RS232Send_P( PSTR("DrawItemValue Int is " ) ); RS232SendInt(nValue); RS232SendCRLF();
//DEBUG_END;	
//				mogd_SendInt( nValue );
//				break;
//			}
	}
}


char HandleItem( const struct Menu * PROGMEM pMenuItem, byte nLine, byte nEEPROMOffset )
{
	byte nType = pgm_read_byte( &(pMenuItem->eType) );
	int nVal1 = pgm_read_word( &(pMenuItem->nVal1) );
	int nVal2 = pgm_read_word( &(pMenuItem->nVal2) );
	byte nEEProm = pgm_read_byte( &(pMenuItem->nEEPROMAddress) );
	nEEProm += nEEPROMOffset;

	DrawItemValue( nType, nEEProm, nLine, nVal1, nVal2 );
	for (;;)
	{
		char c = get_char();
		if ( c == KEY_MENU || c == KEY_UP || c == KEY_DOWN )
		{
			EraseLine( nLine+1 );
			return c;
		}
		if ( c == KEY_ENTER )
		{
			switch ( nType )
			{
				case MENU_TYPE_ENUM:
					//HandleEnum( (const struct EnumMenu * PROGMEM)nVal1, nVal2, nLine, nEEProm );
					break;

				case MENU_TYPE_SUBMENU:
					RunMenu( (const struct Menu * PROGMEM )nVal1, nVal2, nLine, nEEProm );
                    return -1;
					break;

				case MENU_TYPE_FUNC:
					(((MENU_FUNC_PTR)nVal1))();
                    return -1;
					break;

				case MENU_TYPE_INT:
					//HandleInt( nVal1, nVal2, nLine, nEEProm );
					break;
			}
		}
	}
}



void RunMenu( const struct Menu * PROGMEM pMenu, byte nCount, byte nLine, byte nEEPROMOffset )
{
	byte nItem = 0;

	for (;;)
	{
		DrawItem( pMenu+nItem, nLine );
		char c = HandleItem( pMenu+nItem, nLine + 1, nEEPROMOffset );

		if ( c == KEY_MENU )
		{
			break;
		}
		else if ( c == KEY_UP )
		{
			if ( nItem == 0 )
				nItem = nCount - 1;
			else
				nItem--;
		}
		else if ( c == KEY_DOWN )
		{
			if ( nItem == nCount - 1 )
				nItem = 0;
			else
				nItem++;
		}
	}
}

void UpdateCurrentSpeed( void )
{
    if ( CurrentLoco.eSpeedSteps == SPEED_STEPS_128 )
        LocoMap[nCurrentLoco].sSpeed[LocoMap[nCurrentLoco].sSpeed[0]] = (LocoMap[nCurrentLoco].nCurrentSpeed & 0x7F) | (LocoMap[nCurrentLoco].nCurrentDirection ? 0x80: 0);
    else if ( CurrentLoco.eSpeedSteps == SPEED_STEPS_14 )
        LocoMap[nCurrentLoco].sSpeed[LocoMap[nCurrentLoco].sSpeed[0]] = (LocoMap[nCurrentLoco].nCurrentDirection ? SPEED_14_FORWARD: SPEED_14_REVERSE) | (LocoMap[nCurrentLoco].nCurrentSpeed & 0x0F);
    else if ( CurrentLoco.eSpeedSteps == SPEED_STEPS_28 )
        LocoMap[nCurrentLoco].sSpeed[LocoMap[nCurrentLoco].sSpeed[0]] = (LocoMap[nCurrentLoco].nCurrentDirection ? SPEED_28_FORWARD: SPEED_28_REVERSE) | (LocoMap[nCurrentLoco].nCurrentSpeed & 0x1F);
    LocoMap[nCurrentLoco].nSpeedRepeats = 5;

    UART0_Send_P( PSTR("Speed=") );
    for ( uint8_t i = 0; i < LocoMap[nCurrentLoco].sSpeed[0]; i++ )
    {
        UART0_SendHex( LocoMap[nCurrentLoco].sSpeed[i+1] );
        UART0_SendChar( ' ' );
    }
    UART0_SendCRLF();
}

bool CheckSpeedContol( void )
{
    bool bChange = false;
    if ( nLocoMapCount > 0 )
    {
        byte b = KNOB_PIN & (_BV(KNOB_A_BIT) | _BV(KNOB_B_BIT));
        
        if ( b != nLastQuadValue )
        {
            // The output is 2 signals 90 degrees out of phase.  For the PIC, I set up an interrupt on change of port B, 
            // and had the encoder channels connected to port B.  When an interrupt occurs, use an EXCLUSIVE OR from the 
            // previous channel 1 to the new channel 2, it will give you a zero for one direction and a 1 for the other 
            // direction.  Then increment or decrement your counter based on the results of the Exclusive OR.
            byte b1 = (b >> KNOB_A_BIT) & 1;
            byte b2 = (nLastQuadValue >> KNOB_B_BIT) & 1;
            if ( b1 ^ b2 )
            {
                if ( LocoMap[nCurrentLoco].nCurrentSpeed > 0 )
                {
                    bSpeedChanged = true;
                    bChange = true;
                    if ( LocoMap[nCurrentLoco].nCurrentSpeed == 2 )
                    {
                        LocoMap[nCurrentLoco].nCurrentSpeed = 0;
                    }
                    else
                    {
                        LocoMap[nCurrentLoco].nCurrentSpeed--;
                    }
                    UpdateCurrentSpeed();
                }
            }
            else
            {
                if ( LocoMap[nCurrentLoco].nCurrentSpeed < LocoMap[nCurrentLoco].nMaxSpeed - 1 )
                {
                    bSpeedChanged = true;
                    bChange = true;
                    if ( LocoMap[nCurrentLoco].nCurrentSpeed == 0 )
                    {
                        LocoMap[nCurrentLoco].nCurrentSpeed = 2;
                    }
                    else
                    {
                        LocoMap[nCurrentLoco].nCurrentSpeed++;
                    }
                    UpdateCurrentSpeed();
                }
            }
            nLastQuadValue = b;
        }
    }
    return bChange;
}

void ioinit( void )
{
	/////////////////////////
	//// Set IO registers
	/////////////////////////

	DDRA = 0;
	DDRB = 0;
	DDRD = 0;
	DDRE = 0;
	DDRF = 0;
	DDRG = 0;

    BEEPER_DDR |= OUTPUT(BEEPER_BIT);
    BEEPER_PORT &= ~_BV(BEEPER_BIT);

    DCC_DDR |= OUTPUT(DCC_BIT);
    DCC_PORT &= ~_BV(DCC_BIT);

    DDRG = INPUT(KNOB_A_BIT) | INPUT(KNOB_B_BIT);
    KNOB_PORT |= _BV(KNOB_A_BIT) | _BV(KNOB_B_BIT);

	//ENCODER_DDR = 0;		// All inputs
	//ENCODER_PORT = 0xFF;

	//BUZZER_DDR |= _BV(BUZZER_BIT);
	//BUZZER_PORT &= ~_BV(BUZZER_BIT);


	//// PE2-7 are the key pad strobe lines
	//KEYPAD_OUT_DDR =  OUTPUT(0) | OUTPUT(2) | OUTPUT(3) | OUTPUT(4) | OUTPUT(5) | OUTPUT(6);
	//// PB0.2.3.4.5.6 are they keypad return lines
	//KEYPAD_IN_DDR = INPUT(2) | INPUT(3) | INPUT(4) | INPUT(5) | INPUT(6) | INPUT(7);
	//KEYPAD_IN_PIN = KEYPAD_IN_MASK;

    DECODER_ACK_DDR = INPUT(DECODER_ACK_BIT);
    DECODER_ACK_PORT |= _BV(DECODER_ACK_BIT);        // Pull up

	///////////////////////////
	////// Set Timers
	///////////////////////////

	//// encoder poll timer (timer2)
	//TCCR2 = _BV(CS20);		// CPU_CLK / 256 = 16MHz/256 = 62500Hz
	//TIMSK |= _BV(TOIE2);

	//// msec timer
    TCCR0 = _BV(CS02);	// CLK/64
	TCNT0 = MILLISECOND_COUNTER;
	TIMSK |= _BV(TOIE0);

    // DCC PWM timer1
    // Fast PWM, set OCR1C = n, TOP = OCR1A = 2n, set on OCR match, clear on overflow
    TCCR1A = _BV(COM1C1) | _BV(COM1C0) | _BV(WGM11) | _BV(WGM10);
    TCCR1B = _BV(CS11) | _BV(WGM13) | _BV(WGM12);   // clk/8 - 2MHz
	TIMSK |= _BV(TOIE1);


    OCR1A = 0xFFFF;
    OCR1C = 0x7FFF;
}

void Forward( void ) 
{
    if ( nLocoMapCount > 0 ) 
    {
        LocoMap[nCurrentLoco].nCurrentDirection = 1;
        UpdateCurrentSpeed();
        bDirectionChanged = true;
    }
}

void Brake( void ) 
{
    if ( nLocoMapCount > 0 ) 
    {
        LocoMap[nCurrentLoco].nCurrentSpeed = 0;
        UpdateCurrentSpeed();
        bSpeedChanged = true;
    }
}

void Reverse( void ) 
{
    if ( nLocoMapCount > 0 ) 
    {
        LocoMap[nCurrentLoco].nCurrentDirection = 0;
        UpdateCurrentSpeed();
        bDirectionChanged = true;
    }
}

void EmergencyStop( void ) 
{
    cli();
    for ( byte i = 0; i < 4; i++ )
        SendEmergencyStopAll();
    sei();

    if ( nLocoMapCount > 0 ) 
    {
        for ( byte i = 0; i < nLocoMapCount; i++ )
        {
            LocoMap[0].nCurrentSpeed = 1;
        }
        UpdateCurrentSpeed();
        bSpeedChanged = true;
    }
}

void StopAll( void ) 
{
    cli();
    for ( byte i = 0; i < 4; i++ )
        SendStopAll();
    sei();

    if ( nLocoMapCount > 0 ) 
    {
        for ( byte i = 0; i < nLocoMapCount; i++ )
        {
            LocoMap[0].nCurrentSpeed = 0;
        }
        UpdateCurrentSpeed();
        bSpeedChanged = true;
    }
}

void Menu( void ) 
{
    RunMenu( MainMenu, sizeof(MainMenu)/sizeof(MainMenu[0]), 1, 0 );
}

bool kbhit( void )
{
    return UART0_ReadDataAvailable();
}

byte get_char( void )
{
    //for (;;)
    {
        while ( !kbhit() )
        {
            if ( bCharTimerExpired )
                return KEY_TIMER_EXIRED_EVENT;

            if ( bCursorVisible && nCursorFlashTimer == 0 )
            {
                bCursorInOnState = !bCursorInOnState;
                nCursorFlashTimer = CURSOR_FLASH_TIMER;
                FlashCursor(-1,-1);
            }

            if ( CheckSpeedContol() )
                return KEY_SPEED_CHANGE;
        }

        byte c = UART0_ReadByte();
        switch ( c )
        {
            case KEY_FORWARD:
                Forward();
                break;
            case KEY_BRAKE:
                Brake();
                break;
            case KEY_REVERSE:
                Reverse();
                break;
            case KEY_TOGGLE_LOCO:
                ToggleLoco();
                break;
            case KEY_EMERGENCY_STOP:
                EmergencyStop();
                break;
            case KEY_STOP_ALL:
                StopAll();
                break;
            case KEY_F1:
                Function( 0 );
                break;
            case KEY_F2:
                Function( 1 );
                break;
            case KEY_F3:
                Function( 2 );
                break;
            case KEY_F4:
                Function( 3 );
                break;
            case KEY_FL:
                FunctionLights();
                break;
            case KEY_FUNCTION:
                FunctionShift();
                break;
        }
        return c;
    }
}

//#define CV29_CONFIGURATION_DATA         (29-1)
//
//void DisplayConfigDataCV( uint8_t nData )
//{
//    mogd_PositionCursor(16,1);
//    mogd_Send_P( PSTR("Unused     0") );
//    mogd_PositionCursor(16,2);
//    mogd_Send_P( PSTR("Unused     0") );
//    mogd_PositionCursor(16,3);
//    mogd_Send_P( PSTR("Addressing 1") );
//    mogd_PositionCursor(16,4);
//    mogd_Send_P( PSTR("Table      1") );
//    mogd_PositionCursor(16,5);
//    mogd_Send_P( PSTR("Unused     1") );
//    mogd_PositionCursor(16,6);
//    mogd_Send_P( PSTR("Analog     1") );
//    mogd_PositionCursor(16,7);
//    mogd_Send_P( PSTR("SpeedSteps 1") );
//    mogd_PositionCursor(16,8);
//    mogd_Send_P( PSTR("Direction  1") );
//}
//
//bool isBitCV( uint8_t nCV )
//{
//    if ( nCV == CV29_CONFIGURATION_DATA )
//        return true;
//    else 
//        return false;
//}
//void DisplayCV( uint8_t nCV, uint8_t nData, bool bBitCV )
//{
//    if ( !bBitCV )
//    {
//        mogd_PositionCursor(15,1);
//        mogd_Send_P( PSTR("Data") );
//        mogd_PositionCursor(20,1);
//        mogd_SendInt( nData );
//    }
//    else
//    {
//        switch ( nCV )
//        {
//            case CV29_CONFIGURATION_DATA:
//                DisplayConfigDataCV( nData );
//                break;
//        }
//    }
//}


bool WaitForAck( void )
{
    // Wait for quiet
    for ( int i = 0; i < 2000; i++ )
    {
        byte b = DECODER_ACK_PIN & _BV(DECODER_ACK_BIT);
        if ( b != 0 )
        {
            UART0_SendChar('Q');
            break;
        }
        UART0_SendChar('q');
        _delay_us( CPU_CLK, 500 );  // Check ever .5 ms
    }

    // Wait until DCC queue is empty.
    UART0_SendChar('d');
    while ( DCC_Head != DCC_Tail ) {};
    UART0_SendChar('D');

    // Wait for up to 20 ms for the ack
    bool bAckStart = false;
    for ( int i = 0; i < 40; i++ )
    {
        byte b = DECODER_ACK_PIN & _BV(DECODER_ACK_BIT);
        if ( b == 0 )
        {
            bAckStart = true;
            UART0_SendChar('S');
            break;
        }
        _delay_us( CPU_CLK, 500 );  // Check ever .5 ms
        UART0_SendChar('s');
    }

    if ( bAckStart )
    {
        // Wait atleast 5ms
        bool bNotAck = false;
        for ( byte i = 0; i < 10; i++ )
        {
            byte b = DECODER_ACK_PIN & _BV(DECODER_ACK_BIT);
            if ( b != 0 )
            {
                bNotAck = true;
                UART0_SendChar('N');
                break;
            }
            UART0_SendChar('n');
            _delay_us( CPU_CLK, 500 );  // Check ever .5 ms
        }

        if ( !bNotAck )
        {
            // Wait for silence
            for ( byte i = 0; i < 200; i++ )
            {
                byte b = DECODER_ACK_PIN & _BV(DECODER_ACK_BIT);
                if ( b != 0 )
                {
                    UART0_SendChar('D');
                    break;
                }
                UART0_SendChar('d');
                _delay_us( CPU_CLK, 500 );  // Check ever .5 ms
            }
        }


        if ( !bNotAck )
            return true;
    }
    return false;
}


#define CV_BIT_DATA     0xE0
#define CV_BIT_BIT      0x08
#define CV_BIT_WRITE    0x10
#define CV_BIT_READ     0

#define CV_INSTRUCTION          0xE0
#define CV_BIT_MANIPULATION     (0x08 | CV_INSTRUCTION)
#define CV_VERIFY_BYTE          (0x04 | CV_INSTRUCTION)
#define CV_WRITE_BYTE           (0x0C | CV_INSTRUCTION)


void HandleProgram( void )
{
    static uint16_t nAddress = 3;
    static uint16_t nCV = 1;
    static uint16_t nData = 0;
    static uint8_t nDecoderType = DECODER_TYPE_LOCO;

    mogd_ClearDisplay();

    mogd_PositionCursor(1,1);
    mogd_Send_P( PSTR("Type") );
    mogd_PositionCursor(1,2);
    mogd_Send_P( PSTR("Address") );
    mogd_PositionCursor(9,2);
    mogd_SendInt( nAddress );
    mogd_PositionCursor(1,3);
    mogd_Send_P( PSTR("CV") );
    mogd_PositionCursor(9,3);
    mogd_SendInt( nCV );
    mogd_PositionCursor(1,5);
    static const char sWriteButton[] PROGMEM = " Write ";
    mogd_Send_P( sWriteButton );
    mogd_PositionCursor(1,6);
    static const char sWriteOpsModeButton[] PROGMEM = " Ops Mode ";
    mogd_Send_P( sWriteOpsModeButton );
    mogd_PositionCursor(1,7);
    static const char sReadButton[] PROGMEM = " Read ";
    mogd_Send_P( sReadButton );
    mogd_PositionCursor(1,8);
    static const char sExitButton[] PROGMEM = " Exit ";
    mogd_Send_P( sExitButton );

    //bool bBitCV = isBitCV( nCV );
    //DisplayCV( nCV, nData, bBitCV );

    mogd_PositionCursor(1,4);
    mogd_Send_P( PSTR("Data") );
    mogd_PositionCursor(9,4);
    mogd_SendInt( nData );

    enum 
    {
        ecvType,
        ecvAddress,
        ecvCV,
        ecvData,
        ecvWrite,
        ecvWriteOpsMode,
        ecvRead,
        ecvExit
    } nField = ecvType;

    for (;;)
    {
        byte nRet = KEY_ENTER;
        switch ( nField )
        {
            case ecvType:    nRet = EditEnum(9,1,&nDecoderType, DecoderTypesEnum, countof(DecoderTypesEnum) ); break;
            case ecvAddress: nRet = EditInt(9,2,&nAddress,0,9999); break;
            case ecvCV:      nRet = EditInt(9,3,&nCV,1,1024); break;
            case ecvData:    nRet = EditInt(9,4,&nData,0,255); break;
            case ecvWrite:
            case ecvWriteOpsMode:
                {
                    if ( nField == ecvWrite )
                        nRet = EditButton(0,32,sWriteButton); 
                    else
                        nRet = EditButton(0,40,sWriteOpsModeButton); 

                    if ( nRet == KEY_ENTER )
                    {
                        DoMsg( PSTR("Writing CV") );

                        char sBuffer[6];
                        char *sPtr = sBuffer+1;
                        if ( nDecoderType == DECODER_TYPE_LOCO )
                        {
                            if ( nAddress <= 127 )
                            {
                                *(sPtr++) = nAddress;
                            }
                            else
                            {
                                *(sPtr++) = ((nAddress >> 8) & 0xFF) | 0xC0;
                                *(sPtr++) = nAddress & 0xFF;
                            }
                        }
                        else
                        {
                            *(sPtr++) = 0x80 | (nAddress & 0x3F);
                            *(sPtr++) = 0x80 | (~((nAddress >> 2) & 0x70));
                        }

                        *(sPtr++) = CV_WRITE_BYTE | ((nCV - 1) >> 8 );
                        *(sPtr++) = (nCV-1) & 0xFF;
                        *(sPtr++) = nData;
                        sBuffer[0] = sPtr - sBuffer - 1;

extern 	volatile byte UART0_TxHead;
extern volatile byte UART0_TxTail;

UART0_Send_P(PSTR("Putting cvs"));
while ( UART0_TxHead != UART0_TxTail ) {};
                        cli();
                        SendDCCMsg( sBuffer );
                        SendIdlePacket();
                        SendDCCMsg( sBuffer );
                        sei();

UART0_Send_P(PSTR("Done"));
while ( UART0_TxHead != UART0_TxTail ) {};
                        DumpMsg( sBuffer );

                        // Wait for ACK
                        if ( nField == ecvWrite && WaitForAck() )
                        {
                            UART0_Send_P( PSTR("Got Ack") );
                        }
                        //for ( byte i = 0; i < 5; i++ )
                        //{
                        //    SendDCCMsg( sBuffer );
                        //    //for ( byte j = 0; j < 10; j++ )
                        //    //{
                        //    //    _delay_us( CPU_CLK, 1000 );
                        //    //}
                        //}

                        mogd_DrawSolidRectangle( 0, 75, 16, 200, 48 );
                    }
                    break;
                }
            case ecvRead:    
                {
                    nRet = EditButton(0,48,sReadButton); 
                    if ( nRet == KEY_ENTER )
                    {
                        DoMsg( PSTR("Reading CV") );

                        char sBuffer[6];
                        char *sPtr = sBuffer+1;
                        if ( nDecoderType == DECODER_TYPE_LOCO )
                        {
                            if ( nAddress <= 127 )
                            {
                                *(sPtr++) = nAddress;
                            }
                            else
                            {
                                *(sPtr++) = ((nAddress >> 8) & 0xFF) | 0xC0;
                                *(sPtr++) = nAddress & 0xFF;
                            }
                        }
                        else
                        {
                            *(sPtr++) = 0x80 | (nAddress & 0x3F);
                            *(sPtr++) = 0x80 | (~((nAddress >> 2) & 0x70));
                        }

                        *(sPtr++) = CV_BIT_MANIPULATION | ((nCV - 1) >> 8 );
                        *(sPtr++) = (nCV-1) & 0xFF;
                        byte *pData = sPtr;
                        *(sPtr++) = 0;
                        sBuffer[0] = sPtr - sBuffer - 1;


                        // Loop for 8 bits
                        nData = 0;
                        byte nMask = 1;
                        for ( byte i = 0; i < 8; i++, nMask <<= 1 )
                        { 
                            *pData = CV_BIT_DATA | CV_BIT_BIT | CV_BIT_READ | i;
                            DumpMsg( sBuffer );

                            cli();
                            SendDCCMsg( sBuffer );
                            sei();

                            // Wait for ACK
                            if ( WaitForAck() )
                            {
                                UART0_Send_P( PSTR("Got Ack") );
                                nData |= nMask;
                            }

                        }

                        mogd_DrawSolidRectangle( 0, 75, 16, 200, 48 );
                        UART0_Send_P( PSTR("Read data="));
                        UART0_SendInt( nData );

                        mogd_PositionCursor(9,4);
                        mogd_SendInt( nData );

                    }
                    break;
                }
            case ecvExit:    
                {
                    nRet = EditButton(0,56,sExitButton); 
                    if ( nRet == KEY_ENTER )
                        return;
                    break;
                }
        }
        if ( nRet == KEY_UP )
        {
            if ( nField > 0 )
                nField--;
            else
                nField = ecvExit;
        }
        else if ( nRet == KEY_DOWN )
        {
            nField++;
            if ( nField > ecvExit )
                nField = 0;
        }
    }
}

PGM_P GetEnumString( byte nEnum, const struct EnumMenu * PROGMEM pEnum, byte nCount )
{
    for ( byte n = 0; n < nCount; n++)
    {
        byte nVal = pgm_read_byte( &(pEnum[n].nValue) );
        if ( nVal == nEnum )
            return (const char * PROGMEM)pgm_read_word( &(pEnum[n].sName) );
    }
    return PSTR("Unknown Enum");
}

void DisplayLocoLine( byte nLine, byte nLoco, bool bEmpty )
{
UART0_Send_P(PSTR("    DisplayLocoLine nLine="));
UART0_SendInt( nLine );
UART0_Send_P(PSTR("nLoco="));
UART0_SendInt( nLoco );
UART0_Send_P(PSTR("bEmpty="));
UART0_SendInt( bEmpty );
UART0_SendCRLF();

    nLine += 2;
    mogd_PositionCursor( 1, nLine );
    mogd_SendInt( nLoco+1 );
    mogd_Send_P( PSTR("   ") );
    if ( !bEmpty )
    {
        // Display details
        struct SLoco loco;
        eeprom_read_block( &loco, (void *)(EE_LOCO_DATA_START + nLoco * sizeof(loco)), sizeof(loco) );
        mogd_PositionCursor( 4, nLine );
        mogd_SendInt( loco.nAddress );
        mogd_Send_P( PSTR("    ") );
        mogd_PositionCursor( 9, nLine );
        mogd_Send( loco.sName );
    }
    else
    {
        mogd_PositionCursor( 4, nLine );
        mogd_Send_P( PSTR("[Create new entry]") );
    }
}

#define MAX_DISPLAY_LOCOS 7

void DrawLocos( uint8_t nLocos, uint8_t nItem, uint8_t nBlockOffset, bool bExtraLine )
{
UART0_Send_P(PSTR("Draw Locos nLocos="));
UART0_SendInt( nLocos );
UART0_Send_P(PSTR("nBlockOffset=")); UART0_SendInt( nBlockOffset );
UART0_Send_P(PSTR("nItem="));
UART0_SendInt( nItem );
UART0_Send_P(PSTR("bExtraLine="));
UART0_SendInt( bExtraLine );
UART0_SendCRLF();

    mogd_ClearDisplay();
    mogd_Send_P( PSTR("ID Addr Name") );

    uint8_t nMax = nLocos - nBlockOffset;
    if ( bExtraLine ) 
        nMax++;
    if ( nMax > MAX_DISPLAY_LOCOS )
        nMax = MAX_DISPLAY_LOCOS;
    for ( uint8_t i = 0; i < nMax; i++ )
    {
        if ( i == nItem - nBlockOffset )
            SetFontInverse();
        else 
            SetFontNormal();

        DisplayLocoLine( i, nBlockOffset + i, i==nLocos );
    }
    SetFontNormal();
}

void ShowCursor( byte col, byte row, byte nCursor, char *sBuffer, bool bShow )
{
    if ( !bShow )
    {
        if ( bCursorVisible )
        {
            bCursorVisible = false;
            bCursorInOnState = false;
            FlashCursor( -1, -1 );
        }
        bCursorVisible = false;
        //SetFontNormal();
        //mogd_PositionCursor( col+nCursor, row );
        //byte b = sBuffer[nCursor];
        //if ( b == 0 ) 
        //    b = ' ';
        //mogd_SendChar( b );
    }
    else
    {
        bCursorVisible = true;
        bCursorInOnState = true;
        nCursorFlashTimer = CURSOR_FLASH_TIMER;
        FlashCursor( (col+nCursor-1) * 6 - 1, (row-1) * 8 );
    }
}

void FlashCursor( uint8_t x, uint8_t y )
{
    static uint8_t _x, _y;
    if ( y < 64 )
    {
        _x = x;
        _y = y;
    }
    if ( bCursorInOnState )
        mogd_SetDrawingColour( bInvertedCursor ? 0 : 1 );
    else
        mogd_SetDrawingColour( bInvertedCursor ? 1 : 0 );
    mogd_Drawline( _x, _y, _x, _y + 7 );
}


static bool bUpper = false;
char FindNextChar( byte nChar, byte *nIndex )
{
    static const char s0[] PROGMEM = "0 ";
    static const char s1[] PROGMEM = "1";
    static const char s2[] PROGMEM = "2abc";
    static const char s3[] PROGMEM = "3def";
    static const char s4[] PROGMEM = "4ghi";
    static const char s5[] PROGMEM = "5jkl";
    static const char s6[] PROGMEM = "6mno";
    static const char s7[] PROGMEM = "7pqrs";
    static const char s8[] PROGMEM = "8tuv";
    static const char s9[] PROGMEM = "9wxyz";
    static const char s10[] PROGMEM = ",.-_&!@#$%^*<>;':\"[]{}=+";
    static const struct KEY_MAP
    {
	    byte nKey;
	    PGM_P sKeys;
    } sMap[] PROGMEM = { { KEY_0, s0 }, 
                         { KEY_1, s1 }, 
                         { KEY_2, s2 }, 
                         { KEY_3, s3 }, 
                         { KEY_4, s4 }, 
                         { KEY_5, s5 }, 
                         { KEY_6, s6 }, 
                         { KEY_7, s7 }, 
                         { KEY_8, s8 }, 
                         { KEY_9, s9 }, 
                         { KEY_ASTERISK, s10 } };

    // Find the character.
    PGM_P sChars = NULL;
    for ( int i = 0; i < countof(sMap); i++ )
        if ( pgm_read_byte( &(sMap[i].nKey) ) == nChar )
        {
            sChars = (PGM_P)pgm_read_word( &(sMap[i].sKeys) );
            break;
        }

    if ( sChars != NULL )
    {
        int nLen = strlen_P( sChars );
        if ( *nIndex == nLen - 1 )
            *nIndex = 0;
        else
            (*nIndex)++;
        byte c = pgm_read_byte( sChars + *nIndex );
        if ( bUpper )
        {
            if ( c >= 'a' && c <= 'z' )
                c -= 'a' - 'A';
        }
        return c;
    }
    return '?';
}

byte EditString( byte col, byte row, char *sBuffer, byte nLen )
{
    bInvertedCursor = true;
    SetFontInverse();
    mogd_PositionCursor( col, row );
    mogd_Send( sBuffer );
    byte nCursor = strlen(sBuffer);
    for ( int i = nCursor; i < nLen; i++ )
        mogd_SendChar( ' ' );
    ShowCursor( col, row, nCursor, sBuffer, true );
    byte nLastChar = 0;
    byte nCharIndex = 0;
	for (;;)
	{
		byte c = get_char();

        switch ( c )
        {
            case KEY_TIMER_EXIRED_EVENT:
                // Move cursor
                nCursor++;
                bCharTimerExpired = false;
                ShowCursor( col, row, nCursor, sBuffer, true );
                break;
            case KEY_MENU:
            case KEY_UP:
            case KEY_DOWN:
                nCharTimer = 0;
                bCharTimerExpired = false;
                bInvertedCursor = false;
                ShowCursor( col, row, nCursor, sBuffer, false );
                SetFontNormal();
                mogd_PositionCursor( col, row );
                mogd_Send( sBuffer );
                nCursor = strlen(sBuffer);
                for ( int i = nCursor; i < nLen; i++ )
                    mogd_SendChar( ' ' );
                return c;
            case KEY_CANCEL:
                {
                    if ( nCharTimer || bCharTimerExpired )
                    {
                        nCursor++;
                        ShowCursor( col, row, nCursor, sBuffer, true );
                        nCharTimer = 0;
                        bCharTimerExpired = false;
                    }
                    if ( nCursor == 0 )
                        Beep( 20 );
                    else
                    {
                        ShowCursor( col, row, nCursor, sBuffer, false );
                        nCursor--;
                        mogd_PositionCursor( col+nCursor, row );
                        mogd_SendChar( ' ' );
                        sBuffer[nCursor] = 0;
                        ShowCursor( col, row, nCursor, sBuffer, true );
                    }
                    break;
                }
            default:
                {
                    if ( (c >= '0' && c <= '9') || c == KEY_ASTERISK || c == KEY_UPPER )
                    {
                        byte nCurrentChar;
                        if ( c == KEY_UPPER )
                        {
                            bUpper = !bUpper;
                            if ( nCharTimer || bCharTimerExpired )
                            {
                                nCursor++;
                                nCharTimer = 0;
                                bCharTimerExpired = false;
                                nLastChar = c;
                            }
                        }
                        else if ( c == nLastChar && nCharTimer != 0)
                        {
                            nCurrentChar = FindNextChar(c, &nCharIndex);
                            mogd_PositionCursor( col+nCursor, row );
                            mogd_SendChar( nCurrentChar );
                            sBuffer[ nCursor ] = nCurrentChar;
                            nCharTimer = CHAR_REPEAT_TIMER;
                            bCharTimerExpired = false;
                        }
                        else
                        {
                            if ( nCharTimer || bCharTimerExpired )
                            {
                                nCharTimer = 0;
                                bCharTimerExpired = false;
                                nCursor++;
                            }
                            if ( nCursor >= nLen )
                            {
                                Beep( 20 );
                            }
                            else
                            {
                                nLastChar = c;
                                nCurrentChar = c;
                                nCharIndex = 0;
                                nCharTimer = CHAR_REPEAT_TIMER;
                                bCharTimerExpired = false;

                                mogd_PositionCursor( col+nCursor, row );
                                mogd_SendChar( nCurrentChar );
                                sBuffer[ nCursor ] = nCurrentChar;
                                sBuffer[ nCursor+1 ] = 0;
                                ShowCursor( col, row, nCursor, sBuffer, false );
                            }
                        }
                    }
                }
        }
    }
}

byte EditInt( byte col, byte row, int *nValue, int nMin, int nMax )
{
    char sBuffer[10];
    itoa( nMax, sBuffer, 10 );
    int nLen = strlen(sBuffer); // Max len

    itoa( *nValue, sBuffer, 10 );
    SetFontInverse();
    bInvertedCursor = true;
    mogd_PositionCursor( col, row );
    mogd_Send( sBuffer );
    byte nCursor = strlen(sBuffer);
    for ( int i = nCursor; i < nLen; i++ )
        mogd_SendChar( ' ' );

    ShowCursor( col, row, nCursor, sBuffer, true );
	for (;;)
	{
		byte c = get_char();
        switch ( c )
        {
            case KEY_MENU:
            case KEY_UP:
            case KEY_DOWN:
                {
                    int nTemp = atoi( sBuffer );
                    if ( nTemp < nMin || nTemp > nMax )
                        Beep( 20 );
                    else
                    {
                        bInvertedCursor = false;
                        ShowCursor( col, row, nCursor, sBuffer, false );
                        SetFontNormal();
                        mogd_PositionCursor( col, row );
                        mogd_Send( sBuffer );
                        nCursor = strlen(sBuffer);
                        for ( int i = nCursor; i < nLen; i++ )
                            mogd_SendChar( ' ' );

                        *nValue = nTemp;
                        return c;
                    }
                }
                break;

            case KEY_CANCEL:
                {
                    if ( nCursor == 0 )
                        Beep( 20 );
                    else
                    {
                        ShowCursor( col, row, nCursor, sBuffer, false );
                        nCursor--;
                        mogd_PositionCursor( col+nCursor, row );
                        mogd_SendChar( ' ' );
                        sBuffer[nCursor] = 0;
                        ShowCursor( col, row, nCursor, sBuffer, true );
                    }
                    break;
                }
            default:
                {
                    if ( nCursor == nLen || c < '0' || c > '9' )
                        Beep( 20 );
                    else
                    {
                        ShowCursor( col, row, nCursor, sBuffer, false );
                        mogd_PositionCursor( col+nCursor, row );
                        mogd_SendChar( c );
                        sBuffer[nCursor] = c;
                        sBuffer[nCursor+1] = 0;
                        nCursor++;
                        ShowCursor( col, row, nCursor, sBuffer, true );
                    }
                }
        }
    }
}


byte EditEnum( byte col, byte row, byte *nOrigValue, const struct EnumMenu * PROGMEM pEnum, byte nCount )
{
    byte nValue = *nOrigValue;
	if ( nValue >= nCount )
		nValue = 0;

	char c;
	for (;;)
	{
        SetFontInverse();
	    mogd_PositionCursor( col, row );
        mogd_Send_P( GetEnumString( nValue, pEnum, nCount) );

		c = get_char();
		if ( c == KEY_MENU || c == KEY_DOWN || c == KEY_UP)
		{
            *nOrigValue = nValue;
            SetFontNormal();
	        mogd_PositionCursor( col, row );
            mogd_Send_P( GetEnumString( nValue, pEnum, nCount) );
			return c;
		}
		else if ( c == KEY_ENTER )
		{
			if ( nValue < nCount-1 )
				nValue++;
			else
				nValue=0;
		}
	}
	return c;
}

byte EditButton( uint8_t pixel_x, uint8_t pixel_y, PGM_P sButton )
{
    mogd_PositionPixelCursor( pixel_x, pixel_y );
    SetFontInverse();
    mogd_Send_P( sButton );

	for (;;)
	{
	    char c = get_char();
		if ( c == KEY_MENU || c == KEY_DOWN || c == KEY_UP || c == KEY_ENTER )
		{
            mogd_PositionPixelCursor( pixel_x, pixel_y );
            SetFontNormal();
            mogd_Send_P( sButton );
			return c;
		}
	}
}

#define MAX_DISPLAY_FUNCTIONS 4

void DisplayFunctionLines( struct SLoco *loco, uint8_t nOffset )
{
    for ( uint8_t f = 0; f < loco->nFunctions && f < MAX_DISPLAY_FUNCTIONS; f++ )
    {
        struct SFunctions *pFunc = loco->funcs + f + nOffset;

        mogd_PositionCursor( 1, 4+f );
        mogd_SendInt( f+nOffset+1 );
        mogd_PositionCursor( 4, 4+f );
        mogd_SendInt( pFunc->nFunctionId );
        mogd_PositionCursor( 9, 4+f );
        mogd_Send( pFunc->sName1 );
        mogd_PositionCursor( 19, 4+f );
        mogd_Send( pFunc->sName2 );
        mogd_PositionCursor( 29, 4+f );
        mogd_Send_P( GetEnumString( pFunc->eType, LocoFunctionTypesEnum, countof(LocoFunctionTypesEnum) ) );
    }
}

enum EEditItem {
    SAVE_ITEM,
    DELETE_ITEM,
    CANCEL_EDIT
};


enum EEditItem EditLoco( uint8_t nLocoId, struct SLoco *loco )
{
    mogd_ClearDisplay();

    // Display Fixed layout
    SetFontNormal();
    mogd_PositionCursor( 1, 1 );
    mogd_Send_P( PSTR("#") );
    mogd_SendInt( nLocoId + 1 );

    mogd_PositionCursor( 6, 1 );
    mogd_Send_P( PSTR("Name:") );

    mogd_PositionCursor( 1, 2 );
    mogd_Send_P( PSTR("Address:") );

    mogd_PositionCursor( 15, 2 );
    mogd_Send_P( PSTR("Speed Steps:") );

    mogd_PositionCursor( 32, 2 );
    mogd_Send_P( PSTR("Funcs: ") );

    mogd_PositionCursor( 1, 3 );
    mogd_Send_P( PSTR("  Func  Name                Type") );
  
    mogd_PositionPixelCursor( 0, 56 );
    static const char sButtonSave[] PROGMEM = " Save ";
    mogd_Send_P( sButtonSave );

    mogd_PositionPixelCursor( 40, 56 );
    static const char sButtonDelete[] PROGMEM = "Delete";
    mogd_Send_P( sButtonDelete );

    mogd_PositionPixelCursor( 80, 56 );
    static const char sButtonCancel[] PROGMEM = "Cancel";
    mogd_Send_P( sButtonCancel );


    // Display data
    mogd_PositionCursor( 12, 1 );
    mogd_Send( loco->sName );

    mogd_PositionCursor( 10, 2 );
    mogd_SendInt( loco->nAddress );

    mogd_PositionCursor( 28, 2 );
    mogd_Send_P( GetEnumString( loco->eSpeedSteps, SpeedStepsEnum, countof(SpeedStepsEnum) ) );

    mogd_PositionCursor( 39, 2 );
    mogd_SendInt( loco->nFunctions );

    uint8_t nFunctionOffset = 0;
    DisplayFunctionLines( loco, nFunctionOffset );
    SetFontNormal();

    enum EFields
    {
        eName = 0,
        eAddress,
        eSpeed,
        eFunctions,
        eFunctionNumber,    // Function list
        eFunctionDesc1,     //
        eFunctionDesc2,     //
        eFunctionType,      //
        eSave,
        eDelete,
        eCancel,
        eLast
    };

    uint8_t nFunction=0;
    uint8_t nFunctionLine=0;
    enum EFields nField = eName;
    for (;;)
    {
        byte nRet=0;
        switch ( nField )
        {
            case eName:             
                {
                    nRet = EditString( 12, 1, loco->sName, sizeof(loco->sName)-1 ); 
                    break;
                }
            case eAddress:          
                {
                    nRet = EditInt( 10, 2, &(loco->nAddress), 0, 9999 ); 
                    break;
                }
            case eSpeed:            
                {
                    nRet = EditEnum( 28, 2, &(loco->eSpeedSteps), SpeedStepsEnum, countof(SpeedStepsEnum) ); 
                    break;
                }
            case eFunctions:        
                {
                    int nTemp = loco->nFunctions;
                    nRet = EditInt( 39, 2, &nTemp, 0, 12 ); 
                    if ( loco->nFunctions != nTemp )
                    {
                        loco->nFunctions = nTemp;
                        nFunctionOffset = 0;
                        mogd_DrawSolidRectangle( 0, 0, 24, 239, 55 );
                        DisplayFunctionLines( loco, nFunctionOffset );
                    }
                    if ( nRet == KEY_DOWN )
                    {
                        nFunction = 0;
                        if ( loco->nFunctions == 0 )
                        {
                            nField = eSave;
                            nRet = KEY_ENTER;
                        }
                    }
                    break;
                }
            case eFunctionNumber:
                {
                    int nTemp = loco->funcs[nFunction].nFunctionId;
                    nRet = EditInt( 4, 4+nFunctionLine, &nTemp, 1, 12 ); 
                    loco->funcs[nFunction].nFunctionId = nTemp;

                    if ( nRet == KEY_UP && nFunction > 0 )
                    {
                        // Either move up to functions, or scroll to previous function
                        nFunction--;
                        if ( nFunctionLine > 0 )
                            nFunctionLine--;
                        else
                        {
                            nFunctionOffset--;
                            mogd_DrawSolidRectangle( 0, 0, 24, 239, 55 );
                            DisplayFunctionLines( loco, nFunctionOffset );
                        }
                        nField = eFunctionType;
                        nRet = KEY_ENTER;
                    }
                    break;
                }
            case eFunctionDesc1:
                {
                    nRet = EditString( 9, 4+nFunctionLine, loco->funcs[nFunction].sName1, sizeof(loco->funcs[nFunction].sName1)-1 ); 
                    break;
                }
            case eFunctionDesc2:
                {
                    nRet = EditString( 19, 4+nFunctionLine, loco->funcs[nFunction].sName2, sizeof(loco->funcs[nFunction].sName2)-1 ); 
                    break;
                }
            case eFunctionType:     
                {
                    nRet = EditEnum( 29, 4+nFunctionLine, &(loco->funcs[nFunction].eType), LocoFunctionTypesEnum, countof(LocoFunctionTypesEnum) ); 
                    if ( nRet == KEY_DOWN && nFunction < loco->nFunctions-1 )
                    {
                        // Either move down to buttons, or scroll to next function
                        nFunction++;
                        if ( nFunction < MAX_DISPLAY_FUNCTIONS )
                            nFunctionLine++;
                        else
                        {
                            nFunctionOffset++;
                            mogd_DrawSolidRectangle( 0, 0, 24, 239, 55 );
                            DisplayFunctionLines( loco, nFunctionOffset );
                        }
                        nField = eFunctionNumber;
                        nRet = KEY_ENTER;
                    }
                }
                break;
            case eSave:             
                {
                    nRet = EditButton( 0, 56, sButtonSave );
                    if ( nRet == KEY_ENTER )
                        return SAVE_ITEM;
                    else if ( nRet == KEY_UP )
                    {
                        // Scroll up to last function item.
                        if ( loco->nFunctions == 0 )
                            nField = eFunctions;
                        else
                        {
                            nField = eFunctionType;
                            nFunction = loco->nFunctions - 1;
                            byte nNewFunctionOffset;
                            if ( loco->nFunctions > MAX_DISPLAY_FUNCTIONS )
                            {
                                nNewFunctionOffset = loco->nFunctions - MAX_DISPLAY_FUNCTIONS;
                                nFunctionLine = MAX_DISPLAY_FUNCTIONS - 1;
                            }
                            else
                            {
                                nNewFunctionOffset = 0;
                                nFunctionLine = loco->nFunctions - 1;
                            }

                            if ( nNewFunctionOffset != nFunctionOffset )
                            {
                                nFunctionOffset = nNewFunctionOffset;

                                mogd_DrawSolidRectangle( 0, 0, 24, 239, 55 );
                                DisplayFunctionLines( loco, nFunctionOffset );
                            }
                        }
                        nRet = KEY_ENTER;
                    }
                    break;
                }
            case eDelete:
                {
                    nRet = EditButton( 40, 56, sButtonDelete );
                    if ( nRet == KEY_ENTER )
                        return DELETE_ITEM;
                    break;
                }
            case eCancel:
                {
                    nRet = EditButton( 80, 56, sButtonCancel );
                    if ( nRet == KEY_ENTER )
                        return CANCEL_EDIT;
                    break;
                }
        }

        if ( nRet == KEY_UP )
        {
            if ( nField == 0 )
                nField = eLast-1;
            else
                nField--;
        }
        else if ( nRet == KEY_DOWN )
        {
            if ( nField == eLast - 1 )
                nField = 0;
            else
                nField++;
        }
    }
}
bool DoConfirm( PGM_P sMsg )
{
    uint8_t nLen = strlen_P( sMsg );
    uint8_t nWinLen = nLen;
    if ( nWinLen < 14 )
        nWinLen = 14;

    uint8_t nWidth = (nWinLen+2)*6;
    uint8_t nHeight = 4 * 8;
    mogd_DrawSolidRectangle( 0, (239-nWidth)/2, (63-nHeight)/2, (239+nWidth)/2, (63+nHeight)/2 );
    mogd_DrawOutlineRectangle( 1, (239-nWidth)/2, (63-nHeight)/2, (239+nWidth)/2, (63+nHeight)/2 );

    mogd_PositionPixelCursor( (239 - nLen*6)/2, 31-12 );
    mogd_Send_P( sMsg );

    static const char sOK[] PROGMEM = "  OK  ";
    static const char sCancel[] PROGMEM = "CANCEL";

    mogd_PositionPixelCursor( (239 - 14*6)/2, 4*8 );
    mogd_Send_P( sOK );
    mogd_PositionPixelCursor( 239/2 + 6, 4*8 );
    mogd_Send_P( sCancel );

    enum EButtons
    {
        BUTTON_OK,
        BUTTON_CANCEL
    } nButton = BUTTON_CANCEL;

    uint8_t nRet=0;
    for (;;)
    {
        switch ( nButton )
        {
            case BUTTON_OK:
                nRet = EditButton( (239 - 14*6)/2, 4*8, sOK );
                if ( nRet == KEY_ENTER )
                    return true;
                break;
            case BUTTON_CANCEL:
                nRet = EditButton( 239/2 + 6, 4*8, sCancel );
                if ( nRet == KEY_ENTER )
                    return false;
                break;
        }
        if ( nRet == KEY_UP || nRet == KEY_DOWN )
        {
            if ( nButton == BUTTON_OK )
                nButton = BUTTON_CANCEL;
            else
                nButton = BUTTON_OK;
        }
        else if ( nRet == KEY_CANCEL )
            return false;
    }
}

void DoMsg( PGM_P sMsg )
{
    uint8_t nLen = strlen_P( sMsg );

    uint8_t nWidth = (nLen+4)*6;
    uint8_t nHeight = 3 * 8;
    mogd_DrawSolidRectangle( 0, (239-nWidth)/2, (63-nHeight)/2, (239+nWidth)/2, (63+nHeight)/2 );
    mogd_DrawOutlineRectangle( 1, (239-nWidth)/2, (63-nHeight)/2, (239+nWidth)/2, (63+nHeight)/2 );

    mogd_PositionPixelCursor( (239 - nLen*6)/2, 31-8/2 );
    mogd_Send_P( sMsg );
}

void HandleLocoDB( void )
{
    // Show list of locos
    uint8_t nLocos = eeprom_read_byte(EE_LOCODB_COUNT);
    byte nItem = 0;
    byte nRow = 0;
    byte nBlockOffset = 0;
    DrawLocos( nLocos, nItem, nBlockOffset, true );


    // Highlight the first item.
    bool bDone = false;
    while ( !bDone )
    {
        char nKey = get_char();
        switch ( nKey )
        {
            case KEY_UP:
                if ( nItem == 0 )
                {
                    Beep(20);
                }
                else
                {
                    if ( nRow == 0 )    // Scrolling up
                    {

                    }
                    else
                    {
                        SetFontNormal();
                        DisplayLocoLine( nRow, nItem, nItem==nLocos );
                        SetFontInverse();
                        nItem--;
                        nRow--;
                        DisplayLocoLine( nRow, nItem, nItem==nLocos );
                    }
                }
                break;
            case KEY_DOWN:
                if ( nItem == nLocos )      // Already at bottom
                {
                    Beep(20);
                }
                else
                {
                    if ( nRow == MAX_DISPLAY_LOCOS - 1 )    // Scroll next block
                    {
                    }
                    else
                    {
                        SetFontNormal();
                        DisplayLocoLine( nRow, nItem, nItem==nLocos );
                        SetFontInverse();
                        nItem++;
                        nRow++;
                        DisplayLocoLine( nRow, nItem, nItem==nLocos );
                    }
                }
                break;
            case KEY_ENTER:
                {
                    struct SLoco loco;
UART0_Send_P(PSTR("nBlockOffset=")); UART0_SendInt( nBlockOffset );
                    // get the temporary data
                    if ( nItem == nLocos )
                    {
                        // New
                        memset( &loco, 0, sizeof(loco) );
                        loco.eSpeedSteps = SPEED_STEPS_28;
                    }
                    else
                    {
                        eeprom_read_block( &loco, (void *)(EE_LOCO_DATA_START + nItem * sizeof(loco)), sizeof(loco) );
                    }

UART0_Send_P(PSTR("nBlockOffset=")); UART0_SendInt( nBlockOffset );
                    enum EEditItem nRet = EditLoco( nItem, &loco );
UART0_Send_P(PSTR("nBlockOffset=")); UART0_SendInt( nBlockOffset );
                    if ( nRet == SAVE_ITEM )
                    {
                        DoMsg( PSTR("Saving...") );
                        eeprom_write_block( &loco, (void *)(EE_LOCO_DATA_START + nItem * sizeof(loco)), sizeof(loco) );
                        if ( nItem == nLocos )
                        {
                            // New loco, shuffle accessories back.
                            struct SAcc acc;
                            byte nAccs = eeprom_read_byte((void *)EE_ACCDB_COUNT);
                            uint16_t nEeprom = EE_LOCO_DATA_START + nLocos * sizeof(loco);
                            for ( int i = nAccs-1; i >= 0; i-- )
                            {
                                eeprom_read_block( &acc, (void *)(nEeprom + i * sizeof(acc)), sizeof(acc) );
                                eeprom_write_block( &acc, (void *)(nEeprom + sizeof(loco) + i * sizeof(acc)), sizeof(acc) );
                            }

                            nLocos++;
                            eeprom_write_byte(EE_LOCODB_COUNT,nLocos);
                        }
                        if ( nItem == nCurrentLoco )
                        {
                            memcpy( &CurrentLoco, &loco, sizeof(loco) );
                        }
                    }
                    else if ( nRet == DELETE_ITEM && nItem < nLocos )
                    {
                        // Confirm
                        if ( DoConfirm( PSTR("Delete Locomotive?") ) )
                        {
                            mogd_ClearDisplay();
                            mogd_PositionPixelCursor( (239 - 1 * 6)/2, (63-8)/2 );
                            mogd_Send_P( PSTR("Deleting...") );
                            // Delete.  Shuffle following entries forward one.
                            for ( uint8_t i = nItem + 1; i < nLocos; i++ )
                            {
                                eeprom_read_block( &loco, (void *)(EE_LOCO_DATA_START + i * sizeof(loco)), sizeof(loco) );
                                eeprom_write_block( &loco, (void *)(EE_LOCO_DATA_START + (i-1) * sizeof(loco)), sizeof(loco) );
                            }

                            // Shuffle accessory entries forward one.
                            byte nAccs = eeprom_read_byte((void *)EE_ACCDB_COUNT);
                            uint16_t nEeprom = EE_LOCO_DATA_START + nLocos * sizeof(loco);
                            for ( uint8_t i = 0; i < nAccs; i++ )
                            {
                                // reuse the loco buffer
                                eeprom_read_block( &loco, (void *)(nEeprom + i * sizeof(struct SAcc)), sizeof(struct SAcc) );
                                eeprom_write_block( &loco, (void *)(nEeprom - sizeof(struct SLoco) + i * sizeof(struct SAcc)), sizeof(struct SAcc) );
                            }

                            nLocos--;
                            if ( nItem > 0 )
                                nItem--;
                            eeprom_write_byte(EE_LOCODB_COUNT,nLocos);
                        }
                    }
                    DrawLocos( nLocos, nItem, nBlockOffset, true );
                    break;
                }
            case KEY_CANCEL:
                return;
        }
    }
    // 00 byte - # locos
    //      Name - 30 bytes
    //      Address - 14 bits - 2 bytes
    //      Speed steps - 14/28/128
    //      Functions Map
    //          # functions - 1 byte 0-12
    //              Function(s) 0 - n
    //                  function name
    //                  type
    //                      push on/push off
    //                      momentary

    // Insert   - extra blank line
    // delete   - enter for options - edit/delete
    // edit(enter)

    // highlight first one
}

void DisplayAccLine( byte nLine, byte nAcc, uint16_t nEeprom, bool bEmpty )
{
    nLine += 2;
    mogd_PositionCursor( 1, nLine );
    mogd_SendInt( nAcc+1 );
    mogd_Send_P( PSTR("   ") );
    if ( !bEmpty )
    {
        // Display details
        struct SAcc acc;
        eeprom_read_block( &acc, (void *)(nEeprom + nAcc * sizeof(acc)), sizeof(acc) );
        mogd_PositionCursor( 5, nLine );
        mogd_SendInt( acc.nAddress );
        mogd_Send_P( PSTR("    ") );
        mogd_PositionCursor( 10, nLine );
        mogd_SendInt( acc.nSubAddress );
        mogd_Send_P( PSTR("  ") );
        mogd_PositionCursor( 13, nLine );
        mogd_Send( acc.sName );
    }
    else
    {
        mogd_PositionCursor( 5, nLine );
        mogd_Send_P( PSTR("[Create new entry]") );
    }
}

#define MAX_DISPLAY_ACCS 7

void DrawAccs( uint8_t nAccs, uint8_t nItem, uint8_t nBlockOffset, uint16_t nEeprom, bool bExtraLine )
{
    mogd_ClearDisplay();
    mogd_Send_P( PSTR("ID Addr Sub Name") );

    uint8_t nMax = nAccs - nBlockOffset;
    if ( bExtraLine ) 
        nMax++;
    if ( nMax > MAX_DISPLAY_ACCS )
        nMax = MAX_DISPLAY_ACCS;
    for ( uint8_t i = 0; i < nMax; i++ )
    {
        if ( i == nItem - nBlockOffset )
            SetFontInverse();
        else 
            SetFontNormal();

        DisplayAccLine( i, nBlockOffset + i, nEeprom, i==nAccs );
    }
    SetFontNormal();
}

enum EEditItem EditAccessory( uint8_t nAccId, struct SAcc *acc )
{
    mogd_ClearDisplay();

    // Display Fixed layout
    SetFontNormal();
    mogd_PositionCursor( 1, 1 );
    mogd_Send_P( PSTR("#") );
    mogd_SendInt( nAccId + 1 );

    mogd_PositionCursor( 6, 1 );
    mogd_Send_P( PSTR("Name:") );

    mogd_PositionCursor( 1, 2 );
    mogd_Send_P( PSTR("Address:") );

    mogd_PositionCursor( 15, 2 );
    mogd_Send_P( PSTR("Sub Addr:") );

    mogd_PositionCursor( 27, 2 );
    mogd_Send_P( PSTR("Type: ") );

    mogd_PositionPixelCursor( 0, 56 );
    static const char sButtonSave[] PROGMEM = " Save ";
    mogd_Send_P( sButtonSave );

    mogd_PositionPixelCursor( 40, 56 );
    static const char sButtonDelete[] PROGMEM = "Delete";
    mogd_Send_P( sButtonDelete );

    mogd_PositionPixelCursor( 80, 56 );
    static const char sButtonCancel[] PROGMEM = "Cancel";
    mogd_Send_P( sButtonCancel );


    // Display data
    mogd_PositionCursor( 12, 1 );
    mogd_Send( acc->sName );

    mogd_PositionCursor( 10, 2 );
    mogd_SendInt( acc->nAddress );

    mogd_PositionCursor( 25, 2 );
    mogd_SendInt( acc->nSubAddress );

    mogd_PositionCursor( 33, 2 );
    mogd_Send_P( GetEnumString( acc->nType, AccessoryTypesEnum, countof(AccessoryTypesEnum) ) );

    enum EFields
    {
        eName = 0,
        eAddress,
        eSubAddress,
        eType,
        eSave,
        eDelete,
        eCancel,
        eLast
    };

    enum EFields nField = eName;
    for (;;)
    {
        byte nRet=0;
        switch ( nField )
        {
            case eName:             
                {
                    nRet = EditString( 12, 1, acc->sName, sizeof(acc->sName)-1 ); 
                    break;
                }
            case eAddress:          
                {
                    nRet = EditInt( 10, 2, &(acc->nAddress), 0, 9999 ); 
                    break;
                }
            case eSubAddress:          
                {
                    int n = acc->nSubAddress;
                    nRet = EditInt( 25, 2, &n, 0, 8 ); 
                    acc->nSubAddress = n;
                    break;
                }
            case eType:            
                {
                    nRet = EditEnum( 33, 2, &(acc->nType), AccessoryTypesEnum, countof(AccessoryTypesEnum) ); 
                    break;
                }
            case eSave:             
                {
                    nRet = EditButton( 0, 56, sButtonSave );
                    if ( nRet == KEY_ENTER )
                        return SAVE_ITEM;
                    break;
                }
            case eDelete:
                {
                    nRet = EditButton( 40, 56, sButtonDelete );
                    if ( nRet == KEY_ENTER )
                        return DELETE_ITEM;
                    break;
                }
            case eCancel:
                {
                    nRet = EditButton( 80, 56, sButtonCancel );
                    if ( nRet == KEY_ENTER )
                        return CANCEL_EDIT;
                    break;
                }
        }

        if ( nRet == KEY_UP )
        {
            if ( nField == 0 )
                nField = eLast-1;
            else
                nField--;
        }
        else if ( nRet == KEY_DOWN )
        {
            if ( nField == eLast - 1 )
                nField = 0;
            else
                nField++;
        }
    }
}

void HandleAccessoryDB( void )
{
    // Show list of accessories
    uint8_t nAccs = eeprom_read_byte( (uint8_t *)EE_ACCDB_COUNT );
    byte nItem = 0;
    byte nRow = 0;
    byte nBlockOffset = 0;

    uint16_t nEeprom = EE_LOCO_DATA_START + eeprom_read_byte( (uint8_t *)EE_LOCODB_COUNT ) * sizeof(struct SLoco);
    DrawAccs( nAccs, nItem, nBlockOffset, nEeprom, true );


    // Highlight the first item.
    bool bDone = false;
    while ( !bDone )
    {
        char nKey = get_char();
        switch ( nKey )
        {
            case KEY_UP:
                if ( nItem == 0 )
                {
                    Beep(20);
                }
                else
                {
                    if ( nRow == 0 )    // Scrolling up
                    {

                    }
                    else
                    {
                        SetFontNormal();
                        DisplayAccLine( nRow, nItem, nEeprom, nItem==nAccs );
                        SetFontInverse();
                        nItem--;
                        nRow--;
                        DisplayAccLine( nRow, nItem, nEeprom, nItem==nAccs );
                    }
                }
                break;
            case KEY_DOWN:
                if ( nItem == nAccs )      // Already at bottom
                {
                    Beep(20);
                }
                else
                {
                    if ( nRow == MAX_DISPLAY_ACCS - 1 )    // Scroll next block
                    {
                    }
                    else
                    {
                        SetFontNormal();
                        DisplayAccLine( nRow, nItem, nEeprom, nItem==nAccs );
                        SetFontInverse();
                        nItem++;
                        nRow++;
                        DisplayAccLine( nRow, nItem, nEeprom, nItem==nAccs );
                    }
                }
                break;
            case KEY_ENTER:
                {
                    struct SAcc acc;

                    // get the temporary data
                    if ( nItem == nAccs )
                    {
                        // New
                        memset( &acc, 0, sizeof(acc) );
                    }
                    else
                    {
                        eeprom_read_block( &acc, (void *)(nEeprom + nItem * sizeof(acc)), sizeof(acc) );
                    }


                    enum EEditItem nRet = EditAccessory( nItem, &acc );

                    if ( nRet == SAVE_ITEM )
                    {
                        DoMsg( PSTR("Saving...") );
                        eeprom_write_block( &acc, (void *)(nEeprom + nItem * sizeof(acc)), sizeof(acc) );
                        if ( nItem == nAccs )
                        {
                            // New accessory
                            nAccs++;
                            eeprom_write_byte( (void *)EE_ACCDB_COUNT,nAccs);
                        }
                        //if ( nItem == nCurrentLoco )
                        //{
                        //    memcpy( &CurrentLoco, &loco, sizeof(loco) );
                        //}
                    }
                    else if ( nRet == DELETE_ITEM && nItem < nAccs )
                    {
                        // Confirm
                        if ( DoConfirm( PSTR("Delete Accessory?") ) )
                        {
                            mogd_ClearDisplay();
                            mogd_PositionPixelCursor( (239 - 1 * 6)/2, (63-8)/2 );
                            mogd_Send_P( PSTR("Deleting...") );

                            // Delete.  Shuffle following entries up one.
                            for ( uint8_t i = nItem + 1; i < nAccs; i++ )
                            {
                                eeprom_read_block( &acc, (void *)(nEeprom + i * sizeof(acc)), sizeof(acc) );
                                eeprom_write_block( &acc, (void *)(nEeprom + (i-1) * sizeof(acc)), sizeof(acc) );
                            }
                            nAccs--;
                            if ( nItem > 0 )
                                nItem--;
                            eeprom_write_byte((void *)EE_ACCDB_COUNT,nAccs);
                        }
                    }
                    DrawAccs( nAccs, nItem, nBlockOffset, nEeprom, true );
                    break;
                }
            case KEY_CANCEL:
                return;
        }
    }

}

void DumpMsg( char *sBuffer )
{
    uint8_t nLen = sBuffer[0]+1;
    for ( uint8_t i = 0; i < nLen; i++ )
    {
        UART0_SendHex( sBuffer[i] );
        UART0_SendChar( ' ' );
    }
}

void DumpLocoEntry( uint8_t nId )
{
    UART0_Send_P( PSTR("nId=") ); UART0_SendInt( LocoMap[nId].nId ); UART0_SendCRLF();
    UART0_Send_P( PSTR("sSpeed=") ); DumpMsg( LocoMap[nId].sSpeed ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nSpeedRepeats=") ); UART0_SendInt( LocoMap[nId].nSpeedRepeats ); UART0_SendCRLF();
    UART0_Send_P( PSTR("sFunctionBlock1=") ); DumpMsg( LocoMap[nId].sFunctionBlock1 ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nFunctionBlock1Repeats=") ); UART0_SendInt( LocoMap[nId].nFunctionBlock1Repeats ); UART0_SendCRLF();
    UART0_Send_P( PSTR("sFunctionBlock2=") ); DumpMsg( LocoMap[nId].sFunctionBlock2 ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nFunctionBlock2Repeats=") ); UART0_SendInt( LocoMap[nId].nFunctionBlock2Repeats ); UART0_SendCRLF();
    UART0_Send_P( PSTR("sFunctionBlock3=") ); DumpMsg( LocoMap[nId].sFunctionBlock3 ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nFunctionBlock3Repeats=") ); UART0_SendInt( LocoMap[nId].nFunctionBlock3Repeats ); UART0_SendCRLF();

    UART0_Send_P( PSTR("nLastSendTime=") ); UART0_SendInt( LocoMap[nId].nLastSendTime ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nCurrentDirection=") ); UART0_SendInt( LocoMap[nId].nCurrentDirection ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nCurrentSpeed=") ); UART0_SendInt( LocoMap[nId].nCurrentSpeed ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nMaxSpeed=") ); UART0_SendInt( LocoMap[nId].nMaxSpeed ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nFunctionsState=") ); UART0_SendInt( LocoMap[nId].nFunctionsState ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nFunctions=") ); UART0_SendInt( LocoMap[nId].nFunctions ); UART0_SendCRLF();
    UART0_Send_P( PSTR("nLastSent=") ); UART0_SendInt( LocoMap[nId].nLastSent ); UART0_SendCRLF();
}

void FunctionLights(void)
{
    if ( nLocoMapCount > 0 )
    {
        UART0_Send_P( PSTR("Function Lights") );
        UART0_SendCRLF();

        LocoMap[nCurrentLoco].bLights = !LocoMap[nCurrentLoco].bLights;

        if ( LocoMap[nCurrentLoco].bLights )
            LocoMap[nCurrentLoco].sFunctionBlock1[LocoMap[nCurrentLoco].sFunctionBlock1[0]] |= 1 << 4;
        else
            LocoMap[nCurrentLoco].sFunctionBlock1[LocoMap[nCurrentLoco].sFunctionBlock1[0]] &= ~(1 << 4);
        LocoMap[nCurrentLoco].nFunctionBlock1Repeats = 5;

        DumpLocoEntry(nCurrentLoco);

        bFunctionsChanged = true;
    }
}

void Function( byte nFunc )
{
    if ( nLocoMapCount > 0 )
    {
        byte nFunction = nFunctionShift * 4 + nFunc;

        UART0_Send_P( PSTR("Function #") );
        UART0_SendHex( nFunction );
        UART0_SendCRLF();

        if ( nFunction >= CurrentLoco.nFunctions )
        {
            Beep(20);
        }
        else
        {
            uint16_t nMask = _BV(nFunction);
            if ( LocoMap[nCurrentLoco].nFunctionsState & nMask )
                LocoMap[nCurrentLoco].nFunctionsState &= ~nMask;
            else
                LocoMap[nCurrentLoco].nFunctionsState |= nMask;

            // Update function group message
            if ( nFunction < 4 )
            {
                uint8_t nByte = LocoMap[nCurrentLoco].nFunctionsState & 0Xf;
                LocoMap[nCurrentLoco].sFunctionBlock1[LocoMap[nCurrentLoco].sFunctionBlock1[0]] &= ~0XF;
                LocoMap[nCurrentLoco].sFunctionBlock1[LocoMap[nCurrentLoco].sFunctionBlock1[0]] |= nByte;
                LocoMap[nCurrentLoco].nFunctionBlock1Repeats = 5;
            }
            else if ( nFunction < 8 )
            {
                uint8_t nByte = (LocoMap[nCurrentLoco].nFunctionsState >> 4) & 0Xf;
                LocoMap[nCurrentLoco].sFunctionBlock2[LocoMap[nCurrentLoco].sFunctionBlock2[0]] &= ~0XF;
                LocoMap[nCurrentLoco].sFunctionBlock2[LocoMap[nCurrentLoco].sFunctionBlock2[0]] |= nByte;
                LocoMap[nCurrentLoco].nFunctionBlock2Repeats = 5;
            }
            else
            {
                uint8_t nByte = (LocoMap[nCurrentLoco].nFunctionsState >> 8) & 0Xf;
                LocoMap[nCurrentLoco].sFunctionBlock3[LocoMap[nCurrentLoco].sFunctionBlock3[0]] &= ~0XF;
                LocoMap[nCurrentLoco].sFunctionBlock3[LocoMap[nCurrentLoco].sFunctionBlock3[0]] |= nByte;
                LocoMap[nCurrentLoco].nFunctionBlock3Repeats = 5;
            }
            bFunctionsChanged = true;
        }
    }
}

void FunctionShift( void )
{
    if ( CurrentLoco.nFunctions <= 4 )
        Beep( 20 );
    else  if ( CurrentLoco.nFunctions <= 8 )
    {
        nFunctionShift++;
        if ( nFunctionShift == 2 )
            nFunctionShift = 0;
        DisplayFunctionKeys();
    }
    else
    {
        // Max 12
        nFunctionShift++;
        if ( nFunctionShift == 3 )
            nFunctionShift = 0;
        DisplayFunctionKeys();
    }
}

void ToggleLoco( void )
{
    if ( nLocoMapCount > 1 )
    {
        nCurrentLoco++;
        if ( nCurrentLoco >= nLocoMapCount )
            nCurrentLoco = 0;
        DriveLoco( LocoMap[nCurrentLoco].nId );

        bLocoChanged = true;
    }
    else
    {
        Beep(20);
    }
}

bool SelectLoco( uint8_t *nLoco )
{
    // Show list of locos
    uint8_t nLocos = eeprom_read_byte(EE_LOCODB_COUNT);
    byte nItem = 0;
    byte nRow = 0;
    byte nBlockOffset = 0;
    DrawLocos( nLocos, nItem, nBlockOffset, true );

    // Highlight the first item.
    for (;;)
    {
        char nKey = get_char();
        switch ( nKey )
        {
            case KEY_UP:
                if ( nItem == 0 )
                {
                    Beep(20);
                }
                else
                {
                    if ( nRow == 0 )    // Scrolling up
                    {

                    }
                    else
                    {
                        SetFontNormal();
                        DisplayLocoLine( nRow, nItem, nItem==nLocos );
                        SetFontInverse();
                        nItem--;
                        nRow--;
                        DisplayLocoLine( nRow, nItem, nItem==nLocos );
                    }
                }
                break;
            case KEY_DOWN:
                if ( nItem == nLocos )      // Already at bottom
                {
                    Beep(20);
                }
                else
                {
                    if ( nRow == MAX_DISPLAY_LOCOS - 1 )    // Scroll next block
                    {
                    }
                    else
                    {
                        SetFontNormal();
                        DisplayLocoLine( nRow, nItem, nItem==nLocos );
                        SetFontInverse();
                        nItem++;
                        nRow++;
                        DisplayLocoLine( nRow, nItem, nItem==nLocos );
                    }
                }
                break;
            case KEY_ENTER:
                {
                    *nLoco = nItem;
                    //struct SLoco loco;
                    //eeprom_read_block( &loco, (void *)(EE_LOCO_DATA_START + nItem * sizeof(loco)), sizeof(loco) );
                    return true;
                }
            case KEY_CANCEL:
                return false;
        }
    }
}



void AddLoco( uint8_t nId )
{
    // If this loco isn't already in the list, add it.
    bool bFound = false;
    uint8_t nIndex;
    for ( uint8_t i = 0; i < nLocoMapCount; i++ )
        if ( LocoMap[i].nId == nId )
        {
            nIndex = i;
            bFound = true;
        }

    if ( !bFound )
    {
        struct SLoco loco;
        eeprom_read_block( &loco, (void *)(EE_LOCO_DATA_START + nId * sizeof(loco)), sizeof(loco) );

        // Add new entry
        nIndex = nLocoMapCount;
        nLocoMapCount++;

        LocoMap[nIndex].nId = nId;

        // Build the messages

        // Address 
        //    Address 00000000 (0):                               Broadcast address
        //    Addresses 00000001-01111111 (1-127)(inclusive):     Multi-Function decoders with 7 bit addresses
        //    Addresses 10000000-10111111 (128-191)(inclusive):   Basic Accessory Decoders with 9 bit addresses and Extended Accessory Decoders 
        //                                                        with 11-bit addresses
        //    Addresses 11000000-11100111 (192-231)(inclusive):   Multi Function Decoders with 14 bit addresses ( 2 Bytes )
        //    Addresses 11101000-11111110 (232-254)(inclusive):   Reserved for Future Use
        //    Address 11111111 (255):                             Idle Packet
        uint8_t nAddressBytes;
        uint8_t nAddressByte1;
        uint8_t nAddressByte2=0;
        if ( loco.nAddress > 0 && loco.nAddress <= 127 )
        {
            // Single byte address
            nAddressBytes = 1;
            nAddressByte1 = loco.nAddress;
        }
        else
        {
            // 2 byte address
            nAddressBytes = 2;
            nAddressByte1 = ((loco.nAddress >> 8) & 0xFF) | 0xC0;
            nAddressByte2 = loco.nAddress & 0xFF;
        }
UART0_Send_P(PSTR("Address Bytes=")); UART0_SendInt( nAddressBytes );
UART0_Send_P(PSTR(" Byte1=")); UART0_SendHex( nAddressByte1 );
UART0_Send_P(PSTR(" Byte2=")); UART0_SendHex( nAddressByte2 );
UART0_SendCRLF();
        // speed
        if ( loco.eSpeedSteps == SPEED_STEPS_14 )
        {
            uint8_t i = 0;
            LocoMap[nIndex].sSpeed[i++] = nAddressBytes + 1;
            LocoMap[nIndex].sSpeed[i++] = nAddressByte1;
            if ( nAddressBytes > 1 )
                LocoMap[nIndex].sSpeed[i++] = nAddressByte2;
            LocoMap[nIndex].sSpeed[i++] = SPEED_14_FORWARD;
            LocoMap[nIndex].nMaxSpeed = 14;
        }
        else if ( loco.eSpeedSteps == SPEED_STEPS_28 )
        {
            uint8_t i = 0;
            LocoMap[nIndex].sSpeed[i++] = 2;
            LocoMap[nIndex].sSpeed[i++] = nAddressByte1;
            LocoMap[nIndex].sSpeed[i++] = SPEED_28_FORWARD;
            LocoMap[nIndex].nMaxSpeed = 28;
        }
        else // if ( loco.eSpeedSteps == SPEED_STEPS_128 )
        {
            uint8_t i = 0;
            LocoMap[nIndex].sSpeed[i++] = nAddressBytes + 2;
            LocoMap[nIndex].sSpeed[i++] = nAddressByte1;
            if ( nAddressBytes > 1 )
                LocoMap[nIndex].sSpeed[i++] = nAddressByte2;
            LocoMap[nIndex].sSpeed[i++] = SPEED_128;        
            LocoMap[nIndex].sSpeed[i++] = 0;
            LocoMap[nIndex].nMaxSpeed = 127;
        }
        LocoMap[nIndex].nFunctions = loco.nFunctions;
        LocoMap[nIndex].nCurrentDirection = 1;
        LocoMap[nIndex].nSpeedRepeats = 5;
        LocoMap[nIndex].nFunctionBlock1Repeats=0;
        LocoMap[nIndex].nFunctionBlock2Repeats=0;
        LocoMap[nIndex].nFunctionBlock3Repeats=0;
        LocoMap[nIndex].nLastSentMax = 1;
        

        // Functions
        if ( loco.nFunctions > 0 )
        {
            uint8_t i = 0;
            LocoMap[nIndex].sFunctionBlock1[i++] = nAddressBytes + 1;
            LocoMap[nIndex].sFunctionBlock1[i++] = nAddressByte1;
            if ( nAddressBytes > 1 )
                LocoMap[nIndex].sFunctionBlock1[i++] = nAddressByte2;
            LocoMap[nIndex].sFunctionBlock1[i++] = FUNCTION_GROUP_1;
            LocoMap[nIndex].nFunctionBlock1Repeats=5;
            LocoMap[nIndex].nLastSentMax++;

            if ( loco.nFunctions > 4 )
            {
                uint8_t i = 0;
                LocoMap[nIndex].sFunctionBlock2[i++] = nAddressBytes + 1;
                LocoMap[nIndex].sFunctionBlock2[i++] = nAddressByte1;
                if ( nAddressBytes > 1 )
                    LocoMap[nIndex].sFunctionBlock2[i++] = nAddressByte2;
                LocoMap[nIndex].sFunctionBlock2[i++] = FUNCTION_GROUP_2 | FUNCTION_GROUP_SHIFT;
                LocoMap[nIndex].nFunctionBlock2Repeats=5;
                LocoMap[nIndex].nLastSentMax++;

                if ( loco.nFunctions > 8 )
                {
                    uint8_t i = 0;
                    LocoMap[nIndex].sFunctionBlock3[i++] = nAddressBytes + 1;
                    LocoMap[nIndex].sFunctionBlock3[i++] = nAddressByte1;
                    if ( nAddressBytes > 1 )
                        LocoMap[nIndex].sFunctionBlock3[i++] = nAddressByte2;
                    LocoMap[nIndex].sFunctionBlock3[i++] = FUNCTION_GROUP_2;
                    LocoMap[nIndex].nFunctionBlock3Repeats=5;
                    LocoMap[nIndex].nLastSentMax++;
                }
            }
        }
        LocoMap[nIndex].nLastSendTime = 0;
        LocoMap[nIndex].nCurrentSpeed = 0;
        LocoMap[nIndex].nFunctionsState = 0;
        LocoMap[nIndex].nLastSent = 0;
    }
}

void DisplayDirectionValue( void )
{
    //mogd_PositionCursor( 12, 2 );
    mogd_PositionCursor( 14, 4 );
    if ( LocoMap[nCurrentLoco].nCurrentDirection )
        mogd_Send_P( PSTR("Forward") );
    else
        mogd_Send_P( PSTR("Reverse") );
}

void DisplaySpeedValue( void )
{
    //mogd_PositionCursor( 8, 3 );
    uint8_t nSpeed = LocoMap[nCurrentLoco].nCurrentSpeed;
    //mogd_SendInt( nSpeed );
    //mogd_SendChar( '/' );
    uint8_t n = LocoMap[nCurrentLoco].nMaxSpeed;
    //mogd_SendInt( n );
    //mogd_SendChar( ' ' );
    uint16_t p = nSpeed * 100;
    p /= n;
    mogd_PositionCursor( 22, 4 );
    mogd_SendInt( p );
    mogd_SendChar( '%' );
    mogd_SendChar( ' ' );
    mogd_SendChar( ' ' );

    uint16_t nNewDisplaySpeed = nSpeed * 238;
    nNewDisplaySpeed /= n;
    nNewDisplaySpeed++;

    // Only display deltas.
    if ( nNewDisplaySpeed > nOldDisplaySpeed )
    {
        mogd_DrawSolidRectangle(1, nOldDisplaySpeed, 34, nNewDisplaySpeed, 43 );
    }
    else if ( nNewDisplaySpeed < nOldDisplaySpeed )
    {
        mogd_DrawSolidRectangle(0, nNewDisplaySpeed, 34, nOldDisplaySpeed, 43 );
    }
    nOldDisplaySpeed = nNewDisplaySpeed;
}

void DisplayFunctionValues( void )
{
    //mogd_PositionCursor( 12, 4 );
    //uint16_t mask = 1;
    //uint8_t nFunctions = CurrentLoco.nFunctions;
    //for ( uint8_t f = 0; f < nFunctions; f++, mask <<= 1 )
    //    if ( LocoMap[nCurrentLoco].nFunctionsState & mask )
    //        mogd_Send_P( PSTR(" 1") );
    //    else
    //        mogd_Send_P( PSTR(" 0") );
    DisplayFunctionKeys();
}

void ClearFunctionKey( byte nCol )
{
    byte x1 = nCol * 10 * 6;
    byte y1 = 6*8;
    mogd_DrawSolidRectangle(0,x1, y1, x1 + 58, y1 + 15);
}

void ShowFunctionKey( byte nCol, byte nFunction, byte bActive )
{
    byte nColour;
    if ( bActive )
    {
        SetFontInverse();
        nColour = 1;
    }
    else
    {
        SetFontNormal();
        nColour = 0;
    }
    byte x1 = nCol * 10 * 6;
    byte y1 = 6*8;
    mogd_DrawSolidRectangle(nColour,x1, y1, x1 + 58, y1 + 15);

    byte nLen;
    nLen = strlen(CurrentLoco.funcs[nFunction].sName1) * 6;
    mogd_PositionPixelCursor( x1 + (60-nLen)/2,y1);
    mogd_Send( CurrentLoco.funcs[nFunction].sName1 );

    nLen = strlen(CurrentLoco.funcs[nFunction].sName2) * 6;
    mogd_PositionPixelCursor( x1 + (60-nLen)/2,y1+8);
    mogd_Send( CurrentLoco.funcs[nFunction].sName2 );
}

void DisplayFunctionKeys( void )
{
    uint16_t mask = 1 << (nFunctionShift * 4);
    for ( uint8_t f = 0; f < 4; f++, mask <<= 1 )
    {
        uint8_t nFunc = nFunctionShift * 4 + f;
        if ( nFunc < CurrentLoco.nFunctions )
        {
            ShowFunctionKey( f, f + nFunctionShift * 4, LocoMap[nCurrentLoco].nFunctionsState & mask );
        }
        else
        {
            ClearFunctionKey( f );
        }
    }

    if ( LocoMap[nCurrentLoco].bLights )
        SetFontInverse();
    else
        SetFontNormal();

    mogd_PositionCursor(1,1);
    mogd_Send_P( PSTR(" Lights ") );

    SetFontNormal();
}

void DisplayDriveScreen( void )
{
    mogd_ClearDisplay();
    SetFontNormal();
    mogd_PositionCursor( 1, 1 );
    mogd_SendInt( CurrentLoco.nAddress );
    mogd_SendChar( '.' ); mogd_SendChar( ' ' );
    mogd_Send( CurrentLoco.sName );

//    mogd_PositionCursor( 1, 2 );
//    mogd_Send_P( PSTR("Direction:") );
    DisplayDirectionValue();

//    mogd_PositionCursor( 14, 4 );
//    mogd_Send_P( PSTR("Throttle") );
    DisplaySpeedValue();

    // Speedo bar
    mogd_DrawOutlineRectangle(1, 0, 33, 239, 44 );

    // Functions
    //mogd_PositionCursor( 1, 4 );
    //mogd_Send_P( PSTR("Functions:") );
    DisplayFunctionValues();

    // Function keys
    DisplayFunctionKeys();
}

void DriveLoco( uint8_t nId )
{
    // Load up the loco
    for ( byte i = 0; i < nLocoMapCount; i++ )
        if ( nId == LocoMap[i].nId )
        {
            nCurrentLoco = i;
            eeprom_read_block( &CurrentLoco, (void *)(EE_LOCO_DATA_START + nId * sizeof(CurrentLoco)), sizeof(CurrentLoco) );
            nOldDisplaySpeed = 1;
        }
}

void DisplayMainScreen( void )
{
    if ( nLocoMapCount > 0 )
    {
        DisplayDriveScreen();
    }
    else
    {
        mogd_ClearDisplay();
        SetFontNormal();
        mogd_Send_P( PSTR( "Frank's DCC Controller") );
        mogd_PositionCursor( 1, 3 );
        mogd_Send_P( PSTR("Locomotives: " ) );
        mogd_SendInt( eeprom_read_byte((uint8_t *)EE_LOCODB_COUNT) );

        mogd_PositionCursor( 1, 4 );
        mogd_Send_P( PSTR("Accessories: " ) );
        mogd_SendInt( eeprom_read_byte( (uint8_t *)EE_ACCDB_COUNT) );

        mogd_DisplaySavedBitmap( 1, 190, 0 );
    }
}

void SetSwitch( byte nId, bool bState )
{
    struct SAcc acc;
    uint8_t nLocos = eeprom_read_byte(EE_LOCODB_COUNT);
    uint16_t nEeprom = EE_LOCO_DATA_START + nLocos * sizeof(struct SLoco);
    eeprom_read_block( &acc, (void *)(nEeprom + nId * sizeof(acc)), sizeof(acc) );

    //UART0_Send_P( PSTR("Sending switch id=") );
    //UART0_SendInt( nId );
    //UART0_Send_P( PSTR(" addr=") );
    //UART0_SendInt( acc.nAddress );
    //UART0_Send_P( PSTR(" subaddr=") );
    //UART0_SendInt( acc.nSubAddress );
    //UART0_SendCRLF();
    // {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
    DCC_Special[0] = 2;
    DCC_Special[1] = 0x80 | (acc.nAddress & 0x3F);
    DCC_Special[2] = 0x80 | ((~(acc.nAddress>>2)) & 0x70) | acc.nSubAddress | (bState ? 0x08 : 0);

    nSpecialRepeat = 5;
}

void CloseSwitch( void )
{
    SetSwitch( nCurrentAccessoryId, false );
}

void ThrowSwitch( void )
{
    SetSwitch( nCurrentAccessoryId, true );
}

void Switch( byte nId )
{
    byte nAccs = eeprom_read_byte((void *)EE_ACCDB_COUNT);
    if ( nId <= nAccs )
        nCurrentAccessoryId = nId-1;
}

int main (void)
{
	DCC_Tail = 0;
	DCC_Head = 0;
    DCC_State = IDLE;

    nSpecialRepeat=0;
    msec = 0;
    sec = 0;
    tenths = 0;
    nCharTimer = 0;
    bCharTimerExpired = false;
    nLocoMapCount = 0;
    nLastTransmitLoco = 0;
    nFunctionShift=0;
    bFunctionsChanged = false;
    bSpeedChanged = false;
    bDirectionChanged = false;
    bLocoChanged = false;
    nCurrentAccessoryId = 0;


    ioinit();
    UART0_Init(25);   // 38.4kbps
    UART1_Init(51);    // 19.2kbps

    //eeprom_write_byte(EE_LOCODB_COUNT,0);
    //eeprom_write_byte(EE_ACCDB_COUNT,0);

	sei();

    // Wait for mogd to start
    for ( int delay = 0; delay < 1000; delay++ )
    {
        _delay_us( CPU_CLK, 1000 );
    }

    mogd_Init();
    mogd_SetFontMetrics( 0, 0, 0, 0, 64 );
    SetFontNormal();
    UART0_Send_P( PSTR("UART0-This is a test") );
    //UART1_Send_P( PSTR("UART1-This is a test") );
 
    // Send 10 idle packets 10 ms apart
    for ( byte i = 0; i < 10; i++ )
    {
        SendIdlePacket();
        for ( byte delay = 0; delay < 10; delay++ )
        {
            _delay_us( CPU_CLK, 1000 );
        }
    }

    nLastQuadValue = KNOB_PIN & ((_BV(KNOB_A_BIT) | _BV(KNOB_B_BIT)));
    nSpeedCount = 0;
    //int16_t nLastSpeedCount = -1;

    DisplayMainScreen();

    for(;;)
    {
        //UART1_Send_P( PSTR("Tcik ") );
        //for ( int i = 0; i < 1000; i++ )
        //{
        //    _delay_us( CPU_CLK, 1000 );
        //}

        byte b = get_char();
        UART0_Send_P( PSTR("\r\nGot=>") );
        UART0_SendChar( b );
        UART0_SendChar( '(' );
        UART0_SendHex( b );
        UART0_SendChar( ')' );
        UART0_SendCRLF();

        switch ( b )
        {
            case KEY_SELECT_LOCO:
                {
                    uint8_t nId;
                    if ( SelectLoco( &nId ) )
                    {
                        UART0_Send_P( PSTR("Selected loco="));
                        UART0_SendInt( nId );
                        AddLoco( nId );
                        DriveLoco( nId );
                    }
                    DisplayMainScreen();
                }
                break;

            case KEY_MENU:
                Menu();
                DisplayMainScreen();
                break;

            case KEY_0: Switch( 0 ); break;
            case KEY_1: Switch( 1 ); break;
            case KEY_2: Switch( 2 ); break;
            case KEY_3: Switch( 3 ); break;
            case KEY_4: Switch( 4 ); break;
            case KEY_5: Switch( 5 ); break;
            case KEY_6: Switch( 6 ); break;
            case KEY_7: Switch( 7 ); break;
            case KEY_8: Switch( 8 ); break;
            case KEY_9: Switch( 9 ); break;
            case KEY_ASTERISK: ThrowSwitch(); break;
            case KEY_UPPER: CloseSwitch(); break;
        }


        if ( bLocoChanged )
        {
            bLocoChanged = false;
            DisplayMainScreen();
        }
        if ( bFunctionsChanged )
        {
            // Update functions
            DisplayFunctionValues();
            bFunctionsChanged = false;
        }
        if ( bSpeedChanged )
        {
            // Update speed
            DisplaySpeedValue();
            bSpeedChanged = false;
        }
        if ( bDirectionChanged )
        {
            // Direction Changed
            DisplayDirectionValue();
            bDirectionChanged = false;
        }

        //if ( nLastSpeedCount != nSpeedCount )
        //{
        //    nLastSpeedCount = nSpeedCount;

        //    UART0_Send_P( PSTR("Speed=") );
        //    UART0_SendInt( nLastSpeedCount );
        //    UART0_SendCRLF();        
        //}
    }
}


/*
LCD Display - USART1
RS232 I/F - USART0

DCC comms
 One bit - 58us High, 58us Low
 Zero bit - 100us High, 100uS Low

 Data Packet is...
 Preamble, address, data
    Preamble - 14 1's
    0 - address start bit
    address - 8 bits
    0 - data start bit
    data - 8 bits
    [data can be many bytes]
    1 - end packet bit.

    Base line packet
        preamble 0 address 0 data 0 error check 1
        error check = address xor data


1) Speed and Direction packet

111111111111 0  0AAAAAAA 0  01DCSSSS 0  EEEEEEEE 1
Preamble        Byte One    Byte Two    Byte Three (Error Detection Data Byte)

AAAAAAA - Address
SSSS    - Speed
C       - Additional speed bit, or headlight
D       - Direction - 1 forward, 0 reverse
01      - Speed/Dir packet


2) Digital Decoder Reset Packet For All Decoders

111111111111 000000000 0    00000000 0     00000000 1
Preamble     Byte One       Byte Two       Byte Three (Error Detection Data Byte)


3) Digital Decoder Idle Packet For All Decoders

111111111111 0  11111111 0  00000000 0  11111111 1
Preamble Byte   One Byte    Two Byte    Three (Error Detection Data Byte)



4) Digital Decoder Broadcast Stop Packets For All Decoders10
111111111111 0  00000000 0  01DC000S 0  EEEEEEEE 1 100
Preamble        Byte One    Byte Two    Byte Three (Error Detection Data Byte)

S       - Stop - 0 - normal stop, 1 - emergency stop


Extended packets

A: Addresses
    Address 00000000 (0):                               Broadcast address
    Addresses 00000001-01111111 (1-127)(inclusive):     Multi-Function decoders with 7 bit addresses
    Addresses 10000000-10111111 (128-191)(inclusive):   Basic Accessory Decoders with 9 bit addresses and Extended Accessory Decoders 
                                                        with 11-bit addresses
    Addresses 11000000-11100111 (192-231)(inclusive):   Multi Function Decoders with 14 bit addresses ( 2 Bytes )
    Addresses 11101000-11111110 (232-254)(inclusive):   Reserved for Future Use
    Address 11111111 (255):                             Idle Packet


B: Broadcast Command For Multi Function Digital Decoders
    The format for this packet is:
    {preamble} 0 00000000 0 {instruction-bytes} 0 EEEEEEEE 1


C: Instruction Packets For Multi Function Digital Decoders
    The format for these packets are:
    {preamble} 0 [ AAAAAAAA ] 0 {instruction-bytes} 0 EEEEEEEE 1

  {instruction-bytes} = CCCDDDDD,
                        CCCDDDDD 0 DDDDDDDD, or
                        CCCDDDDD 0 DDDDDDDD 0 DDDDDDDD

    CCC - command
            000 Decoder and Consist Control Instruction
            001 Advanced Operation Instructions
            010 Speed and Direction Instruction for reverse operation
            011 Speed and Direction Instruction for forward operation
            100 Function Group One Instruction
            101 Function Group Two Instruction
            110 Future Expansion
            111 Configuration Variable Access Instruction


000 Decoder and Consist Control Instruction

    0000 Decoder Control
        {instruction byte} = 0000CCCD, or 
        {instruction byte} = 0000CCCD dddddddd

            CCC = 000 D = "0":  Digital Decoder Reset - A Digital Decoder Reset shall erase all volatile memory
                                    (including and speed and direction data), and return to its initial power up state as defined in RP-
                                    9.2.4 section A. Command Stations shall not send packets to addresses 112-127 for 10 packet
                                    times following a Digital Decoder Reset. This is to ensure that the decoder does not start
                                    executing service mode instruction packets as operations mode packets (Service Mode
                                    instruction packets have a short address in the range of 112 to 127 decimal.)
                      D = "1":  Hard Reset - Configuration Variables 29, 31 and 32 are reset to its factory default
                                    conditions, CV#19 is set to "00000000" and a Digital Decoder reset (as in the above instruction)
                                    shall be performed.
            CCC = 001           Factory Test Instruction - This instruction is used by manufacturers to test decoders at the
                                    factory. It must not be sent by any command station during normal operation. This
                                    instruction may be a multi-byte instruction.
            CCC = 010           Reserved for future use
            CCC = 011           Set Decoder Flags (see below)
                                    {instruction bytes} = 0000011D cccc0aaa
                                        aaa is the decoder’s sub-address.
            CCC = 100           Reserved for future use
            CCC = 101           Set Advanced Addressing (CV#29 bit 5)
            CCC = 110           Reserved for future use
            CCC = 111 D= "1":   Decoder Acknowledgment Request-


001 Advanced Operations Instruction

These instructions control advanced decoder functions. Only a single advanced operations instruction may be
contained in a packet.

The format of this instruction is 001CCCCC 0 DDDDDDDD

CCCCC - The five bit sub-instruction 

CCCCC = 11111: 128 Speed Step Control - used to send one of 126 Digital Decoder speed steps. The subsequent single byte 
                shall define speed and direction with bit 7 being direction ("1" is forward and "0" is reverse) and the 
                remaining bits used to indicate speed. The most significant speed bit is bit 6. A data-byte value
                of U0000000 is used for stop, and a data-byte value of U0000001 is used for emergency stop. This allows up to
                126 speed steps. When operations mode acknowledgment is enabled, receipt of a 128 Speed Step Control packet
                must be acknowledged with an operations mode acknowledgement.

CCCCC = 11110: Restricted Speed Step Instruction - used to restrict the maximum speed of a decoder. Bit 7 of the data byte 
                (‘DDDDDDDD’ above) is used to enable (‘0’) or disable (‘1’) restricted speed operation. Bits 0-5 of the Data byte 
                are the Speed Steps defined in S-9.23. When operations mode acknowledgment is enabled, receipt of a 
                Restricted Speed Instruction must be acknowledged with an operations mode acknowledgement.

010 and 011 Speed and Direction Instructions
These two instructions have these formats:

for Reverse Operation 010DDDDD
for Forward Operation 011DDDDD



100 Function Group One Instruction

100DDDDD

Bits 0-3 control the external functions. 0 for off, 1 for on.  F1=bit 0, F2=bit 1, etc.
If Bit 1 of CV#29 has a value of one (1), then bit 4 controls function FL, otherwise bit 4 has no meaning.

101 Function Group Two Instruction

101SDDDD

when S=0, DDDD represents functions F5-F8
when S=1, DDDD represents functions F9-F12
0 for off, 1 for on.  


111     Configuration Variable Access Instruction

The Configuration Variable Access instructions are intended to set up or modify Configurations Variables either on
the programming track or on the main line. There are two forms of this instruction. The short form is for modifying
selected frequently modified Configuration Variables. The long form is for verifying or modifying any selected
Configuration Variable. Only a single configuration variable access instruction may be contained in a packet.

Configuration Variable Access Acknowledgment

If an configuration variable access acknowledgment is required, and the decoder has "decoder operations-mode
acknowledgment" enabled, the decoder shall respond with an operations mode acknowledgment.

Configuration Variable Access Instruction - Short Form

This instruction has the format of

1111CCCC 0 DDDDDDDD

The 8 bit data DDDDDDDD is placed in the configuration variable identified by CCCC according to the following 275
table.

CCCC = 0000 - Not available for use
CCCC = 0010 - Acceleration Value (CV#23)
CCCC = 0011 - Deceleration Value (CV#24) 280

The remaining values of CCCC are reserved and will be selected by the NMRA as need is determined.
Only a single packet is necessary to change a configuration variable using this instruction. If the decoder
successfully receives this packet, it shall respond with an operations mode acknowledgment.

Configuration Variable Access Instruction - Long Form

The long form allows the direct manipulation of all CVs. This instruction is valid both when the Digital Decoder
has its long address active and short address active. Digital Decoders shall not act on this instruction if sent to its
consist address. The format of the instructions using Direct CV addressing is:

1110CCAA 0 AAAAAAAA 0 DDDDDDDD

The actual Configuration Variable desired is selected via the 10 bit address with with the two bit address(AA) in the first
data byte being the most significant bits of the address. The Configuration variable being addressed is the provided
10 bit address plus 1. For example, to address CV#1 the 10 bit address is “00 00000000”.

The defined values for Instruction type (CC) are:
CC=00 Reserved for future use
CC=01 Verify byte
CC=11 Write byte
CC=10 Bit manipulation

Type = "01" VERIFY BYTE
The contents of the Configuration Variable as indicated by the 10 bit address are compared with the data
byte (DDDDDDDD). If the decoder successfully receives this packet and the values are identical, the Digital
Decoder shall respond with an the contents of the CV as the Decoder Response Transmission, if enabled.

Type = "11" WRITE BYTE
The contents of the Configuration Variable as indicated by the 10 bit address are replaced by the data byte
(DDDDDDDD). Two identical packets are needed before the decoder shall modify a configuration variable8.
These two packets need not be back to back on the track. However any other packet to the same decoder will
invalidate the write operation. (This includes broadcast packets.) If the decoder successfully receives this second
identical packet, it shall respond with a configuration variable access acknowledgment.

Type = "10" BIT MANIPULATION
The bit manipulation instructions use a special format for the data byte (DDDDDDDD): 111C DAAA

AAA     represents the bit position within the CV, 
D       contains the value of the bit to be verified or written, and 
C       describes whether the operation is a verify bit or a write bit operation.
                C = "1" WRITE BIT
                C = "0" VERIFY BIT

The VERIFY BIT and WRITE BIT instructions operate in a manner similar to the VERIFY BYTE and WRITE
BYTE instruction (but operates on a single bit). Using the same criteria as the VERIFY BYTE instruction, a
operations mode acknowledgment will be generated in response to a VERIFY BIT instruction if appropriate. Using
the same criteria as the WRITE BYTE instruction, a configuration variable access acknowledgment will be
generated in response to the second identical WRITE BIT instruction if appropriate.



D: Basic Accessory Decoder Packet Format
The format for packets intended for Accessory Digital Decoders is:
{preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1

AAAAAAAAA   - Address
C - on/off
DDD - device - convention is that switches are in pairs, hence bit zero selects which of the pair of 4 (from the other 2 D bits)



Extended Accessory Decoder Control Packet Format

The Extended Accessory Decoder Control Packet is included for the purpose of transmitting aspect control to signal
decoders or data bytes to more complex accessory decoders. Each signal head can display one aspect at a time.
{preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1

XXXXX - custom info



Broadcast Command for Basic Accessory Decoders

The format for the broadcast instruction is:
{preamble} 0 10111111 0 1000CDDD 0 EEEEEEEE 1



Broadcast Command for Extended Accessory Decoders

The format for the broadcast instruction is:
{preamble} 0 10111111 0 00000111 0 000XXXXX 0 EEEEEEEE 1






Packets are constantly repeated - minimum of 5 ms between repeats.

10 idle packets are sent at start up.


- Configuration
    - Trains
        - Train database
            - Engine #
            - Engine Name
            - Engine Decoder Address
            - Decoder attributes
                - 2/4 digit address
                - 
            - Function map
                - decoder function #
                - 
                - function name
                - type
                    - push on/push off
                    - momentary
    - Accessories
        - ID
        - Type
            - On/off mapping - on = throw, off = close
            - single/double
        - Name

    - Consisting
- Programming
    - CVs
        - normal - separate track
        - ops mode - cv for loco addressed only.
        - Accessories
- Train Control
    - Throttle
    - Direction
    - Features, F1,F2,F3,F4     - Display on LCD above buttons what each does. (F5-8?)
- accessory control
    - throw
    - close
- routes
    - define a list of turn outs
- Display
- Keypad
    - 1,2abc,3def,4ghi,5jkl,6mno,7pqrs,8tuv,9wxyz,*,0' ',# with letters 
- External throttles




Setup
    Trains
    Accessories
        

        These are pre-built messages.

    Map of devices
    - Specials
        - eg stop all, reset all, track power.
    - locos                                     # repeats - for new messages
        - speed/dir msg                         time till next send (max every 5 ms)
                111111111111 0  0AAAAAAA 0  01DCSSSS 0  EEEEEEEE 1, or
                                AAAAAAAA 0  [AAAAAAA] 0 001CCCCC 0 DDDDDDDD         Max 4 bytes
                                AAAAAAAA 0  [AAAAAAA] 0 010DDDDD 
        - functions 1-4
                AAAAAAAA 0  [AAAAAAA] 0 100DDDDD                                    Max 3 bytes
        - functions 5-8
                AAAAAAAA 0  [AAAAAAA] 0 100SDDDD                                    Max 3 bytes
        - functions 9-11
                AAAAAAAA 0  [AAAAAAA] 0 100SDDDD                                    Max 3 bytes

    - accessories
        - functions 1 - ?                       ditto
                {preamble} 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1
                {preamble} 0 10AAAAAA 0 0AAA0AA1 0 000XXXXX 0 EEEEEEEE 1            Max 3 bytes



    Current Loco
        - maps to the Map of devices
    Current accessory
        - special accessory mapping







*/

/*
Todo 
 - Left right arrows for future stuff. - rationalise of menu/cancel/enter/arrows - consistency
 - What is F0? Lights
 - support menu == cancel
 - make menu scroll list
 - Accessories
 - Install decoders
 - Keypad layout + keypad
 - Hardware
    - cable for ack line - signal, ack, gnd cable
    - More external switches - gnd, vcc, io ports
            - don't confuse this with accessories.
            - main track power on.
            - programming track power on.
 - don't beep when changing speed and not on loco list.
*/
