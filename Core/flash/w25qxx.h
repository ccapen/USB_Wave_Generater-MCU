#ifndef _W25QXX_H_
#define _W25QXX_H_

#include <stdint.h>


#define OFLASH_PAGE_SIZE	256


typedef enum
{
	CMD_WE		= 0x06,	//Write Enable
	CMD_VSR_WE	= 0x50,	//Volatile SR Write Enable
	CMD_WDIS	= 0x04,	//Write Disable

	CMD_RPD		= 0xab,	//Release Power-down/ID
	CMD_MDID	= 0x90,	//Manufacturer/Device ID
	CMD_JID		= 0x9f,	//JEDEC ID
	CMD_RUID	= 0x4b,	//Read Unique ID

	CMD_RD		= 0x03,	//Read Data
	CMD_FRD		= 0x0b,	//Fast Read

	CMD_PP		= 0x02,	//Page Program

	CMD_SE		= 0x20,	//Sector Erase(4KB)
	CMD_HBE		= 0x52,	//Block Erase(32KB)
	CMD_FBE		= 0xd8,	//Block Erase(64KB)
	CMD_CE		= 0xc7,	//or 0x60	//Chip Erase

	CMD_RSR1	= 0x05,	//Read Status Register-1
	CMD_WSR1	= 0x01,	//Write Status Register-1
	CMD_RSR2	= 0x35,	//Read Status Register-2
	CMD_WSR2	= 0x31,	//Write Status Register-2
	CMD_RSR3	= 0x15,	//Read Status Register-3
	CMD_WSR3	= 0x11,	//Write Status Register-3

	CMD_RSFDPR	= 0x5a,	//Read SFDP Register
	CMD_ESER	= 0x44,	//Erase Security Register
	CMD_PSER	= 0x42,	//Program Security Register
	CMD_RSER	= 0x48,	//Read Security Register

	CMD_GBL		= 0x7e,	//Global Block Lock
	CMD_GBUL	= 0x98,	//Global Block Unlock
	CMD_RBL		= 0x3d,	//Read Block Lock
	CMD_IBL		= 0x36,	//Individual Block Lock
	CMD_IBUL	= 0x39,	//Individual Block Unlock

	CMD_EPSP	= 0x75,	//Erase/Program Suspend
	CMD_EPRS	= 0x7a,	//Erase/Program Resume
	CMD_PD		= 0xb9,	//Power-down

	CMD_ERST	= 0x66,	//Enable Reaet
	CMD_RST		= 0x99	//Reset Device
}OFLASH_CMD;



typedef uint8_t (*OFLASH_WRITE_NSS_TypeDef)(uint8_t pin_state);
typedef uint8_t (*OFLASH_TRANSMIT_TypeDef)(uint8_t *txdata, uint16_t size);
typedef uint8_t (*OFLASH_RECEIVE_TypeDef)(uint8_t *rxdata, uint16_t size);
typedef uint8_t (*OFLASH_TRANSMIT_RECEIVE_TypeDef)(uint8_t *txdata, uint8_t *rxdata, uint16_t size);



typedef struct
{
	OFLASH_WRITE_NSS_TypeDef write_nss;
	OFLASH_TRANSMIT_TypeDef tx;
	OFLASH_RECEIVE_TypeDef rx;
	OFLASH_TRANSMIT_RECEIVE_TypeDef txrx;
}OFLASH_TypeDef;



uint8_t flash_is_busy(OFLASH_TypeDef *flash);
uint8_t flash_set_drive_strength(OFLASH_TypeDef *flash, uint8_t strength);	//strength = [0, 1, 2, 3]
uint8_t flash_read_id(OFLASH_TypeDef *flash, uint8_t *mid, uint16_t *jid);
uint8_t flash_read_data(OFLASH_TypeDef *flash, uint32_t addr, uint8_t *data, uint16_t len);
uint8_t flash_erase_sector(OFLASH_TypeDef *flash, uint32_t addr);
uint8_t flash_erase_half_block(OFLASH_TypeDef *flash, uint32_t addr);
uint8_t flash_erase_full_block(OFLASH_TypeDef *flash, uint32_t addr);
uint8_t flash_write_data(OFLASH_TypeDef *flash, uint32_t addr, uint8_t *data, uint16_t len);


#endif
