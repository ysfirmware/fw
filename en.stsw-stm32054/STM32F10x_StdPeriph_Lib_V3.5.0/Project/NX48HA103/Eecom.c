/******************************************************************************
  * @file       : EeCom.c 
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
#include "stm32f10x_usart.h"
#include "Eecom.h"
#include "Comm.h"


u16 EecomStep[MAX_EECOM_PORT];			
u16 EecomDataNum[MAX_EECOM_PORT];										// MLoader data 길이
u16 *pEecomRxSz[MAX_EECOM_PORT], *pEecomTxSz[MAX_EECOM_PORT];	// Rx/Tx data 길이 포인터
u8 *pEecomRxBuf[MAX_EECOM_PORT], *pEecomTxBuf[MAX_EECOM_PORT];	// Rx/Tx 버퍼 포인터
u8 fFrameFullOk;

u8 ModuleID=0x01;

void Eecom_Init(u8 PortNm, u8 *pRxBuf, u16 *pRxSize, u8 *pTxBuf, u16 *pTxSize)
{
	pEecomRxBuf[PortNm] = pRxBuf;
	pEecomRxSz[PortNm] = pRxSize;
	pEecomTxBuf[PortNm] = pTxBuf;
	pEecomTxSz[PortNm] = pTxSize;
	
	EecomStep[PortNm] = ECOM_RXSTEP_HD1;
}

u8 Eecom_Comm(u16 PortNm, u8 *pRxBuf, u16 pRxSize)
{
	u16 RtnVal = FALSE;				// 함수 수행 완료 처리용
	//u16 fFrameFullOk;				// 프레임 전체 수신 완료용
	u32 i=100000;
	//comport로 들어온 frame 실행
	if(PortNm < SERIAL_PORT)
	{
			RtnVal = Eecom_FrameCheck(PortNm);			
			if(fFrameFullOk)
			{
				if(Eecom_CmdExe(PortNm, pEecomRxBuf[PortNm][4], pEecomRxBuf[PortNm], *pEecomRxSz[PortNm]))
				{
					while(i--);
					Comm_TxService(PortNm, *pEecomTxSz[PortNm]);	//응답 프레임 송신
					RtnVal = TRUE;
				}
			}
	}
	
    return RtnVal;
}

u16 test111;
u8  Eecom_FrameCheck(u8 nPort)
{
	u16 csc; 
	u16 RtnVal = TRUE;
	u16 CSS=0;
	unsigned char RecvData;
	u8 *pTempRBuf = pEecomRxBuf[nPort];
	u16 *pTempRdSz = pEecomRxSz[nPort];	//pTempRdSz 는 온전한 Loader frame size
        
	u16 (*RecvDataChk_Func)(u8);
	u8 (*RxBuf_GetData_Func)(u8);
        
	if(nPort < SERIAL_PORT) /* uart 통신일 경우 해당 함수를 등록 */
	{
          RecvDataChk_Func  =  RxBuf_RecvDataChk;
          RxBuf_GetData_Func = UartRx_GetData;
	}
	fFrameFullOk = FALSE;
	while( RecvDataChk_Func(nPort) && !(fFrameFullOk) && RtnVal)
	{
        RecvData = RxBuf_GetData_Func(nPort);
		switch (EecomStep[nPort])
		{
			case ECOM_RXSTEP_HD1:								// HD1
				 *pTempRdSz = 0;
				 if(RecvData == HEADER1_EECOM)
				 	EecomStep[nPort]++;
				 else 
				 	RtnVal = FALSE;
				break;
			case ECOM_RXSTEP_HD2:								// HD2
				if(RecvData == HEADER2_EECOM)
					EecomStep[nPort]++;
				else 
				{
					if(RecvData != HEADER1_LOADER)
						EecomStep[nPort] = ECOM_RXSTEP_HD1;
					RtnVal = FALSE;
				}
				break;
			case ECOM_RXSTEP_FR:								// FR
				//*pTempRdSz = 0;
				EecomStep[nPort]++;
				break;
			case ECOM_RXSTEP_ID:								// ST
				if(RecvData == ModuleID)
					EecomStep[nPort]++;
				else
				{
					EecomStep[nPort] = ECOM_RXSTEP_HD1;
					RtnVal = FALSE;
				}
				break;
			case ECOM_RXSTEP_CD:								// CD
				//*pTempRdSz = 0;			//
				EecomStep[nPort]++;
				break;
			case ECOM_RXSTEP_LH:
				EecomDataNum[nPort] = (u16)(RecvData << 8);
				EecomStep[nPort]++;
				break;
			case ECOM_RXSTEP_LL:
				EecomDataNum[nPort] |= (u16)RecvData;
				
				if(EecomDataNum[nPort] < COMM_RXBUF_SIZE)
					EecomStep[nPort]++;
				break;
			case ECOM_RXSTEP_RV:
				EecomStep[nPort]++;
				break;
			case ECOM_RXSTEP_DA:
				// 내가 카운트한 값과 프레임내에 전달되어 온 길이 값 비교
				if((*pTempRdSz + 1) >= EecomDataNum[nPort]+10)		// counter + RV(1) 값 >= Len + 3
					EecomStep[nPort] = ECOM_RXSTEP_COMPTE;
				break;
			default :
				if(RecvData == ECOM_RXSTEP_HD1)
					EecomStep[nPort] = ECOM_RXSTEP_HD2;
				else
					EecomStep[nPort] = ECOM_RXSTEP_HD1;
				break;
		}
		pTempRBuf[pTempRdSz[0]++] = RecvData;
		//crc check
		if(EecomStep[nPort] == ECOM_RXSTEP_COMPTE)
		{
			for(csc = 5; csc < EecomDataNum[nPort]+8; csc++)
			{
				CSS += pTempRBuf[csc];
				test111 = CSS;
			}
			
			if( ((CSS & 0xFF) != pTempRBuf[*pTempRdSz -1]) || (((CSS >> 8) & 0xFF) != pTempRBuf[*pTempRdSz - 2]) )
			{
				EecomStep[nPort] = ECOM_RXSTEP_HD1;
				RtnVal = FALSE;
			}	
			else
			{
				EecomStep[nPort] = ECOM_RXSTEP_HD1;
				fFrameFullOk = TRUE;
			}
		}
	}//while end
	return RtnVal;
}

u16 TempRdSz1;

u16 Eecom_CmdExe(u8 nPort, u8 Cmd, u8 *pRxBuf, u16 RbufSz)
{
	u8 RtnVal = FALSE;
	u8 *pTempBuf;
	u16 TempRdSz;
	u16 nData;
	
	u16 SendSz;
	//data 분석
	
	
	pTempBuf = &pRxBuf[8];		// DA0
	TempRdSz = RbufSz - 8;		// DA0 ~ DA end 갯수 
	TempRdSz1 = TempRdSz;
	
	switch(Cmd)
	{
		case 'i':
			pEecomTxBuf[nPort][4] = 'i';
			pEecomTxBuf[nPort][8] = 'o';
			pEecomTxBuf[nPort][9] = 'o';
			Eecom_AckFrame(nPort, 2, &pRxBuf[0]);
			//Motion Action status set
			nData = pTempBuf[0];
			//nData = (nData<<8) | pTempBuf[1];
			Motion_Action('i', nData);
			RtnVal = TRUE;
			break;
		case 'd':
			pEecomTxBuf[nPort][4] = 'd';
			pEecomTxBuf[nPort][8] = 'o';
			pEecomTxBuf[nPort][9] = 'o';
			//Motion Action status set
			Eecom_AckFrame(nPort, 2, &pRxBuf[0]);
			nData = pTempBuf[0];
			//nData = (nData<<8) | pTempBuf[1];
			Motion_Action('d', nData);
			RtnVal = TRUE;
			break;
		case 'z':
			pEecomTxBuf[nPort][4] = 'z';
			pEecomTxBuf[nPort][8] = 'o';
			pEecomTxBuf[nPort][9] = 'o';
		case 'm':
			break;
		default:
			break;
	}
	return RtnVal;
}

void Eecom_AckFrame(u8 PortNm, u16 SendSz, u8 *pRevData)
{
	u16 TempCS=0;  
	u8 cnt;
	
	// SendSz 는 data 
	// 응답 프레임 구성 및 전송
	pEecomTxBuf[PortNm][0] = HEADER1_EECOM;							// Header 1
	pEecomTxBuf[PortNm][1] = HEADER2_EECOM;							// Header 2
	pEecomTxBuf[PortNm][2] = pRevData[2];							// FR
	
	pEecomTxBuf[PortNm][3] = ModuleID;//									// ST(파라미터)

	pEecomTxBuf[PortNm][5] = (((SendSz ) >> 8) & 0xff);			// LH
	pEecomTxBuf[PortNm][6] = ((SendSz ) & 0xff);					// LL
	pEecomTxBuf[PortNm][7] = 0;										// RV
	
	
	for(cnt=5; cnt < SendSz+8; cnt++)
		TempCS += pEecomTxBuf[PortNm][cnt];
		// crc make
	pEecomTxBuf[PortNm][SendSz + 9] = (TempCS & 0xFF);			
	pEecomTxBuf[PortNm][SendSz + 8] = ((TempCS >> 8) & 0xFF);		
	
	*pEecomTxSz[PortNm] = SendSz + 10;		// Tx Size 값 저장(송신 서비스를 위해 포인터에 저장해 둔다)

}

void Mload_ErrorFrame(u8 PortNm, u8 ErrorCode, u8 *pRevData)
{
	pEecomTxBuf[PortNm][0] = HEADER1_EECOM;							// Header 1
	pEecomTxBuf[PortNm][1] = HEADER2_EECOM;							// Header 2
	pEecomTxBuf[PortNm][2] = pRevData[2];								// FR
	pEecomTxBuf[PortNm][3] = ModuleID;					// ST(파라미터)
	pEecomTxBuf[PortNm][4] = 'E';
	pEecomTxBuf[PortNm][5] = 'E';//ErrorCode
    *pEecomTxSz[PortNm] = 6;
}


#if 0
case COMMAND_LIST_DOWNLOAD:                        
			RtnVal = ParamProcFunc(pTempBuf, MldrDataNum[nPort] - 5, &pMloaderTxBuf[nPort][8], &SendSz); /* 데이타 + crc 의 길이만 인자로 넘김*/           
			//RtnVal = ParamProcFunc_test(pTempBuf, MldrDataNum[nPort] - 5, &pMloaderTxBuf[nPort][8], &SendSz);

			if(RtnVal == TRUE)    /* 성공시*/
			{                                                                   
			  pMloaderTxBuf[nPort][4] = COMMAND_LIST_DOWNLOAD;                                                    
			  Mload_AckFrame(nPort, SendSz, &pRxBuf[0]);                        
			  
			}
			else  /* 실패시 */
			{
			/* error frame 생성 */
			  Mload_ErrorFrame(nPort, 3, &pRxBuf[0]); //jjun, 임의,, error code 아직 정의되지 않았음
			}         
			break;
		case COMMAND_LIST_CARD_LOAD:
				pMloaderTxBuf[nPort][4] = 'c';
				pMloaderTxBuf[nPort][8] = MODULE_ID;					// card ID
				pMloaderTxBuf[nPort][9] = (u8)((ErrCode >> 8) & 0xFF);	// Error code H
				pMloaderTxBuf[nPort][10] = (u8)(ErrCode & 0xFF);		// Error code L
				pMloaderTxBuf[nPort][11] = card_info.Cardstatus;		// Status 1
				pMloaderTxBuf[nPort][12] = 0; //reserver				// reserved
				Mload_AckFrame(nPort, 5, &pRxBuf[0]);
				RtnVal = TRUE;
			break;
		case COMMAND_LIST_CONNECTION:
			if(!((pTempBuf[0] == 'p') && (pTempBuf[1] == 'l') && (pTempBuf[2] == 'u') && (pTempBuf[3] == 'g')))
				return RtnVal;
			else 
			{
				//fault 문장
				/*
				pAckData[TxIdx++] = 't'; 	//send cmd
				
				pAckData[TxIdx++] = 'I';	//
				pAckData[TxIdx++] = 'n';
				*/
				pMloaderTxBuf[nPort][4] = 't';
				pMloaderTxBuf[nPort][8] = 'I';			//0x49
				pMloaderTxBuf[nPort][9] = 'n';			//0x6E
				Mload_AckFrame(nPort, 2, &pRxBuf[0]);
				RtnVal = TRUE;
			}
			break;
		case COMMAND_LIST_PPTUP:
			
			if(Flsh_ParmaMain.NoUpload)
			{
				RtnVal = TRUE;
				pMloaderTxBuf[nPort][4] = COMMAND_LIST_PPTUP;
				pMloaderTxBuf[nPort][8] = pTempBuf[0];
				pMloaderTxBuf[nPort][9] = 0x15;
				SendSz=2;
				Mload_AckFrame(nPort, SendSz, &pRxBuf[0]);
			}
			else
			{
				RtnVal = uploadFunc(nPort, pTempBuf, &pMloaderTxBuf[nPort][8], &SendSz);
				{
					pMloaderTxBuf[nPort][4] = COMMAND_LIST_PPTUP;                                                    
					Mload_AckFrame(nPort, SendSz, &pRxBuf[0]); 			
				}
			}
			break;
		case COMMAND_LIST_MOTION:       // CD='M'(0x4D) 명령어 처리(위치결정 용)
			//remote flag set 되면 동작 
            RtnVal = Mloader_Motion(nPort, pRxBuf, RbufSz, pMloaderTxBuf[nPort]);
			break;
		case COMMAND_LIST_REMOTEMODE:
			//remote flag set
			if(pTempBuf[0])
				fRemotMode = 1;
				
			else
				fRemotMode = 0;
			pMloaderTxBuf[nPort][8] = fRemotMode;//pTempBuf[0];
			pMloaderTxBuf[nPort][4] = COMMAND_LIST_REMOTEMODE;
			
            Mload_AckFrame(nPort, 1, &pRxBuf[0]);
			RtnVal = TRUE;
			break;
		
		case COMMAND_LIST_RUNMODE:
            //remote flag set 되면 동작 
			RunStPgm = STPGM_RUN;
			Step_init();
			Motion_Init();
            pMloaderTxBuf[nPort][4] = COMMAND_LIST_RUNMODE;
            Mload_AckFrame(nPort, 0, &pRxBuf[0]);     
            RtnVal = TRUE; 
			break;
            
		case COMMAND_LIST_STOPPMODE:
			//remote flag set 되면 동작 
			Step_init();
            RunStPgm = STPGM_STOP;
            pMloaderTxBuf[nPort][4] = COMMAND_LIST_STOPPMODE;
            Mload_AckFrame(nPort, 0, &pRxBuf[0]);
            RtnVal = TRUE;            
			break;
		case COMMAND_LIST_SYSTEMREST:
			//에러리셋, 정지, 위치 0
			//NVIC_SystemReset();
			Step_init();
			Motion_Init();
			for(nAx=0; nAx<AVAIL_AXIS; nAx++)
			{
				Error_Set(nAx, 0);
			}
			RunStPgm = STPGM_STOP;
			GPIO_WriteBit(GPIOG, GPIO_Pin_8, Bit_SET);//Error LED off
			/*E/F error Output*/
			GPIO_WriteBit(GPIOD, GPIO_Pin_12, Bit_SET);
			GPIO_WriteBit(GPIOD, GPIO_Pin_13, Bit_SET);
			pMloaderTxBuf[nPort][4] = COMMAND_LIST_SYSTEMREST;
			Mload_AckFrame(nPort, 0, &pRxBuf[0]);
            RtnVal = TRUE;
			break;
		case COMMAND_LIST_FWDOWNLOAD:

			// OS Data Write 후 결과 반환.
			// FALSE : 비정상 종료
			// TRUE : 마지막 프레임 데이터 까지 정상 종료
			// addFlash : 멀티 패킷 수신 중, 최종 Flash 주소 반환
			RtnMloader = Mloader_OSDownload(addFlash, &pTempBuf[0], TempRdSz);

			if(RtnMloader == TRUE)
			{
				pMloaderTxBuf[nPort][10] = 0x04;	
				RtnVal = TRUE;
                        }
			else if(RtnMloader == FALSE)
				RtnVal = FALSE;
			else
			{
				addFlash = RtnMloader;
				pMloaderTxBuf[nPort][10] = 0x06;
				RtnVal = TRUE;
			}

			pMloaderTxBuf[nPort][4] = 'F'; // 46H
			pMloaderTxBuf[nPort][8] = pTempBuf[0];	
			pMloaderTxBuf[nPort][9] = pTempBuf[1];			
			Mload_AckFrame(nPort, 3, &pRxBuf[0]);

			break;
		case COMMAND_LIST_ALARMCLEAR:
			break;
#endif