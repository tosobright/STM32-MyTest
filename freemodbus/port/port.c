
#include "port.h"
#include "mb.h" 

/* 私有类型定义 --------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define REG_INPUT_START 0
#define REG_INPUT_NREGS 4

#define REG_HOLDING_START 0
#define REG_HOLDING_NREGS 130


/* 私有变量 ------------------------------------------------------------------*/
static USHORT   usRegInputStart = REG_INPUT_START;
static USHORT   usRegInputBuf[REG_INPUT_NREGS];

static USHORT   usRegHoldingStart = REG_HOLDING_START;
static USHORT   usRegHoldingBuf[REG_HOLDING_NREGS];


//读数字寄存器 功能码0x04

eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
  eMBErrorCode    eStatus = MB_ENOERR;
  int             iRegIndex;
	
  usAddress--;
//	用作例子
	usRegInputBuf[0] = 0x11;
	usRegInputBuf[1] = 0x22;
	usRegInputBuf[2] = 0x33;
	usRegInputBuf[3] = 0x44;
//	例子结束
	
  if((usAddress>=REG_INPUT_START)&&(usAddress+usNRegs<=REG_INPUT_START+REG_INPUT_NREGS))
  {
    iRegIndex=(int)(usAddress-usRegInputStart);
    while( usNRegs > 0 )
    {
      *pucRegBuffer++ = (unsigned char)(usRegInputBuf[iRegIndex]>>8);
      *pucRegBuffer++ = (unsigned char)(usRegInputBuf[iRegIndex]&0xFF);
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

// 寄存器的读写函数 支持的命令为读 0x03 和写0x06
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;
	
		usAddress--;
    if( ( usAddress >= REG_HOLDING_START ) && ( usAddress + usNRegs <= REG_HOLDING_START + REG_HOLDING_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegHoldingStart );
        switch ( eMode )
        {
            /* Pass current register values to the protocol stack. */
        case MB_REG_READ:
            while( usNRegs > 0 )
            {
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] >> 8 );
                *pucRegBuffer++ = ( unsigned char )( usRegHoldingBuf[iRegIndex] & 0xFF );
                iRegIndex++;
                usNRegs--;
            }
            break;

            /* Update current register values with new values from the
             * protocol stack. */
        case MB_REG_WRITE:
            while( usNRegs > 0 )
            {
                usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
                usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
                iRegIndex++;
                usNRegs--;
            }
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}

//读/写开关寄存器  0x01  x05

eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils, eMBRegisterMode eMode )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNCoils;
    ( void )eMode;
    return MB_ENOREG;
}

//读开关寄存器 0x02
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    ( void )pucRegBuffer;
    ( void )usAddress;
    ( void )usNDiscrete;
    return MB_ENOREG;
}
