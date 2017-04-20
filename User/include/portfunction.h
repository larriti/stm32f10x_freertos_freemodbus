#ifndef _PORTFUNCTION_H
#define _PORTFUNCTION_H

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
#define REG_INPUT_START     0x0001
#define REG_INPUT_NREGS     8
#define REG_HOLDING_START 	0x0001
#define REG_HOLDING_NREGS 	8

/* Private variables ---------------------------------------------------------*/
static uint16_t usRegInputStart = REG_INPUT_START;
static uint16_t usRegInputBuf[REG_INPUT_NREGS];
static uint16_t usRegHoldingStart = REG_HOLDING_START;
static uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = \
 {0x147b,0x3f8e,0x147b,0x400e,0x1eb8,0x4055,0x147b,0x408e};


#endif