#pragma once

#include "main.h"
#include "stm32f4xx_hal_gpio.h"
#include "cmsis_os.h"
#include "semphr.h"
#include "fatfs.h"


#define AUDIOFREQ_192K               (192000U)
#define AUDIOFREQ_96K                (96000U)
#define AUDIOFREQ_48K                (48000U)
#define AUDIOFREQ_44K                (44100U)
#define AUDIOFREQ_32K                (32000U)
#define AUDIOFREQ_22K                (22050U)
#define AUDIOFREQ_16K                (16000U)
#define AUDIOFREQ_11K                (11025U)
#define AUDIOFREQ_8K                 (8000U)
#define AUDIOFREQ_DEFAULT            (2U)

#define AUDIO_FREQ					AUDIOFREQ_22K
#define AUDIO_BITS					16
#define AUDIO_BYTES					(AUDIO_BITS/8)

#define BEEP_MIN_FREQ				150
#define BEEP_MAX_FREQ               15000
#define BEEP_DEFAULT_FREQ           2250

// Tone flags
#define PLAY_REPEAT(x)				(x)                 /* Range 0 to 15 */
#define PLAY_INCREMENT(x)			((uint8_t)(((uint8_t)x) << 6))   /* -1, 0, 1, 2 */



#define WAVE_CHUNKID				0x46464952	// little endian
#define WAVE_FILE_FORMAT			0x45564157	// little endian
#define WAVE_SUB_CHUNKID_HEADER		0x20746d66	// little endian
#define WAVE_SUB_CHUNKID_DATA		0x01f90002	// little endian
#define WAVE_AUDIO_FORMAT_PCM		1
#define WAVE_CHANNELS_MONO			1


const int PLAYSOUND_QUEUE_LENGTH = 5;
const int PLAY_LIST_SIZE = 5;
const int AUDIO_BUFFER_DURATION = 100;				// ms
const int AUDIO_BUFFER_SIZE = 2 * AUDIO_FREQ * AUDIO_BUFFER_DURATION / 1000;		// samples needed for 2 buffers of 100ms storage (22khz, 16 bit, mono)
	


enum EAudioSounds
{
	Bell1=0,
	Bell2,
	Chug,
	Whistle1,
	Whistle2,
	Whistle5,
	Whistle9,
	Tone,
	LAST_SAMPLE,
	KeyPressTone,
	BadKeyPressTone,
	LAST_TONE,
	BUFFER_HALF_EMPTY = 0xFE,
	BUFFER_EMPTY = 0xFF
};

struct PlaySoundRecord
{
	EAudioSounds sound;
};

struct PlayListRecord
{
	enum EPlayType
	{
		None,
		Wave,
		Tone
	} type;
	union
	{
		struct
		{
			FIL fil;
			uint32_t stillToPlay;
		} wave;
		struct
		{
			// config
			uint16_t freq;		// Frequency in Hz
			uint16_t len;		// Len in ms
			uint16_t pause;		// pause between repeats in ms
			uint8_t repeats;	// repeats
			
			// working variables
			uint16_t currentFreq;
			float step;
			float idx;
			uint16_t playedLen;
			bool playing;		// playing or pausing
		} tone;
	};
};


class AudioTask
{
	QueueHandle_t hQueue;
	PlaySoundRecord QueueStorageBuffer[PLAYSOUND_QUEUE_LENGTH];
	StaticQueue_t QueueBuffer;
	PlayListRecord playList[PLAY_LIST_SIZE];
	bool playing;
	bool mute;
	int16_t AudioTransferBuffer[AUDIO_BUFFER_SIZE];
	int16_t tempBuffer[AUDIO_BUFFER_SIZE];
	static_assert(AUDIO_BITS == 16,"Wave file bits must be 16bit");
	
	void CreateTone(uint16_t freq, uint16_t len, uint8_t repeats=0, uint16_t pause = 0);
	void mix(int16_t *dest, int16_t *source, uint16_t len);
	void ProcessEvent(EAudioSounds event);
	void ProcessSample(EAudioSounds sample);
	void ProcessTone(EAudioSounds tone);
	void ProcessBufferFill(EAudioSounds event);
	int FindFreeEntry()
	{
		for (int i = 0; i < PLAY_LIST_SIZE; i++)
		{
			if ( playList[i].type == PlayListRecord::EPlayType::None )
				return i;
		}
		return -1;
	}
	int GetEntryCount()
	{
		int count = 0;
		for (int i = 0; i < PLAY_LIST_SIZE; i++)
		{
			if (playList[i].type != PlayListRecord::EPlayType::None)
				count++;
		}
		return count;
	}
	
public:
	void Init()
	{
		hQueue = xQueueCreateStatic( PLAYSOUND_QUEUE_LENGTH, sizeof(PlaySoundRecord), (uint8_t *)QueueStorageBuffer, &QueueBuffer );
		for (int i = 0; i < PLAY_LIST_SIZE; i++)
		{
			playList[i].type = PlayListRecord::EPlayType::None;
		}
		playing = false;
		mute = false;
	}
	
	void Run();
	void PlaySound(EAudioSounds sound, bool highPriority=false);
	void SoundEvent(EAudioSounds sound);
	void Mute(bool mute);
};


extern "C" void AudioTask_Entry(void *argument);
extern AudioTask audioTask;
