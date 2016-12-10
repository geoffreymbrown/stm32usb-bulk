#include "stm32f3xx_hal.h"
#include "spi.h"
#include "usb_device.h"
#include "gpio.h"


#include "usbd_bulk_if.h"
#include "unistd.h"

void SystemClock_Config(void);
void Error_Handler(void);

void __libc_init_array(){}
uint8_t BULK_Transmit_FS(uint8_t* Buf, uint16_t Len);
uint32_t bulk_read(uint8_t *pBuf, uint32_t buf_len);
extern volatile int rxEOF;

void Toggle_Leds(void)
{
  static uint32_t ticks;

  if(ticks++ == 500)
    {
      HAL_GPIO_TogglePin(GPIOE,LD6_Pin);
      ticks = 0;
    }
}


int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  //  MX_SPI3_Init();
  MX_USB_DEVICE_Init();

  while (1)
  {
    uint8_t   buf[256];
    uint32_t  len;

    if ((len = bulk_read(buf, sizeof(buf)))) {
      HAL_GPIO_TogglePin(GPIOE,LD4_Pin);
      BULK_Transmit_FS(buf, len);
    } else {
      if (rxEOF) {
	HAL_GPIO_WritePin(GPIOE,LD7_Pin, 0);
	rxEOF = 0;
	BULK_Transmit_FS(buf, 0);
      }
      //      if (!(len &63))
      //	BULK_Transmit_FS(buf, 0);
    }
  }
}

void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_PeriphCLKInitTypeDef PeriphClkInit;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.USBClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

void Error_Handler(void)
{
  while(1) 
  {
  }
}

#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t* file, uint32_t line)
{
}
#endif

