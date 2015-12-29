/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-UART.h
           UART-Funktionen (Bytes Senden/Empfangen)

  Datum:   17.10.2011 OK
*****************************************************/

#ifndef __EVAL562_UART_H
#define __EVAL562_UART_H

void InitUART(int BaudRate);
void WriteUART(UCHAR ch);
UCHAR ReadUART(UCHAR *ch);
void SetUARTTransmitDelay(UINT Delay);
UINT CheckUARTTimeout(void);
void ClearUARTRxBuffer(void);

void InitUART_2(int BaudRate);
void WriteUART_2(UCHAR ch);
UCHAR ReadUART_2(UCHAR *ch);
void SetUARTTransmitDelay_2(UINT Delay);
UINT CheckUARTTimeout_2(void);
void ClearUARTRxBuffer_2(void);
void DisableRS485(void);
void EnableRS485(void);

#endif
