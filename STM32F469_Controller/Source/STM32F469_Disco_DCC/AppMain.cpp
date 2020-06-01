#include "AppMain.h"
#include "main.h"
#include "DRV8873S.h"
#include "AudioTask.h"


extern "C" void AppMainTask_Entry(void *argument)
{
	AppMain app;
	app.Run();
}


void AppMain::Run()
{
	// Play startup sound
//	audioTask.PlaySound(EAudioSounds::Bell2);
	
	for(;;) vTaskDelay(pdMS_TO_TICKS(1000));
	
			GPIO_InitTypeDef GPIO_InitStruct = { 0 };
			GPIO_InitStruct.Pin = PrgTrk_DCC_Signal_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
			GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
			HAL_GPIO_Init(PrgTrk_DCC_Signal_GPIO_Port, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(PrgTrk_Disable_GPIO_Port, PrgTrk_Disable_Pin, GPIO_PIN_SET);
	
	uint8_t status, ic1, diag;
	DRV8873S d(&hspi2);
	d.WriteRegister(PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin, MemoryMap::IC3ControlRegister, 0b01000000 );
	d.WriteRegister(PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin, MemoryMap::IC1ControlRegister, IC1_TOFF_40us | IC1_SPIIN_FOLLOW_INx | IC1_SR_10_8 | IC1_MODE_PHEN );
	d.WriteRegister(PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin, MemoryMap::IC1ControlRegister, IC1_TOFF_40us | IC1_SPIIN_FOLLOW_INx | IC1_SR_10_8 | IC1_MODE_PHEN );
	d.ReadRegister(PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin, MemoryMap::IC1ControlRegister, status, ic1);
	d.ReadRegister(PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin, MemoryMap::DiagRegister, status, diag);
	//	d.ReadFaultStatus(PrgTrk_CS_GPIO_Port, PrgTrk_CS_Pin);
	
	bool bFault = HAL_GPIO_ReadPin(PrgTrk_Fault_GPIO_Port, PrgTrk_Fault_Pin) == GPIO_PIN_RESET;
	
	
	HAL_GPIO_WritePin(PrgTrk_Disable_GPIO_Port, PrgTrk_Disable_Pin, GPIO_PIN_RESET);
	
	HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_RESET);
	//HAL_GPIO_WritePin(PrgTrk_Disable_GPIO_Port, PrgTrk_Disable_Pin, GPIO_PIN_SET);			// Enable from Ph/En
	//HAL_GPIO_WritePin(PrgTrk_Disable_GPIO_Port, PrgTrk_Disable_Pin, GPIO_PIN_SET); 

	
	//for (;;)
	{
		HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_SET);
		//HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_SET);
		//vTaskDelay(pdMS_TO_TICKS(500));
		HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_RESET);
		//HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_RESET);
		//vTaskDelay(pdMS_TO_TICKS(500));
		HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(PrgTrk_DCC_Signal_GPIO_Port, PrgTrk_DCC_Signal_Pin, GPIO_PIN_RESET);
	}



	//vTaskDelay(pdMS_TO_TICKS(500));


	
	for (;;)
		osDelay(pdMS_TO_TICKS(1000));
}





/*
	- App will always start in the "drive" screen.
		- swipe to next loco/accessory
	- Go back to the main screen
		- 4x2 buttons per page
			- can swipe for more
			- one button will be the config menu (default to last button)
			- the others will be to select a trains, or accessories
			- buttons will be image + text.  user configurable image/text for train/acc
			- buttons can be ordered.
		- buttons will zoom to full screen config (flip over and zoom)
	- config
		- app settings
		- configure dcc/lcc devices - that is define the objects in the list
		- dcc track programming
		- lcc config
		- switch to usb_msc mode
		- usb on the go
			- usb hid device input
		
	- dcc programming 
		- track 
			- search to find existing 
			- read CVs
			- define each loco/accessory in a config file. (like lcc)
		- global (terminology)
			- send commands without ACK (can't query CVs, values, etc)
	- lcc
		- search
		- map events.
	- Do we want to show a map?
		- with all accessories, so we can select, or zoom then select?
	
*/