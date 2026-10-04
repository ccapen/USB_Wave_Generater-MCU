/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../wave_generater_driver/wave_generater_stm.h"
#include "../crc/crc.h"
#include "../flash/w25qxx_stm.h"
#include "usbd_cdc_if.h" 

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern uint8_t is_program_mode;
extern uint8_t recv_buf_usb_owned;
extern uint8_t recv_buf[2][RECV_BUFFER_SIZE];
uint8_t recv_buf_main_read = 0;
uint8_t flash_is_ok = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  MX_GPIO_Init();
  HAL_Delay(500);
  uint16_t fpga_release_ms = 0;
  uint16_t fpga_stall_ms = 0;
  while((fpga_release_ms < 500) && (fpga_stall_ms < 2500))
  {
	if(HAL_GPIO_ReadPin(MF2FL_NSS_GPIO_Port, MF2FL_NSS_Pin) == GPIO_PIN_SET)
	{
		fpga_release_ms++;
	}
	else 
	{
		fpga_release_ms = 0;
		fpga_stall_ms++;
	}
	HAL_Delay(1);
  }

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USB_DEVICE_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */
	if(fpga_stall_ms >= 2500)
	{
		HAL_GPIO_WritePin(FPROG_N_GPIO_Port, FPROG_N_Pin, GPIO_PIN_RESET);
	}
	
	wave_generater_init();

	if(flash_init() == 0)
	{
		flash_is_ok = 1;
	}


//   SysTick()
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	if(is_program_mode && flash_is_ok)
	{
		if(recv_buf[recv_buf_main_read][0] == 0xa0)
		{
			uint16_t packet_len = ((recv_buf[recv_buf_main_read][3] << 8) + recv_buf[recv_buf_main_read][4]);
			uint16_t crc = crc16_ccitt_false(&recv_buf[recv_buf_main_read][5], (packet_len + 2), 0xffff);
			if((crc == 0) && (recv_buf[recv_buf_main_read][5] == 0xf0))
			{
				CDC_Transmit_FS(&recv_buf[recv_buf_main_read][0], 9);
			}
			if((crc == 0) && (recv_buf[recv_buf_main_read][5] == 0xf1))
			{
				uint32_t addr;
				uint16_t len;
				uint8_t return_buf[14];
				addr = (recv_buf[recv_buf_main_read][7] << 16);
				addr += (recv_buf[recv_buf_main_read][8] << 8);
				addr += recv_buf[recv_buf_main_read][9];
				switch (recv_buf[recv_buf_main_read][6])
				{
				case 0x03:
					len = (recv_buf[recv_buf_main_read][10] << 8);
					len += recv_buf[recv_buf_main_read][11];
					flash_read_data(&flash1, addr, &recv_buf[recv_buf_main_read][10], len);
					len += 5;
					crc = crc16_ccitt_false(&recv_buf[recv_buf_main_read][5], len, 0xffff);
					recv_buf[recv_buf_main_read][3] = ((len >> 8) & 0xff);
					recv_buf[recv_buf_main_read][4] = (len & 0xff);
					recv_buf[recv_buf_main_read][len + 5] = ((crc >> 8) & 0xff);
					recv_buf[recv_buf_main_read][len + 6] = (crc & 0xff);
					CDC_Transmit_FS(&recv_buf[recv_buf_main_read][0], (len + 7));
					break;
				
				case 0x02:
					len = packet_len - 5;
					memcpy(&return_buf[0], &recv_buf[recv_buf_main_read][0], 10);
					return_buf[3] = 0x00;
					return_buf[4] = 0x07;
					return_buf[10] = ((len >> 8) & 0xff);
					return_buf[11] = (len & 0xff);
					crc = crc16_ccitt_false(&return_buf[5], 7, 0xffff);
					return_buf[12] = ((crc >> 8) & 0xff);
					return_buf[13] = (crc & 0xff);
					CDC_Transmit_FS(&return_buf[0], 14);
					flash_write_data(&flash1, addr, &recv_buf[recv_buf_main_read][10], len);
					break;
				
				case 0x20:
					memcpy(&return_buf[0], &recv_buf[recv_buf_main_read][0], 12);
					CDC_Transmit_FS(&recv_buf[recv_buf_main_read][0], 12);
					flash_erase_sector(&flash1, addr);
					break;
				
				case 0x52:
					memcpy(&return_buf[0], &recv_buf[recv_buf_main_read][0], 12);
					CDC_Transmit_FS(&recv_buf[recv_buf_main_read][0], 12);
					flash_erase_half_block(&flash1, addr);
					break;
				
				case 0xd8:
					memcpy(&return_buf[0], &recv_buf[recv_buf_main_read][0], 12);
					CDC_Transmit_FS(&recv_buf[recv_buf_main_read][0], 12);
					flash_erase_full_block(&flash1, addr);
					break;
				
				default:
					break;
				}
			}
			recv_buf[recv_buf_main_read][0] = 0;
			recv_buf_main_read = !recv_buf_main_read;
		}
	}
	// HAL_Delay(600);
	// uint8_t buf[8];
	// for(uint8_t i = 0; i < 8; i++)
	// {
	// 	buf[i] = 'A' + i;
	// }
	// // CDC_Transmit_FS(&buf[0], 8);
	// while(HAL_SPI_Transmit_DMA(&hspi1, &buf[0], 8) != HAL_OK);

	// HAL_Delay(2000);
	// uint8_t tbuf[2];
	// tbuf[0] = flash_is_ok;
	// tbuf[1] = is_program_mode;
	// CDC_Transmit_FS(&tbuf[0], 2);
	// uint8_t mid = 0x00;
	// uint16_t jid = 0xffff;
	// flash_read_id(&flash1, &mid, &jid);
	// CDC_Transmit_FS(&jid, 2);
	// uint8_t buf[1000];
	// flash_read_data(&flash1, 0, &buf[0], 1000);
	// CDC_Transmit_FS(&buf[0], 1000);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;

  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// 实现回调函数，在 DMA 传输完成后触发
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1)
    {
        // 数据传输完成，可以拉高片选或进行下一步操作
        HAL_GPIO_WritePin(M2F_NSS_GPIO_Port, M2F_NSS_Pin, GPIO_PIN_SET);
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM14) {
        // 你的定时任务
		GPIO_PinState pin_state = HAL_GPIO_ReadPin(LED_GPIO_Port, LED_Pin);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, (!pin_state));
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
