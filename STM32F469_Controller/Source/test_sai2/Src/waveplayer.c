/**
  ******************************************************************************
  * @file    Audio/Audio_playback_and_record/Src/waveplayer.c 
  * @author  MCD Application Team
  * @brief   This file provides the Audio Out (playback) interface API
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "waveplayer.h"
#include "fatfs.h"
#include "stm32469i_discovery_audio.h"
#include "stm32f4xx_hal_i2s.h"
#include <math.h>	

/* Private define ------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
AUDIO_DEMO_StateMachine     AudioDemo;
AUDIO_PLAYBACK_StateTypeDef AudioState;

static AUDIO_OUT_BufferTypeDef  BufferCtl;
static int16_t FilePos = 0;
static __IO uint32_t uwVolume = 90;

WAVE_FormatTypeDef WaveFormat;
FIL WavFile;
extern FILELIST_FileTypeDef FileList;

/* Private function prototypes -----------------------------------------------*/
static AUDIO_ErrorTypeDef GetFileInfo(uint16_t file_idx, WAVE_FormatTypeDef *info);
static uint8_t PlayerInit(uint32_t AudioFreq);
static void AUDIO_PlaybackDisplayButtons(void);
static void AUDIO_AcquireTouchButtons(void);

/* Private functions ---------------------------------------------------------*/






FILELIST_FileTypeDef FileList;
uint16_t NumObs = 0;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

extern FATFS FatFs;

/**
  * @brief  Copies disk content in the explorer list.
  * @param  None
  * @retval Operation result
  */
FRESULT AUDIO_StorageParse(void)
{
  FRESULT res = FR_OK;
  FILINFO fno;
  DIR dir;
  char *fn;
  
  
  res = f_opendir(&dir, "/Audio");
  FileList.ptr = 0;
  
  if(res == FR_OK)
  {
    for(;;)
    {
      res = f_readdir(&dir, &fno);
      if(res != FR_OK || fno.fname[0] == 0)
      {
        break;
      }
      if(fno.fname[0] == '.')
      {
        continue;
      }
 
      fn = fno.fname;
      
      if(FileList.ptr < FILEMGR_LIST_DEPDTH)
      {
        if((fno.fattrib & AM_DIR) == 0)
        {
          if((strstr(fn, "wav")) || (strstr(fn, "WAV")))
          {
            strncpy((char *)FileList.file[FileList.ptr].name, (char *)fn, FILEMGR_FILE_NAME_SIZE);
            FileList.file[FileList.ptr].type = FILETYPE_FILE;
            FileList.ptr++;
          }
        }
      }   
    }
  }
  NumObs = FileList.ptr;
  f_closedir(&dir);
  return res;
}


/**
  * @brief  Shows audio file (*.wav) on the root
  * @param  None
  * @retval None
  */
uint8_t AUDIO_ShowWavFiles(void)
{
  uint8_t i = 0;
  uint8_t line_idx = 0;
  //if(AUDIO_StorageInit() == FR_OK)
  {
    if(AUDIO_StorageParse() ==  FR_OK)
    {
      if(FileList.ptr > 0)
      {
        //BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        printf("audio file(s) [ROOT]:\n\n");
        
        for(i = 0; i < FileList.ptr; i++)
        {
          line_idx++;
          if(line_idx > 9)
          {
            line_idx = 0;
            //printf("> Press [Key] To Continue.\n");
            
            /* USER Button in polling */
//            while(BSP_PB_GetState(BUTTON_USER) != RESET)
//            {
//              /* Wait for User Input */
//            }
          } 
          printf("   |__");
          printf((char *)FileList.file[i].name);
          printf("\n");
        }
//        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        printf("\nEnd of files list.\n");
        return 0;
      }
      return 1;
    }
    return 2;
  }
//  else
//  {
//    return 3;
//  }
}

/**
  * @brief  Gets Wav Object Number.
  * @param  None
  * @retval None
  */
uint16_t AUDIO_GetWavObjectNumber(void)
{
  return NumObs;
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/








/**
  * @brief  Initializes Audio Interface.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Init(void)
{
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_SPEAKER, uwVolume, I2S_AUDIOFREQ_48K) == 0)
  {
    return AUDIO_ERROR_NONE;
  }
  else
  {
    return AUDIO_ERROR_IO;
  }
}

/**
  * @brief  Starts Audio streaming.    
  * @param  idx: File index
  * @retval Audio error
  */ 
AUDIO_ErrorTypeDef AUDIO_PLAYER_Start(uint8_t idx)
{
  uint32_t bytesread;
  
  f_close(&WavFile);
  if(AUDIO_GetWavObjectNumber() > idx)
  { 
    GetFileInfo(idx, &WaveFormat);
    
    /*Adjust the Audio frequency */
    PlayerInit(WaveFormat.SampleRate); 
	  if (WaveFormat.NbrChannels == 2)
	  {
		  BSP_AUDIO_OUT_ChangeAudioConfig(BSP_AUDIO_OUT_STEREOMODE | BSP_AUDIO_OUT_CIRCULARMODE);
		  BSP_AUDIO_OUT_SetAudioFrameSlot(SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_2);
	  }
	  else
	  {
		    BSP_AUDIO_OUT_ChangeAudioConfig(BSP_AUDIO_OUT_MONOMODE | BSP_AUDIO_OUT_CIRCULARMODE);
            BSP_AUDIO_OUT_SetAudioFrameSlot(SAI_SLOTACTIVE_0);
	  }
    
    BufferCtl.state = BUFFER_OFFSET_NONE;
    
    /* Get Data from USB Flash Disk */
    f_lseek(&WavFile, 0);
    
    /* Fill whole buffer at first time */
    if(f_read(&WavFile, 
              &BufferCtl.buff[0], 
              AUDIO_OUT_BUFFER_SIZE, 
              (void *)&bytesread) == FR_OK)
    {
      AudioState = AUDIO_STATE_PLAY;
      AUDIO_PlaybackDisplayButtons();
      //BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t *)"  [PLAY ]", LEFT_MODE);
      { 
        if(bytesread != 0)
        {
          BSP_AUDIO_OUT_Play((uint16_t*)&BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE); 
          BufferCtl.fptr = bytesread;
          return AUDIO_ERROR_NONE;
        }
      }
    }
  }
  return AUDIO_ERROR_IO;
}


/**
  * @brief  Starts Audio streaming.    
  * @param  idx: File index
  * @retval Audio error
  */ 
AUDIO_ErrorTypeDef SINE_PLAYER_Start(uint8_t idx)
{
    PlayerInit(22050); 
	  if (0)
	  {
		  BSP_AUDIO_OUT_ChangeAudioConfig(BSP_AUDIO_OUT_STEREOMODE | BSP_AUDIO_OUT_CIRCULARMODE);
		  BSP_AUDIO_OUT_SetAudioFrameSlot(SAI_SLOTACTIVE_0 | SAI_SLOTACTIVE_2);
	  }
	  else
	  {
		    BSP_AUDIO_OUT_ChangeAudioConfig(BSP_AUDIO_OUT_MONOMODE | BSP_AUDIO_OUT_CIRCULARMODE);
            BSP_AUDIO_OUT_SetAudioFrameSlot(SAI_SLOTACTIVE_0);
	  }
    
    BufferCtl.state = BUFFER_OFFSET_NONE;

	const int BUFFER_SIZE = AUDIO_OUT_BUFFER_SIZE/2;
	int16_t *ptr = &(BufferCtl.buff[0]);
	for (int i = 0; i < BUFFER_SIZE; i++, ptr++)
		*ptr = 8000 * sin(M_PI * (double)i / (double)(BUFFER_SIZE / 4));
    {
      AudioState = AUDIO_STATE_PLAY;
      AUDIO_PlaybackDisplayButtons();
      //BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t *)"  [PLAY ]", LEFT_MODE);
      { 
        {
          BSP_AUDIO_OUT_Play((uint16_t*)&BufferCtl.buff[0], AUDIO_OUT_BUFFER_SIZE); 
          return AUDIO_ERROR_NONE;
        }
      }
    }
  
  return AUDIO_ERROR_IO;
}
/**
  * @brief  Manages Audio process. 
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Process(void)
{
  uint32_t bytesread, elapsed_time;
  AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
  static uint32_t prev_elapsed_time = 0xFFFFFFFF;
  uint8_t str[16];
  
  switch(AudioState)
  {
  case AUDIO_STATE_PLAY:
    if(BufferCtl.fptr >= WaveFormat.FileSize)
    {
      BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
      AudioState = AUDIO_STATE_NEXT;
    }
    
    if(BufferCtl.state == BUFFER_OFFSET_HALF)
    {
      if(f_read(&WavFile, 
                &BufferCtl.buff[0], 
                AUDIO_OUT_BUFFER_SIZE/2, 
                (void *)&bytesread) != FR_OK)
      { 
        BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW); 
        return AUDIO_ERROR_IO;       
      } 
      BufferCtl.state = BUFFER_OFFSET_NONE;
      BufferCtl.fptr += bytesread; 
    }
    
    if(BufferCtl.state == BUFFER_OFFSET_FULL)
    {
      if(f_read(&WavFile, 
                &BufferCtl.buff[AUDIO_OUT_BUFFER_SIZE /2], 
                AUDIO_OUT_BUFFER_SIZE/2, 
                (void *)&bytesread) != FR_OK)
      { 
        BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW); 
        return AUDIO_ERROR_IO;       
      } 
 
      BufferCtl.state = BUFFER_OFFSET_NONE;
      BufferCtl.fptr += bytesread; 
    }
    
    /* Display elapsed time */
    elapsed_time = BufferCtl.fptr / WaveFormat.ByteRate; 
    if(prev_elapsed_time != elapsed_time)
    {
      prev_elapsed_time = elapsed_time;
      sprintf((char *)str, "[%02d:%02d]", (int)(elapsed_time /60), (int)(elapsed_time%60));
      //BSP_LCD_SetTextColor(LCD_COLOR_CYAN); 
      //BSP_LCD_DisplayStringAt(263, LINE(8), str, LEFT_MODE);
      //BSP_LCD_SetTextColor(LCD_COLOR_WHITE); 
    }

    /* Update audio state machine according to touch acquisition */
    AUDIO_AcquireTouchButtons();
    break;
    
  case AUDIO_STATE_STOP:
    //BSP_LCD_SetTextColor(LCD_COLOR_RED);
//    BSP_LCD_FillRect(TOUCH_STOP_XMIN, TOUCH_STOP_YMIN , /* Stop rectangle */
//                     TOUCH_STOP_XMAX - TOUCH_STOP_XMIN,
//                     TOUCH_STOP_YMAX - TOUCH_STOP_YMIN);
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AudioState = AUDIO_STATE_IDLE; 
    audio_error = AUDIO_ERROR_IO;
    break;
    
  case AUDIO_STATE_NEXT:
    if(++FilePos >= AUDIO_GetWavObjectNumber())
    {
      FilePos = 0; 
    }
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AUDIO_PLAYER_Start(FilePos);
    break;    
    
  case AUDIO_STATE_PREVIOUS:
    if(--FilePos < 0)
    {
      FilePos = AUDIO_GetWavObjectNumber() - 1; 
    }
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AUDIO_PLAYER_Start(FilePos);
    break;   
    
  case AUDIO_STATE_PAUSE:
//    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//    BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t *)"  [PAUSE]", LEFT_MODE);
//    BSP_LCD_SetTextColor(LCD_COLOR_RED);    /* Display red pause rectangles */
//    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
//    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
//    BSP_AUDIO_OUT_Pause();
    AudioState = AUDIO_STATE_WAIT;
    break;
    
  case AUDIO_STATE_RESUME:
//    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//    BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t *)"  [PLAY ]", LEFT_MODE);
//                                            /* Display blue cyan pause rectangles */
//    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
//    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
//    BSP_AUDIO_OUT_Resume();
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_VOLUME_UP: 
    if( uwVolume <= 90)
    {
      uwVolume += 10;
    }
    BSP_AUDIO_OUT_SetVolume(uwVolume);
//    BSP_LCD_SetTextColor(LCD_COLOR_WHITE); 
    sprintf((char *)str,  "Volume : %lu ", uwVolume);
//    BSP_LCD_DisplayStringAtLine(9, str);
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_VOLUME_DOWN:    
    if( uwVolume >= 10)
    {
      uwVolume -= 10;
    }
    BSP_AUDIO_OUT_SetVolume(uwVolume);
//    BSP_LCD_SetTextColor(LCD_COLOR_WHITE); 
//    sprintf((char *)str,  "Volume : %lu ", uwVolume);
//    BSP_LCD_DisplayStringAtLine(9, str);
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_WAIT:
  case AUDIO_STATE_IDLE:
  case AUDIO_STATE_INIT:    
  default:
    /* Update audio state machine according to touch acquisition */
    AUDIO_AcquireTouchButtons();
    break;
  }
  return audio_error;
}

/**
  * @brief  Manages Audio process. 
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef SINE_PLAYER_Process(void)
{
  uint32_t bytesread, elapsed_time;
  AUDIO_ErrorTypeDef audio_error = AUDIO_ERROR_NONE;
  static uint32_t prev_elapsed_time = 0xFFFFFFFF;
  uint8_t str[16];
	static int count = 0;
  
  switch(AudioState)
  {
  case AUDIO_STATE_PLAY:
    if(count > 20)
    {
      BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
      AudioState = AUDIO_STATE_STOP;
	    count = 0;
    }
    
    if(BufferCtl.state == BUFFER_OFFSET_HALF)
    {
	    count++;

      BufferCtl.state = BUFFER_OFFSET_NONE;
      BufferCtl.fptr += bytesread; 
    }
    
    if(BufferCtl.state == BUFFER_OFFSET_FULL)
    {
	    count++;
 
      BufferCtl.state = BUFFER_OFFSET_NONE;
      BufferCtl.fptr += bytesread; 
    }
    
    break;
    
  case AUDIO_STATE_STOP:
    //BSP_LCD_SetTextColor(LCD_COLOR_RED);
//    BSP_LCD_FillRect(TOUCH_STOP_XMIN, TOUCH_STOP_YMIN , /* Stop rectangle */
//                     TOUCH_STOP_XMAX - TOUCH_STOP_XMIN,
//                     TOUCH_STOP_YMAX - TOUCH_STOP_YMIN);
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AudioState = AUDIO_STATE_IDLE; 
    audio_error = AUDIO_ERROR_IO;
    break;
    
  case AUDIO_STATE_NEXT:

    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    SINE_PLAYER_Start(FilePos);
    break;    
    
  case AUDIO_STATE_PREVIOUS:
    if(--FilePos < 0)
    {
      FilePos = AUDIO_GetWavObjectNumber() - 1; 
    }
    BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
    AUDIO_PLAYER_Start(FilePos);
    break;   
    
  case AUDIO_STATE_PAUSE:
//    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//    BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t *)"  [PAUSE]", LEFT_MODE);
//    BSP_LCD_SetTextColor(LCD_COLOR_RED);    /* Display red pause rectangles */
//    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
//    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
//    BSP_AUDIO_OUT_Pause();
    AudioState = AUDIO_STATE_WAIT;
    break;
    
  case AUDIO_STATE_RESUME:
//    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//    BSP_LCD_DisplayStringAt(250, LINE(9), (uint8_t *)"  [PLAY ]", LEFT_MODE);
//                                            /* Display blue cyan pause rectangles */
//    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
//    BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
//    BSP_AUDIO_OUT_Resume();
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_VOLUME_UP: 
    if( uwVolume <= 90)
    {
      uwVolume += 10;
    }
    BSP_AUDIO_OUT_SetVolume(uwVolume);
//    BSP_LCD_SetTextColor(LCD_COLOR_WHITE); 
    sprintf((char *)str,  "Volume : %lu ", uwVolume);
//    BSP_LCD_DisplayStringAtLine(9, str);
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_VOLUME_DOWN:    
    if( uwVolume >= 10)
    {
      uwVolume -= 10;
    }
    BSP_AUDIO_OUT_SetVolume(uwVolume);
//    BSP_LCD_SetTextColor(LCD_COLOR_WHITE); 
//    sprintf((char *)str,  "Volume : %lu ", uwVolume);
//    BSP_LCD_DisplayStringAtLine(9, str);
    AudioState = AUDIO_STATE_PLAY;
    break;
    
  case AUDIO_STATE_WAIT:
  case AUDIO_STATE_IDLE:
  case AUDIO_STATE_INIT:    
  default:
    /* Update audio state machine according to touch acquisition */
    AUDIO_AcquireTouchButtons();
    break;
  }
  return audio_error;
}

/**
  * @brief  Stops Audio streaming.
  * @param  None
  * @retval Audio error
  */
AUDIO_ErrorTypeDef AUDIO_PLAYER_Stop(void)
{
  AudioState = AUDIO_STATE_STOP;
  FilePos = 0;
  
  BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW);
  f_close(&WavFile);
  return AUDIO_ERROR_NONE;
}

/**
  * @brief  Calculates the remaining file size and new position of the pointer.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
  if(AudioState == AUDIO_STATE_PLAY)
  {
    BufferCtl.state = BUFFER_OFFSET_FULL;
  }
}

/**
  * @brief  Manages the DMA Half Transfer complete interrupt.
  * @param  None
  * @retval None
  */
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{ 
  if(AudioState == AUDIO_STATE_PLAY)
  {
    BufferCtl.state = BUFFER_OFFSET_HALF;
  }
}
/*******************************************************************************
                            Static Functions
*******************************************************************************/

/**
  * @brief  Gets the file info.
  * @param  file_idx: File index
  * @param  info: Pointer to WAV file info
  * @retval Audio error
  */
static AUDIO_ErrorTypeDef GetFileInfo(uint16_t file_idx, WAVE_FormatTypeDef *info)
{
  uint32_t bytesread;
  uint32_t duration;
  uint8_t str[FILEMGR_FILE_NAME_SIZE + 20];  
  
	strcpy(str, "\\Audio\\");
	strcat(str, (char *)FileList.file[file_idx].name);
  if(f_open(&WavFile, str, FA_OPEN_EXISTING | FA_READ) == FR_OK) 
  {
    /* Fill the buffer to Send */
    if(f_read(&WavFile, info, sizeof(WaveFormat), (void *)&bytesread) == FR_OK)
    {
//      BSP_LCD_SetTextColor(LCD_COLOR_WHITE); 
      printf("Playing file (%d/%d): %s", 
              file_idx + 1, FileList.ptr,
              (char *)FileList.file[file_idx].name);
//      BSP_LCD_ClearStringLine(4);
//      BSP_LCD_DisplayStringAtLine(4, str);
//      
//      BSP_LCD_SetTextColor(LCD_COLOR_CYAN); 
      printf("Sample rate : %d Hz", (int)(info->SampleRate));
//      BSP_LCD_ClearStringLine(6);
//      BSP_LCD_DisplayStringAtLine(6, str);
      
      printf("Channels number : %d", info->NbrChannels);
//      BSP_LCD_ClearStringLine(7);      
//      BSP_LCD_DisplayStringAtLine(7, str);
      
      duration = info->FileSize / info->ByteRate; 
      printf("File Size : %d KB [%02d:%02d]", (int)(info->FileSize/1024), (int)(duration/60), (int)(duration%60));
//      BSP_LCD_ClearStringLine(8);
//      BSP_LCD_DisplayStringAtLine(8, str);
//      BSP_LCD_DisplayStringAt(263, LINE(8), (uint8_t *)"[00:00]", LEFT_MODE);
// 
//      BSP_LCD_SetTextColor(LCD_COLOR_WHITE); 
      printf("Volume : %lu", uwVolume);
//      BSP_LCD_ClearStringLine(9);      
//      BSP_LCD_DisplayStringAtLine(9, str);
      return AUDIO_ERROR_NONE;
    }
    f_close(&WavFile);
  }
  return AUDIO_ERROR_IO;
}

/**
  * @brief  Initializes the Wave player.
  * @param  AudioFreq: Audio sampling frequency
  * @retval None
  */
static uint8_t PlayerInit(uint32_t AudioFreq)
{
  /* Initialize the Audio codec and all related peripherals (I2S, I2C, IOExpander, IOs...) */  
  if(BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_BOTH, uwVolume, AudioFreq) != 0)
  {
    return 1;
  }
  else
  {
    BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
    return 0;
  } 
}

/**
  * @brief  Display interface touch screen buttons
  * @param  None
  * @retval None
  */
static void AUDIO_PlaybackDisplayButtons(void)
{
//  BSP_LCD_SetFont(&LCD_LOG_HEADER_FONT);
//  BSP_LCD_ClearStringLine(14);            /* Clear dedicated zone */
//  BSP_LCD_ClearStringLine(15);           
//  BSP_LCD_ClearStringLine(16);
//  BSP_LCD_ClearStringLine(17);
//  BSP_LCD_ClearStringLine(18);
//  BSP_LCD_ClearStringLine(19);
//  BSP_LCD_ClearStringLine(20);
//
//  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//  BSP_LCD_FillPolygon(PreviousPoints, 3);   /* Previous track icon */
//  BSP_LCD_FillRect(TOUCH_PREVIOUS_XMIN, TOUCH_PREVIOUS_YMIN , 10, TOUCH_PREVIOUS_YMAX - TOUCH_PREVIOUS_YMIN);
//  BSP_LCD_FillPolygon(NextPoints, 3);       /* Next track icon */
//  BSP_LCD_FillRect(TOUCH_NEXT_XMAX-9, TOUCH_NEXT_YMIN , 10, TOUCH_NEXT_YMAX - TOUCH_NEXT_YMIN);
//  BSP_LCD_FillRect(TOUCH_PAUSE_XMIN, TOUCH_PAUSE_YMIN , 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);    /* Pause rectangles */
//  BSP_LCD_FillRect(TOUCH_PAUSE_XMIN + 20, TOUCH_PAUSE_YMIN, 15, TOUCH_PAUSE_YMAX - TOUCH_PAUSE_YMIN);
//  BSP_LCD_FillRect(TOUCH_STOP_XMIN, TOUCH_STOP_YMIN , /* Stop rectangle */
//                   TOUCH_STOP_XMAX - TOUCH_STOP_XMIN,
//                   TOUCH_STOP_YMAX - TOUCH_STOP_YMIN);
//  BSP_LCD_DrawRect(TOUCH_VOL_MINUS_XMIN, TOUCH_VOL_MINUS_YMIN , /* VOl- rectangle */
//                   TOUCH_VOL_MINUS_XMAX - TOUCH_VOL_MINUS_XMIN,
//                   TOUCH_VOL_MINUS_YMAX - TOUCH_VOL_MINUS_YMIN);
//  BSP_LCD_DisplayStringAt(26, LINE(17), (uint8_t *)"VOL-", LEFT_MODE);
//  BSP_LCD_DrawRect(TOUCH_VOL_PLUS_XMIN, TOUCH_VOL_PLUS_YMIN , /* VOl+ rectangle */
//                   TOUCH_VOL_PLUS_XMAX - TOUCH_VOL_PLUS_XMIN,
//                   TOUCH_VOL_PLUS_YMAX - TOUCH_VOL_PLUS_YMIN);
//  BSP_LCD_DisplayStringAt(626, LINE(17), (uint8_t *)"VOL+", LEFT_MODE);
//  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
//  BSP_LCD_ClearStringLine(22);
//  BSP_LCD_DisplayStringAtLine(22, (uint8_t *)"Use stop button to exit");
//  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
//  BSP_LCD_SetFont(&LCD_LOG_TEXT_FONT);
}

/**
  * @brief  Test touch screen state and modify audio state machine according to that
  * @param  None
  * @retval None
  */
static void AUDIO_AcquireTouchButtons(void)
{
//  static TS_StateTypeDef  TS_State={0};
//
//  if(TS_State.touchDetected == 1)   /* If previous touch has not been released, we don't proceed any touch command */
//  {
//    BSP_TS_GetState(&TS_State);
//  }
//  else
//  {
//    BSP_TS_GetState(&TS_State);
//    if(TS_State.touchDetected == 1)
//    {
//      if ((TS_State.touchX[0] > TOUCH_PAUSE_XMIN) && (TS_State.touchX[0] < TOUCH_PAUSE_XMAX) &&
//          (TS_State.touchY[0] > TOUCH_PAUSE_YMIN) && (TS_State.touchY[0] < TOUCH_PAUSE_YMAX))
//      {
//        if (AudioState == AUDIO_STATE_PLAY)
//        {
//          AudioState = AUDIO_STATE_PAUSE;
//        }
//        else
//        {
//          AudioState = AUDIO_STATE_RESUME;
//        }
//      }
//      else if ((TS_State.touchX[0] > TOUCH_NEXT_XMIN) && (TS_State.touchX[0] < TOUCH_NEXT_XMAX) &&
//               (TS_State.touchY[0] > TOUCH_NEXT_YMIN) && (TS_State.touchY[0] < TOUCH_NEXT_YMAX))
//      {
//        AudioState = AUDIO_STATE_NEXT;
//      }
//      else if ((TS_State.touchX[0] > TOUCH_PREVIOUS_XMIN) && (TS_State.touchX[0] < TOUCH_PREVIOUS_XMAX) &&
//               (TS_State.touchY[0] > TOUCH_PREVIOUS_YMIN) && (TS_State.touchY[0] < TOUCH_PREVIOUS_YMAX))
//      {
//        AudioState = AUDIO_STATE_PREVIOUS;
//      }
//      else if ((TS_State.touchX[0] > TOUCH_STOP_XMIN) && (TS_State.touchX[0] < TOUCH_STOP_XMAX) &&
//               (TS_State.touchY[0] > TOUCH_STOP_YMIN) && (TS_State.touchY[0] < TOUCH_STOP_YMAX))
//      {
//        AudioState = AUDIO_STATE_STOP;
//      }
//      else if((TS_State.touchX[0] > TOUCH_VOL_MINUS_XMIN) && (TS_State.touchX[0] < TOUCH_VOL_MINUS_XMAX) &&
//              (TS_State.touchY[0] > TOUCH_VOL_MINUS_YMIN) && (TS_State.touchY[0] < TOUCH_VOL_MINUS_YMAX))
//      {
//        AudioState = AUDIO_STATE_VOLUME_DOWN;
//      }
//      else if((TS_State.touchX[0] > TOUCH_VOL_PLUS_XMIN) && (TS_State.touchX[0] < TOUCH_VOL_PLUS_XMAX) &&
//              (TS_State.touchY[0] > TOUCH_VOL_PLUS_YMIN) && (TS_State.touchY[0] < TOUCH_VOL_PLUS_YMAX))
//      {
//        AudioState = AUDIO_STATE_VOLUME_UP;
//      }
//    }
//  }
}

void DoSDCard()
{
	AUDIO_ShowWavFiles();
	BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_SPEAKER, uwVolume, I2S_AUDIOFREQ_48K);

	AudioState = AUDIO_STATE_NEXT;
    for(;;)
    {
	    AUDIO_PLAYER_Process();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void DoSine()
{
	BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_SPEAKER, uwVolume, I2S_AUDIOFREQ_48K);

	AudioState = AUDIO_STATE_NEXT;
    for(;;)
    {
	    SINE_PLAYER_Process();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void DoWave()
{
	//DoSDCard();
	DoSine();

	
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
