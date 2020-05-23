#include "cmsis_os.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_nucleo.h"

static void LED_Thread1(void const *argument);
static void LED_Thread2(void const *argument);

osThreadId_t LEDThread1Handle, LEDThread2Handle;

const osThreadAttr_t LEDThread1_attributes = {
    .name = "LEDThread1",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};

const osThreadAttr_t LEDThread2_attributes = {
    .name = "LEDThread2",
    .priority = (osPriority_t)osPriorityNormal,
    .stack_size = 128 * 4};

void App_Init(void) {
  COM_InitTypeDef com_init;
  com_init.BaudRate = 115200;
  com_init.WordLength = UART_WORDLENGTH_8B;
  com_init.StopBits = UART_STOPBITS_1;
  com_init.Parity = UART_PARITY_NONE;
  com_init.HwFlowCtl = UART_HWCONTROL_NONE;
  BSP_COM_Init(COM1, &com_init);
  BSP_COM_SelectLogPort(COM1);

  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);

  LEDThread1Handle = osThreadNew(LED_Thread1, NULL, &LEDThread1_attributes);

  LEDThread2Handle = osThreadNew(LED_Thread2, NULL, &LEDThread2_attributes);
}

/**
 * @brief  Toggle LED1 thread
 * @param  thread not used
 * @retval None
 */
static void LED_Thread1(void const *argument) {
  uint32_t count = 0;
  (void)argument;

  for (;;) {
    count = osKernelSysTick() + 5000;

    HAL_UART_Transmit(&hcom_uart[COM1], (uint8_t *)"test\n\r", 6,
                      COM_POLL_TIMEOUT);
    printf("[Thread1: Toggle LED1 every 200 ms for 5 s\n");

    /* Toggle LED1 every 200 ms for 5 s */
    while (count > osKernelSysTick()) {
      BSP_LED_Toggle(LED1);

      osDelay(200);
    }

    /* Turn off LED1 */
    BSP_LED_Off(LED1);

    /* Suspend Thread 1 */
    osThreadSuspend(NULL);

    count = osKernelSysTick() + 5000;

    printf("[Thread1: Toggle LED1 every 500 ms for 5 s\n");

    /* Toggle LED1 every 500 ms for 5 s */
    while (count > osKernelSysTick()) {
      BSP_LED_Toggle(LED1);

      osDelay(500);
    }

    /* Resume Thread 2*/
    osThreadResume(LEDThread2Handle);
  }
}

/**
 * @brief  Toggle LED2 thread
 * @param  argument not used
 * @retval None
 */
static void LED_Thread2(void const *argument) {
  uint32_t count;
  (void)argument;

  for (;;) {
    count = osKernelSysTick() + 10000;

    printf("[Thread1: Toggle LED2 every 500 ms for 10 s\n");

    /* Toggle LED2 every 500 ms for 10 s */
    while (count > osKernelSysTick()) {
      BSP_LED_Toggle(LED2);

      osDelay(500);
    }

    /* Turn off LED2 */
    BSP_LED_Off(LED2);

    /* Resume Thread 1 */
    osThreadResume(LEDThread1Handle);

    /* Suspend Thread 2 */
    osThreadSuspend(NULL);
  }
}
