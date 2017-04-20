/* STM32 Includes ------------------------------------------------------------*/
#include "portfunction.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
eMBErrorCode eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START ) && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = (uint8_t)( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = (uint8_t)( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

eMBErrorCode eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;

	if( (usAddress >= REG_HOLDING_START) && (usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS) )
	{
		iRegIndex = (int)(usAddress - usRegHoldingStart);
		switch(eMode)
		{
			case MB_REG_READ: // MB_REG_READ = 0
            while(usNRegs > 0)
			{
				*pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] >> 8);
				*pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNRegs--;
			}break;
			case MB_REG_WRITE: // MB_REG_WRITE = 1
			while(usNRegs > 0)
			{
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }break;
            default: break;
        }
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}


eMBErrorCode eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;


	if((usAddress >= REG_HOLDING_START)&&((usAddress+usNCoils) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegHoldingStart);
		switch(eMode)
		{
			case MB_REG_READ://MB_REG_READ = 0
            while(usNCoils > 0)
			{
                *pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] >> 8);
                *pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] & 0xFF);
                iRegIndex++;
                usNCoils--;
			}break;
			case MB_REG_WRITE://MB_REG_WRITE = 1
            while(usNCoils > 0)
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNCoils--;
            }break;
            default: break;
		}
	}
	else
	{
		eStatus = MB_ENOREG;
	}

	return eStatus;
}

eMBErrorCode eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNDiscrete;
    return MB_ENOREG;
}
