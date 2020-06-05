#include "FileSystem.h"
#include "FreeRTOS.h"
#include "semphr.h"

FATFS FileSystem::FatFs;
bool FileSystem::initialised = false;
	

void FileSystem::Initialise()
{
	if (!initialised)
	{
		extern osMutexId_t initialisationMutexHandle;
		
		osMutexAcquire(initialisationMutexHandle, portMAX_DELAY);
		if (!initialised)
		{
			FRESULT res = f_mount(&FatFs, "/", 1);
			if (res == FR_OK)
			{
				initialised = true;
			}
		}
		osMutexRelease(initialisationMutexHandle);
	}
}
