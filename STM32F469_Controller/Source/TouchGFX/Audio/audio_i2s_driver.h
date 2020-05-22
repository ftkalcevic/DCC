#pragma once

void audioInit( void ) ;
void audioEnd( void ) ;
void audioConsumeCurrentBuffer();
#define audioDisableIrq()       __disable_irq()
#define audioEnableIrq()        __enable_irq()

void setSampleRate( uint32_t frequency ) ;

#define VOLUME_LEVEL_MAX  23
#define VOLUME_LEVEL_DEF  12

void setScaledVolume(uint8_t volume);


