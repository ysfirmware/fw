#include "lcd.h"
/*******************Һ����Ӳ������**********************

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
/* ѡ��BANK1-NORSRAM1 ���� TFT����ַ��ΧΪ0X60000000~0X63FFFFFF
 * FSMC_A16 ��LCD��DC(�Ĵ���/����ѡ��)��
 * �Ĵ�������ַ = 0X60000000
 * RAM����ַ = 0X60020000 = 0X60000000+2^16*2 = 0X60000000 + 0X20000 = 0X60020000
 * ������Ҫ���ڿ���RS����д�Ĵ���д���ݲ�������Ϊ���ӵ�Һ��RS����A16�ܽţ�����Ҫʹ��ַ��A16Ϊ�ߵ�ƽ����д��ַ��2��16�η�����������ΪҺ����16bit��
 *�����ٳ���2���ó��ĵ�ַ����BANK���ڵĵ�ַ���ɡ���RS��A0������RAM����ַΪbank���ڵ�ַ����2��0�η���2��������8bit������Ҫ����2
 */
#define LCD_RAM   *(vu16*)((u32)0x60020000)  //disp Data ADDR
#define LCD_REG   *(vu16*)((u32)0x60000000)	 //disp Reg  ADDR
  /*Һ����������*/
static sFONT *LCD_Currentfonts;
sFONT Font16x24;
  /*Һ������ǰ��ɫ�ͱ���ɫ*/
static __IO uint16_t TextColor = 0x0000, BackColor = 0xFFFF;
/**************************************************************************************************************
 * ��������GPIO_AF_FSMC_Config()
 * ����  ��void
 * ���  ��void
 * ����  ������LCD���ӵ�GPIO�������ӵ�FSMC
 * ����  ���ڲ�����        
 *****************************************************************************************************************/
static void GPIO_AF_FSMC_Config(void)
{
  /*����GPIOD��Ӧ�ĹܽŸ������ӵ�FSMC*/
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_FSMC);	        //����GPIOD��Pin0�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_FSMC);			//����GPIOD��Pin1�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource4,GPIO_AF_FSMC);			//����GPIOD��Pin4�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource5,GPIO_AF_FSMC);			//����GPIOD��Pin5�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource7,GPIO_AF_FSMC);			//����GPIOD��Pin7�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_FSMC);			//����GPIOD��Pin8�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_FSMC);			//����GPIOD��Pin9�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource10,GPIO_AF_FSMC);		//����GPIOD��Pin10�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource11,GPIO_AF_FSMC);		//����GPIOD��Pin11�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource14,GPIO_AF_FSMC);		//����GPIOD��Pin14�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOD,GPIO_PinSource15,GPIO_AF_FSMC);		//����GPIOD��Pin15�������ӵ�FSMC
   /*����GPIOE��Ӧ�ĹܽŸ������ӵ�FSMC*/
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource7,GPIO_AF_FSMC);			//����GPIOE��Pin7�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource8,GPIO_AF_FSMC);			//����GPIOE��Pin8�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource9,GPIO_AF_FSMC);			//����GPIOE��Pin9�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_FSMC);		//����GPIOE��Pin10�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource11,GPIO_AF_FSMC);		//����GPIOE��Pin11�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource12,GPIO_AF_FSMC);		//����GPIOE��Pin12�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource13,GPIO_AF_FSMC);	    //����GPIOE��Pin13�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource14,GPIO_AF_FSMC);		//����GPIOE��Pin14�������ӵ�FSMC
  GPIO_PinAFConfig(GPIOE,GPIO_PinSource15,GPIO_AF_FSMC);		//����GPIOE��Pin15�������ӵ�FSMC
}
/**************************************************************************************************************
 * ��������LCD_GPIO_Config()
 * ����  ��void
 * ���  ��void
 * ����  ������LCD��GPIO�ܽ���FSMC����
 * ����  ���ڲ�����        
 *****************************************************************************************************************/
static void LCD_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;		                                              //����GPIO��ʼ���ṹ��
     
    /* ʹ��FSMC��AHB3ʱ�ӣ�����ע�⣬��ͬоƬ���߻᲻ͬ */
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
    
    /* ʹ��GPIOB��GPIOD��GPIOE��AHB1ʱ��*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE , ENABLE);
   	 /*����Һ�����ı�����ƹܽ�*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;											  //���ùܽ�ģʽΪ���ģʽ
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //���÷�ת�ٶ�Ϊ100Mhz��M4���ǿ찡
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;											  //�����������Ϊ�������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;												  //���ùܽ�ΪPin_5,�����Լ�Һ���ӷ�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;										  //������/��������Ϊ��������������		
    GPIO_Init(GPIOB, &GPIO_InitStructure);													  //��ʼ��GPIOB��Pin_5
    		   
    /* ���û���FSMC��Һ������������
	 * Һ���������ߣ�FSMC-D0~D15: PD 14 15 0 1,PE 7 8 9 10 11 12 13 14 15,PD 8 9 10
	 */	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;										  //���÷�ת�ٶ�Ϊ100Mhz
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;											  //���ùܽ�ģʽΪ����ģʽ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | 
                                  GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOD, &GPIO_InitStructure);													  //��ʼ��GPIOB��Pin_0��Pin_��Pin_2��
	                                                                    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                  GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                  GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure); 
    
    /* ���û���FSMC��Һ����������
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
    GPIO_AF_FSMC_Config();				             //��F407�б��������������������ã����������ӵ�FSMC
    GPIO_SetBits(GPIOB, GPIO_Pin_5);			 
    GPIO_SetBits(GPIOD, GPIO_Pin_4);		  
    GPIO_SetBits(GPIOD, GPIO_Pin_5);		 
    GPIO_SetBits(GPIOD, GPIO_Pin_7);		 
   
    GPIO_ResetBits(GPIOD, GPIO_Pin_3);
    GPIO_SetBits(GPIOD, GPIO_Pin_6); 	
}

/**************************************************************************************************************
 * ��������LCD_FSMC_Config()
 * ����  ��void
 * ���  ��void
 * ����  ������LCD��FSMC
 * ����  ���ڲ�����        
 *****************************************************************************************************************/
static void LCD_FSMC_Config(void)
{    
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  	FSMC_NORSRAMTimingInitTypeDef  p;
	  LCD_GPIO_Config();                                                     //LCD�ܽ�����
  	RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
  	p.FSMC_AddressSetupTime = 5;                                           //��ַ����ʱ��
  	p.FSMC_AddressHoldTime = 0;                                            //��ַ����ʱ��
  	p.FSMC_DataSetupTime = 9;                                              //���ݽ���ʱ��
  	p.FSMC_BusTurnAroundDuration = 0;
  	p.FSMC_CLKDivision = 0;
  	p.FSMC_DataLatency = 0;
  	p.FSMC_AccessMode = FSMC_AccessMode_A;                                  // ��ģʽA����LCD
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
 * ��������LCD_WriteReg()
 * ����  ��uint8_t LCD_Reg �Ĵ�����ַ, uint16_t LCD_RegValue �Ĵ�����ֵ
 * ���  ��void
 * ����  ��дLCD�Ĵ�������
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_WriteReg(uint8_t LCD_Reg, uint16_t LCD_RegValue)
{
  /*д�Ĵ�����ַ*/
  LCD_REG = LCD_Reg;
  /*д��Ĵ���ֵ*/
  LCD_RAM = LCD_RegValue;
}
/**************************************************************************************************************
 * ��������LCD_ReadReg()
 * ����  ��uint8_t LCD_Reg ��Ҫ��ȡ�ļĴ�����ַ
 * ���  ��uint16_t �Ĵ�����ֵ
 * ����  ����ȡLCD�Ĵ�����ֵ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
uint16_t LCD_ReadReg(uint8_t LCD_Reg)
{
  /*д�Ĵ�����ַ*/
  LCD_REG = LCD_Reg;
  /*�����Ĵ���ֵ������*/
  return (LCD_RAM);
}
/**************************************************************************************************************
 * ��������LCD_WriteRAM_Start()
 * ����  ��void
 * ���  ��void
 * ����  ����ʼдLCD��RAM
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_WriteRAM_Start(void)
{
  LCD_REG = 0x22;
}

/**************************************************************************************************************
 * ��������LCD_WriteRAM()
 * ����  ��uint16_t RGB_data  16bit��ɫ����
 * ���  ��void
 * ����  ����LCD��RAMд����
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_WriteRAM(uint16_t RGB_data)
{
  /*��LCD��RAMд����*/
  LCD_RAM = RGB_data;
}

/**************************************************************************************************************
 * ��������LCD_ReadRAM()
 * ����  ��void
 * ���  ��uint16_t  16bit��ɫֵ
 * ����  ����ȡLCD��RAM����
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
uint16_t LCD_ReadRAM(void)
{ 
	uint16_t dumry;
  LCD_REG = 0x22; /*ѡ��RAM�Ĵ���*/
	dumry=LCD_RAM;   /*�ٶ�*/
  /*��ȡ16bit RAMֵ*/
  return LCD_RAM;
}

/**************************************************************************************************************
 * ��������LCD_DisplayOn()
 * ����  ��void
 * ���  ��void
 * ����  ��LCD����ʾ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_DisplayOn(void)
{
  /*����ʾ */
  LCD_WriteReg(0x07, 0x0173); /*����LCDΪ262Kɫ������ʾ*/
}

/**************************************************************************************************************
 * ��������LCD_DisplayOff()
 * ����  ��void
 * ���  ��void
 * ����  ��LCD�ر���ʾ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_DisplayOff(void)
{
  /*�ر���ʾ*/
  LCD_WriteReg(0x07, 0x0); 
}
/**************************************************************************************************************
 * ��������LCD_Init()
 * ����  ��void
 * ���  ��void
 * ����  ��LCD��ʼ������
 * ����  ���ⲿ����        
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
 * ��������LCD_SetMode()
 * ����  ��uint8_t mode ������Ļˢ��ģʽ
 * ���  ��void
 * ����  ������LCDˢ��ģʽ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetMode(uint8_t mode)
{
	switch(mode)//����4(����)��7(����)
	{
		case 1:LCD_WriteReg(0x03,0x1000);break;//��������:���I
		case 2:LCD_WriteReg(0x03,0x1008);break;//��������:���I
		case 3:LCD_WriteReg(0x03,0x1010);break;//��������:���J
		case 4:LCD_WriteReg(0x03,0x1018);break;//��������:���J
		case 5:LCD_WriteReg(0x03,0x1020);break;//��������:���L
		case 6:LCD_WriteReg(0x03,0x1028);break;//��������:���L
		case 7:LCD_WriteReg(0x03,0x1030);break;//��������:���K
		case 8:LCD_WriteReg(0x03,0x1038);break;//��������:���K
	}
}
/**************************************************************************************************************
 * ��������LCD_Clear()
 * ����  ��void
 * ���  ��void
 * ����  ��LCD��������
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_Clear(uint16_t Color)
{
  uint32_t index = 0;
  
  LCD_SetCursor(0,150); 
  LCD_WriteRAM_Start(); /*��ʼдGRAM */
  for(index = 0; index < 76800; index++)
  {
    LCD_RAM = Color;
  }  
}

/**************************************************************************************************************
 * ��������LCD_SetCursor()
 * ����  ��uint16_t Xpos, uint16_t Ypos �趨����Ļ��Xֵ��Yֵ
 * ���  ��void
 * ����  ��LCD���ù��λ�ú���
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetCursor(uint16_t Xpos, uint16_t Ypos)
{
  LCD_WriteReg(0x20, Xpos);
  LCD_WriteReg(0x21, Ypos);
}
/**************************************************************************************************************
 * ��������LCD_SetColors()
 * ����  ��_TextColor ǰ��ɫ,_BackColor ����ɫ
 * ���  ��void
 * ����  ������LCD��ǰ��ɫ�ͱ���ɫ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetColors(__IO uint16_t _TextColor, __IO uint16_t _BackColor)
{
  TextColor = _TextColor; 
  BackColor = _BackColor;
}
/**************************************************************************************************************
 * ��������LCD_GetColors()
 * ����  ��*_TextColor ǰ��ɫ��ָ��,*_BackColor ����ɫ��ָ��
 * ���  ��void
 * ����  ����ȡLCD��ǰ��ɫ�ͱ���ɫ
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_GetColors(__IO uint16_t *_TextColor, __IO uint16_t *_BackColor)
{
  *_TextColor = TextColor; *_BackColor = BackColor;
}
/**************************************************************************************************************
 * ��������LCD_SetFont()
 * ����  ��sFONT *fonts Ҫ���õ�����
 * ���  ��void
 * ����  ������LCD������
 * ����  ���ⲿ����        
 *****************************************************************************************************************/
void LCD_SetFont(sFONT *fonts)
{
  LCD_Currentfonts = fonts;
}




