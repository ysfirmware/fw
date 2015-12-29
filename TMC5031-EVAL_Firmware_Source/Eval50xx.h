/****************************************************
  Projekt: TMC562-Eval

  Modul:   Eval562.h
           Allgemeine Definitionen

  Datum:   17.10.2011 OK
*****************************************************/


#ifndef __EVAL_50xx_H
#define __EVAL_50xx_H

//Allgemeine Definitionen
#define FALSE     0
#define TRUE      1

typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned int UINT;
typedef signed long long INT64;
typedef unsigned long long UINT64;

#define SW_TYPE_HIGH  0x02
#define SW_TYPE_LOW   0x32
#define SW_VERSION_HIGH  1
#define SW_VERSION_LOW   3

#define CM_SPI 0
#define CM_UART 1

#define MIN(a,b) (a<b) ? (a) : (b)
#define MAX(a,b) (a>b) ? (a) : (b)

#define RESET_WATCHDOG()

#define SPI_DEV_TMC50xx  1

#define SELECT_TMC50xx()        GPIOA->BRR=BIT4
#define DESELECT_TMC50xx()      GPIOA->BSRR=BIT4
#define TMC50xx_SELECTED()      (GPIOA->ODR & BIT4==0)

#define ENABLE_TMC50xx()        GPIOB->BRR=BIT7
#define DISABLE_TMC50xx()       GPIOB->BSRR=BIT7

#define IS_CPU_DISABLE()        (!(GPIOB->IDR & BIT12))

#define UART_1 1
#define UART_2 2
#define UART_3 3
#define UART_INTERFACE_1 UART_2
#define SET_RS485_SEND_MODE_1()
#define SET_RS485_RECEIVE_MODE_1()
#define IS_RS485_SENDING_1()   0

//LEDs
#define LED_LEFT_ON()            GPIOB->BRR=BIT5
#define LED_LEFT_OFF()           GPIOB->BSRR=BIT5
#define LED_LEFT_TOGGLE()        GPIOB->ODR^=BIT5
#define LED_RIGHT_ON()           GPIOB->BRR=BIT6
#define LED_RIGHT_OFF()          GPIOB->BSRR=BIT6
#define LED_RIGHT_TOGGLE()       GPIOB->ODR^=BIT6

#define N_O_MOTORS 2

//Allgemeine Funktionen
void ResetCPU(UCHAR ResetPeripherals);
void EnableInterrupts(void);
void DisableInterrupts(void);

#endif
