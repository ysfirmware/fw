
/*
  ex05.1_systick

  SysTick timer ����
  Ÿ�̸� ������ ���ͷ�Ʈ ����
 */

#include <stdio.h>
#include "stm32f4xx.h"
#include "led.h"
#include "llio.h"

static volatile unsigned long tick_count;

int main()
{
    LED_Init();
    LED_R_ON();
    LED_G_ON();

    LLIO_Init(115200);


    //
    // SysTick Timer ����
    //
    // SysTick timer�� 24bit counter register�� ������ �ִ�.
    // 2^24 = 16777216(16M)
    // system core clock�� 1:1�� count�ϹǷ� �ִ� �ֱ�� 168M/16M = 10.5Hz = 95ms
    // �̺��� �� ���� �ֱ�� SysTick Ÿ�̸Ӹ� �����ϸ� SysTick_Config()�� ���� �����Ѵ�.
    SystemCoreClockUpdate();
    if (SysTick_Config(SystemCoreClock / 20))   // 20Hz tick interrupt
    {
        assert_param(0);
    }



    while(1)
    {
        __WFI();

        printf("System on time = %.2f[s]\r\n", tick_count / 20.);
    }
}


void SysTick_Handler(void)
{
    LED_G_TOGGLE();

    tick_count++;
}


