#include "lcd.h"
/*******************液晶屏硬件连接**********************

PD14 -----FSMC_D0  ----D0
PD15 -----FSMC_D1  ----D1
PD0   -----FSMC_D2  ----D2
PD1   -----FSMC_D3  ----D3
PE7    -----FSMC_D4  ---D4
PE8    -----FSMC_D5  ---D5
PE9    -----FSMC_D6  ---D6
PE10  -----FSMC_D7   ----D7
PE11  -----FSMC_D8   ----D8
PE12  -----FSMC_D9   ----D9
PE13  -----FSMC_D10   ----D10
PE14  -----FSMC_D11   ----D11
PE15  -----FSMC_D12   ----D12
PD8   -----FSMC_D13   ----D13
PD9   -----FSMC_D14   ----D14
PD10 -----FSMC_D15   ----D15
PD4   -----FSMC_NOE -----RD
PD5   -----FSMC_NWE ----WR
PD7    -----FSMC_NE1  ----CS
PD11 -----FSMC_A16   ----RS

PB5-------------------LCD_BL

***************************************************************/
/* 选择BANK1-NORSRAM1 连接 TFT，地址范围为0X60000000~0X63FFFFFF
 * FSMC_A16 接LCD的DC(寄存器/数据选择)脚
 * 寄存器基地址 = 0X60000000
 * RAM基地址 = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
 * 这里主要用于控制RS进行写寄存器写数据操作，因为板子的液晶RS接在A16管脚，所以要使地址线A16为高电平，则写地址到2的16次方处，并且因为液晶是16bit的
 *所以再乘上2，得出的地址加上BANK所在的地址即可。如RS接A0处，则RAM基地址为bank所在地址加上2的0次方的2倍，如是8bit屏则不需要乘以2
 */
#define LCD_RAM   *(vu16*)((u32)0x60020000)  //disp Data ADDR
#define LCD_REG   *(vu16*)((u32)0x60000000)	 //disp Reg  ADDR
  /*液晶屏的字体*/
static sFONT *LCD_Currentfonts;
sFONT Font16x24;
  /*液晶屏的前景色和背景色*/
static __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;
/**************************************************************************************************************
 * 函数名：GPIO_AF_FSMC_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD连接的GPIO复用连接到FSMC
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void GPIO_AF_FSMC_Config(void)
{
  /*配置GPIOD相应的管脚复用连接到FSMC*/
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);	        //配置GPIOD的Pin0复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);			//配置GPIOD的Pin1复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);			//配置GPIOD的Pin4复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);			//配置GPIOD的Pin5复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);			//配置GPIOD的Pin7复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);			//配置GPIOD的Pin8复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);			//配置GPIOD的Pin9复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);		//配置GPIOD的Pin10复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);		//配置GPIOD的Pin11复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);		//配置GPIOD的Pin14复用连接到FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);		//配置GPIOD的Pin15复用连接到FSMC
   /*配置GPIOE相应的管脚复用连接到FSMC*/
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);			//配置GPIOE的Pin7复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);			//配置GPIOE的Pin8复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);			//配置GPIOE的Pin9复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);		//配置GPIOE的Pin10复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);		//配置GPIOE的Pin11复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);		//配置GPIOE的Pin12复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);	    //配置GPIOE的Pin13复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);		//配置GPIOE的Pin14复用连接到FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);		//配置GPIOE的Pin15复用连接到FSMC
}
/**************************************************************************************************************
 * 函数名：LCD_GPIO_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD的GPIO管脚与FSMC连接
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;		                                              //定义GPIO初始化结构体
     
    /* 使能FSMC的AHB3时钟，这里注意，不同芯片总线会不同 */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
    
    /* 使能GPIOB，GPIOD，GPIOE的AHB1时钟*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE , ENABLE);
   	 /*配置液晶屏的背光控制管脚*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;											  //设置管脚模式为输出模式
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //设置翻转速度为100Mhz，M4就是快啊
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;											  //设置输出类型为推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;												  //设置管脚为Pin_5,根据自己液晶接法设置
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  //设置上/下拉电阻为无上拉下拉电阻		
    GPIO_Init(GPIOB, &GPIO_InitStructure);													  //初始化GPIOB的Pin_5
    		   
    /* 配置基于FSMC的液晶屏的数据线
	 * 液晶屏数据线：FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	 */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //设置翻转速度为100Mhz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;											  //设置管脚模式为复用模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);													  //初始化GPIOB的Pin_0，Pin_，Pin_2，
	                                                                    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* 配置基于FSMC的液晶屏控制线
	 * PD4-FSMC_NOE  :LCD-RD
   * PD5-FSMC_NWE  :LCD-WR
	 * PD7-FSMC_NE1  :LCD-CS
   * PD11-FSMC_A16 :LCD-DC
	 */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 ; 
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
    GPIO_AF_FSMC_Config();				             //在F407中必须加上这个复用连接配置，否则不能连接到FSMC
    GPIO_SetBits(GPIOB, GPIO_Pin_5);			 
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 
   
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
    GPIO_SetBits(GPIOD, GPIO_Pin_6); 	
}

/**************************************************************************************************************
 * 函数名：LCD_FSMC_Config()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：配置LCD的FSMC
 * 调用  ：内部调用        
 *****************************************************************************************************************/
static void LCD_FSMC_Config(void)
{    
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  	FSMC_NORSRAMTimingInitTypeDef  p;
	  LCD_GPIO_Config();                                                     //LCD管脚配置
  	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  	p.FSMC_AddressSetupTime = 5;                                           //地址建立时间
  	p.FSMC_AddressHoldTime = 0;                                            //地址保持时间
  	p.FSMC_DataSetupTime = 9;                                              //数据建立时间
  	p.FSMC_BusTurnAroundDuration = 0;
  	p.FSMC_CLKDivision = 0;
  	p.FSMC_DataLatency = 0;
  	p.FSMC_AccessMode = FSMC_AccessMode_A;                                  // 用模式A控制LCD
  	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
  	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
  	FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);   
  	FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE); 
}
/**************************************************************************************************************
 * 函数名：LCD_WriteReg()
 * 输入  ：uint8_t LCD_Reg 寄存器地址, uint16_t LCD_RegValue 寄存器的值
 * 输出  ：void
 * 描述  ：写LCD寄存器函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  /*写寄存器地址*/
  LCD_REG = LCD_Reg;
  /*写入寄存器值*/
  LCD_RAM = LCD_RegValue;
}
/**************************************************************************************************************
 * 函数名：LCD_ReadReg()
 * 输入  ：uint8_t LCD_Reg 需要读取的寄存器地址
 * 输出  ：uint16_t 寄存器的值
 * 描述  ：读取LCD寄存器的值
 * 调用  ：外部调用        
 *****************************************************************************************************************/
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  /*写寄存器地址*/
  LCD_REG = LCD_Reg;
  /*读出寄存器值并返回*/
  return (LCD_RAM);
}
/**************************************************************************************************************
 * 函数名：LCD_WriteRAM_Start()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：开始写LCD的RAM
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_WriteRAM_Start(void)
{
  LCD_REG = 0x22;
}

/**************************************************************************************************************
 * 函数名：LCD_WriteRAM()
 * 输入  ：uint16_t RGB_data  16bit颜色数据
 * 输出  ：void
 * 描述  ：向LCD的RAM写数据
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_WriteRAM(uint16_t RGB_data)
{
  /*向LCD的RAM写数据*/
  LCD_RAM = RGB_data;
}

/**************************************************************************************************************
 * 函数名：LCD_ReadRAM()
 * 输入  ：void
 * 输出  ：uint16_t  16bit颜色值
 * 描述  ：读取LCD的RAM数据
 * 调用  ：外部调用        
 *****************************************************************************************************************/
uint16_t LCD_ReadRAM(void)
{ 
	uint16_t dumry;
  LCD_REG = 0x22; /*选择RAM寄存器*/
	dumry=LCD_RAM;   /*假读*/
  /*读取16bit RAM值*/
  return LCD_RAM;
}

/**************************************************************************************************************
 * 函数名：LCD_DisplayOn()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD打开显示
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DisplayOn(void)
{
  /*打开显示 */
  LCD_WriteReg(0x07, 0x0173); /*设置LCD为262K色并打开显示*/
}

/**************************************************************************************************************
 * 函数名：LCD_DisplayOff()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD关闭显示
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_DisplayOff(void)
{
  /*关闭显示*/
  LCD_WriteReg(0x07, 0x0); 
}
/**************************************************************************************************************
 * 函数名：LCD_Init()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD初始化函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_Init(void)
{ 
  	u32 lcdid=0;

   	LCD_FSMC_Config();
  	delay_nms(5); 
  	lcdid=LCD_ReadReg(0x00);  
  	if(lcdid == 0x9320)
  	{
		LCD_WriteReg(0xE5,0x8000);
		LCD_WriteReg(0x00,0x0001);
		LCD_WriteReg(0x01,0x0100);
		LCD_WriteReg(0x02,0x0700);
		LCD_WriteReg(0x03,0x1030);
		LCD_WriteReg(0x04,0x0000);
		LCD_WriteReg(0x08,0x0202);
		LCD_WriteReg(0x09,0x0000);
		LCD_WriteReg(0x0A,0x0000);
		LCD_WriteReg(0x0B,0x0000);
		LCD_WriteReg(0x0C,0x0000);
		LCD_WriteReg(0x0F,0x0000);
		LCD_WriteReg(0x10,0x0000);
		LCD_WriteReg(0x11,0x0000);
		LCD_WriteReg(0x12,0x0000);
		LCD_WriteReg(0x13,0x0000);
		delay_nms(20);
		LCD_WriteReg(0x10,0x17B0);
		LCD_WriteReg(0x11,0x0137);
		delay_nms(5);
		LCD_WriteReg(0x12,0x0139);
		delay_nms(5);
		LCD_WriteReg(0x13,0x1d00);
		LCD_WriteReg(0x29,0x0013);
		delay_nms(5);
		LCD_WriteReg(0x20,0x0000);
		LCD_WriteReg(0x21,0x0000);
		LCD_WriteReg(0x30,0x0006);
		LCD_WriteReg(0x31,0x0101);
		LCD_WriteReg(0x32,0x0003);
		LCD_WriteReg(0x35,0x0106);
		LCD_WriteReg(0x36,0x0b02);
		LCD_WriteReg(0x37,0x0302);
		LCD_WriteReg(0x38,0x0707);
		LCD_WriteReg(0x39,0x0007);
		LCD_WriteReg(0x3C,0x0600);
		LCD_WriteReg(0x3D,0x020b);
		LCD_WriteReg(0x50,0x0000);
		LCD_WriteReg(0x51,0x00EF);
		LCD_WriteReg(0x52,0x0000);
		LCD_WriteReg(0x53,0x013F);
		LCD_WriteReg(0x60,0x2700);
		LCD_WriteReg(0x61,0x0001);
		LCD_WriteReg(0x6A,0x0000);
		LCD_WriteReg(0x80,0x0000);
		LCD_WriteReg(0x81,0x0000);
		LCD_WriteReg(0x82,0x0000);
		LCD_WriteReg(0x83,0x0000);
		LCD_WriteReg(0x84,0x0000);
		LCD_WriteReg(0x85,0x0000);
		LCD_WriteReg(0x90,0x0010);
		LCD_WriteReg(0x92,0x0000);
		LCD_WriteReg(0x93,0x0003);
		LCD_WriteReg(0x95,0x0110);
		LCD_WriteReg(0x97,0x0000);
		LCD_WriteReg(0x98,0x0000);
		LCD_WriteReg(0x03,0x1018);
		LCD_WriteReg(0x07,0x0173); 
  	}
  	else if(lcdid == 0x9325)
  	{
		LCD_WriteReg(0x00,0x0001);
		LCD_WriteReg(0x01,0x0100);
		LCD_WriteReg(0x02,0x0700);
		LCD_WriteReg(0x03,0x1018);
		LCD_WriteReg(0x04,0x0000);
		LCD_WriteReg(0x08,0x0202);
		LCD_WriteReg(0x09,0x0000);
		LCD_WriteReg(0x0A,0x0000);
		LCD_WriteReg(0x0C,0x0000);
		LCD_WriteReg(0x0D,0x0000);
		LCD_WriteReg(0x0F,0x0000);
		LCD_WriteReg(0x10,0x0000);
		LCD_WriteReg(0x11,0x0000);
		LCD_WriteReg(0x12,0x0000);
		LCD_WriteReg(0x13,0x0000);
		delay_nms(20);
		LCD_WriteReg(0x10,0x17B0);
		LCD_WriteReg(0x11,0x0137);
		delay_nms(5);
		LCD_WriteReg(0x12,0x0139);
		delay_nms(5);
		LCD_WriteReg(0x13,0x1d00);
		LCD_WriteReg(0x29,0x0013);
		delay_nms(5);
		LCD_WriteReg(0x20,0x0000);
		LCD_WriteReg(0x21,0x0000);
		LCD_WriteReg(0x30,0x0007);
		LCD_WriteReg(0x31,0x0302);
		LCD_WriteReg(0x32,0x0105);
		LCD_WriteReg(0x35,0x0206);
		LCD_WriteReg(0x36,0x0808);
		LCD_WriteReg(0x37,0x0206);
		LCD_WriteReg(0x38,0x0504);
		LCD_WriteReg(0x39,0x0007);
		LCD_WriteReg(0x3C,0x0105);
		LCD_WriteReg(0x3D,0x0808);
		LCD_WriteReg(0x50,0x0000);
		LCD_WriteReg(0x51,0x00EF);
		LCD_WriteReg(0x52,0x0000);
		LCD_WriteReg(0x53,0x013F);
		LCD_WriteReg(0x60,0xA700);
		LCD_WriteReg(0x61,0x0001);
		LCD_WriteReg(0x6A,0x0000);
		LCD_WriteReg(0x80,0x0000);
		LCD_WriteReg(0x81,0x0000);
		LCD_WriteReg(0x82,0x0000);
		LCD_WriteReg(0x83,0x0000);
		LCD_WriteReg(0x84,0x0000);
		LCD_WriteReg(0x85,0x0000);
		LCD_WriteReg(0x90,0x0010);
		LCD_WriteReg(0x92,0x0000);
		LCD_WriteReg(0x93,0x0003);
		LCD_WriteReg(0x95,0x0110);
		LCD_WriteReg(0x97,0x0000);
		LCD_WriteReg(0x98,0x0000);
		LCD_WriteReg(0x03,0x1018);
		LCD_WriteReg(0x07,0x0133);
  	}
  	else if(lcdid==0x4531)
  	{		
		LCD_WriteReg(0x00,0x0001);
		LCD_WriteReg(0x10,0x0628);
		LCD_WriteReg(0x12,0x0006);
		LCD_WriteReg(0x13,0x0A32);
		LCD_WriteReg(0x11,0x0040);
		LCD_WriteReg(0x15,0x0050);
		LCD_WriteReg(0x12,0x0016);
		delay_nms(15);
		LCD_WriteReg(0x10,0x5660);
		delay_nms(15);
		LCD_WriteReg(0x13,0x2A4E);
		LCD_WriteReg(0x01,0x0100);
		LCD_WriteReg(0x02,0x0300);	
		LCD_WriteReg(0x03,0x1018);
		LCD_WriteReg(0x08,0x0202);
		LCD_WriteReg(0x0A,0x0000);
		LCD_WriteReg(0x30,0x0000);
		LCD_WriteReg(0x31,0x0402);
		LCD_WriteReg(0x32,0x0106);
		LCD_WriteReg(0x33,0x0700);
		LCD_WriteReg(0x34,0x0104);
		LCD_WriteReg(0x35,0x0301);
		LCD_WriteReg(0x36,0x0707);
		LCD_WriteReg(0x37,0x0305);
		LCD_WriteReg(0x38,0x0208);
		LCD_WriteReg(0x39,0x0F0B);
		delay_nms(15);
		LCD_WriteReg(0x41,0x0002);
		LCD_WriteReg(0x60,0x2700);
		LCD_WriteReg(0x61,0x0001);
		LCD_WriteReg(0x90,0x0119);
		LCD_WriteReg(0x92,0x010A);
		LCD_WriteReg(0x93,0x0004);
		LCD_WriteReg(0xA0,0x0100);
		delay_nms(15);
		delay_nms(15);
		LCD_WriteReg(0x07,0x0133);
		delay_nms(15);
		LCD_WriteReg(0xA0,0x0000);
		delay_nms(20);
  	} 
	GPIO_SetBits(GPIOB,GPIO_Pin_5);
	LCD_Clear(BLUE);
	LCD_SetFont(&Font16x24);
	LCD_SetMode(1);							 
}
/**************************************************************************************************************
 * 函数名：LCD_SetMode()
 * 输入  ：uint8_t mode 设置屏幕刷屏模式
 * 输出  ：void
 * 描述  ：设置LCD刷屏模式
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetMode(uint8_t mode)
{
	switch(mode)//常用4(横屏)和7(竖屏)
	{
		case 1:LCD_WriteReg(0x03,0x1000);break;//向左向上:←↖
		case 2:LCD_WriteReg(0x03,0x1008);break;//向上向左:↑↖
		case 3:LCD_WriteReg(0x03,0x1010);break;//向右向上:→↗
		case 4:LCD_WriteReg(0x03,0x1018);break;//向上向右:↑↗
		case 5:LCD_WriteReg(0x03,0x1020);break;//向左向下:←↙
		case 6:LCD_WriteReg(0x03,0x1028);break;//向下向左:↓↙
		case 7:LCD_WriteReg(0x03,0x1030);break;//向右向下:→↘
		case 8:LCD_WriteReg(0x03,0x1038);break;//向下向右:↓↘
	}
}
/**************************************************************************************************************
 * 函数名：LCD_Clear()
 * 输入  ：void
 * 输出  ：void
 * 描述  ：LCD清屏函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_Clear(uint16_t Color)
{
  uint32_t index = 0;
  
  LCD_SetCursor(0,150); 
  LCD_WriteRAM_Start(); /*开始写GRAM */
  for(index = 0; index < 76800; index++)
  {
    LCD_RAM = Color;
  }  
}

/**************************************************************************************************************
 * 函数名：LCD_SetCursor()
 * 输入  ：uint16_t Xpos, uint16_t Ypos 设定的屏幕的X值和Y值
 * 输出  ：void
 * 描述  ：LCD设置光标位置函数
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
  LCD_WriteReg(0x20, Xpos);
  LCD_WriteReg(0x21, Ypos);
}
/**************************************************************************************************************
 * 函数名：LCD_SetColors()
 * 输入  ：_TextColor 前景色,_BackColor 背景色
 * 输出  ：void
 * 描述  ：设置LCD的前景色和背景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
  TextColor = _TextColor; 
  BackColor = _BackColor;
}
/**************************************************************************************************************
 * 函数名：LCD_GetColors()
 * 输入  ：*_TextColor 前景色的指针,*_BackColor 背景色的指针
 * 输出  ：void
 * 描述  ：获取LCD的前景色和背景色
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
  *_TextColor = TextColor; *_BackColor = BackColor;
}
/**************************************************************************************************************
 * 函数名：LCD_SetFont()
 * 输入  ：sFONT *fonts 要设置的字体
 * 输出  ：void
 * 描述  ：设置LCD的字体
 * 调用  ：外部调用        
 *****************************************************************************************************************/
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}




