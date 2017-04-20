/**
  ******************************************************************************
  * @file    User/Source/main.c
  * @author  wangxian
  * @version V1.1.0
  * @date    21-Sep-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* STM32 Includes ------------------------------------------------------------*/
#include "stm32f10x.h"

/* FreeRTOS Includes ---------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* Modbus Includes -----------------------------------------------------------*/
#include "mb.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define LED_PORT            ( GPIOB )
#define LED_PIN             ( GPIO_Pin_0 )
#define LED_CLOCK           ( RCC_APB2Periph_GPIOB )

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void vLEDInit();
static void vLEDToggle();
static void vLEDTask(void *pvParameters);
static void vMBTask(void *pvParameters);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    /* Create led task */
    xTaskCreate(vLEDTask, "LED task", configMINIMAL_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(vMBTask, "Modbus task", configMINIMAL_STACK_SIZE, NULL, 2, NULL);
    /* Start task scheduler */
    vTaskStartScheduler();
    /* Infinite loop */
    while (1)
    {
    }
}

void vLEDInit()
{
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(LED_CLOCK, ENABLE);

    GPIO_InitStruct.GPIO_Pin = LED_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(LED_PORT, &GPIO_InitStruct);
    GPIO_ResetBits(LED_PORT, LED_PIN);
}

void vLEDToggle()
{
    if(GPIO_ReadOutputDataBit(LED_PORT, LED_PIN) == 0)
    {
        GPIO_SetBits(LED_PORT, LED_PIN);
    }
    else
    {
        GPIO_ResetBits(LED_PORT, LED_PIN);
    }
}

static void vLEDTask(void *pvParameters)
{
    vLEDInit();
    for( ;; )
    {
        vLEDToggle();
        vTaskDelay(200);
    }
}

static void vMBTask( void *pvParameters )
{
    /* Select either ASCII or RTU Mode. */
    eMBInit( MB_RTU, 0x01, 0, 9600, MB_PAR_NONE );
    /* Enable the Modbus Protocol Stack. */
    eMBEnable();
    for( ;; )
    {
        /* Call the main polling loop of the Modbus protocol stack. Internally
         * the polling loop waits for a new event by calling the port
         * dependent function xMBPortEventGet(  ). In the FreeRTOS port the
         * event layer is built with queues.
         */
        ( void )eMBPoll();
        vTaskDelay(50);
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {
    }
}
#endif





/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
