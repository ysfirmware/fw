/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-SysTick.c
           Systick-Timer für 1ms-Takt

  Datum:   17.8.2009 OK
*****************************************************/

#include "stm32f10x_lib.h"
#include "bits.h"

#include "Eval50xx.h"

#define SYSTICK_PRE_EMPTION_PRIORITY 3
#define SYSTICK_SUB_PRIORITY 0

static volatile UINT SysTickTimer;
static volatile UCHAR SysTickDivFlag;
extern volatile UCHAR UART1TimeoutFlag;
extern volatile UINT UART1TimeoutTimer;
extern volatile UINT UART1TransmitDelayTimer;
extern volatile UCHAR UART2TimeoutFlag;
extern volatile UINT UART2TimeoutTimer;
extern volatile UINT UART2TransmitDelayTimer;



/*******************************************************************
   Funktion: SysTickHandler()
   Parameter: ---

   Rückgabewert: ---

   Zweck: Handler für den SysTick-Interrupt.
********************************************************************/
void SysTickHandler(void)
{
  if(SysTickDivFlag)
  {
    //1ms-Timer hochzählen (für GetSysTimer())
    SysTickTimer++;

    //Timer für RS485-Sendeverzögerung herunterzählen
    #if defined(UART_INTERFACE_1)
    if(UART1TransmitDelayTimer>0) UART1TransmitDelayTimer--;
    #endif
    #if defined(UART_INTERFACE_2)
    if(UART2TransmitDelayTimer>0) UART2TransmitDelayTimer--;
    #endif

    SysTickDivFlag=0;
  }
  else SysTickDivFlag++;

  //Timer für RS232/RS485-Timeout wird im 0,5ms-Takt heruntergezählt.
  #if defined(UART_INTERFACE_1)
  if(UART1TimeoutTimer>0)
  {
    UART1TimeoutTimer--;
    if(UART1TimeoutTimer==0) UART1TimeoutFlag=TRUE;
  }
  #endif
  #if defined(UART_INTERFACE_2)
  if(UART2TimeoutTimer>0)
  {
    UART2TimeoutTimer--;
    if(UART2TimeoutTimer==0) UART2TimeoutFlag=TRUE;
  }
  #endif
}


/*******************************************************************
   Funktion: InitSysTick
   Parameter: ---

   Rückgabewert: ---

   Zweck: Initialisierung des Systen-Timers.
********************************************************************/
void InitSysTick(void)
{
  /* Select AHB clock(HCLK) as SysTick clock source */
  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);

  /* SysTick interrupt each 500usec with Core clock equal to 72MHz */
  SysTick_SetReload(36000);

  /* Enable SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);

  NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick,
                            SYSTICK_PRE_EMPTION_PRIORITY, SYSTICK_SUB_PRIORITY);
  /* Enable SysTick interrupt */
  SysTick_ITConfig(ENABLE);
}


/*******************************************************************
   Funktion: GetSysTimer()
   Parameter: ---

   Rückgabewert: Zeit in ms

   Zweck: Auslesen des Systemtimers.
********************************************************************/
UINT GetSysTimer(void)
{
  return SysTickTimer;
}
