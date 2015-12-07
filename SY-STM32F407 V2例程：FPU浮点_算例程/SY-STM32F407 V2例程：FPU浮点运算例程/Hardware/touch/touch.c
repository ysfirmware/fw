#include "touch.h" 
#include "lcd.h"
#include "delay.h"	

uint16_t Phy_X,Phy_Y;                        //���ʵ���������
uint16_t ADC_X,ADC_Y;                        //���ʵ�ADCֵ
float xoff,yoff,xfac,yfac;                   //���ʵ�У��ֵ�������ֵ
/******************************************************************************************
*��������Touch_GetPhyXY()
* ������void
* ����ֵ��uint8_t ����1��ʾ�ɹ�������0��ʾʧ��
* ���ܣ���ȡXY����λ��
*********************************************************************************************/
uint8_t Touch_GetPhyXY(void)
{
	uint8_t i, j, k, min;
	uint16_t temp;
	uint16_t tempXY[2][SAMP_CNT], XY[2];// ����
	for(i=0; i<SAMP_CNT; i++)
	{
				tempXY[0][i] = ADS_Read(CMD_RDX);
				tempXY[1][i] = ADS_Read(CMD_RDY);
	}
	// �˲�
	for(k=0; k<2; k++)
	{ 
		// ��������
		for(i=0; i<SAMP_CNT-1; i++)
		{
			min=i;
			for (j=i+1; j<SAMP_CNT; j++)
			{
				if (tempXY[k][min] > tempXY[k][j]) 
					min=j;
				}
				temp = tempXY[k][i];
				tempXY[k][i] = tempXY[k][min];
				tempXY[k][min] = temp;
		}
	 // �趨��ֵ
		if((tempXY[k][SAMP_CNT_DIV2]-tempXY[k][SAMP_CNT_DIV2-1]) > 5)
			return 0;
	// ���м�ֵ�ľ�ֵ
		XY[k] = (tempXY[k][SAMP_CNT_DIV2]+tempXY[k][SAMP_CNT_DIV2-1])>>1;
		}
		if(XY[0]==0||XY[1]==2047)
			{
				return 0;
			}
		ADC_X=XY[0];
		ADC_Y=XY[1];
		// ��������
		Phy_X = (uint16_t)(ADC_X*xfac+xoff);
		Phy_Y = (uint16_t)(ADC_Y*yfac+yoff);
		return 1;
}
/******************************************************************************************
*��������ADS_ReadWrite()
* ������uint8_t byte ���͵�8λ����
* ����ֵ��uint8_t 8λ����
* ���ܣ�SPI��д����
*********************************************************************************************/
static uint8_t ADS_ReadWrite(uint8_t byte)
{
 	while((SPI1->SR&SPI_I2S_FLAG_TXE)==RESET);               //�ȴ�SPI���Ϳ���
 	SPI1->DR = byte;                                         //��������
 	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==RESET);              //�ȴ���������
 	return(SPI1->DR);                                        //���ؽ��յ�����
}	
/******************************************************************************************
*��������ADS_Read()
* ������uint8_t CMD ָ���x���y
* ����ֵ��void
* ���ܣ���ȡADS��ADCֵ
*********************************************************************************************/
uint16_t ADS_Read(uint8_t CMD)	  
{ 	 	  
	uint16_t Num=0,temp;  	 
	TCS_LOW;                                        //ʹ��Ƭѡ
	ADS_ReadWrite(CMD);                             //����ָ��
	delay_nus(6);	                                  //��ʱ
	temp=ADS_ReadWrite(0x00);                       //��ȡ��8λ
	Num=temp<<8;                                    //���ƴ���
	delay_nus(1);                                   //��ʱ
	temp=ADS_ReadWrite(0x00);                        //��ȡ����λ
	Num|=temp;  	                                  //���ϵ���λ��ֵ
	Num>>=4;                                        //������λ������Ч����λȥ��������12λADCֵ
	TCS_HIGH;                                       //ʧ��Ƭѡ
	return(Num);  
} 
/******************************************************************************************
*��������AdjustTouchPanel()
* ������void
* ����ֵ��void
* ���ܣ�������У׼����
*********************************************************************************************/
void AdjustTouchPanel(void)
{
	uint16_t touchXY[5][2];
	uint8_t cnt=0;
	uint16_t temp1,temp2,temp3;
	LCD_Clear(WHITE);                                //����
	LCD_SetColors(BLUE,WHITE);                       //����ǰ��ɫ�ͱ���ɫ
	DrawAdjustPoint(20,20);                          //���Ƶ�һ��׼��
	LCD_DrawString(40,50,"Touch Sight");
	while(1)
		{
			if(PEN_State==RESET)
				{
				 	if(Touch_GetPhyXY())
						{
							touchXY[cnt][0]=ADC_X;             //��ȡX��ADCֵ
							touchXY[cnt][1]=ADC_Y;             //��ȡy��ADCֵ
							while(PEN_State==RESET);           //�ȴ������ɿ�
							delay_nms(50);                     //����
							cnt++;                             //��ת���¸���
						}
label:				switch(cnt)
						{
							case 0:
								ClearAera(105,145,30,30);        //�����5��׼��λ��
								DrawAdjustPoint(20,20);          //���Ƶ�һ��׼��
							 break;
							case 1:
								ClearAera(5,5,30,30);             //�����1��׼��λ��
								DrawAdjustPoint(20,300);          //���Ƶڶ���׼��
							  break;
							case 2:
								ClearAera(5,285,30,30);          //�����2��׼��λ��
								DrawAdjustPoint(220,300);        //���Ƶ�����׼��  
							  break;
							case 3:
								ClearAera(205,285,30,30);         //�����3��׼��λ��
								DrawAdjustPoint(220,20);          //���Ƶ��ĸ�׼��
							  break;
							case 4:
								ClearAera(205,5,30,30);          //�����4��׼��λ��
								DrawAdjustPoint(120,160);        //���Ƶ����׼��
							  break;
							case 5:
								 temp1=touchXY[3][0]-touchXY[0][0];                               //��ȡ�Ϸ������X��ADCֵ�Ĳ�
							   temp2=touchXY[2][0]-touchXY[1][0];                               //����·������X��ADCֵ�Ĳ�
								 if(ABS(temp1-temp2)>ADJUST_DE)                                    //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
									 cnt=0;                                                          //����
							     goto label;                                                     //��ת���¿�ʼ
									 }
									 temp3=temp1+temp2;                                              //������X��ADC��ֵ�Ĳ�ֵ���
									 xfac=200.0/(temp3>>1);                                          //��ȡx�ı�ֵ
									 temp3=temp3>>2;                                                 //������X��ADC��ֵ�Ĳ�ֵ��ӳ���4�����ĵ��X��ADCֵ�Ա�
								  if(ABS(touchXY[4][0]-touchXY[0][0]-temp3)>ADJUST_DE)             //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
									 cnt=0;                                                          //����
							     goto label;                                                     //��ת���¿�ʼ
									 }
							   temp1=touchXY[1][1]-touchXY[0][1];                                 //��ȡ��������Y��ADCֵ�Ĳ�
							   temp2=touchXY[2][1]-touchXY[3][1];                                 //��ȡ�ұ������Y��ADCֵ�Ĳ�
								 if(ABS(temp1-temp2)>ADJUST_DE)                                     //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
									 cnt=0;                                                           //����
							     goto label;                                                      //��ת���¿�ʼ
									 }
									 temp3=temp1+temp2;                                               //������Y��ADC�����
									 yfac = 280.0/(temp3>>1);                                         //��ȡy�ı�ֵ
									 temp3=temp3>>2;                                                  //������Y��ADC��ĺͳ���4�����ĵ��Y��ADCֵ�Ա�
								  if(ABS(touchXY[4][1]-touchXY[0][1]-temp3)>ADJUST_DE)              //�Ա�ƫ���Ƿ���ڷ�ֵ���������򲻷���
									 {
									 cnt=0;                                                           //����
							     goto label;                                                      //��ת���¿�ʼ
									 }
									 xoff=(240-xfac*(touchXY[0][0]+touchXY[3][0]))/2;                 //��ȡ��X������ֵ
									 yoff=(320-yfac*(touchXY[0][1]+touchXY[1][1]))/2;                 //��ȡ��Y������ֵ
									 LCD_DrawString(40,180," Finish");                                //У�����
									 delay_nms(500);                                                  //��ʱ
									 return ;                                                         //����
						}
				}
		}

}
/******************************************************************************************
*��������DrawAdjustPoint()
* ������uint16_t x,uint16_t y xy����
* ����ֵ��void
* ���ܣ����ƴ���У׼ʱʹ�õ�׼��
*********************************************************************************************/
void DrawAdjustPoint(uint16_t x,uint16_t y)
{ 
	/*�������ĵ�*/
	DrawPixel(x,y);
	DrawPixel(x-1,y);
	DrawPixel(x,y-1);
	DrawPixel(x-1,y-1);
	DrawPixel(x+1,y-1);
	DrawPixel(x,y+1);
  DrawPixel(x-1,y+1);
	DrawPixel(x+1,y+1);
	DrawPixel(x+1,y);
	/*��������ͬ��Բ*/
	LCD_DrawCircle(x,y,12);
	LCD_DrawCircle(x,y,10);
	/*����ʮ����*/
	LCD_DrawHLine(x-14,y,28);
	LCD_DrawVLine(x,y-14,28);
	/*����С����*/
	LCD_DrawRect(x-3,y-3,6,6);
}
/******************************************************************************************
*��������DrawTouchPoint()
* ������uint16_t x,uint16_t y xy����
* ����ֵ��void
* ���ܣ����ƴ����ʱʼ�ĵ�
*********************************************************************************************/
void DrawTouchPoint(uint16_t x,uint16_t y)
{
	DrawPixel(x,y);                  //�������ĵ�
	/*�����ֱܷ���Ƶ�*/
	DrawPixel(x-1,y);                
	DrawPixel(x,y-1);
	DrawPixel(x-1,y-1);
	DrawPixel(x+1,y-1);
	DrawPixel(x,y+1);
  DrawPixel(x-1,y+1);
	DrawPixel(x+1,y+1);
	DrawPixel(x+1,y);
}
/******************************************************************************************
*��������Touch_Init()
* ������void
* ����ֵ��void
* ���ܣ�����оƬ��ʼ��
*********************************************************************************************/
void Touch_Init(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
	  SPI_InitTypeDef  SPI_InitStructure;
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOD, ENABLE);
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
  	GPIO_SetBits(GPIOD, GPIO_Pin_12);//��ѡ�� 
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);  
	  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//ȫ˫��
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8λ����ģʽ
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//����ģʽ��SCKΪ1
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//���ݲ����ӵ�2��ʱ����ؿ�ʼ
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS�������
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//������
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//���ģʽ
  	SPI_InitStructure.SPI_CRCPolynomial = 7;//CRC����ʽ
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//����ģʽ
  	SPI_Init(SPI1, &SPI_InitStructure);
  	SPI_Cmd(SPI1, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//pen_int:PD13   
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);	
}

