
/*
  ex07.2_ADC_int

  ADC peripheral ���� Ȯ��.
  ���� ������ ���� �о� ���а� ���װ��� ����Ѵ�.
  ADC ���ͷ�Ʈ�� Ÿ�̸Ӹ� �̿��� Ʈ���� ���.
 */

#include <stdio.h>
#include "stm32f4xx.h"
#include "led.h"
#include "llio.h"

#define TIMER_PRESCALER_FREQ        1000000
#define TIMER_FREQ                  100            // 100Hz �ֱ�� Ʈ���� �߻�



int main()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    uint16_t PrescalerValue;




    LED_Init();
    LED_R_ON();

    LLIO_Init(115200);


    /* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);


    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0f;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0f;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);


    /* ADC Common Init */
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;     // ADC clock = PCLK2/prescaler. ADC freq = typical 30MHz, Max 36MHz. PCLK2 = 168/2=84MHz.
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);



    /* ADC channel 10 (PC0) configuration */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);


    /* ADC channel configuration */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;  // TIM3�� Ʈ���� �߻��� ADC ����
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);


    /* ADC1 regular channel10 configuration */
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);

    // Enable ADC End Of Conversion interrupt
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);





    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0f;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0f;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Compute the prescaler value */
    SystemCoreClockUpdate();
    PrescalerValue = (uint16_t) (SystemCoreClock / 2 / TIMER_PRESCALER_FREQ) - 1;           // timer base counter�� 1MHz �Է�

    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = TIMER_PRESCALER_FREQ / TIMER_FREQ - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    // TIM3 update �̺�Ʈ���� Ʈ���� �߻��ϵ��� ����
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    /* TIM3 enable counter */
    TIM_Cmd(TIM3, ENABLE);






    printf("\r\nADC Test\r\n");


    while(1)
    {
        __WFI();
    }

}


void ADC_IRQHandler(void)
{
    if (ADC_GetITStatus(ADC1, ADC_IT_EOC) == SET)
    {
        LED_G_OFF();
        printf("%d\r\n", ADC_GetConversionValue(ADC1));     // ADC ��� �о� ���
    }
}




/*
    TIM3 ���ͷ�Ʈ�� ADC ���� ������ Ȯ���ϱ� ���� ����Ѵ�.
    �� ���ͷ�Ʈ�� ������� �ʴ��� ADC�� ���������� ����ȴ�.

    TIM3 ���ͷ�Ʈ���� ADC�� ���۵ǰ� ���ÿ� LED_G�� ������.
    ADC�� ����Ǹ� ADC ���ͷ�Ʈ�� �߻��ǰ�, ADC ���ͷ�Ʈ �ڵ鷯���� LED_G�� ����.
    ���Ƿν������� �̿��� LED_G�� �����ϸ� ADC ���࿡ �� 600ns ���� �ð��� �ɸ��� ���� Ȯ���� �� �ִ�.
    ADC ���� �ð��� ADC Common�� prescaler�� ADC_RegularChannelConfig()�� �̿��� ���� �����ϴ�.
*/
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
        LED_G_ON();
    }
}



