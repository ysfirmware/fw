/****************************************************
  Projekt: TMC562-Eval

  Modul:   Eval562-Main.c
           Hauptprogramm für das TMC562-Evalboard

  Datum:   14.10.2011 OK
*****************************************************/

#include <stdlib.h>

#include "stm32f10x_lib.h"
#include "bits.h"

#include "Eval50xx.h"
#include "Eval50xx-IO.h"
#include "Eval50xx-SPI.h"
#include "Eval50xx-UART.h"
#include "Eval50xx-USB.h"
#include "Eval50xx-SysTick.h"
#include "Eval50xx-Commands.h"
#include "Eval50xx-Registers.h"

const char *VersionString="5031V100";
UCHAR ExtClockFlag;


/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void RCC_Configuration(void)
{
  ErrorStatus HSEStartUpStatus;

  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }

    ExtClockFlag=TRUE;
  }

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
}


/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
  #if defined(BOOTLOADER)
  //Set the vector table base location at 0x08004000 (right after the TMCM boot loader)
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x4000);
  #else
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
  #endif
#endif
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
}


/*******************************************************************
   Funktion: InitMotorDrivers()
   Parameter: ---

   Rückgabewert: ---

   Zweck: Initialisierung der Motortreiber-Einstellungen aufgrund
          der Jumper.
********************************************************************/
void InitMotorDrivers(void)
{
  //Richtung von Motor 2 invertieren (damit beide gleich laufen)
  WriteTMC562Datagram(0x00, 0x00, 0x00, 0x02, 0x00);

  //Basiskonfiguration der Treiber einstellen
  //Motor 1
  WriteTMC562Datagram(0x6c, 0x00, 0x01, 0x01, 0xd5);
  WriteTMC562Datagram(0x30, 0x00, 0x07, 0x17, 0x03);

  //Motor 2
  WriteTMC562Datagram(0x7c, 0x00, 0x01, 0x01, 0xd5);
  WriteTMC562Datagram(0x50, 0x00, 0x07, 0x17, 0x03);
}


//Hauptprogramm
int main(void)
{
  UINT Delay;

  //Konfiguration des Systemtaktes
  RCC_Configuration();

  //Konfiguration des NVIC
  NVIC_Configuration();

  //Initialisierung der Peripherie
  InitIO();

  if(IS_CPU_DISABLE())
  {
    DisablePins();
    LED_LEFT_OFF();
    LED_RIGHT_ON();
    for(;;) RESET_WATCHDOG();
  }
  else
  {
    InitSPI();
    EnableInterrupts();
    InitSysTick();
  }

  InitUART(115200);
  SetUARTTransmitDelay(0);

  InitUSB();

  //Software-Initialisierung
  Delay=0;

  //Grundkonfiguration der Motortreiber
  InitMotorDrivers();

  //Hauptschleife
  for(;;)
  {
    RESET_WATCHDOG();
    ProcessCommands();
    SystemControl();

    if(abs(GetSysTimer()-Delay)>1000)
    {
      LED_LEFT_TOGGLE();
      Delay=GetSysTimer();
    }
  }
}
