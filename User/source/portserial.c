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
 * File: $Id: portserial.c,v 1.1 2006/11/02 23:14:44 wolti Exp $
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
#define MB_UART_DEV             ( USART1 )
#define MB_UART_CLOCK           ( RCC_APB2Periph_USART1 )
#define MB_UART_AFIO_CLOCK      ( RCC_APB2Periph_AFIO )
#define MB_UART_PORT_CLOCK      ( RCC_APB2Periph_GPIOA )
#define MB_UART_TX_PORT         ( GPIOA )
#define MB_UART_TX_PIN          ( GPIO_Pin_9 )
#define MB_UART_RX_PORT         ( GPIOA )
#define MB_UART_RX_PIN          ( GPIO_Pin_10 )
#define MB_UART_IRQ_CH          ( USART1_IRQn )
#define MB_UART_IRQ_HANDLER     ( USART1_IRQHandler )
#define MB_IRQ_PRIORITY         ( 1 )

/* ----------------------- Start implementation -----------------------------*/

BOOL xMBPortSerialInit( UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    BOOL            xResult = TRUE;
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;

    ( void )ucPort;

    switch ( eParity )
    {
    case MB_PAR_EVEN:
        USART_InitStruct.USART_Parity = USART_Parity_Even;
        break;
    case MB_PAR_ODD:
        USART_InitStruct.USART_Parity = USART_Parity_Odd;
        break;
    case MB_PAR_NONE:
        USART_InitStruct.USART_Parity = USART_Parity_No;
        break;
    default: break;
    }

    switch ( ucDataBits )
    {
    case 7:
        if( eParity == MB_PAR_NONE )
        {
            /* not supported by our hardware. */
            xResult = FALSE;
        }
        else
        {
            USART_InitStruct.USART_WordLength = USART_WordLength_8b;
        }
        break;
    case 8:
        if( eParity == MB_PAR_NONE )
        {
            USART_InitStruct.USART_WordLength = USART_WordLength_8b;
        }
        else
        {
            USART_InitStruct.USART_WordLength = USART_WordLength_9b;
        }
        break;
    default:
        xResult = FALSE;
    }

    if( xResult != FALSE )
    {
        /* Enable the USART GPIO clock */
        RCC_APB2PeriphClockCmd(MB_UART_PORT_CLOCK | MB_UART_CLOCK | MB_UART_AFIO_CLOCK, ENABLE);

        /* GPIO Configration PA.09 output PA.10 input mode */
        GPIO_InitStruct.GPIO_Pin = MB_UART_TX_PIN;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(MB_UART_TX_PORT, &GPIO_InitStruct);

        GPIO_InitStruct.GPIO_Pin = MB_UART_RX_PIN;
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(MB_UART_RX_PORT, &GPIO_InitStruct);

        /* Configure the UART. */
        USART_InitStruct.USART_BaudRate = ulBaudRate;
        USART_InitStruct.USART_StopBits = USART_StopBits_1;
        USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
        USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
        USART_Init(MB_UART_DEV, &USART_InitStruct);
        USART_Cmd(MB_UART_DEV, ENABLE);
        vMBPortSerialEnable( FALSE, FALSE );

        /* Configure the IEC for the UART interrupts. */
        NVIC_InitStruct.NVIC_IRQChannel = MB_UART_IRQ_CH;
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = MB_IRQ_PRIORITY;
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);
    }
    return xResult;
}

void vMBPortSerialClose(void)
{
    vMBPortSerialEnable(FALSE, FALSE);
    USART_Cmd(MB_UART_DEV, DISABLE);
}

void vMBPortClose(void)
{
    vMBPortSerialClose();
    vMBPortTimersDisable();
}

void vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if( xRxEnable )
    {
        USART_ITConfig(MB_UART_DEV, USART_IT_RXNE, ENABLE);
    }
    else
    {
        USART_ITConfig(MB_UART_DEV, USART_IT_RXNE, DISABLE);
    }

    if( xTxEnable )
    {
        USART_ITConfig(MB_UART_DEV, USART_IT_TXE, ENABLE);
    }
    else
    {
        USART_ITConfig(MB_UART_DEV, USART_IT_TXE, DISABLE);
    }
}

BOOL xMBPortSerialPutByte( CHAR ucByte )
{
    USART_SendData(MB_UART_DEV, ucByte);
    return TRUE;
}

BOOL xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = USART_ReceiveData(MB_UART_DEV);
    return TRUE;
}

void MB_UART_IRQ_HANDLER(void)
{
    if(USART_GetITStatus(MB_UART_DEV, USART_IT_TXE) == SET)
    {
        //function eMBInit in mb.c pxMBFrameCBTransmitterEmpty = xMBRTUTransmitFSM
        pxMBFrameCBTransmitterEmpty();
    }
    if(USART_GetITStatus(MB_UART_DEV, USART_IT_RXNE) == SET)
    {
        //function eMBInit in mb.c pxMBFrameCBByteReceived = xMBRTUReceiveFSM
        pxMBFrameCBByteReceived();
        USART_ClearITPendingBit(MB_UART_DEV, USART_IT_RXNE);
    }
}
