#include "w25qxx.h"

static inline uint8_t is_little_endian(void) {
    uint16_t x = 0x0001;
    return *(uint8_t*)&x == 0x01;   // 低地址是 0x01 → 小端
}

uint8_t flash_is_busy(OFLASH_TypeDef *flash)
{
	uint8_t status;
	flash->write_nss(0);
	status = CMD_RSR1;
	flash->tx(&status, 1);
	flash->rx(&status, 1);
	flash->write_nss(1);
	return (status & 0x01);
}

uint8_t flash_set_drive_strength(OFLASH_TypeDef *flash, uint8_t strength)
{
	uint8_t cmd;
	flash->write_nss(0);
	cmd = CMD_WSR3;
	flash->tx(&cmd, 1);
	cmd = 0x04 + ((3 - (strength % 4)) * 32);
	flash->tx(&cmd, 1);
	flash->write_nss(1);
	return 0;
}

uint8_t flash_read_id(OFLASH_TypeDef *flash, uint8_t *mid, uint16_t *jid)
{
	uint8_t cmd;
	flash->write_nss(0);
	cmd = CMD_JID;
	flash->tx(&cmd, 1);
	flash->rx(mid, 1);
	if(is_little_endian())
	{
		flash->rx(((uint8_t *)jid) + 1, 1);
		flash->rx((uint8_t *)jid, 1);
	}
	else 
	{
		flash->rx((uint8_t *)jid, 2);
	}
	flash->write_nss(1);
	return 0;
}

uint8_t flash_read_data(OFLASH_TypeDef *flash, uint32_t addr, uint8_t *data, uint16_t len)
{
	while(flash_is_busy(flash));

	flash->write_nss(0);
	uint8_t cmd;
	cmd = CMD_RD;
	flash->tx(&cmd, 1);
	cmd = ((addr >> 16) & 0xff);
	flash->tx(&cmd, 1);
	cmd = ((addr >> 8) & 0xff);
	flash->tx(&cmd, 1);
	cmd = (addr & 0xff);
	flash->tx(&cmd, 1);
	flash->rx(data, len);
	flash->write_nss(1);
	return 0;
}

uint8_t flash_erase(OFLASH_TypeDef *flash, uint32_t addr, uint8_t erase_cmd)
{
	while(flash_is_busy(flash));

	flash->write_nss(0);
	uint8_t cmd;
	cmd = CMD_WE;
	flash->tx(&cmd, 1);
	flash->write_nss(1);
	//(CS Not Active Setup Time relative to CLK) == 3ns
	flash->write_nss(0);

	cmd = erase_cmd;
	flash->tx(&cmd, 1);
	cmd = ((addr >> 16) & 0xff);
	flash->tx(&cmd, 1);
	cmd = ((addr >> 8) & 0xff);
	flash->tx(&cmd, 1);
	cmd = (addr & 0xff);
	flash->tx(&cmd, 1);
	flash->write_nss(1);
	return 0;
}

uint8_t flash_erase_sector(OFLASH_TypeDef *flash, uint32_t addr)
{
	return flash_erase(flash, addr, CMD_SE);
}

uint8_t flash_erase_half_block(OFLASH_TypeDef *flash, uint32_t addr)
{
	return flash_erase(flash, addr, CMD_HBE);
}

uint8_t flash_erase_full_block(OFLASH_TypeDef *flash, uint32_t addr)
{
	return flash_erase(flash, addr, CMD_FBE);
}

uint8_t flash_page_program(OFLASH_TypeDef *flash, uint32_t addr, uint8_t *data, uint16_t len)
{
	while(flash_is_busy(flash));

	flash->write_nss(0);
	uint8_t cmd;
	cmd = CMD_WE;
	flash->tx(&cmd, 1);
	flash->write_nss(1);
	//(CS Not Active Setup Time relative to CLK) == 3ns
	flash->write_nss(0);

	cmd = CMD_PP;
	flash->tx(&cmd, 1);
	cmd = ((addr >> 16) & 0xff);
	flash->tx(&cmd, 1);
	cmd = ((addr >> 8) & 0xff);
	flash->tx(&cmd, 1);
	cmd = (addr & 0xff);
	flash->tx(&cmd, 1);
	flash->tx(data, len);
	flash->write_nss(1);
	return 0;
}

uint8_t flash_write_data(OFLASH_TypeDef *flash, uint32_t addr, uint8_t *data, uint16_t len)
{
	while(flash_is_busy(flash));

	while(len > 0)
	{
		uint16_t page_remain = OFLASH_PAGE_SIZE - (addr % OFLASH_PAGE_SIZE);
		uint16_t write_len = (len < page_remain) ? len : page_remain;

		flash_page_program(flash, addr, data, write_len);

		addr += write_len;
		data += write_len;
		len  -= write_len;
	}
	return 0;
}
