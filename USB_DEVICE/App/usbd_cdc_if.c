/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v2.0_Cube
  * @brief          : Usb device for Virtual Com Port.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#include <string.h>
#include "spi.h"
#include "tim.h"

/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */
uint8_t is_first_recv_after_powerup = 1;
uint8_t is_program_mode = 0;
uint8_t recv_buf_usb_owned = 0;
uint16_t recv_buf_waddr = 0;
uint16_t packet_remain = 0;
uint8_t recv_buf[2][RECV_BUFFER_SIZE] = {0};
uint8_t spi_txbuf[128];
uint8_t spi_rxbuf[128];
uint8_t spi_dma_error = 0;

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  static uint8_t line_coding[7];
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:
		memcpy(&line_coding[0], pbuf, 7);
    break;

    case CDC_GET_LINE_CODING:
		memcpy(pbuf, &line_coding[0], 7);
    break;

    case CDC_SET_CONTROL_LINE_STATE:

    break;

    case CDC_SEND_BREAK:

    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
	USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
	if(is_first_recv_after_powerup)
	{
		is_first_recv_after_powerup = 0;
		if(*Len >= 9)
		{
			uint8_t *enter_program_mode = (uint8_t *)"\xa0\xa5\x00\x00\x02\xf0\x01\x1e\xef";
			uint8_t i;
			for(i = 0; i < 9; i++)
			{
				if(i == 2)
				{
					continue;
				}
				if(Buf[i] != enter_program_mode[i])
				{
					break;
				}
			}
			if(i == 9)
			{
				is_program_mode = 1;
				HAL_GPIO_WritePin(FPROG_N_GPIO_Port, FPROG_N_Pin, GPIO_PIN_RESET);
				uint8_t tmp_buf[9];
				memcpy(&tmp_buf[0], &Buf[0], 9);
				CDC_Transmit_FS(&tmp_buf[0], 9);
				HAL_TIM_Base_Start_IT(&htim14);
			}
		}
	}
	else if(is_program_mode)
	{
		if(packet_remain == 0)
		{
			if(recv_buf[recv_buf_usb_owned][0] != 0xa0)
			{
				uint8_t i = 0;
				for(; i < *Len; i++)
				{
					if(Buf[i] == 0xa0)
						break;
				}
				for(; i < *Len; i++)
				{
					if(Buf[i] == 0xa5)
						break;
				}
				if(i < (*Len - 3))
				{
					uint16_t packet_size = ((Buf[i+2] << 8) + Buf[i+3]) + 7;
					if(packet_size <= RECV_BUFFER_SIZE)
					{
						memcpy(&recv_buf[recv_buf_usb_owned][1], &Buf[i], (*Len - 1));
						if(packet_size <= *Len)
						{
							recv_buf[recv_buf_usb_owned][0] = 0xa0;
							recv_buf_usb_owned = !recv_buf_usb_owned;
						}
						else 
						{
							recv_buf_waddr = *Len;
							packet_remain = packet_size - *Len;
						}
					}
				}
			}
		}
		else 
		{
			uint8_t recv_len = (packet_remain > *Len) ? *Len : packet_remain;
			memcpy(&recv_buf[recv_buf_usb_owned][recv_buf_waddr], Buf, recv_len);
			recv_buf_waddr += *Len;
			packet_remain -= *Len;
			if(packet_remain == 0)
			{
				recv_buf[recv_buf_usb_owned][0] = 0xa0;
				recv_buf_usb_owned = !recv_buf_usb_owned;
				recv_buf_waddr = 0;
			}
		}
	}
	else 
	{
		memcpy(&spi_txbuf[0], &Buf[0], *Len);
		if(*Len != 0)
		{
			while(HAL_GPIO_ReadPin(M2F_NSS_GPIO_Port, M2F_NSS_Pin) != GPIO_PIN_SET);
			HAL_GPIO_WritePin(M2F_NSS_GPIO_Port, M2F_NSS_Pin, GPIO_PIN_RESET);
			// if(HAL_SPI_TransmitReceive_DMA(&hspi1, &spi_txbuf[0], &spi_rxbuf[0], *Len) != HAL_OK)
			if(HAL_SPI_Transmit_DMA(&hspi1, &spi_txbuf[0], *Len) != HAL_OK)
			{
				spi_dma_error = -1;
			}
		}
	}
	USBD_CDC_ReceivePacket(&hUsbDeviceFS);
	return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /* USER CODE END 7 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */

/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
