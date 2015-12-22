
/*
  ex06.1_repeat_timer

  timer ����
  50ms �ֱ� �ݺ� Ÿ�̸� ����.
 */

#include <stdio.h>
#include "stm32f4xx.h"
#include "led.h"
#include "llio.h"


#define TIMER_PRESCALER_FREQ        1000000     // timer �Է� Ŭ�� 1MHz
#define TIMER_FREQ                  20          // timer �ݺ� �ֱ� 50ms = 20Hz

static volatile unsigned long tick_count;

int main()
{
    uint16_t PrescalerValue;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;


    LED_Init();
    LED_R_ON();
    LED_G_ON();

    LLIO_Init(115200);


    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0f;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0f;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* TIM3 clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* Compute the prescaler value */
    // timer base counter�� 1MHz �Է�.
    // TIMER3�� APB1�� ����Ǿ� �ִ�.
    // TIMER input clock�� APBx�� clock divider�� 1�� ��� HCLK�� Ŭ���� �״�� ���̰�,
    //                                            1�� �ƴ� ��� HCLK�� Ŭ�� /2�� ���ȴ�.
    // ���� �ҽ� ���������� drv/system_stm32f4xx.c ������ SetSysClock()�Լ� ������ APB1 clock�� HCLK /4�� �����ϰ� �ִ�.
    SystemCoreClockUpdate();
    PrescalerValue = (uint16_t) (SystemCoreClock / 2 / TIMER_PRESCALER_FREQ) - 1;

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = TIMER_PRESCALER_FREQ / TIMER_FREQ - 1;           // 20Hz timer
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* TIM IT enable */
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);




    while(1)
    {
        __WFI();

        printf("System on time = %.2f[s]\r\n", (float)tick_count / (float)TIMER_FREQ);
    }
}


void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

        LED_G_TOGGLE();

        tick_count++;
    }
}


