#include "wave_generater_stm.h"

WAVE_TypeDef wave1 = {0};

extern SPI_HandleTypeDef WAVE_SPI_HANDLE;

//CALLBACK

uint8_t wave_write_nss(uint8_t pin_state)
{
	HAL_GPIO_WritePin(WAVE_NSS_GPIO_PORT, WAVE_NSS_PIN, (pin_state ? GPIO_PIN_SET : GPIO_PIN_RESET));
	return 0;
}

uint8_t wave_transmit(uint8_t *pdata, uint16_t size)
{
	return HAL_SPI_Transmit(&WAVE_SPI_HANDLE, &pdata[0], size, 200);
}

//END CALLBACK



// uint16_t wave_data[500];

uint8_t wave_generater_init(void)
{
	wave1.wave_write_nss = wave_write_nss;
	wave1.wave_transmit = wave_transmit;

	
	// HAL_Delay(200);
	// wave_generater_set_run(&wave1, 0);

	// HAL_Delay(200);
	// wave_generater_set_parameter(&wave1, 0, 0, 1000, 0);

	// HAL_Delay(200);
	// uint16_t i;
	// uint16_t tmp;
	// for(i = 0; i < 500; i++)
	// {
	// 	double deg = i * asin(1.0) * 4 / 500;
	// 	deg = sin(deg);
	// 	deg = deg * 0x01ff;
	// 	wave_data[i] = ((int16_t)deg << 8) + ((int16_t)deg >> 8);
	// }
	// wave_generater_transmit_data(&wave1, 0, (uint8_t*)&wave_data[0], 1000);
	// for(i = 0; i < 250; i++)
	// {
	// 	tmp = 0x01ff;
	// 	wave_data[i] = ((int16_t)tmp << 8) + ((int16_t)tmp >> 8);
	// }
	// for(i = 250; i < 500; i++)
	// {
	// 	tmp = 0x0200;
	// 	wave_data[i] = ((int16_t)tmp << 8) + ((int16_t)tmp >> 8);
	// }
	// wave_generater_transmit_data(&wave1, 500, (uint8_t*)&wave_data[0], 1000);

	// HAL_Delay(200);
	// wave_generater_set_run(&wave1, 1);

	return 0;
}




