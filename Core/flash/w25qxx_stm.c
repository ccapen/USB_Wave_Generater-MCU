#include "w25qxx_stm.h"

OFLASH_TypeDef flash1 = {0};

extern SPI_HandleTypeDef OFLASH_SPI_HANDLE;

//CALLBACK

uint8_t flash_write_nss(uint8_t pin_state)
{
	HAL_GPIO_WritePin(OFLASH_NSS_GPIO_PORT, OFLASH_NSS_PIN, (pin_state ? GPIO_PIN_SET : GPIO_PIN_RESET));
	return 0;
}

uint8_t flash_transmit(uint8_t *txdata, uint16_t size)
{
	return HAL_SPI_Transmit(&OFLASH_SPI_HANDLE, &txdata[0], size, 200);
}

uint8_t flash_receive(uint8_t *rxdata, uint16_t size)
{
	return HAL_SPI_Receive(&OFLASH_SPI_HANDLE, &rxdata[0], size, 200);
}

uint8_t flash_transmit_receive(uint8_t *txdata, uint8_t *rxdata, uint16_t size)
{
	return HAL_SPI_TransmitReceive(&OFLASH_SPI_HANDLE, &txdata[0], &rxdata[0], size, 200);
}

//END CALLBACK


uint8_t flash_init(void)
{
	flash1.write_nss = flash_write_nss;
	flash1.tx = flash_transmit;
	flash1.rx = flash_receive;
	flash1.txrx = flash_transmit_receive;

	// flash_set_drive_strength(&flash1, 2);
	uint8_t mid = 0x00;
	uint16_t jid = 0xffff;
	flash_read_id(&flash1, &mid, &jid);
	if((mid == 0x00) || (jid == 0xffff))
	{
		return -1;
	}
	return 0;
}

