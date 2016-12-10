#include "stm32f3xx_hal.h"
#include "stm32f3xx.h"
#include "stm32f3xx_it.h"
extern PCD_HandleTypeDef hpcd_USB_FS;

void SysTick_Handler(void)
{
  extern void Toggle_Leds();
  Toggle_Leds();
  HAL_IncTick();
  HAL_SYSTICK_IRQHandler();
}

void USB_LP_CAN_RX0_IRQHandler(void)
{
  HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

