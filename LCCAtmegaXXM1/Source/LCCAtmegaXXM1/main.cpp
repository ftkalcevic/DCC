/*
 * LCCAtmegaXXM1.cpp
 *
 * Created: 16/05/2020 12:30:05 PM
 * Author : Frank Tkalcevic
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include "board.h"
#include "decoder.h"


#define OLCB_NO_BLUE_GOLD
#define NUM_CHANNEL 4
#define NUM_EVENT	2*NUM_CHANNEL

#define NEW_NODEID 0x02, 0x01, 0x0D, 0x00, 0x00, 0x02
#define RESET_TO_FACTORY_DEFAULTS


// Board definitions
#define MANU		"Franksworkshop"	// The manufacturer of node
#define MODEL		"LCCAtmegaXXM1"		// The model of the board
#define HWVERSION	"0.1"				// Hardware version
#define SWVERSION	"0.1"				// Software version

#include "processor.h"            // auto-selects the processor type, and CAN lib, EEPROM lib etc.
#include "OpenLcbCore.h"
#include "OpenLCBHeader.h"        // System house-keeping.


extern "C" {                      // the following are defined as external

// ===== CDI =====
//   Configuration Description Information in xml, **must match MemStruct below**
//   See: http://openlcb.com/wp-content/uploads/2016/02/S-9.7.4.1-ConfigurationDescriptionInformation-2016-02-06.pdf
//   CDIheader and CDIFooter contain system-parts, and includes user changable node name and description fields. 
     const char configDefInfo[] PROGMEM = 
       // ===== Enter User definitions below CDIheader line =====
       CDIheader R"(
         <group>
           <name>I/O Events</name>
           <description>Define events associated with input and output pins</description>
           <group replication='2'>
             <name>Inputs</name>
             <repname>Input</repname>
             <string size='16'><name>Description</name></string>
             <eventid><name>Activation Event</name></eventid>
             <eventid><name>Inactivation Event</name></eventid>
           </group>
           <group replication='2'>
             <name>Outputs</name>
             <repname>Output</repname>
             <string size='16'><name>Description</name></string>
             <eventid><name>Set Event</name></eventid>
             <eventid><name>Reset Event</name></eventid>
           </group>
         </group>
       )" CDIfooter;
       // ===== Enter User definitions above CDIfooter line =====
}

// ===== MemStruct =====
//   Memory structure of EEPROM, **must match CDI above**
//     -- nodeVar has system-info, and includes the node name and description fields
    typedef struct { 
          EVENT_SPACE_HEADER eventSpaceHeader; // MUST BE AT THE TOP OF STRUCT - DO NOT REMOVE!!!
          
          char nodeName[20];  // optional node-name, used by ACDI
          char nodeDesc[24];  // optional node-description, used by ACDI
      // ===== Enter User definitions below =====
            struct {
              char desc[16];        // description of this input-pin
              EventID activation;   // eventID which is Produced on activation of this input-pin 
              EventID inactivation; // eventID which is Produced on inactivation of this input-pin
            } inputs[2];            // 2 inputs
            struct {
              char desc[16];        // decription of this output
              EventID setEvent;     // Consumed eventID which sets this output-pin
              EventID resetEvent;   // Consumed eventID which resets this output-pin
            } outputs[2];           // 2 outputs
      // ===== Enter User definitions above =====
    } MemStruct;                 // type definition

extern "C" {
  
  // ===== eventid Table =====
  //  Array of the offsets to every eventID in MemStruct/EEPROM/mem, and P/C flags
  //    -- each eventid needs to be identified as a consumer, a producer or both.  
  //    -- PEID = Producer-EID, CEID = Consumer, and PC = Producer/Consumer
  //    -- note matching references to MemStruct.  
       const EIDTab eidtab[NUM_EVENT] PROGMEM = {
        PEID(inputs[0].activation), PEID(inputs[0].inactivation),  // 1st channel - input, ie producer
        PEID(inputs[1].activation), PEID(inputs[1].inactivation),  // 2nd channel - input
        CEID(outputs[0].setEvent),  CEID(outputs[0].resetEvent),   // 3rd channel - output, ie consumer
        CEID(outputs[1].setEvent),  CEID(outputs[1].resetEvent),   // 4th channel - output
      };
 
  // SNIP Short node description for use by the Simple Node Information Protocol 
  // See: http://openlcb.com/wp-content/uploads/2016/02/S-9.7.4.3-SimpleNodeInformation-2016-02-06.pdf
        extern const char SNII_const_data[] PROGMEM = "\001" MANU "\000" MODEL "\000" HWVERSION "\000" OlcbCommonVersion ; // last zero in double-quote

}; // end extern "C"

// PIP Protocol Identification Protocol uses a bit-field to indicate which protocols this node supports
// See 3.3.6 and 3.3.7 in http://openlcb.com/wp-content/uploads/2016/02/S-9.7.3-MessageNetwork-2016-02-06.pdf
uint8_t protocolIdentValue[6] = {   //0xD7,0x58,0x00,0,0,0};
pSimple | pDatagram | pMemConfig | pPCEvents | !pIdent    | pTeach     | !pStream   | !pReservation, // 1st byte
pACDI   | pSNIP     | pCDI       | !pRemote  | !pDisplay  | !pTraction | !pFunction | !pDCC        , // 2nd byte
0, 0, 0, 0                                                                                           // remaining 4 bytes
};

// userInitAll() -- include any initialization after Factory reset "Mfg clear" or "User clear"
//  -- clear or pre-define text variables.  
// USER defined
void userInitAll() {
  /*
    Serial.print("/nEEADDRs\n");
    Serial.print("EEADDR(nodeVar.nodeName)");Serial.println(EEADDR(nodeVar.nodeName),HEX);
    Serial.print("EEADDR(nodeVar.nodeDesc)");Serial.println(EEADDR(nodeVar.nodeDesc),HEX);
    Serial.print("EEADDR(inputs[0].desc)");Serial.println(EEADDR(inputs[0].desc),HEX);
    Serial.print("EEADDR(inputs[1].desc)");Serial.println(EEADDR(inputs[1].desc),HEX);
    Serial.print("EEADDR(outputs[0].desc)");Serial.println(EEADDR(outputs[0].desc),HEX);
    Serial.print("EEADDR(outputs[1].desc)");Serial.println(EEADDR(outputs[1].desc),HEX);
  */
    
    //NODECONFIG.put(EEADDR(nodeName), ESTRING("OlcbBasicNode"));
    //NODECONFIG.put(EEADDR(nodeDesc), ESTRING("Testing"));
    //NODECONFIG.put(EEADDR(inputs[0].desc), ESTRING("Input1"));
    //NODECONFIG.put(EEADDR(inputs[1].desc), ESTRING("Input2"));
    //NODECONFIG.put(EEADDR(outputs[0].desc), ESTRING("Output1"));
    //NODECONFIG.put(EEADDR(outputs[1].desc), ESTRING("Output2")); 
}


#include "OpenLCBMid.h"           // System house-keeping


void ioinit()
{
	arduino_init();

	// Leds
	DDRD |= _BV(LED_GOLD_PIN) | _BV(LED_BLUE_PIN);	// LED outputs

	// buttons
	PIND |= _BV(BUTTON_GOLD_PIN) | _BV(BUTTON_BLUE_PIN);	// button pullups

	DDRC |= _BV(PC2);	// TXCAN
	PORTC |= _BV(PC2);
	//PINC |= _BV(PC3);	// RXCAN
	//for (;;)
	//{
		//PORTC |= _BV(PC2);
		//_delay_ms(1);
		//PORTC &= ~_BV(PC2);
		//_delay_ms(1);
	//}

#ifdef NEW_NODEID
	NodeID newNodeID(NEW_NODEID);
	nm.changeNodeID(&newNodeID);
#endif

#ifdef RESET_TO_FACTORY_DEFAULTS
	Olcb_init(1);
#else
	Olcb_init(0);
#endif
}

void produceFromInputs() 
{
}
int main(void)
{
	ioinit();

    /* Replace with your application code */
    while (1) 
    {
	  bool activity = Olcb_process();     // System processing happens here, with callbacks for app action.

	  #ifdef DEBUG
		  static unsigned long T = millis()+5000;
		  if(millis()>T) 
		  {
			  T+=5000;
			  Serial.print("\n.");
		  }
	  #endif

	#ifndef OLCB_NO_BLUE_GOLD
		if (activity) 
		{
			// blink blue to show that the frame was received
			//Serial.print("\nrcv");
			//blue.blink(0x1);
		}
		if (olcbcanTx.active) 
		{ // set when a frame sent
			//gold.blink(0x1);
			//Serial.print("\nsnd");
			olcbcanTx.active = false;
		}
		// handle the status lights
		//blue.process();
		//gold.process();
  
		// process inputs
		if(produceFromInputs) 
		{
			produceFromInputs();
		}
	#endif //OLCB_NO_BLUE_GOLD

    }
}



/*
	* CAN Message header 29bit extended format only
		* No remote frames (RTR)
	* Frame
		* Header
			* MSBit of frame is reserved.  It must be transmitted 1.  It is ignored on receipt (don't filter)
			* 2nd bit is frame type - 0 CAN control frame, 1 OpenLCB message
			* Next 15 bits are the Variable Field
			* Definition
				* 28 Reserved
				* 27 1=OpenLCB, 0=Control
				* 26-12 Variable Field
				* 11-0 Source Node ID Alias
		* Body
			* 0-8 bytes of data
	* Nodes can be in inhibited or permitted state
		* start in inhibited
		* Node in inhibited state can only transmit "Check ID", "Reserve ID", and "Alias Map Definition" frames.
		* Permitted node can send anything.



		

*/

__extension__ typedef int __guard __attribute__((mode (__DI__)));

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
//void __cxa_guard_abort (__guard *) {};
extern "C" void __cxa_pure_virtual(void) {};
