#ifndef _W25QXX_STM_H_
#define _W25QXX_STM_H_

#include "w25qxx.h"
#include "../Inc/main.h"
#include "../Inc/spi.h"


#define OFLASH_SPI_HANDLE		hspi1
#define OFLASH_NSS_GPIO_PORT	MF2FL_NSS_GPIO_Port
#define OFLASH_NSS_PIN			MF2FL_NSS_Pin


extern OFLASH_TypeDef flash1;

uint8_t flash_init(void);


#endif
