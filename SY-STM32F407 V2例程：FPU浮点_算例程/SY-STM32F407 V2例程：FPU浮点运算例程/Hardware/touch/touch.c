#include "touch.h" 
#include "lcd.h"
#include "delay.h"	

uint16_t Phy_X,Phy_Y;                        //触笔的物理坐标
uint16_t ADC_X,ADC_Y;                        //触笔的ADC值
float xoff,yoff,xfac,yfac;                   //触笔的校正值及换算比值
/******************************************************************************************
*函数名：Touch_GetPhyXY()
* 参数：void
* 返回值：uint8_t 返回1表示成功，返回0表示失败
* 功能：读取XY物理位置
*********************************************************************************************/
uint8_t Touch_GetPhyXY(void)
{
	uint8_t i, j, k, min;
	uint16_t temp;
	uint16_t tempXY[2][SAMP_CNT], XY[2];// 采样
	for(i=0; i<SAMP_CNT; i++)
	{
				tempXY[0][i] = ADS_Read(CMD_RDX);
				tempXY[1][i] = ADS_Read(CMD_RDY);
	}
	// 滤波
	for(k=0; k<2; k++)
	{ 
		// 降序排列
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
	 // 设定阈值
		if((tempXY[k][SAMP_CNT_DIV2]-tempXY[k][SAMP_CNT_DIV2-1]) > 5)
			return 0;
	// 求中间值的均值
		XY[k] = (tempXY[k][SAMP_CNT_DIV2]+tempXY[k][SAMP_CNT_DIV2-1])>>1;
		}
		if(XY[0]==0||XY[1]==2047)
			{
				return 0;
			}
		ADC_X=XY[0];
		ADC_Y=XY[1];
		// 矫正坐标
		Phy_X = (uint16_t)(ADC_X*xfac+xoff);
		Phy_Y = (uint16_t)(ADC_Y*yfac+yoff);
		return 1;
}
/******************************************************************************************
*函数名：ADS_ReadWrite()
* 参数：uint8_t byte 发送的8位数据
* 返回值：uint8_t 8位数据
* 功能：SPI读写函数
*********************************************************************************************/
static uint8_t ADS_ReadWrite(uint8_t byte)
{
 	while((SPI1->SR&SPI_I2S_FLAG_TXE)==RESET);               //等待SPI发送空闲
 	SPI1->DR = byte;                                         //发送数据
 	while((SPI1->SR&SPI_I2S_FLAG_RXNE)==RESET);              //等待接收数据
 	return(SPI1->DR);                                        //返回接收的数据
}	
/******************************************************************************************
*函数名：ADS_Read()
* 参数：uint8_t CMD 指令，读x或读y
* 返回值：void
* 功能：读取ADS的ADC值
*********************************************************************************************/
uint16_t ADS_Read(uint8_t CMD)	  
{ 	 	  
	uint16_t Num=0,temp;  	 
	TCS_LOW;                                        //使能片选
	ADS_ReadWrite(CMD);                             //发送指令
	delay_nus(6);	                                  //延时
	temp=ADS_ReadWrite(0x00);                       //读取高8位
	Num=temp<<8;                                    //左移处理
	delay_nus(1);                                   //延时
	temp=ADS_ReadWrite(0x00);                        //读取低四位
	Num|=temp;  	                                  //加上低四位的值
	Num>>=4;                                        //右移四位，将无效低四位去除，构成12位ADC值
	TCS_HIGH;                                       //失能片选
	return(Num);  
} 
/******************************************************************************************
*函数名：AdjustTouchPanel()
* 参数：void
* 返回值：void
* 功能：触摸屏校准函数
*********************************************************************************************/
void AdjustTouchPanel(void)
{
	uint16_t touchXY[5][2];
	uint8_t cnt=0;
	uint16_t temp1,temp2,temp3;
	LCD_Clear(WHITE);                                //清屏
	LCD_SetColors(BLUE,WHITE);                       //设置前景色和背景色
	DrawAdjustPoint(20,20);                          //绘制第一个准星
	LCD_DrawString(40,50,"Touch Sight");
	while(1)
		{
			if(PEN_State==RESET)
				{
				 	if(Touch_GetPhyXY())
						{
							touchXY[cnt][0]=ADC_X;             //获取X的ADC值
							touchXY[cnt][1]=ADC_Y;             //获取y的ADC值
							while(PEN_State==RESET);           //等待触笔松开
							delay_nms(50);                     //消抖
							cnt++;                             //跳转到下个点
						}
label:				switch(cnt)
						{
							case 0:
								ClearAera(105,145,30,30);        //清除第5个准星位置
								DrawAdjustPoint(20,20);          //绘制第一个准星
							 break;
							case 1:
								ClearAera(5,5,30,30);             //清除第1个准星位置
								DrawAdjustPoint(20,300);          //绘制第二个准星
							  break;
							case 2:
								ClearAera(5,285,30,30);          //清除第2个准星位置
								DrawAdjustPoint(220,300);        //绘制第三个准星  
							  break;
							case 3:
								ClearAera(205,285,30,30);         //清除第3个准星位置
								DrawAdjustPoint(220,20);          //绘制第四个准星
							  break;
							case 4:
								ClearAera(205,5,30,30);          //清除第4个准星位置
								DrawAdjustPoint(120,160);        //绘制第五个准星
							  break;
							case 5:
								 temp1=touchXY[3][0]-touchXY[0][0];                               //获取上方两点的X的ADC值的差
							   temp2=touchXY[2][0]-touchXY[1][0];                               //获得下方两点的X的ADC值的差
								 if(ABS(temp1-temp2)>ADJUST_DE)                                    //对比偏差是否大于阀值，若大于则不符合
									 {
									 cnt=0;                                                          //归零
							     goto label;                                                     //跳转重新开始
									 }
									 temp3=temp1+temp2;                                              //将上下X的ADC的值的差值相加
									 xfac=200.0/(temp3>>1);                                          //获取x的比值
									 temp3=temp3>>2;                                                 //将上下X的ADC的值的差值相加除以4和中心点的X的ADC值对比
								  if(ABS(touchXY[4][0]-touchXY[0][0]-temp3)>ADJUST_DE)             //对比偏差是否大于阀值，若大于则不符合
									 {
									 cnt=0;                                                          //归零
							     goto label;                                                     //跳转重新开始
									 }
							   temp1=touchXY[1][1]-touchXY[0][1];                                 //获取左边两点的Y的ADC值的差
							   temp2=touchXY[2][1]-touchXY[3][1];                                 //获取右边两点的Y的ADC值的差
								 if(ABS(temp1-temp2)>ADJUST_DE)                                     //对比偏差是否大于阀值，若大于则不符合
									 {
									 cnt=0;                                                           //归零
							     goto label;                                                      //跳转重新开始
									 }
									 temp3=temp1+temp2;                                               //将左右Y的ADC差相加
									 yfac = 280.0/(temp3>>1);                                         //获取y的比值
									 temp3=temp3>>2;                                                  //将左右Y的ADC差的和除以4和中心点的Y的ADC值对比
								  if(ABS(touchXY[4][1]-touchXY[0][1]-temp3)>ADJUST_DE)              //对比偏差是否大于阀值，若大于则不符合
									 {
									 cnt=0;                                                           //归零
							     goto label;                                                      //跳转重新开始
									 }
									 xoff=(240-xfac*(touchXY[0][0]+touchXY[3][0]))/2;                 //获取到X的修正值
									 yoff=(320-yfac*(touchXY[0][1]+touchXY[1][1]))/2;                 //获取到Y的修正值
									 LCD_DrawString(40,180," Finish");                                //校验完成
									 delay_nms(500);                                                  //延时
									 return ;                                                         //返回
						}
				}
		}

}
/******************************************************************************************
*函数名：DrawAdjustPoint()
* 参数：uint16_t x,uint16_t y xy坐标
* 返回值：void
* 功能：绘制触摸校准时使用的准星
*********************************************************************************************/
void DrawAdjustPoint(uint16_t x,uint16_t y)
{ 
	/*绘制中心点*/
	DrawPixel(x,y);
	DrawPixel(x-1,y);
	DrawPixel(x,y-1);
	DrawPixel(x-1,y-1);
	DrawPixel(x+1,y-1);
	DrawPixel(x,y+1);
  DrawPixel(x-1,y+1);
	DrawPixel(x+1,y+1);
	DrawPixel(x+1,y);
	/*绘制两个同心圆*/
	LCD_DrawCircle(x,y,12);
	LCD_DrawCircle(x,y,10);
	/*绘制十字线*/
	LCD_DrawHLine(x-14,y,28);
	LCD_DrawVLine(x,y-14,28);
	/*绘制小方框*/
	LCD_DrawRect(x-3,y-3,6,6);
}
/******************************************************************************************
*函数名：DrawTouchPoint()
* 参数：uint16_t x,uint16_t y xy坐标
* 返回值：void
* 功能：绘制触摸笔笔尖的点
*********************************************************************************************/
void DrawTouchPoint(uint16_t x,uint16_t y)
{
	DrawPixel(x,y);                  //绘制中心点
	/*在四周分别绘制点*/
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
*函数名：Touch_Init()
* 参数：void
* 返回值：void
* 功能：触摸芯片初始化
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
  	GPIO_SetBits(GPIOD, GPIO_Pin_12);//不选中 
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
  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//全双工
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//8位数据模式
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//空闲模式下SCK为1
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//数据采样从第2个时间边沿开始
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS软件管理
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//波特率
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//大端模式
  	SPI_InitStructure.SPI_CRCPolynomial = 7;//CRC多项式
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;//主机模式
  	SPI_Init(SPI1, &SPI_InitStructure);
  	SPI_Cmd(SPI1, ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//pen_int:PD13   
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  	GPIO_Init(GPIOD, &GPIO_InitStructure);	
}

