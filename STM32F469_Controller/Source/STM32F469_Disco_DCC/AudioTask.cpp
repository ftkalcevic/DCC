// Parts from audio_arm.cpp/audio_arm.h...
/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <string.h>
#include "main.h"
#include "AudioTask.h"
#include "stm32469i_discovery_audio.h"
#include "fatfs.h"

//void printf(...){}
extern "C" void BSP_AUDIO_OUT_ChangeAudioConfig(uint32_t AudioOutOption);


extern FATFS FatFs;	// this should be in a header but struggling because of the FATFS unnamed struct typedef, and other include clashes
AudioTask audioTask;


struct WAVE_FormatType_Chunk1 {
	uint32_t ChunkID; /* 0 */ 
	uint32_t FileSize; /* 4 */
	uint32_t FileFormat; /* 8 */
};
struct WAVE_FormatType_SubChunk1 {
	uint32_t SubChunk1ID; /* 12 */
	uint32_t SubChunk1Size; /* 16*/  
	uint16_t AudioFormat; /* 20 */ 
	uint16_t NbrChannels; /* 22 */   
	uint32_t SampleRate; /* 24 */
  
	uint32_t ByteRate; /* 28 */
	uint16_t BlockAlign; /* 32 */  
	uint16_t BitPerSample; /* 34 */  
};
struct WAVE_FormatType_SubChunk2 {
  uint32_t SubChunk2ID;   /* 36 */   
  uint32_t SubChunk2Size; /* 40 */    
};
const int WAVE_HEADER_LEN = 44;
const char *rootAudioPath = "/Audio/";

// The order of audioFilenames must match those in the enum EAudioSounds
static const char * audioFilenames[] = { 
	"Bell1.wav",
	"Bell2.wav",
	"Chug.wav",
	"Whistle1.wav",
	"Whistle2.wav",
	"Whistle5.wav",
	"Whistle9.wav",
	"Tone2.wav"
};

template<class t> t min(t a, t b) { return a<b?a:b; }
template<class t> t max(t a, t b) { return a>b?a:b; }
template<class t> t sgn(t a) { return a>0 ? 1 : (a < 0 ? -1 : 0); }
template<class t> t limit(t mi, t x, t ma) { return min(max(mi,x),ma); }

const int16_t sineValues[] =
{
    0, 196, 392, 588, 784, 980, 1175, 1370, 1564, 1758,
    1951, 2143, 2335, 2525, 2715, 2904, 3091, 3278, 3463, 3647,
    3829, 4011, 4190, 4369, 4545, 4720, 4894, 5065, 5235, 5403,
    5569, 5733, 5895, 6055, 6213, 6369, 6522, 6673, 6822, 6969,
    7113, 7255, 7395, 7532, 7667, 7799, 7929, 8056, 8180, 8302,
    8422, 8539, 8653, 8765, 8874, 8980, 9084, 9185, 9283, 9379,
    9472, 9562, 9650, 9735, 9818, 9898, 9975, 10050, 10123, 10192,
    10260, 10324, 10387, 10447, 10504, 10559, 10612, 10663, 10711, 10757,
    10801, 10843, 10882, 10920, 10955, 10989, 11020, 11050, 11078, 11104,
    11128, 11151, 11172, 11191, 11209, 11225, 11240, 11254, 11266, 11277,
    11287, 11296, 11303, 11310, 11316, 11320, 11324, 11327, 11330, 11331,
    11332, 11333, 11333, 11333, 11332, 11331, 11329, 11328, 11326, 11324,
    11323, 11321, 11319, 11318, 11316, 11315, 11314, 11313, 11313, 11313,
    11314, 11315, 11317, 11319, 11323, 11326, 11331, 11336, 11342, 11349,
    11356, 11365, 11375, 11385, 11397, 11409, 11423, 11437, 11453, 11470,
    11488, 11507, 11527, 11548, 11571, 11595, 11620, 11646, 11673, 11702,
    11732, 11763, 11795, 11828, 11863, 11899, 11936, 11974, 12013, 12054,
    12095, 12138, 12182, 12227, 12273, 12320, 12368, 12417, 12467, 12518,
    12570, 12623, 12676, 12731, 12786, 12842, 12898, 12956, 13014, 13072,
    13131, 13191, 13251, 13311, 13372, 13433, 13495, 13556, 13618, 13680,
    13743, 13805, 13867, 13929, 13991, 14053, 14115, 14177, 14238, 14299,
    14359, 14419, 14479, 14538, 14597, 14655, 14712, 14768, 14824, 14879,
    14933, 14986, 15039, 15090, 15140, 15189, 15237, 15284, 15330, 15375,
    15418, 15460, 15500, 15539, 15577, 15614, 15648, 15682, 15714, 15744,
    15772, 15799, 15825, 15849, 15871, 15891, 15910, 15927, 15942, 15955,
    15967, 15977, 15985, 15991, 15996, 15999, 16000, 15999, 15996, 15991,
    15985, 15977, 15967, 15955, 15942, 15927, 15910, 15891, 15871, 15849,
    15825, 15799, 15772, 15744, 15714, 15682, 15648, 15614, 15577, 15539,
    15500, 15460, 15418, 15375, 15330, 15284, 15237, 15189, 15140, 15090,
    15039, 14986, 14933, 14879, 14824, 14768, 14712, 14655, 14597, 14538,
    14479, 14419, 14359, 14299, 14238, 14177, 14115, 14053, 13991, 13929,
    13867, 13805, 13743, 13680, 13618, 13556, 13495, 13433, 13372, 13311,
    13251, 13191, 13131, 13072, 13014, 12956, 12898, 12842, 12786, 12731,
    12676, 12623, 12570, 12518, 12467, 12417, 12368, 12320, 12273, 12227,
    12182, 12138, 12095, 12054, 12013, 11974, 11936, 11899, 11863, 11828,
    11795, 11763, 11732, 11702, 11673, 11646, 11620, 11595, 11571, 11548,
    11527, 11507, 11488, 11470, 11453, 11437, 11423, 11409, 11397, 11385,
    11375, 11365, 11356, 11349, 11342, 11336, 11331, 11326, 11323, 11319,
    11317, 11315, 11314, 11313, 11313, 11313, 11314, 11315, 11316, 11318,
    11319, 11321, 11323, 11324, 11326, 11328, 11329, 11331, 11332, 11333,
    11333, 11333, 11332, 11331, 11330, 11327, 11324, 11320, 11316, 11310,
    11303, 11296, 11287, 11277, 11266, 11254, 11240, 11225, 11209, 11191,
    11172, 11151, 11128, 11104, 11078, 11050, 11020, 10989, 10955, 10920,
    10882, 10843, 10801, 10757, 10711, 10663, 10612, 10559, 10504, 10447,
    10387, 10324, 10260, 10192, 10123, 10050, 9975, 9898, 9818, 9735,
    9650, 9562, 9472, 9379, 9283, 9185, 9084, 8980, 8874, 8765,
    8653, 8539, 8422, 8302, 8180, 8056, 7929, 7799, 7667, 7532,
    7395, 7255, 7113, 6969, 6822, 6673, 6522, 6369, 6213, 6055,
    5895, 5733, 5569, 5403, 5235, 5065, 4894, 4720, 4545, 4369,
    4190, 4011, 3829, 3647, 3463, 3278, 3091, 2904, 2715, 2525,
    2335, 2143, 1951, 1758, 1564, 1370, 1175, 980, 784, 588,
    392, 196, 0, -196, -392, -588, -784, -980, -1175, -1370,
    -1564, -1758, -1951, -2143, -2335, -2525, -2715, -2904, -3091, -3278,
    -3463, -3647, -3829, -4011, -4190, -4369, -4545, -4720, -4894, -5065,
    -5235, -5403, -5569, -5733, -5895, -6055, -6213, -6369, -6522, -6673,
    -6822, -6969, -7113, -7255, -7395, -7532, -7667, -7799, -7929, -8056,
    -8180, -8302, -8422, -8539, -8653, -8765, -8874, -8980, -9084, -9185,
    -9283, -9379, -9472, -9562, -9650, -9735, -9818, -9898, -9975, -10050,
    -10123, -10192, -10260, -10324, -10387, -10447, -10504, -10559, -10612, -10663,
    -10711, -10757, -10801, -10843, -10882, -10920, -10955, -10989, -11020, -11050,
    -11078, -11104, -11128, -11151, -11172, -11191, -11209, -11225, -11240, -11254,
    -11266, -11277, -11287, -11296, -11303, -11310, -11316, -11320, -11324, -11327,
    -11330, -11331, -11332, -11333, -11333, -11333, -11332, -11331, -11329, -11328,
    -11326, -11324, -11323, -11321, -11319, -11318, -11316, -11315, -11314, -11313,
    -11313, -11313, -11314, -11315, -11317, -11319, -11323, -11326, -11331, -11336,
    -11342, -11349, -11356, -11365, -11375, -11385, -11397, -11409, -11423, -11437,
    -11453, -11470, -11488, -11507, -11527, -11548, -11571, -11595, -11620, -11646,
    -11673, -11702, -11732, -11763, -11795, -11828, -11863, -11899, -11936, -11974,
    -12013, -12054, -12095, -12138, -12182, -12227, -12273, -12320, -12368, -12417,
    -12467, -12518, -12570, -12623, -12676, -12731, -12786, -12842, -12898, -12956,
    -13014, -13072, -13131, -13191, -13251, -13311, -13372, -13433, -13495, -13556,
    -13618, -13680, -13743, -13805, -13867, -13929, -13991, -14053, -14115, -14177,
    -14238, -14299, -14359, -14419, -14479, -14538, -14597, -14655, -14712, -14768,
    -14824, -14879, -14933, -14986, -15039, -15090, -15140, -15189, -15237, -15284,
    -15330, -15375, -15418, -15460, -15500, -15539, -15577, -15614, -15648, -15682,
    -15714, -15744, -15772, -15799, -15825, -15849, -15871, -15891, -15910, -15927,
    -15942, -15955, -15967, -15977, -15985, -15991, -15996, -15999, -16000, -15999,
    -15996, -15991, -15985, -15977, -15967, -15955, -15942, -15927, -15910, -15891,
    -15871, -15849, -15825, -15799, -15772, -15744, -15714, -15682, -15648, -15614,
    -15577, -15539, -15500, -15460, -15418, -15375, -15330, -15284, -15237, -15189,
    -15140, -15090, -15039, -14986, -14933, -14879, -14824, -14768, -14712, -14655,
    -14597, -14538, -14479, -14419, -14359, -14299, -14238, -14177, -14115, -14053,
    -13991, -13929, -13867, -13805, -13743, -13680, -13618, -13556, -13495, -13433,
    -13372, -13311, -13251, -13191, -13131, -13072, -13014, -12956, -12898, -12842,
    -12786, -12731, -12676, -12623, -12570, -12518, -12467, -12417, -12368, -12320,
    -12273, -12227, -12182, -12138, -12095, -12054, -12013, -11974, -11936, -11899,
    -11863, -11828, -11795, -11763, -11732, -11702, -11673, -11646, -11620, -11595,
    -11571, -11548, -11527, -11507, -11488, -11470, -11453, -11437, -11423, -11409,
    -11397, -11385, -11375, -11365, -11356, -11349, -11342, -11336, -11331, -11326,
    -11323, -11319, -11317, -11315, -11314, -11313, -11313, -11313, -11314, -11315,
    -11316, -11318, -11319, -11321, -11323, -11324, -11326, -11328, -11329, -11331,
    -11332, -11333, -11333, -11333, -11332, -11331, -11330, -11327, -11324, -11320,
    -11316, -11310, -11303, -11296, -11287, -11277, -11266, -11254, -11240, -11225,
    -11209, -11191, -11172, -11151, -11128, -11104, -11078, -11050, -11020, -10989,
    -10955, -10920, -10882, -10843, -10801, -10757, -10711, -10663, -10612, -10559,
    -10504, -10447, -10387, -10324, -10260, -10192, -10123, -10050, -9975, -9898,
    -9818, -9735, -9650, -9562, -9472, -9379, -9283, -9185, -9084, -8980,
    -8874, -8765, -8653, -8539, -8422, -8302, -8180, -8056, -7929, -7799,
    -7667, -7532, -7395, -7255, -7113, -6969, -6822, -6673, -6522, -6369,
    -6213, -6055, -5895, -5733, -5569, -5403, -5235, -5065, -4894, -4720,
    -4545, -4369, -4190, -4011, -3829, -3647, -3463, -3278, -3091, -2904,
    -2715, -2525, -2335, -2143, -1951, -1758, -1564, -1370, -1175, -980,
    -784, -588, -392, -196,
};

extern "C" void AudioTask_Entry(void *argument)
{
	static_assert(countof(audioFilenames) == EAudioSounds::LAST_SAMPLE,"audioFilenames array does not match EAudioSounds");
	
//	uint8_t uwVolume = 90;
//	if (BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_SPEAKER, uwVolume, AUDIO_FREQ) != AUDIO_OK)
//	{
//		printf("BSP_AUDIO_OUT_Init failed\n");
//		vTaskDelete(NULL);
//	}
//	//BSP_AUDIO_OUT_SetAudioFrameSlot(CODEC_AUDIOFRAME_SLOT_02);
//	//BSP_AUDIO_OUT_SetAudioFrameSlot(SAI_SLOTACTIVE_2 | SAI_SLOTACTIVE_3);
//	//BSP_AUDIO_OUT_ChangeAudioConfig(BSP_AUDIO_OUT_MONOMODE | BSP_AUDIO_OUT_CIRCULARMODE);
//	
//	BSP_AUDIO_OUT_ChangeAudioConfig(BSP_AUDIO_OUT_MONOMODE | BSP_AUDIO_OUT_CIRCULARMODE);
//    BSP_AUDIO_OUT_SetAudioFrameSlot(SAI_SLOTACTIVE_0);	
	
	audioTask.Init();
	audioTask.Run();
}


void AudioTask::Run()
{
//	vTaskDelay(pdMS_TO_TICKS(500));
//	audioTask.PlaySound(EAudioSounds::KeyPressTone);

	for (;;)
	{
		PlaySoundRecord rec;
		if (xQueueReceive(hQueue, &rec, portMAX_DELAY) == pdPASS)
			//if (xQueueReceive(hQueue, &rec, pdMS_TO_TICKS(1000)) == pdPASS)
		{
			// Something to do.  Create a new play queue buffer entry.
			ProcessEvent(rec.sound);
		}
	}
}

void AudioTask::PlaySound(EAudioSounds sound, bool highPriority)
{
	PlaySoundRecord rec;
	rec.sound = sound;
	
	if (uxQueueSpacesAvailable(hQueue) > 0)
	{
		xQueueSend(hQueue, &rec, 0);
	}
	else if (highPriority)
	{
		// Full queue, but high priority, remove last entry.
		xQueueOverwrite(hQueue, &rec);
	}
}

void AudioTask::SoundEvent(EAudioSounds sound)
{
	PlaySoundRecord rec;
	rec.sound = sound;
	
	if ( !xQueueIsQueueFullFromISR(hQueue) )
	{
		xQueueSendFromISR(hQueue, &rec, 0);
	}
	else 
	{
		// Full queue, but high priority, remove last entry.
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xQueueOverwriteFromISR(hQueue, &rec, &xHigherPriorityTaskWoken);
	}
}

void AudioTask::ProcessSample(EAudioSounds sample)
{
	if (sample >= EAudioSounds::LAST_SAMPLE)
	{
		static_assert(("sample out of range", true));
		return;
	}
	
	FIL soundFile;
	{
		const int MAX_PATH_LEN = 128;
		char filePath[MAX_PATH_LEN];
		strcpy(filePath, rootAudioPath);
		strncat(filePath, audioFilenames[sample],MAX_PATH_LEN);

		if (f_open(&soundFile, filePath, FA_OPEN_EXISTING | FA_READ) != FR_OK) 
			return;
	}
	
	bool error = true;
	{
		
		bool localPlaying = playing;
		UINT bytesread;
		uint8_t buffer[256];
		if (f_read(&soundFile, (void*)buffer, sizeof(buffer), &bytesread) == FR_OK)
		{
			WAVE_FormatType_Chunk1 *info1 = (WAVE_FormatType_Chunk1 *)buffer;
			WAVE_FormatType_SubChunk1 *info2 = (WAVE_FormatType_SubChunk1 *)(buffer+sizeof(WAVE_FormatType_Chunk1));
			WAVE_FormatType_SubChunk2 *info3 = (WAVE_FormatType_SubChunk2 *)((uint8_t*)info2+8+info2->SubChunk1Size);
			// Make sure we can play this type of file...
			if(info1->ChunkID == WAVE_CHUNKID &&
				 info1->FileFormat == WAVE_FILE_FORMAT &&
				 info2->SubChunk1ID == WAVE_SUB_CHUNKID_HEADER &&
				 info2->AudioFormat == WAVE_AUDIO_FORMAT_PCM //&&
				 //info2->NbrChannels == WAVE_CHANNELS_MONO &&
				 //info2->BitPerSample == AUDIO_BITS &&
				// info2->SampleRate == AUDIO_FREQ
				)
			{
				// Happy.  Add file to playlist.
				int index = FindFreeEntry();
				if (index >= 0)
				{
					uint32_t dataOffset = sizeof(WAVE_FormatType_Chunk1) + (8+info2->SubChunk1Size) + sizeof(WAVE_FormatType_SubChunk2);
					f_lseek(&soundFile, dataOffset);
					playList[index].type = PlayListRecord::EPlayType::Wave;
					playList[index].wave.fil = soundFile;
					playList[index].wave.stillToPlay = info3->SubChunk2Size / 2;
					error = false;
					if (!localPlaying)
					{
						// Kickstart player.
						PlaySound(EAudioSounds::BUFFER_EMPTY);
						localPlaying = true;
					}
				}
			}
		}
		
	}
	if (error)
		f_close(&soundFile);
}

void AudioTask::CreateTone(uint16_t freq, uint16_t len, uint16_t pause, uint8_t flags, int8_t freqIncr)
{
	int index = FindFreeEntry();
	if (index >= 0)
	{
		playList[index].type = PlayListRecord::EPlayType::Tone;
		playList[index].tone.freq = freq;
		playList[index].tone.len = len;
		playList[index].tone.pause = pause;
		playList[index].tone.flags = flags;
		playList[index].tone.freqIncr = freqIncr;
		
		playList[index].tone.currentFreq = 0;
		playList[index].tone.step = 0;
		playList[index].tone.idx = 0;
		playList[index].tone.playedLen = 0;
		if (!playing)
		{
			// Kickstart player.
			PlaySound(EAudioSounds::BUFFER_EMPTY);
		}
	}
}

void AudioTask::ProcessTone(EAudioSounds tone)
{
	switch (tone)
	{
		case KeyPressTone:	
			//ProcessSample(Bell2); 
			CreateTone(1600, 550, 0); //CreateTone(BEEP_DEFAULT_FREQ, 400, 0); 
			break;
		default:
			break;
	}
}

void AudioTask::mix(int16_t *dest, int16_t *source, uint16_t len)
{
	while(len)
	{
		*dest += *source;
		dest++;
		source++;
		len--;
	}
}

		int16_t tempBuffer[AUDIO_BUFFER_SIZE];
void AudioTask::ProcessBufferFill(EAudioSounds event)
{
	printf("%dms %d>", xTaskGetTickCount()*1000/configTICK_RATE_HZ, event);
	int16_t sounds = 0;
	if (GetEntryCount() > 0)
	{
		int bufferDuration, bufferLen, bufferOffset;		// in samples (int16)
		if (!playing)
		{
			bufferLen = AUDIO_BUFFER_SIZE;  	// full buffer
			bufferDuration = 2*AUDIO_BUFFER_DURATION;
			bufferOffset = 0;
		}
		else
		{
			if (event == EAudioSounds::BUFFER_HALF_EMPTY)
			{
				bufferOffset = 0;
				bufferLen = AUDIO_BUFFER_SIZE / 2;  		// 1/2 buffer
				bufferDuration = AUDIO_BUFFER_DURATION;
			}
			else
			{
				bufferOffset = AUDIO_BUFFER_SIZE / 2;
				bufferLen = AUDIO_BUFFER_SIZE / 2;  		// 1/2 buffer
				bufferDuration = AUDIO_BUFFER_DURATION;
			}
		}
		printf(" %d:%d(%d) ", bufferOffset, bufferLen,DMA2_Stream3->NDTR);
			
		// Clear buffer
		memset(AudioTransferBuffer + bufferOffset, 0, bufferLen);
//		for(int i = bufferOffset; i < bufferOffset+bufferLen ; i++)
//			AudioTransferBuffer[i] = AUDIO_DATA_SILENCE;
		
		// Merge data.
		for(int pl = 0 ; pl < PLAY_LIST_SIZE ; pl++)
		{
			PlayListRecord &play = playList[pl];
			switch (play.type)
			{
				case PlayListRecord::EPlayType::Wave:
					{
						UINT bytesRead;
						if (f_read(&(play.wave.fil), (void*)tempBuffer, bufferLen*AUDIO_BYTES, &bytesRead) == FR_OK)
						{
							uint16_t samples = bytesRead / AUDIO_BYTES;
							if (play.wave.stillToPlay > 0 && samples > 0)
							{
								mix(AudioTransferBuffer + bufferOffset, tempBuffer, samples);
								if (play.wave.stillToPlay > samples)
									play.wave.stillToPlay -= samples;
								else
									play.wave.stillToPlay = 0; 
								sounds++;
							}
							if (play.wave.stillToPlay == 0 || samples != bufferLen)
							{
								f_close(&(play.wave.fil));
								play.type = PlayListRecord::EPlayType::None;
							}
							printf(" %d,%d", samples, play.wave.stillToPlay);
						}
						else
						{
							// Problem reading.  Cancel this playback.
							f_close(&(play.wave.fil));
							play.type = PlayListRecord::EPlayType::None;
						}
					}
					break;
				case PlayListRecord::EPlayType::Tone:
{
	int duration = 0;
	int result = 0;

	int remainingDuration = play.tone.len - play.tone.playedLen;
	if (remainingDuration > 0) 
	{
		sounds++;
		int points;
		float toneIdx = play.tone.idx;

//    if (fragment.tone.reset) {
//      fragment.tone.reset = 0;
//      state.duration = 0;
//      state.pause = 0;
//    }

		if (play.tone.freq != play.tone.currentFreq) 
		{
			play.tone.currentFreq = play.tone.freq;
			play.tone.step = limit<float>(1, float(play.tone.freq) * (float(countof(sineValues))/float(AUDIO_FREQ)), countof(sineValues)/2);
		}
	  
//    if (fragment.tone.freqIncr) {
//      int freqChange = AUDIO_BUFFER_DURATION * fragment.tone.freqIncr;
//      if (freqChange > 0) {
//        fragment.tone.freq += freqChange;
//        if (fragment.tone.freq > BEEP_MAX_FREQ) {
//          fragment.tone.freq = BEEP_MAX_FREQ;
//        }
//      }
//      else {
//        if (fragment.tone.freq > BEEP_MIN_FREQ - freqChange) {
//          fragment.tone.freq += freqChange;
//        }
//        else {
//          fragment.tone.freq = BEEP_MIN_FREQ;
//        }
//      }
//    }

		if (remainingDuration > bufferDuration) 
			{
			duration = bufferDuration;
			points = bufferLen;
		}
		else 
		{
			duration = remainingDuration;
			points = (duration * bufferLen) / bufferDuration;
			unsigned int end = toneIdx + (play.tone.step * points);
			if (end > countof(sineValues))
				end -= (end % countof(sineValues));
//			else
//				end = countof(sineValues);
			points = (float(end) - toneIdx) / play.tone.step;
		}
		printf("P%d ", points);

		for (int i=0; i<points; i++) 
		{
			int16_t sample = sineValues[int(toneIdx)];
			tempBuffer[i] = sample;
			toneIdx += play.tone.step;
			if ((unsigned int)toneIdx >= countof(sineValues))
				toneIdx -= countof(sineValues);
		}
		mix(AudioTransferBuffer + bufferOffset, tempBuffer, points);

		if (remainingDuration > bufferDuration) 
		{
			play.tone.playedLen += bufferDuration;
		}
		else 
		{
			play.type = PlayListRecord::EPlayType::None;
		}
	}

//	remainingDuration = fragment.tone.pause - state.pause;
//	if (remainingDuration > 0) 
//	{
//		result = AUDIO_BUFFER_SIZE;
//		state.pause += min<unsigned int>(AUDIO_BUFFER_DURATION-duration, fragment.tone.pause);
//		if (fragment.tone.pause > state.pause)
//		return result;
//	}
//
//  clear();
//  return result;
}
					
					break;
				case PlayListRecord::EPlayType::None:
					break;
			}
		}
		if (sounds > 0 && !playing)
		{
			printf("P");

			
			uint8_t uwVolume = 80;
			BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_SPEAKER, uwVolume, AUDIO_FREQ);
			BSP_AUDIO_OUT_SetAudioFrameSlot(SAI_SLOTACTIVE_0);	
			BSP_AUDIO_OUT_ChangeAudioConfig(BSP_AUDIO_OUT_MONOMODE | BSP_AUDIO_OUT_CIRCULARMODE);

			
			
			BSP_AUDIO_OUT_Play((uint16_t*)AudioTransferBuffer, bufferLen*AUDIO_BYTES);	// len in bytes 
			playing = true;
		}
	}
	if ( sounds == 0 )
	{
		printf("S");
        BSP_AUDIO_OUT_Stop(CODEC_PDWN_SW); 
		playing = false;
	}
	printf(" %dms\n",xTaskGetTickCount()*1000/configTICK_RATE_HZ);
}

void AudioTask::ProcessEvent(EAudioSounds event)
{
	switch (event)
	{
		case EAudioSounds::BUFFER_HALF_EMPTY:
		case EAudioSounds::BUFFER_EMPTY:
			// Build next buffer
			ProcessBufferFill(event);
			break;
		default:
			if (event < EAudioSounds::LAST_SAMPLE)
			{
				ProcessSample(event);
			}
			else if (event < EAudioSounds::LAST_TONE)
			{
				ProcessTone(event);
			}
	}
}


/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

void BSP_AUDIO_OUT_HalfTransfer_CallBack(void)
{ 
	audioTask.SoundEvent(BUFFER_HALF_EMPTY);
}


void BSP_AUDIO_OUT_TransferComplete_CallBack(void)
{
	audioTask.SoundEvent(BUFFER_EMPTY);
}
