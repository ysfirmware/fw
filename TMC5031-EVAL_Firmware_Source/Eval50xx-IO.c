/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-IO.c
           I/O-Funktionen

  Datum:   17.10.2011 OK
*****************************************************/

#include <stdlib.h>

#include "stm32f10x_lib.h"
#include "bits.h"
#include "Eval50xx.h"
#include "Eval50xx-UART.h"
#include "Eval50xx-SysTick.h"
#include "Eval50xx-Registers.h"


static int TOld[2];
static int XOld[2];
static int MeasuredSpeed[2];


/*******************************************************************
   Funktion: InitIO
   Parameter: ---

   Rückgabewert: ---

   Zweck: Initialisierung der I/O-Ports.
********************************************************************/
void InitIO(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_Base;
  TIM_OCInitTypeDef       TIM_Oc;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);

  //---------Port A
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  //Ausgänge Port A
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  GPIOA->BSRR=BIT4;   //CS_562 high

  //Eingänge Port A (mit Pull-up)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //---------Port B
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

  //Ausgänge Port B
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|
                                GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIOB->BSRR=BIT5|BIT6;             //LEDs aus
  GPIOB->BRR=BIT7;                   //DRV_ENN low => eingeschaltet

  //Eingänge Port B (mit Pull-up)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_13|GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  //---------Port C
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

  //Ausgänge Port C
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //Eingänge Port C (ohne Pull-up)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  //---------Timer 2 Kanal 3 erzeugt Taktfrequenz für den TMC562 (an PB10)
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  TIM_DeInit(TIM2);
  TIM_Base.TIM_Period = 2;
  TIM_Base.TIM_Prescaler = 0;
  TIM_Base.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_Base.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
  TIM_TimeBaseInit(TIM2, &TIM_Base);

  //Einstellungen für Kanal 3
  TIM_Oc.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_Oc.TIM_OutputState = TIM_OutputState_Enable;
  TIM_Oc.TIM_Pulse = 1;
  TIM_Oc.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC3Init(TIM2, &TIM_Oc);
  TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);
  TIM_ARRPreloadConfig(TIM2, ENABLE);
  TIM_Cmd(TIM2, ENABLE);

  //Ausgang 3 des Timer 2 mit PB10 verbinden
  GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/*******************************************************************
   Funktion: DisablePins
   Parameter: ---

   Rückgabewert: ---

   Zweck: Abschalten aller Pins, die zum TMC50xx gehen (außer Takt).
********************************************************************/
void DisablePins(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  //PA7, PA6, PA5, PA4
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7|GPIO_Pin_6|GPIO_Pin_5|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //PB12
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/*******************************************************************
   Funktion: GetMeasuredSpeed
   Parameter: Axis: 0 oder 1

   Rückgabewert: gemessene Geschwindigkeit

   Zweck: Abfragen der gemessenen Geschwindigkeit.
********************************************************************/
int GetMeasuredSpeed(UINT Axis)
{
  if(Axis<2)
    return MeasuredSpeed[Axis];
  else
    return 0;
}


/*******************************************************************
   Funktion: SystemControl
   Parameter: ---

   Rückgabewert: ---

   Zweck: Messen der Geschwindigkeit für dcStep.
********************************************************************/
void SystemControl(void)
{
  int XActual;
  int t2;

  if(abs(GetSysTimer()-TOld[0])>10)
  {
    t2=GetSysTimer();
    XActual=ReadTMC562Int(0x21);

    MeasuredSpeed[0]=((float) (XActual-XOld[0])/ (float) (t2-TOld[0]))*1048.576;
    TOld[0]=t2;
    XOld[0]=XActual;
  }

  if(abs(GetSysTimer()-TOld[1])>10)
  {
    t2=GetSysTimer();
    XActual=ReadTMC562Int(0x41);

    MeasuredSpeed[1]=((float) (XActual-XOld[1])/ (float) (t2-TOld[1]))*1048.576;
    TOld[1]=t2;
    XOld[1]=XActual;
  }
}


/*******************************************************************
   Funktion: ResetCPU
   Parameter: ResetPeripherals: Gibt an, ob nur die eigentliche CPU
              oder auch die On-Chip-Peripherie zurückgesetzt werden
              soll (TRUE = auch Peripherie, FALSE = nur Core).

   Rückgabewert: ---

   Zweck: Reset der CPU mit oder ohne der integrierten Peripherie.
********************************************************************/
void ResetCPU(UCHAR ResetPeripherals)
{
  if(ResetPeripherals)
    NVIC_GenerateSystemReset();
  else
    NVIC_GenerateCoreReset();
}


/*******************************************************************
   Funktion: EnableInterrupts
   Parameter: ---

   Rückgabewert: ---

   Zweck: Globales Freigeben aller Interrupts.
********************************************************************/
void EnableInterrupts(void)
{
  asm volatile("CPSIE I\n");
}


/*******************************************************************
   Funktion: DisableInterrupts
   Parameter: ---

   Rückgabewert: ---

   Zweck: Globales Sperren aller Interrupts.
********************************************************************/
void DisableInterrupts(void)
{
  asm volatile("CPSID I\n");
}
