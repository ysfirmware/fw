/******************************************************************************
  * @file       : Comm.c 
  * @author     : JUNG SH, EETECH
  * @version    : Ver 0.1
  * @date       : 12/07/2016
  * @brief      : Initailize stm32F405 MPU. 
  * @pjt model  : ADP
  ******************************************************************************/
/* @Story*/
/* @2015-12-07 make*/
/**/
/**/
/*******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f4xx_usart.h"
#include "Comm.h"




u8 RingBufRxd[AVAIL_COMPORT][RING_BUF_SIZE];
u16 RingBufEnd[AVAIL_COMPORT], RingBufFst[AVAIL_COMPORT];
USART_TypeDef *UsartType[AVAIL_COMPORT]={USART1, USART2, USART3};

u16 UsartTxSvcSz[AVAIL_COMPORT];                              // Tx 요청 size
u16 TxSvcPtr[AVAIL_COMPORT];                                  // 현재 Tx 서비스 중인 버퍼 위치
u16 UsartTxSz[AVAIL_COMPORT];
u16 CommTxSz[AVAIL_COMPORT], CommRxPtr[AVAIL_COMPORT];
u8 ComSharTxBuf[AVAIL_COMPORT][COMM_TXBUF_SIZE];
u16 CommErrCnt[AVAIL_COMPORT];
u8 ComSharRxBuf[AVAIL_COMPORT][COMM_RXBUF_SIZE], NowProtoType[AVAIL_COMPORT];

//==========================================================================//
//  -> USART ISR
//  -> 
//  @ :
//===========================================================================//
void Usart2_RxdISR(void)
{	
	// 버퍼에 수신프레임을 저장한다..    
	RingBufRxd[USART_COM2][RingBufEnd[USART_COM2]] = (u16)(UsartType[USART_COM2]->DR & (u16)0x01FF);
	RingBufEnd[USART_COM2] = (RingBufEnd[USART_COM2] + 1) % RING_BUF_SIZE;

	/* Clear the USART1 Receive interrupt */
	USART_ClearITPendingBit(UsartType[USART_COM2], USART_IT_RXNE);
}
void Usart2_TxdISR(void)
{
	// 잔여 송신 데이터가 있는지 확인
	if(UsartTxSvcSz[USART_COM2] > TxSvcPtr[USART_COM2])
	{	// 1 byte 송신
		USART_SendData(UsartType[USART_COM2], ComSharTxBuf[USART_COM2][TxSvcPtr[USART_COM2]]);
		TxSvcPtr[USART_COM2]++;
	}
	else
	{	// 송신 종료
		USART_ITConfig(UsartType[USART_COM2], USART_IT_TXE, DISABLE);
        UsartTxSvcSz[USART_COM2] = 0;
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
	}
	/* Clear the USART1 Tx interrupt */
	USART_ClearITPendingBit(UsartType[USART_COM2], USART_IT_TXE);
}

////////////////////////////////////////////////////////////////////////////////

u16 RxBuf_RecvDataChk(u8 PortNo)
{
	return ((RingBufEnd[PortNo] + RING_BUF_SIZE - RingBufFst[PortNo]) % RING_BUF_SIZE);
}

u8 IS_TxBusy(u8 PortNo)
{
	return (UsartTxSz[PortNo] == 0) ? 0 : 1;
}

u8 Uart_TxService(u8 PortNo, u16 Len)
{
	// 송신중인지 확인
	if(IS_TxBusy(PortNo))
		return FALSE;												// 송신 중 : Busy

	UsartTxSvcSz[PortNo] = Len;						// service 길이 전달
	TxSvcPtr[PortNo] = 0;									// pointer clear
	USART_ITConfig(UsartType[PortNo], USART_IT_TXE, ENABLE);		// 송신 Interrupt Enable
	return TRUE;
}

void Comm_TxService(u8 nPort, u16 DataSz)
{
	u8 TxDly;

	CommTxSz[nPort] = DataSz;
	// 송신 지연시간 확인(필요 있을경우)
	if(TxDly != 0)
		//UartTxDely = 0;
		TxDly = 1;
	else
		Uart_TxService(nPort, DataSz);
}

u8 UartRx_RefData(u8 PortNo, u16 Offset)
{
	u8 Rtn=0;
	u8 Sz;
	
	Sz = RxBuf_RecvDataChk(PortNo);
	if(Sz > Offset)
		Rtn = RingBufRxd[PortNo][(RingBufFst[PortNo]+Offset) % RING_BUF_SIZE];
	return Rtn;
}

u8 UartRx_GetData(u8 PortNo)
{
	u8 Rtn=0;
	
	if(RingBufFst[PortNo] != RingBufEnd[PortNo])
	{
		Rtn = RingBufRxd[PortNo][RingBufFst[PortNo]];
		RingBufFst[PortNo] = (RingBufFst[PortNo] + 1) % RING_BUF_SIZE;
	}
	return Rtn;
}

//==========================================================================//
//  -> Comm_main
//  -> Communication
//  @ :
//===========================================================================//
//u16 RxTest, RxTest1;

void Comm_main()
{
	u8 nPort;
	u16 nRxd, RxHead1, RxHead2;

	for(nPort=0; nPort < AVAIL_COMPORT; nPort++)
	{
		// Receive data check
        nRxd = RxBuf_RecvDataChk(nPort);	

                // Receive data 없는 경우
		if(nRxd == 0)
			continue;
        
		// Receive data 있는 경우
		// protocol 판별이 완료된 경우
		switch(NowProtoType[nPort])
		{
			case PROTOCOL_EECOM:				// Loader port 
				if(Eecom_Comm(nPort, 0, 0) == FALSE)
					CommErrCnt[nPort]++;
				else 
					CommErrCnt[nPort] = 0;
				break;
			case PROTOCOL_MODBUS_RTU:
               	
				break;
			case PROTOCOL_GUI:
				break;
			
			default :
				NowProtoType[nPort] = PROTOCOL_UNKNOWN;
				break;
		}
		// 통신 error 시 처음부터 재 시작
		if(NowProtoType[nPort] != PROTOCOL_UNKNOWN)
		{
			if(CommErrCnt[nPort] >0)
			{
				NowProtoType[nPort] = PROTOCOL_UNKNOWN;
				CommErrCnt[nPort] = 0;
				continue;
			}
		}
		// data 수신 2byte 후 protocol 판별
		if(nRxd == 1)
			continue;
			
		// protocol 판별
		RxHead1 = UartRx_RefData(nPort, 0);
		RxHead2 = UartRx_RefData(nPort, 1);
		
		// protocol Loader
		


		//PROTOCOL_BASE
		if(RxHead1 == HEADER1_EECOM && RxHead2 == HEADER2_EECOM)
		{
			NowProtoType[nPort] = PROTOCOL_EECOM;		//MLoader_Port
			Eecom_Init(nPort, &ComSharRxBuf[nPort][0], &CommRxPtr[nPort], &ComSharTxBuf[nPort][0], &CommTxSz[nPort]);
		}
		// PROTOCOL_GUI
		else if((RxHead1 == HEADER1_LOADER) && (RxHead2 == HEADER2_LOADER))
		{
			NowProtoType[nPort] = PROTOCOL_GUI;
			//
			//fTxCardSvice = 0;
		}
		else
			UartRx_GetData(nPort);
		
		
	}

}



