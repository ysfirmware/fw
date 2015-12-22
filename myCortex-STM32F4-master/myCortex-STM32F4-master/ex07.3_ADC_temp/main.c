
/*
  ex07.3_ADC_temp

  ADC�̿� MCU ���� �µ� ���� �� �б�.
  �µ� ���� ����� �о� ���� �µ��� ȯ���� �� printf�� ����Ѵ�.
 */

#include <stdio.h>
#include "stm32f4xx.h"
#include "led.h"
#include "llio.h"


#define V25                 0.76            // 25�������� �µ� ���� ��� ����
#define AVG_SLOPE           2.5             // mV/degC




int main()
{
    ADC_InitTypeDef ADC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;
    unsigned short adc_result;
    float volt, temp;

    LED_Init();
    LED_R_ON();



    /* Enable ADC1 and GPIOC clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);



    /* ADC Common Init */
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;     // ADC clock = PCLK2/prescaler. ADC freq = typical 30MHz, Max 36MHz. PCLK2 = 168/2=84MHz.
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInit(&ADC_CommonInitStructure);



    /* ADC channel configuration */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfConversion = 1;
    ADC_Init(ADC1, &ADC_InitStructure);


    ADC_TempSensorVrefintCmd(ENABLE);           // ���� �µ� ���� Ȱ��ȭ.

    /* ADC1 regular channel configuration */
    // ���� �µ� ������ ä��16�� ����Ǿ� �ִ�.
    ADC_RegularChannelConfig(ADC1, ADC_Channel_16, 1, ADC_SampleTime_56Cycles);


    /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);


    LLIO_Init(115200);
    printf("\r\nADC Test\r\n");


    SystemCoreClockUpdate();
    if (SysTick_Config(SystemCoreClock / 20))   // 20Hz tick interrupt
    {
        assert_param(0);
    }

    while(1)
    {
        __WFI();

        if (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == SET)       // ADC �Ϸ�
        {
            adc_result = ADC_GetConversionValue(ADC1);
            volt = (float)adc_result * 3.3 / 4096.;
            temp = (volt - V25) * 1000 / AVG_SLOPE + 25.;
            printf("%d,%.2f,%.2f\r\n", adc_result, volt, temp);     // ADC ��� �о� ���
        }
    }

}

void SysTick_Handler(void)
{
    LED_G_TOGGLE();
    ADC_SoftwareStartConv(ADC1);                            // ADC ����
}



