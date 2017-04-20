/*
 * FreeModbus Libary: STR71x Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: porttimer.c,v 1.1 2006/11/02 23:14:44 wolti Exp $
 */

/* ----------------------- System includes ----------------------------------*/
#include "assert.h"

/* ----------------------- FreeRTOS includes --------------------------------*/
#include "FreeRTOS.h"
#include "task.h"

/* ----------------------- STM32 includes ----------------------------------*/
#include "stm32f10x.h"

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- Defines ------------------------------------------*/
#define MB_TIMER_DEV            ( TIM2 )
#define MB_TIMER_RCC            ( RCC_APB1Periph_TIM2 )
#define MB_TIMER_IRQ_CH         ( TIM2_IRQn )
#define MB_TIMER_IRQ_HANDLER    ( TIM2_IRQHandler )
#define MB_IRQ_PRIORITY         ( 2 )

/* ----------------------- Start implementation -----------------------------*/
BOOL xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    /* Calculate output compare value for timer2. */
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    /* Enable the timer clock */
    RCC_APB1PeriphClockCmd(MB_TIMER_RCC, ENABLE);
    TIM_TimeBaseInitStruct.TIM_ClockDivision = 0;
    TIM_TimeBaseInitStruct.TIM_Prescaler = (uint16_t)(SystemCoreClock/20000)-1;
    TIM_TimeBaseInitStruct.TIM_Period = (uint16_t)usTim1Timerout50us;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(MB_TIMER_DEV, &TIM_TimeBaseInitStruct);

    TIM_ClearFlag(MB_TIMER_DEV, TIM_FLAG_Update);
    vMBPortTimersDisable();
    /* Setting the timer nvic */
    NVIC_InitStruct.NVIC_IRQChannel = MB_TIMER_IRQ_CH;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = MB_IRQ_PRIORITY;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    return TRUE;
}

void vMBPortTimersEnable(void)
{
    TIM_ClearITPendingBit(MB_TIMER_DEV, TIM_IT_Update);
    TIM_SetCounter(MB_TIMER_DEV, 0);
    TIM_ITConfig(MB_TIMER_DEV, TIM_IT_Update, ENABLE);
    TIM_Cmd(MB_TIMER_DEV, ENABLE);
}

void vMBPortTimersDisable(void)
{
    /* We can always clear both flags. This improves performance. */
    TIM_ClearITPendingBit(MB_TIMER_DEV, TIM_IT_Update);
    TIM_SetCounter(MB_TIMER_DEV, 0);
    TIM_ITConfig(MB_TIMER_DEV, TIM_IT_Update, DISABLE);
    TIM_Cmd(MB_TIMER_DEV, DISABLE);
}

void MB_TIMER_IRQ_HANDLER(void)
{
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)
    {
        (void) pxMBPortCBTimerExpired();
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}
