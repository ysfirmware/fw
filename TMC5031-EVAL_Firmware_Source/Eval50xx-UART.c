/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-UART.c
           UART-Funktionen (Bytes Senden/Empfangen)

  Datum:   17.10.2011 OK
*****************************************************/


#include "stm32f10x_lib.h"
#include "bits.h"
#include "Eval50xx.h"

#if defined(UART_INTERFACE_1)

#define UART_INTR_PRI        7

#define UART_BUFFER_SIZE 32
#define UART_TIMEOUT_VALUE 10   //Timeout-Wert in 0.5ms (also 10 => 5ms)

#if UART_INTERFACE_1==UART_1
  #define USARTx USART1
#elif UART_INTERFACE_1==UART_2
  #define USARTx USART2
#elif UART_INTERFACE_1==UART_3
  #define USARTx USART3
#else
  #error "UART device not supported"
#endif


static volatile UCHAR UART1RxBuffer[UART_BUFFER_SIZE];
static volatile UCHAR UART1TxBuffer[UART_BUFFER_SIZE];
static volatile int UART1RxReadPtr;
static volatile int UART1RxWritePtr;
static volatile int UART1TxReadPtr;
static volatile int UART1TxWritePtr;
volatile UCHAR UART1TimeoutFlag;
volatile UINT UART1TimeoutTimer;
static volatile UINT UART1TransmitDelay;
volatile UINT UART1TransmitDelayTimer;


/*******************************************************************
   Funktion: InitUART()
   Parameter: Baudrate: Baudrate der Schnittstelle (z.B. 9600)

   Zweck: Intialisierung der UART
********************************************************************/
void InitUART(int BaudRate)
{
  USART_InitTypeDef  UART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  #if UART_INTERFACE_1 == UART_2
  //UART2 aktivieren
  USART_DeInit(USART2);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  //GPIOA aktivieren (UART2-Pins)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);
  GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);

  //UART2-Pins zuweisen (PA2 und PA3)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  #elif UART_INTERFACE_1 == UART_1
  //UART1 aktivieren
  USART_DeInit(USART1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


  //UART1-Pins zuweisen (PB6 und PB7)
  GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  #elif UART_INTERFACE_1 == UART_3
  //UART3 aktivieren
  USART_DeInit(USART3);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);


  //UART3-Pins zuweisen (PB10 und PB11)
  GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, DISABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  #else
  #error "UART pin mapping not defined"
  #endif

  //UART konfigurieren
  USART_StructInit(&UART_InitStructure);
  UART_InitStructure.USART_BaudRate=BaudRate;

  USART_Init(USARTx,&UART_InitStructure);

  //Interrupt für UART aktivieren
  #if UART_INTERFACE_1 == UART_2
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
  #elif UART_INTERFACE_1 == UART_1
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
  #elif UART_INTERFACE_1 == UART_3
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
  #else
  #error "Device not supported!"
  #endif
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_INTR_PRI;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  USART_ClearFlag(USARTx, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
                          USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
                          USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE |
                          USART_FLAG_PE);
  USART_ITConfig(USARTx,USART_IT_PE  ,DISABLE);
  USART_ITConfig(USARTx,USART_IT_TXE ,ENABLE);
  USART_ITConfig(USARTx,USART_IT_TC  ,ENABLE);
  USART_ITConfig(USARTx,USART_IT_RXNE,ENABLE);
  USART_ITConfig(USARTx,USART_IT_IDLE,DISABLE);
  USART_ITConfig(USARTx,USART_IT_LBD ,DISABLE);
  USART_ITConfig(USARTx,USART_IT_CTS ,DISABLE);
  USART_ITConfig(USARTx,USART_IT_ERR ,DISABLE);

  USART_Cmd(USARTx, ENABLE);
}


/*******************************************************************
  UART-Interrupthandler
  Wird durch den NVIC aufgerufen, wenn ein UART-Interrupt auftritt.
  Dies paasiert, wenn ein Zeichen angekommen ist oder
  ein Zeichen gesendet werden kann.
  Der Aufruf dieser Funktion muß in stm32f10x_it.c eingetragen werden.
********************************************************************/
#if UART_INTERFACE_1 == UART_1
void USART1_IRQHandler(void)
#elif UART_INTERFACE_1 == UART_2
void USART2_IRQHandler(void)
#elif UART_INTERFACE_1 == UART_3
void USART3_IRQHandler(void)
#else
#error "UART interrupt handler not defined"
#endif
{
  int i;

  //Ist ein Zeichen  abgekommen?
  if(USARTx->SR & USART_FLAG_RXNE)
  {
    //Wenn RS485 gerade auf Senden geschaltet ist, dann ist
    //es ein Echo, das hier ignoriert wird.
    if(IS_RS485_SENDING_1())
    {
      i=USARTx->DR;
    }
    else
    {
      //Zeichen in den Empfangspuffer kopieren
      i=UART1RxWritePtr+1;
      if(i==UART_BUFFER_SIZE) i=0;

      if(i!=UART1RxReadPtr)
      {
        UART1RxBuffer[UART1RxWritePtr]=USARTx->DR;
        UART1RxWritePtr=i;
      }

      //Empfangs-Timeout auf Startwert setzen
      UART1TimeoutTimer=UART_TIMEOUT_VALUE;

      //Sendeverzögerung auf Startwert setzen
      UART1TransmitDelayTimer=UART1TransmitDelay;
    }
  }

  //Kann das nächste Zeichen gesendet werden?
  if(USARTx->SR & USART_FLAG_TXE)
  {
    if(UART1TransmitDelayTimer==0)
    {
      if(UART1TxWritePtr!=UART1TxReadPtr)
      {
        SET_RS485_SEND_MODE_1();
        USARTx->DR=UART1TxBuffer[UART1TxReadPtr++];
        if(UART1TxReadPtr==UART_BUFFER_SIZE) UART1TxReadPtr=0;
      }
      else
      {
        //Sendeinterrupt deaktivieren, wenn kein Zeichen im Sendepuffer ist
        USART_ITConfig(USARTx,USART_IT_TXE ,DISABLE);
      }
    }
  }

  //Allerletztes Bit gesendet?
  if(USARTx->SR & USART_FLAG_TC)
  {
    USART_ClearITPendingBit(USARTx, USART_IT_TC);
    if(UART1TxReadPtr==UART1TxWritePtr) SET_RS485_RECEIVE_MODE_1();
  }
}


/*******************************************************************
   Funktion: WriteUART()
   Parameter: ch: Zu schreibendes Zeichen

   Zweck: Senden eines Zeichens über UART3
          (Einstellen in den Sendepuffer)
********************************************************************/
void WriteUART(UCHAR ch)
{
  int i;

  //Zeichen in die Warteschlange stellen
  i=UART1TxWritePtr+1;
  if(i==UART_BUFFER_SIZE) i=0;

  if(i!=UART1TxReadPtr)
  {
    UART1TxBuffer[UART1TxWritePtr]=ch;
    UART1TxWritePtr=i;

    //Sendeinterrupt aktivieren
    USART_ITConfig(USARTx, USART_IT_TXE, ENABLE);
  }
}


/*******************************************************************
   Funktion: ReadUART()
   Parameter: *ch: Zeiger auf Variable für zu lesendes Zeichen

   Rückgabewert: TRUE bei Erfolg
                 FALSE wenn kein Zeichen da war
   Zweck: Lesen eines Zeichens aus dem Empfangspuffer der UART3
          (Einstellen in den Sendepuffer)
********************************************************************/
UCHAR ReadUART(UCHAR *ch)
{
  //Kein Zeichen vorhanden?
  if(UART1RxReadPtr==UART1RxWritePtr) return FALSE;

  //Zeichen aus dem Puffer holen
  *ch=UART1RxBuffer[UART1RxReadPtr++];
  if(UART1RxReadPtr==UART_BUFFER_SIZE)  UART1RxReadPtr=0;

  return TRUE;
}


/*******************************************************************
   Funktion: SetUARTTransmitDelay()
   Parameter: Delay: Sendeverzögerung in ms

   Rückgabewert: ---

   Zweck: Setzen der Sendeverzögerung
          (für einige RS485-Adapter wichtig)
********************************************************************/
void SetUARTTransmitDelay(UINT Delay)
{
  UART1TransmitDelay=Delay;
}


/*******************************************************************
   Funktion: CheckUARTTimeout()
   Parameter: ---

   Rückgabewert: TRUE wenn zwischenzeitlich ein Timeout aufgetreten ist
                 FALSE wenn kein Timeout aufgetreten ist
   Zweck: Prüfen, ob beim Empfangen ein Timeout aufgetreten ist (also
          Zeit zwischen zwei Bytes >5ms) und Zurücksetzen des
          Timeout-Flag.
********************************************************************/
UINT CheckUARTTimeout(void)
{
  if(UART1TimeoutFlag)
  {
    UART1TimeoutFlag=FALSE;
    return TRUE;
  }
  else return FALSE;
}


/*******************************************************************
   Funktion: ClearUARTRxBuffer()
   Parameter: ---

   Rückgabewert: ---

   Zweck: Löschen des Empfangspuffers
********************************************************************/
void ClearUARTRxBuffer(void)
{
  DisableInterrupts();
  UART1RxReadPtr=0;
  UART1RxWritePtr=0;
  EnableInterrupts();
}
#endif


#if defined(UART_INTERFACE_2)

#if UART_INTERFACE_2==UART_1
   #define USARTy USART1
#elif UART_INTERFACE_2==UART_2
   #define USARTy USART2
#elif UART_INTERFACE_2==UART_3
   #define USARTy USART3
#else
  #error "UART device not supported"
#endif


static volatile UCHAR UART2RxBuffer[UART_BUFFER_SIZE];
static volatile UCHAR UART2TxBuffer[UART_BUFFER_SIZE];
static volatile int UART2RxReadPtr;
static volatile int UART2RxWritePtr;
static volatile int UART2TxReadPtr;
static volatile int UART2TxWritePtr;
volatile UCHAR UART2TimeoutFlag;
volatile UINT UART2TimeoutTimer;
static volatile UINT UART2TransmitDelay;
volatile UINT UART2TransmitDelayTimer;


/*******************************************************************
   Funktion: InitUART_2()
   Parameter: Baudrate: Baudrate der Schnittstelle (z.B. 9600)

   Zweck: Intialisierung der UART
********************************************************************/
void InitUART_2(int BaudRate)
{
  USART_InitTypeDef  UART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  #if UART_INTERFACE_2 == UART_2
  //UART2 aktivieren
  USART_DeInit(USART2);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

  //GPIOA aktivieren (UART2-Pins)
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphResetCmd(RCC_APB2Periph_GPIOA, DISABLE);
  GPIO_PinRemapConfig(GPIO_Remap_USART2, DISABLE);

  //UART2-Pins zuweisen (PA2 und PA3)
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  #elif UART_INTERFACE_2 == UART_1
  //UART1 aktivieren
  USART_DeInit(USART1);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);


  //UART1-Pins zuweisen (PA9 und PA10)
  GPIO_PinRemapConfig(GPIO_Remap_USART1, DISABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  #elif UART_INTERFACE_2 == UART_3
  //UART3 aktivieren
  USART_DeInit(USART3);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);


  //UART3-Pins zuweisen (PB10 und PB11)
  GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, DISABLE);
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  #else
  #error "UART pin mapping not defined"
  #endif

  //UART konfigurieren
  USART_StructInit(&UART_InitStructure);
  UART_InitStructure.USART_BaudRate=BaudRate;

  USART_Init(USARTy,&UART_InitStructure);

  //Interrupt für UART aktivieren
  #if UART_INTERFACE_2 == UART_2
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
  #elif UART_INTERFACE_2 == UART_1
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
  #elif UART_INTERFACE_2 == UART_3
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQChannel;
  #else
  #error "Device not supported!"
  #endif
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART_INTR_PRI;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);


  USART_ClearFlag(USARTy, USART_FLAG_CTS | USART_FLAG_LBD  | USART_FLAG_TXE  |
                          USART_FLAG_TC  | USART_FLAG_RXNE | USART_FLAG_IDLE |
                          USART_FLAG_ORE | USART_FLAG_NE   | USART_FLAG_FE |
                          USART_FLAG_PE);
  USART_ITConfig(USARTy,USART_IT_PE  ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_TXE ,ENABLE);
  USART_ITConfig(USARTy,USART_IT_TC  ,ENABLE);
  USART_ITConfig(USARTy,USART_IT_RXNE,ENABLE);
  USART_ITConfig(USARTy,USART_IT_IDLE,DISABLE);
  USART_ITConfig(USARTy,USART_IT_LBD ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_CTS ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_ERR ,DISABLE);

  USART_Cmd(USARTy, ENABLE);
}


/*******************************************************************
  UART-Interrupthandler
  Wird durch den NVIC aufgerufen, wenn ein UART-Interrupt auftritt.
  Dies paasiert, wenn ein Zeichen angekommen ist oder
  ein Zeichen gesendet werden kann.
  Der Aufruf dieser Funktion muß in stm32f10x_it.c eingetragen werden.
********************************************************************/
#if UART_INTERFACE_2 == UART_1
void USART1_IRQHandler(void)
#elif UART_INTERFACE_2 == UART_2
void USART2_IRQHandler(void)
#elif UART_INTERFACE_2 == UART_3
void USART3_IRQHandler(void)
#else
#error "UART interrupt handler not defined"
#endif
{
  int i;

  //Ist ein Zeichen  abgekommen?
  if(USARTy->SR & USART_FLAG_RXNE)
  {
    //Wenn RS485 gerade auf Senden geschaltet ist, dann ist
    //es ein Echo, das hier ignoriert wird.
    if(IS_RS485_SENDING_2())
    {
      i=USARTy->DR;
    }
    else
    {
      //Zeichen in den Empfangspuffer kopieren
      i=UART2RxWritePtr+1;
      if(i==UART_BUFFER_SIZE) i=0;

      if(i!=UART2RxReadPtr)
      {
        UART2RxBuffer[UART2RxWritePtr]=USARTy->DR;
        UART2RxWritePtr=i;
      }

      //Empfangs-Timeout auf Startwert setzen
      UART2TimeoutTimer=UART_TIMEOUT_VALUE;

      //Sendeverzögerung auf Startwert setzen
      UART2TransmitDelayTimer=UART2TransmitDelay;
    }
  }

  //Kann das nächste Zeichen gesendet werden?
  if(USARTy->SR & USART_FLAG_TXE)
  {
    if(UART2TransmitDelayTimer==0)
    {
      if(UART2TxWritePtr!=UART2TxReadPtr)
      {
        SET_RS485_SEND_MODE_2();
        USARTy->DR=UART2TxBuffer[UART2TxReadPtr++];
        if(UART2TxReadPtr==UART_BUFFER_SIZE) UART2TxReadPtr=0;
      }
      else
      {
        //Sendeinterrupt deaktivieren, wenn kein Zeichen im Sendepuffer ist
        USART_ITConfig(USARTy, USART_IT_TXE ,DISABLE);
      }
    }
  }

  //Allerletztes Bit gesendet?
  if(USARTy->SR & USART_FLAG_TC)
  {
    USART_ClearITPendingBit(USARTy, USART_IT_TC);
    if(UART2TxReadPtr==UART2TxWritePtr) SET_RS485_RECEIVE_MODE_2();
  }
}


/*******************************************************************
   Funktion: WriteUART()
   Parameter: ch: Zu schreibendes Zeichen

   Zweck: Senden eines Zeichens über UART3
          (Einstellen in den Sendepuffer)
********************************************************************/
void WriteUART_2(UCHAR ch)
{
  int i;

  //Zeichen in die Warteschlange stellen
  i=UART2TxWritePtr+1;
  if(i==UART_BUFFER_SIZE) i=0;

  if(i!=UART2TxReadPtr)
  {
    UART2TxBuffer[UART2TxWritePtr]=ch;
    UART2TxWritePtr=i;

    //Sendeinterrupt aktivieren
    USART_ITConfig(USARTy, USART_IT_TXE, ENABLE);
  }
}


/*******************************************************************
   Funktion: ReadUART()
   Parameter: *ch: Zeiger auf Variable für zu lesendes Zeichen

   Rückgabewert: TRUE bei Erfolg
                 FALSE wenn kein Zeichen da war
   Zweck: Lesen eines Zeichens aus dem Empfangspuffer der UART3
          (Einstellen in den Sendepuffer)
********************************************************************/
UCHAR ReadUART_2(UCHAR *ch)
{
  //Kein Zeichen vorhanden?
  if(UART2RxReadPtr==UART2RxWritePtr) return FALSE;

  //Zeichen aus dem Puffer holen
  *ch=UART2RxBuffer[UART2RxReadPtr++];
  if(UART2RxReadPtr==UART_BUFFER_SIZE)  UART2RxReadPtr=0;

  return TRUE;
}


/*******************************************************************
   Funktion: SetUARTTransmitDelay()
   Parameter: Delay: Sendeverzögerung in ms

   Rückgabewert: ---

   Zweck: Setzen der Sendeverzögerung
          (für einige RS485-Adapter wichtig)
********************************************************************/
void SetUARTTransmitDelay_2(UINT Delay)
{
  UART2TransmitDelay=Delay;
}


/*******************************************************************
   Funktion: CheckUARTTimeout()
   Parameter: ---

   Rückgabewert: TRUE wenn zwischenzeitlich ein Timeout aufgetreten ist
                 FALSE wenn kein Timeout aufgetreten ist
   Zweck: Prüfen, ob beim Empfangen ein Timeout aufgetreten ist (also
          Zeit zwischen zwei Bytes >5ms) und Zurücksetzen des
          Timeout-Flag.
********************************************************************/
UINT CheckUARTTimeout_2(void)
{
  if(UART2TimeoutFlag)
  {
    UART2TimeoutFlag=FALSE;
    return TRUE;
  }
  else return FALSE;
}


/*******************************************************************
   Funktion: ClearUARTRxBuffer()
   Parameter: ---

   Rückgabewert: ---

   Zweck: Löschen des Empfangspuffers
********************************************************************/
void ClearUARTRxBuffer_2(void)
{
  DisableInterrupts();
  UART2RxReadPtr=0;
  UART2RxWritePtr=0;
  EnableInterrupts();
}


/*******************************************************************
   Funktion: DisableRS485()
   Parameter: ---

   Rückgabewert: ---

   Zweck: Abschalten der RS485-Schnittstelle zum TMC562
********************************************************************/
void DisableRS485(void)
{
  USART_ITConfig(USARTy,USART_IT_TXE ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_TC  ,DISABLE);
  USART_ITConfig(USARTy,USART_IT_RXNE,DISABLE);
  SET_RS485_RECEIVE_MODE_2();
}


/*******************************************************************
   Funktion: DisableRS485()
   Parameter: ---

   Rückgabewert: ---

   Zweck: Wiedereinschalten der RS485-Schnittstelle zum TMC562
********************************************************************/
void EnableRS485(void)
{
  USART_ITConfig(USARTy,USART_IT_TXE ,ENABLE);
  USART_ITConfig(USARTy,USART_IT_TC  ,ENABLE);
  USART_ITConfig(USARTy,USART_IT_RXNE,ENABLE);
}
#endif
