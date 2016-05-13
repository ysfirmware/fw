
/* Define to prevent recursive inclusion -------------------------------------*/
#define TRUE							1
#define FALSE							0

#define MAX_EECOM_PORT					3	//
#define	SERIAL_PORT						3

#define ECOM_RXSTEP_HD1					0
#define ECOM_RXSTEP_HD2					1
#define	ECOM_RXSTEP_FR					2
#define	ECOM_RXSTEP_ID					3
#define ECOM_RXSTEP_CD					4
#define ECOM_RXSTEP_LH					5
#define ECOM_RXSTEP_LL					6
#define	ECOM_RXSTEP_RV					7
#define ECOM_RXSTEP_DA					8
#define ECOM_RXSTEP_CH					9
#define	ECOM_RXSTEP_CL					10
#define	ECOM_RXSTEP_END					11			//비정상완료
#define	ECOM_RXSTEP_COMPTE				0xFF		//정상완료

/* Includes ------------------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/



/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Eecom_Init(u8 PortNm, u8 *pRxBuf, u16 *pRxSize, u8 *pTxBuf, u16 *pTxSize);
u8 Eecom_Comm(u16 PortNm, u8 *pRxBuf, u16 pRxSize);
u8  Eecom_FrameCheck(u8 nPort);
u16 Eecom_CmdExe(u8 nPort, u8 Cmd, u8 *pRxBuf, u16 RbufSz);
void Eecom_AckFrame(u8 PortNm, u16 SendSz, u8 *pRevData);
void Mload_ErrorFrame(u8 PortNm, u8 ErrorCode, u8 *pRevData);

extern void Motion_Action(u8 cmd, u16 nValue);
