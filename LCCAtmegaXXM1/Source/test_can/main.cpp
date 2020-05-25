/*
 * test_can.cpp
 *
 * Created: 17/05/2020 2:18:34 PM
 * Author : Frank Tkalcevic
 */ 

#include <avr/io.h>

#define AT90CAN_MOBS 6
#define MOB_DIS_CMD  0
void can_init()
{
	unsigned char mob;   //temp MOB register
	CANGCON = (1<<SWRES);							//reset controller

	for (mob = 0; mob < AT90CAN_MOBS; mob++)
	{
		CANPAGE = (mob << MOBNB0);					//Enable all mobs
		CANCDMOB = MOB_DIS_CMD;						//Set all mob conf. disabled
		CANSTMOB = 0;								//all mob RXOK TXOK cleared/enabled
	}
	CANHPMOB = 0xF0;								//default value
	
	//	CANTCON = 0x01;									//CAN timer pre-scalar div.2
	CANGIE  = (1<<ENIT) | (1<<ENRX) | (1<<ENTX);
	//	CANIE = ((u16) 1 << (AT90CAN_MOBS)) -1;			//all mob int enabled
	CANBT1 = 0x0E; CANBT2 = 0x0C; CANBT3 = 0x37;
	//CANBT1 = 0x1E; CANBT2 = 0x04; CANBT3 = 0x13;

	CANGCON = (1<<ENASTB);							// Enable the CAN.
	
	
}

void Can_Tx(unsigned char pg){
	
	int data1[7], data2[7];
	
	for (int i=0; i<8; i++){
		data1[i]=0x0f;
		data2[i]=0xf0;
	}
	// Tx from pg 11bit ID IDE=0
	unsigned char ide=0, length = 8;
	CANPAGE= (pg<<4);								//set page
	
	CANIDT4 = 0;									//set ID & mask & rtr
	CANIDT3 = 0;
	CANIDT2 = (0x668 << 5);
	CANIDT1 = (0x668 >> 3);							//ID=011001101000
	CANIDM = 0x00;
	
	for (int i=0; i<8; i++)
	CANMSG = data1[i];							//set data

	if (pg < 7)
	CANIE2 |= 1<<pg;
	else
	CANIE1 |= 1<< (pg-7);						//Mob int. enable
	CANCDMOB = 0x58;								//set Tx mode, 10
}

int CanTx_log = 0, CanRx_log = 0;
unsigned char mob;


int main(void)
{
	can_init();
	Can_Tx(4);

    /* Replace with your application code */
    while (1) 
    {
    }
}

