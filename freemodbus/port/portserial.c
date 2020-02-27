/*
 * FreeModbus Libary: BARE Port
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
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 *
 */

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

#include "usart.h"

/* ----------------------- static functions ---------------------------------*/
//void prvvUARTTxReadyISR( void );
//void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void vMBPortSerialEnable(BOOL xRxEnable, BOOL xTxEnable)
{
  /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
  if (xRxEnable)
  {
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
#if USE_RS485 == 1
    RS485_RX_MODE();
#endif
  }
  else
  {
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_RXNE);
#if USE_RS485 == 1
    RS485_TX_MODE();
#endif
  }
  if (xTxEnable)
  {
    __HAL_UART_ENABLE_IT(&huart3, UART_IT_TXE);
  }
  else
  {
    __HAL_UART_DISABLE_IT(&huart3, UART_IT_TXE);
  }
}

void RS485_USARTx_Init(void)
{
#if USE_RS485 == 1
  /* RS485控制引脚：(1)低电平：接收模式 (2)高电平：发送模式 */
  RS485_RX_MODE();
#endif
}

BOOL xMBPortSerialInit(UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity)
{
  RS485_USARTx_Init();
  return TRUE;
}

BOOL xMBPortSerialPutByte(CHAR ucByte)
{
#if USE_RS485 == 1
  RS485_TX_MODE();
#endif
  /* Put a byte in the UARTs transmit buffer. This function is called
   * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
   * called. */
  if (HAL_UART_Transmit(&huart3, (uint8_t *)&ucByte, 1, 0x01) != HAL_OK)
    return FALSE;
  else
    return TRUE;
}

BOOL xMBPortSerialGetByte(CHAR *pucByte)
{
#if USE_RS485 == 1
  RS485_RX_MODE();
#endif
  /* Return the byte in the UARTs receive buffer. This function is called
   * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
   */
  if (HAL_UART_Receive(&huart3, (uint8_t *)pucByte, 1, 0x01) != HAL_OK)
    return FALSE;
  else
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
void prvvUARTTxReadyISR(void)
{
  pxMBFrameCBTransmitterEmpty();
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
void prvvUARTRxISR(void)
{
  pxMBFrameCBByteReceived();
}

///**
//  * 函数功能: 串口接收完成回调函数
//  * 输入参数: 无
//  * 返 回 值: 无
//  * 说    明：无
//  */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//  printf("HAL_UART_RxCpltCallback\n");
//  prvvUARTRxISR();//接受中断
//
//}

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
//  printf("HAL_UART_RxCpltCallback\n");
//  prvvUARTTxReadyISR();//发送完成中断
//}
