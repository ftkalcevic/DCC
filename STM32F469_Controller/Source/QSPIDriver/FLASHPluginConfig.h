#pragma once

#pragma once

#include <stm32f4xx_hal.h>
#include <stm32469i_discovery_qspi.h>
#include <n25q128a\n25q128a.h>


#define MINIMUM_PROGRAMMED_BLOCK_SIZE N25Q128A_PAGE_SIZE
#define FLASH_PLUGIN_SUPPORT_ASYNC_PROGRAMMING 1
