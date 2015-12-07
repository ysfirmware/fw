#include "adc.h"

#define ADC1_DR_ADDRESS    ((uint32_t)0x4001204C)   //����ADC1�����ݵ�ַ����stm32f4xx.h�п����ҵ���ַ
 uint16_t	ADC1ConvertedValue;                       //����ADC1��ת��ֵ
 /***************************************************************************
*��������ADC_GPIO_Config()
*������void
*����ֵ:void
*���ܣ�ADC��GPIO���ú������ڲ�����
*****************************************************************************/ 
static void ADC_GPIO_Config(void)
	{
	GPIO_InitTypeDef    GPIO_InitStructure;
		/*ʹ��GPIOBʱ��*/
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	/*����PB0Ϊģ�������PB0->ADC12��ͨ��8*/
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;                         //��ʼ��IO���ó�PIN0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;                      //GPIOģʽ���ó�ģ������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;                 //����������������
  GPIO_Init(GPIOB, &GPIO_InitStructure);                            //��ʼ��GPIOB��PIN0
  }
/***************************************************************************
*��������ADC_CH8_Config()
*������void
*����ֵ:void
*���ܣ�ADC1ͨ��8�����ã��ڲ�����
*****************************************************************************/ 
static	void ADC_CH8_Config(void)
	{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
		/*ʹ��ADC1��ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
		  /*ADC������ʼ������*/
  ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;                     //����ADCģʽΪ����ģʽ
  ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;                  //����ADCʱ�ӷ�ƵΪ2��Ƶ
  ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;      //ʧ��DMA����ģʽ
  ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; //�������β��õļ��ʱ��
  ADC_CommonInit(&ADC_CommonInitStructure);                                     //��ʼ��ADC��������

  /*ADC1��ʼ������*/
  ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                        //����ADC�ֱ���Ϊ12λ
  ADC_InitStructure.ADC_ScanConvMode = DISABLE;                                 //ʧ�ܶ�ͨ��ɨ��ת��ģʽ
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                            //ʹ������ת��ģʽ
  ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
  ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                        //�������ݶ��뷽ʽΪ�Ҷ���
  ADC_InitStructure.ADC_NbrOfConversion = 1;                                    //ָ��ADC��ת������Ϊ1
  ADC_Init(ADC1, &ADC_InitStructure);                                            //��ʼ������ADC1
  /*ADC1ͨ��8����ͨ�����ã�����Ƶ��Ϊ3������*/
  ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 1, ADC_SampleTime_3Cycles);
 /*ʹ��ADC1��DMA�������������һ�δ�������Ժ�*/
  ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
  /* ʹ��ADC1��DMA����*/
  ADC_DMACmd(ADC1, ENABLE);
  /* ʹ��ADC1*/
  ADC_Cmd(ADC1, ENABLE);	
	}
/***************************************************************************
*��������ADC_DMA_Config()
*������void
*����ֵ:void
*���ܣ�ADC��DMA�������ã��ڲ�����
*****************************************************************************/ 
	static void ADC_DMA_Config(void)
	{
		DMA_InitTypeDef DMA_InitStructure;
		/*ʹ��DMA2ʱ��*/
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	  /* DMA2 Stream0 ͨ��0����*/
  DMA_InitStructure.DMA_Channel = DMA_Channel_0;                                        //ָ��DMAͨ��Ϊͨ��0
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADDRESS;                 //ָ��DMA���������ַΪADC1�����ݵ�ַ
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;                //ָ��DNA���ڴ����ַΪADC1ת��ֵ�洢��ַ
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;                               //����DMA���䷽��Ϊ���赽�ڴ�
  DMA_InitStructure.DMA_BufferSize = 1;                                                 //���û����СΪ1
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;                      //ʧ�������ַ����
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;                              //ʧ���ڴ��ַ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;           //�����������ݴ�СΪ����ֳ�����16bit
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;                   //�����ڴ����ݴ�СΪ����ֳ�����16bit
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;                                       //����DMAģʽΪѭ��ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;                                    //��������ȨΪ��
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;                                //ʧ��FIFOģʽ
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;                     //����FIFO�ķ�ֵΪ����
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;                           //�����ڴ�ͻ������Ϊ��һ
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;                   //��������ͻ������Ϊ��һ
  DMA_Init(DMA2_Stream0, &DMA_InitStructure);                                           //��ʼ��DMA2_Stream0

  /* ʹ��DMA2_Stream0*/
  DMA_Cmd(DMA2_Stream0, ENABLE);
	}
/***************************************************************************
*��������ADC1_CH8_Init()
*������void
*����ֵ:void
*���ܣ�ADC1ͨ��8��ʼ�����ⲿ����
*****************************************************************************/ 
void ADC1_CH8_Init(void)
	{ 
		/*����ADC��GPIO*/
		ADC_GPIO_Config();  
    /*����ADC��DMA����*/		
		ADC_DMA_Config();
		/*����ADC��ͨ��8*/
		ADC_CH8_Config();
		/*�������ADC1ת��*/
		ADC_SoftwareStartConv(ADC1);
	}
