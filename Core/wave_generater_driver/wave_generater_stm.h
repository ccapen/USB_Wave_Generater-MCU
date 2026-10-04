#ifndef _WAVE_GENERATER_STM_
#define _WAVE_GENERATER_STM_

#include "wave_generater.h"
#include "../Inc/main.h"
#include "../Inc/spi.h"
#include <math.h>

#define WAVE_SPI_HANDLE		hspi1
#define WAVE_NSS_GPIO_PORT	M2F_NSS_GPIO_Port
#define WAVE_NSS_PIN		M2F_NSS_Pin


extern WAVE_TypeDef wave1;


uint8_t wave_generater_init(void);


#endif
