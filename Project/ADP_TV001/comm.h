
/* Define to prevent recursive inclusion -------------------------------------*/


/* Includes ------------------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/
#define TRUE			1
#define FALSE			0

#define RING_BUF_SIZE		256
#define COMM_RXBUF_SIZE		256
#define COMM_TXBUF_SIZE		256

#define AVAIL_COMPORT		3

#define USART_COM1			0
#define USART_COM2			1 
#define USART_COM3			2
#define USB					3	//disalbe
#define ETHERNET			4	//disalbe



#define PROTOCOL_UNKNOWN	0
#define PROTOCOL_EECOM		1
#define PROTOCOL_MODBUS_RTU	2
#define PROTOCOL_GUI		3


#define HEADER1_LOADER		0x01
#define HEADER2_LOADER		0x02
#define HEADER1_EECOM		0x41
#define HEADER2_EECOM		0x50

/* Exported constants --------------------------------------------------------*/

extern u16 CommTxSz[AVAIL_COMPORT];
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

extern void Eecom_Init(u8 PortNm, u8 *pRxBuf, u16 *pRxSize, u8 *pTxBuf, u16 *pTxSize);
extern u8 Eecom_Comm(u16 PortNm, u8 *pRxBuf, u16 pRxSize);
void Usart2_RxdISR();
void Usart2_TxdISR();
u16 RxBuf_RecvDataChk(u8 PortNo);
void Comm_TxService(u8 nPort, u16 DataSz);
u8 UartRx_GetData(u8 PortNo);