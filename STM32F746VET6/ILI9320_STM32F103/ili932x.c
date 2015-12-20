/******************************************************************************
* ����٣?��ili932x.c
* �� 驣���ILI9320��ILI9325??IC�����QVGA?��?������16��?��??
  ��?���������� ?ح����۰����??IC?��
  ���16��?��?��������>>  BGR(565)
 
* ?�����⣺V1.3
* ����?٥��������??٥��
  1.����??ټ������ASCII�ݬ?Bug
  2.��ʥʦ��??ټ�������?��?
  3.?����?�� ����׵�?۰�������
*���?٥��
�.h�����飬#define Immediately?��ء??��?��?��
�����#define Delay��?�����?����LCD_WR_REG(0x0007,0x0173);
�����??�ƣ�?�����LCD_WR_REG(0x0007,0x0173)�������?��?
��Դء??�ơ�
#define Delay�������?��?����?�ƣ����?������??��������
?��
******************************************************************************/
#include "stm32f10x_lib.h"
#include "ili932x.h"
#include  "HzLib_65k.h"
 
/****************************************************************
��?٣��Lcd������?
���������������Lcd��?��GPIO��??
��������?��
PB8--PB15����16Bit?��??�8��
PC0--PC7 ����16Bit?��??��8��
PC8 ����Lcd_cs
PC9 ����Lcd_rs*
PC10����Lcd_wr
PC11����Lcd_rd*
PC12����Lcd_rst
PC13����Lcd_blaklight ����??��?η??����ټ?
*****************************************************************/
void Lcd_Configuration(void)
{
  
GPIO_InitTypeDef GPIO_InitStructure;
/*??��??? */
/*RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE);  
//���Lcd��������?��ظ?��
//16��?���8��
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_Init(GPIOB, &GPIO_InitStructure);
//16��?����8��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
GPIO_Init(GPIOC, &GPIO_InitStructure);
//�����
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12;
GPIO_Init(GPIOC, &GPIO_InitStructure);
//�������
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_13;  
GPIO_Init(GPIOC, &GPIO_InitStructure);*/
 
RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD, ENABLE);  
//���Lcd��������?��ظ?��
//16��?��
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7 |\
  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_Init(GPIOE, &GPIO_InitStructure);
 
//�����
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
GPIO_Init(GPIOD, &GPIO_InitStructure);
//�������
GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0;  
GPIO_Init(GPIOC, &GPIO_InitStructure);
Lcd_Light_ON;
 
}
  
void DataToWrite(u16 data) 
{
 
//u16 temp;
/*temp = GPIO_ReadOutputData(GPIOB);
GPIO_Write(GPIOB, (data<<8)|(temp&0x00ff));
temp = GPIO_ReadOutputData(GPIOC);
GPIO_Write(GPIOC, (data>>8)|(temp&0xff00));*/
 
//temp = GPIO_ReadOutputData(GPIOE);
GPIO_Write(GPIOE, data);
 
}
 
/****************************************************************************
* ٣    ?��u16 CheckController(void)
* ��    ���������������?
* ��Ϣ??����
* ��Ϣ??���������?
* ?    ٥��?�������������?���������?
* ?��۰����code=CheckController();
****************************************************************************/
u16 CheckController(void)
{
 
  u16 tmp=0,tmp1=0,tmp2=0; 
GPIO_InitTypeDef GPIO_InitStructure;
 
  DataToWrite(0xffff);//?��?����
Set_Rst;
Set_nWr;
Set_Cs;
Set_Rs;
Set_nRd;
Set_Rst;
Delay_nms(1);
Clr_Rst;
Delay_nms(1);
Set_Rst;
Delay_nms(1);
LCD_WR_REG(0x0000,0x0001);  //start oscillation
Delay_nms(1);
 
 
//16��?����8��
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7 |
  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
GPIO_Init(GPIOE, &GPIO_InitStructure);
 
   
  GPIO_ResetBits(GPIOD,GPIO_Pin_12);
   
  GPIO_SetBits(GPIOD,GPIO_Pin_14);
   
  GPIO_ResetBits(GPIOD,GPIO_Pin_15);
 
 // tmp1 = GPIO_ReadInputData(GPIOB);
//tmp2 = GPIO_ReadInputData(GPIOC);
tmp1 = GPIO_ReadInputData(GPIOE);
 
tmp = tmp1;
   
/*  GPIO_SetBits(GPIOC,GPIO_Pin_11);
   
  GPIO_SetBits(GPIOC,GPIO_Pin_8);   */
GPIO_SetBits(GPIOD,GPIO_Pin_15);
GPIO_SetBits(GPIOD,GPIO_Pin_12);
 
//16��?��
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7 |
  GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
GPIO_Init(GPIOE, &GPIO_InitStructure);
   
  return tmp;
 
}
 
/**********************************************
��?٣��Lcd�������?
�����������Lcd
��Ϣ??����
����?����
***********************************************/
void Lcd_Initialize(void)
{
 
  u16 i;
Lcd_Light_ON;
DataToWrite(0xffff);//?��?����
Set_Rst;
Set_nWr;
Set_Cs;
Set_Rs;
Set_nRd;
Set_Rst;
Delay_nms(1);
Clr_Rst;
Delay_nms(1);
Set_Rst;
Delay_nms(1); 
 
/*      
#ifdef ILI9325        
LCD_WR_REG(0x00e3,0x3008);
LCD_WR_REG(0x00e7,0x0010);
LCD_WR_REG(0x00ef,0x1231);  //Set the internal vcore voltage
LCD_WR_REG(0x0001,0x0100);//When SS = 0, the shift direction of outputs is from S1 to S720
#endif      //When SS = 1, the shift direction of outputs is from S720 to S1.
#ifdef ILI9320
LCD_WR_REG(0x00e5,0x8000);
LCD_WR_REG(0x0000,0x0001);
LCD_WR_REG(0x0001,0x0100);//S
Delay_nms(10);
#endif        
LCD_WR_REG(0x0002,0x0700);//Line inversion        
#if   ID_AM==000       
LCD_WR_REG(0x0003,0x0000);//?ح��?��� TFM=0,TRI=0,SWAP=1,16 bits system interface  swap RGB to BRG���?ORG��HWM ?0
#elif ID_AM==001        
LCD_WR_REG(0x0003,0x0008);      
#elif ID_AM==010  
LCD_WR_REG(0x0003,0x0010);        
#elif ID_AM==011
LCD_WR_REG(0x0003,0x0018);
#elif ID_AM==100  
LCD_WR_REG(0x0003,0x0020);      
#elif ID_AM==101  
LCD_WR_REG(0x0003,0x0028);      
#elif ID_AM==110  
LCD_WR_REG(0x0003,0x0030);      
#elif ID_AM==111  
LCD_WR_REG(0x0003,0x0038);
#endif      
LCD_WR_REG(0x0004,0x0000);
LCD_WR_REG(0x0008,0x0207);
LCD_WR_REG(0x0009,0x0000);
LCD_WR_REG(0x000a,0x0000);
LCD_WR_REG(0x000c,0x0001);//�?������Ϣ?�� 16�� system��Ϣ
LCD_WR_REG(0x000d,0x0000);
LCD_WR_REG(0x000f,0x0000);      
//?�����
LCD_WR_REG(0x0010,0x0000);
LCD_WR_REG(0x0011,0x0000);
LCD_WR_REG(0x0012,0x0000);
LCD_WR_REG(0x0013,0x0000);
Delay_nms(200);
LCD_WR_REG(0x0010,0x17b0);
LCD_WR_REG(0x0011,0x0137);
Delay_nms(50);
LCD_WR_REG(0x0012,0x0139);
Delay_nms(50);
LCD_WR_REG(0x0013,0x1700);
LCD_WR_REG(0x0029,0x0000);
LCD_WR_REG(0x002b,0x0000);
Delay_nms(50);      
#if   ID_AM==000         
LCD_WR_REG(0x0020,0x00ef);//GRAM����������
LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==001
LCD_WR_REG(0x0020,0x00ef);
LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==010
LCD_WR_REG(0x0020,0x0000);
LCD_WR_REG(0x0021,0x013f);      
#elif ID_AM==011
LCD_WR_REG(0x0020,0x0000);
LCD_WR_REG(0x0021,0x013f);       
#elif ID_AM==100
LCD_WR_REG(0x0020,0x00ef);
LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==101  
LCD_WR_REG(0x0020,0x00ef);
LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==110
LCD_WR_REG(0x0020,0x0000);
LCD_WR_REG(0x0021,0x0000);      
#elif ID_AM==111
LCD_WR_REG(0x0020,0x0000);
LCD_WR_REG(0x0021,0x0000);         
#endif       
LCD_WR_REG(0x0030,0x0000);
LCD_WR_REG(0x0031,0x0507);
LCD_WR_REG(0x0032,0x0104);
LCD_WR_REG(0x0035,0x0105);
LCD_WR_REG(0x0036,0x0404);
LCD_WR_REG(0x0037,0x0603);
LCD_WR_REG(0x0038,0x0004);
LCD_WR_REG(0x0039,0x0007);
LCD_WR_REG(0x003c,0x0501);
LCD_WR_REG(0x003d,0x0404);
LCD_WR_REG(0x0050,0x0000);//��� GRAM�������
LCD_WR_REG(0x0051,0x00ef);//���GRAM?�����
LCD_WR_REG(0x0052,0x0000);//����GRAM�������
LCD_WR_REG(0x0053,0x013f);//����GRAM?�����
#ifdef ILI9325        
LCD_WR_REG(0x0060,0xa700);//G
#endif
#ifdef ILI9320        
LCD_WR_REG(0x0060,0x2700);//G  
#endif    
LCD_WR_REG(0x0061,0x0001);//Enables the grayscale inversion of the image by setting REV=1.??????????????????????????????
LCD_WR_REG(0x006a,0x0000);//��������������
LCD_WR_REG(0x0080,0x0000);
LCD_WR_REG(0x0081,0x0000);
LCD_WR_REG(0x0082,0x0000);
LCD_WR_REG(0x0083,0x0000);
LCD_WR_REG(0x0084,0x0000);
LCD_WR_REG(0x0085,0x0000);
LCD_WR_REG(0x0090,0x0010);
LCD_WR_REG(0x0092,0x0000);
LCD_WR_REG(0x0093,0x0003);
LCD_WR_REG(0x0095,0x0110);
LCD_WR_REG(0x0097,0x0000);
LCD_WR_REG(0x0098,0x0000);     
//?��?��?�� 0x0130����?��?��?��
//0x0173��?��?��?��
LCD_WR_REG(0x0007,0x0130);
#ifdef Immediately
LCD_WR_REG(0x0007,0x0173);      
#endif        
//���?������?��?��������?��?�����?����󭣬��������?
  */
 
i = CheckController();
//i=0x9320;
if(i==0x9325||i==0x9328)
{
 
  LCD_WR_REG(0x00e7,0x0010);      
        LCD_WR_REG(0x0000,0x0001);  //start internal osc
        LCD_WR_REG(0x0001,0x0100);     
        LCD_WR_REG(0x0002,0x0700); //power on sequence                     
        LCD_WR_REG(0x0003,(1<<12)|(1<<5)|(1<<4) ); //65K 
        LCD_WR_REG(0x0004,0x0000);                                   
        LCD_WR_REG(0x0008,0x0207);           
        LCD_WR_REG(0x0009,0x0000);         
        LCD_WR_REG(0x000a,0x0000); //display setting         
        LCD_WR_REG(0x000c,0x0001);//display setting          
        LCD_WR_REG(0x000d,0x0000); //0f3c          
        LCD_WR_REG(0x000f,0x0000);
        LCD_WR_REG(0x0010,0x0000);   
        LCD_WR_REG(0x0011,0x0007);
        LCD_WR_REG(0x0012,0x0000);                                                                 
        LCD_WR_REG(0x0013,0x0000);                 
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0010,0x1590);   
        LCD_WR_REG(0x0011,0x0227);
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0012,0x009c);                 
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0013,0x1900);   
        LCD_WR_REG(0x0029,0x0023);
        LCD_WR_REG(0x002b,0x000e);
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0020,0x0000);                                                            
        LCD_WR_REG(0x0021,0x0000);                 
       
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0030,0x0007); 
        LCD_WR_REG(0x0031,0x0707);   
        LCD_WR_REG(0x0032,0x0006);
        LCD_WR_REG(0x0035,0x0704);
        LCD_WR_REG(0x0036,0x1f04); 
        LCD_WR_REG(0x0037,0x0004);
        LCD_WR_REG(0x0038,0x0000);        
        LCD_WR_REG(0x0039,0x0706);     
        LCD_WR_REG(0x003c,0x0701);
        LCD_WR_REG(0x003d,0x000f);
        for(i=50000;i>0;i--);
        LCD_WR_REG(0x0050,0x0000);        
        LCD_WR_REG(0x0051,0x00ef);   
        LCD_WR_REG(0x0052,0x0000);     
        LCD_WR_REG(0x0053,0x013f);
        LCD_WR_REG(0x0060,0xa700);        
        LCD_WR_REG(0x0061,0x0001); 
        LCD_WR_REG(0x006a,0x0000);
        LCD_WR_REG(0x0080,0x0000);
        LCD_WR_REG(0x0081,0x0000);
        LCD_WR_REG(0x0082,0x0000);
        LCD_WR_REG(0x0083,0x0000);
        LCD_WR_REG(0x0084,0x0000);
        LCD_WR_REG(0x0085,0x0000);
       
        LCD_WR_REG(0x0090,0x0010);     
        LCD_WR_REG(0x0092,0x0000);  
        LCD_WR_REG(0x0093,0x0003);
        LCD_WR_REG(0x0095,0x0110);
        LCD_WR_REG(0x0097,0x0000);        
        LCD_WR_REG(0x0098,0x0000);  
         //display on sequence     
        LCD_WR_REG(0x0007,0x0133);
     
        LCD_WR_REG(0x0020,0x0000);                                                            
        LCD_WR_REG(0x0021,0x0000);
 
}
else if(i==0x9320)
{
 
LCD_WR_REG(0x00,0x0000);
LCD_WR_REG(0x01,0x0100);//Driver Output Contral.
LCD_WR_REG(0x02,0x0700);//LCD Driver Waveform Contral.
LCD_WR_REG(0x03,0x1030);//Entry Mode Set.
 
LCD_WR_REG(0x04,0x0000);//Scalling Contral.
LCD_WR_REG(0x08,0x0202);//Display Contral 2.(0x0207)
LCD_WR_REG(0x09,0x0000);//Display Contral 3.(0x0000)
LCD_WR_REG(0x0a,0x0000);//Frame Cycle Contal.(0x0000)
LCD_WR_REG(0x0c,(1<<0));//Extern Display Interface Contral 1.(0x0000)
LCD_WR_REG(0x0d,0x0000);//Frame Maker Position.
LCD_WR_REG(0x0f,0x0000);//Extern Display Interface Contral 2.
 
for(i=50000;i>0;i--);
LCD_WR_REG(0x07,0x0101);//Display Contral.
for(i=50000;i>0;i--);
 
LCD_WR_REG(0x10,(1<<12)|(0<<8)|(1<<7)|(1<<6)|(0<<4));//Power Control 1.(0x16b0)
LCD_WR_REG(0x11,0x0007);//Power Control 2.(0x0001)
LCD_WR_REG(0x12,(1<<8)|(1<<4)|(0<<0));//Power Control 3.(0x0138)
LCD_WR_REG(0x13,0x0b00);//Power Control 4.
LCD_WR_REG(0x29,0x0000);//Power Control 7.
 
LCD_WR_REG(0x2b,(1<<14)|(1<<4));
 
LCD_WR_REG(0x50,0);//Set X Start.
LCD_WR_REG(0x51,239);//Set X End.
LCD_WR_REG(0x52,0);//Set Y Start.
LCD_WR_REG(0x53,319);//Set Y End.
 
LCD_WR_REG(0x60,0x2700);//Driver Output Control.
LCD_WR_REG(0x61,0x0001);//Driver Output Control.
LCD_WR_REG(0x6a,0x0000);//Vertical Srcoll Control.
 
LCD_WR_REG(0x80,0x0000);//Display Position? Partial Display 1.
LCD_WR_REG(0x81,0x0000);//RAM Address Start? Partial Display 1.
LCD_WR_REG(0x82,0x0000);//RAM Address End-Partial Display 1.
LCD_WR_REG(0x83,0x0000);//Displsy Position? Partial Display 2.
LCD_WR_REG(0x84,0x0000);//RAM Address Start? Partial Display 2.
LCD_WR_REG(0x85,0x0000);//RAM Address End? Partial Display 2.
 
LCD_WR_REG(0x90,(0<<7)|(16<<0));//Frame Cycle Contral.(0x0013)
LCD_WR_REG(0x92,0x0000);//Panel Interface Contral 2.(0x0000)
LCD_WR_REG(0x93,0x0001);//Panel Interface Contral 3.
LCD_WR_REG(0x95,0x0110);//Frame Cycle Contral.(0x0110)
LCD_WR_REG(0x97,(0<<8));//
LCD_WR_REG(0x98,0x0000);//Frame Cycle Contral.
 
 
LCD_WR_REG(0x07,0x0173);//(0x0173)
 
}
else if(i==0x7783)
{
 
// Start Initial Sequence
LCD_WR_REG(0x00FF,0x0001);
LCD_WR_REG(0x00F3,0x0008);
LCD_WR_REG(0x0001,0x0100);
LCD_WR_REG(0x0002,0x0700);
LCD_WR_REG(0x0003,0x1030);  //0x1030
LCD_WR_REG(0x0008,0x0302);
LCD_WR_REG(0x0008,0x0207);
LCD_WR_REG(0x0009,0x0000);
LCD_WR_REG(0x000A,0x0000);
LCD_WR_REG(0x0010,0x0000);  //0x0790
LCD_WR_REG(0x0011,0x0005);
LCD_WR_REG(0x0012,0x0000);
LCD_WR_REG(0x0013,0x0000);
for(i=50000;i>0;i--);
LCD_WR_REG(0x0010,0x12B0);
for(i=50000;i>0;i--);
LCD_WR_REG(0x0011,0x0007);
for(i=50000;i>0;i--);
LCD_WR_REG(0x0012,0x008B);
for(i=50000;i>0;i--);
LCD_WR_REG(0x0013,0x1700);
for(i=50000;i>0;i--);
LCD_WR_REG(0x0029,0x0022);
 
//################# void Gamma_Set(void) ####################//
LCD_WR_REG(0x0030,0x0000);
LCD_WR_REG(0x0031,0x0707);
LCD_WR_REG(0x0032,0x0505);
LCD_WR_REG(0x0035,0x0107);
LCD_WR_REG(0x0036,0x0008);
LCD_WR_REG(0x0037,0x0000);
LCD_WR_REG(0x0038,0x0202);
LCD_WR_REG(0x0039,0x0106);
LCD_WR_REG(0x003C,0x0202);
LCD_WR_REG(0x003D,0x0408);
for(i=50000;i>0;i--);
 
LCD_WR_REG(0x0050,0x0000);
LCD_WR_REG(0x0051,0x00EF);
LCD_WR_REG(0x0052,0x0000);
LCD_WR_REG(0x0053,0x013F);
LCD_WR_REG(0x0060,0xA700);
LCD_WR_REG(0x0061,0x0001);
LCD_WR_REG(0x0090,0x0033);
LCD_WR_REG(0x002B,0x000B);
LCD_WR_REG(0x0007,0x0133);
for(i=50000;i>0;i--);
 
}  
 
}
 
 
/******************************************
��?٣��Lcd?٤ֵ��?
��������Lcd�������?��?��٤ֵ��?��
��Ϣ??��Index �?�����������
          ConfigTemp ?����?����٤ֵ?
����?����
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
 
Clr_Cs;
Clr_Rs;
Set_nRd;
DataToWrite(Index);
Clr_nWr;
Set_nWr;
Set_Rs;       
DataToWrite(CongfigTemp);       
Clr_nWr;
Set_nWr;
Set_Cs;
 
}
 
 
/************************************************
��?٣��Lcd??���?
���������Lcd������� ?��?����
��Ϣ??����
����?����
************************************************/
void Lcd_WR_Start(void)
{
 
Clr_Cs;
Clr_Rs;
Set_nRd;
DataToWrite(0x0022);
Clr_nWr;
Set_nWr;
Set_Rs;
 
}
 
 
/*************************************************
��?٣��Lcd��?����������?
���������320240����߾��������???���������
��Ϣ??��x �? 0~239
          y �? 0~319
����?����
*************************************************/
void Lcd_SetCursor(u8 x,u16 y)
{
  
LCD_WR_REG(0x20,x);
LCD_WR_REG(0x21,y);    
 
}
 
 
/**********************************************
��?٣��Lcd��?�����?
������?Lcd��?��?���?��
��Ϣ??��color ���Lcd��??�� RGB(5-6-5)
����?����
***********************************************/
void Lcd_Clear(u16 Color)
{
 
u32 temp;
   
Lcd_SetCursor(0x00, 0x0000);
LCD_WR_REG(0x0050,0x00);//��� GRAM�������
LCD_WR_REG(0x0051,239);//���GRAM?�����
LCD_WR_REG(0x0052,0x00);//����GRAM�������
LCD_WR_REG(0x0053,319);//����GRAM?�����   
Lcd_WR_Start();
Set_Rs;
   
for (temp = 0; temp < 76800; temp++)
{
 
DataToWrite(Color);
Clr_nWr;
Set_nWr;
 
}
   
Set_Cs;
 
}
/**********************************************
��?٣��Lcd??��?
������?��Lcd߾�����ϻ��?�
 
���xStart�� yStart?�?ح����?���?��������ϻ��?����?��
 
��Ϣ??��xStart x۰���������
          ySrart y۰����?���
          xLong �?��ϻ����x۰��?��
          yLong  �?��ϻ����y۰��?��
����?����
***********************************************/
void Lcd_SetBox(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u16 x_offset,u16 y_offset)
{
 
   
#if ID_AM==000    
Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+yLong-1+y_offset);
 
#elif ID_AM==001
Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+yLong-1+y_offset);
      
#elif ID_AM==010
Lcd_SetCursor(xStart+x_offset,yStart+yLong-1+y_offset);
      
#elif ID_AM==011 
Lcd_SetCursor(xStart+x_offset,yStart+yLong-1+y_offset);
      
#elif ID_AM==100
Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+y_offset);     
      
#elif ID_AM==101
Lcd_SetCursor(xStart+xLong-1+x_offset,yStart+y_offset);     
      
#elif ID_AM==110
Lcd_SetCursor(xStart+x_offset,yStart+y_offset); 
      
#elif ID_AM==111
Lcd_SetCursor(xStart+x_offset,yStart+y_offset);  
      
#endif
      
LCD_WR_REG(0x0050,xStart+x_offset);//��� GRAM�������
LCD_WR_REG(0x0051,xStart+xLong-1+x_offset);//���GRAM?�����
LCD_WR_REG(0x0052,yStart+y_offset);//����GRAM�������
LCD_WR_REG(0x0053,yStart+yLong-1+y_offset);//����GRAM?����� 
 
}
 
 
void Lcd_ColorBox(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u16 Color)
{
 
u32 temp;
   
Lcd_SetBox(xStart,yStart,xLong,yLong,0,0);
Lcd_WR_Start();
Set_Rs;
   
for (temp=0; temp<xLong*yLong; temp++)
{
 
DataToWrite(Color);
Clr_nWr;
Set_nWr;
 
}
 
Set_Cs;
 
}
 
 
void Lcd_ClearCharBox(u8 x,u16 y,u16 Color)
{
 
u32 temp;
   
Lcd_SetBox(x*8,y*16,8,16,0,0); 
Lcd_WR_Start();
Set_Rs;
   
for (temp=0; temp < 128; temp++)
{
 
DataToWrite(Color); 
Clr_nWr;
//Delay_nus(22);
Set_nWr; 
 
}
 
Set_Cs;
 
}
 
 
/****************************************************************
��?٣��Lcd?1?ASCII�ݬ��?
��Ϣ??��x,?���?����������?��0~29 
          y,?���?���߾������??0~19
          CharColaor,�ݬ��?�� 
          CharBackColor,�ݬ����?�� 
         ASCIICode,��?�ݬ��ASCII?
�����?��320240��ܩ����?��?��?����?��30?ASCII�ݬ��?����?��20��
����?����
 
��򣡣�����������??�������????ʥ߾Lcd_Rs_H()��Set_Cs;?��?��ͧ?������
??�ֵ��ʣ?���??����߾��??ϣ��?٤ֵ����RS_L��?��??��?��?
����?ILI9320???RS_L??������٤ֵ
*****************************************************************/
void Lcd_WriteASCII(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode)
{
 
u8 RowCounter,BitCounter;
u8 *ASCIIPointer;
u8 ASCIIBuffer[16];
   
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
Lcd_SetBox(x*8,y*16,8,16,x_offset,y_offset);    
#else
Lcd_SetBox(x*16,y*8,16,8,x_offset,y_offset);    
#endif 
         
Lcd_WR_Start();
GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//��??�ݬ��?����?
ASCIIPointer=ASCIIBuffer;
   
for (RowCounter=0; RowCounter<16; RowCounter++)
{
  
for (BitCounter=0; BitCounter<8; BitCounter++)
{
 
if ((*ASCIIPointer&(0x80 >> BitCounter)) == 0x00)
{
 
//Set_Rs;
DataToWrite(CharBackColor);
Clr_nWr;
Set_nWr;
 
}
else
{
 
//Set_Rs;
DataToWrite(CharColor);
Clr_nWr;
Set_nWr; 
 
}
 
}
ASCIIPointer++;
 
}
// Set_Cs;
 
}
 
 
void Lcd_WriteASCIIClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u8 ASCIICode)
{
 
   
  GPIO_InitTypeDef GPIO_InitStructure;
  u8 RowCounter,BitCounter;
  u8 *ASCIIPointer;
  u8 ASCIIBuffer[16];
  u16 Temp;
         
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
  Lcd_SetBox(x*8,y*16,8,16,x_offset,y_offset);    
#else
  Lcd_SetBox(x*16,y*8,16,8,x_offset,y_offset);    
#endif
     
  Lcd_WR_Start();
  GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//��??�ݬ��?����?
  ASCIIPointer=ASCIIBuffer;
   
  for (RowCounter=0; RowCounter<16; RowCounter++)
  {
  
    for(BitCounter=0;BitCounter<8;BitCounter++)
    {
 
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
 
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}
    ASCIIPointer++;
   
}
  // Set_Cs;
   
 
}
/**************************************************************
������??��32*16��ASCII�ݬ
?��??��
x���?   x :(0~14)
y���?   y :(0~9)
x������� x_offset:(��?߾��0~239)
y������� y_offset:(��?߾��0~319)  ������������?�ޣ�?x?������16��y?����?32
                                          �����32*16�ݬ���?˭֭��?����x��y?�ڣ�
                                          ?�������ʦ�??����?���ӣ��ʦ?����?
                                          ?��Դ?�
�ݬ?�� CharColor:�ݬ?��
����?�� CharBackColor������?��
***************************************************************/
void Lcd_Write32X16ASCII(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode)
{
 
 
  u8 RowCounter,BitCounter;
  u8 *ASCIIPointer;
  u8 ASCIIBuffer[16];
   
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
  Lcd_SetBox(x*16,y*32,16,32,x_offset,y_offset);    
#else
  Lcd_SetBox(x*32,y*16,32,16,x_offset,y_offset);    
#endif
     
  Lcd_WR_Start();
  GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//��??�ݬ��?����?
  ASCIIPointer=ASCIIBuffer;
   
  for(RowCounter=0; RowCounter<16; RowCounter++)
  {
  
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}      
     
}     
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
          
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}
    ASCIIPointer++;
   
}
  // Set_Cs;
 
}
//ʦ�??������?��?ASCII�ݬ��?
void Lcd_Write32X16ASCIIWrite(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u8 ASCIICode)
{
 
        u8 RowCounter,BitCounter;
        u8 *ASCIIPointer;
         
        //����ASCII�ݬ����
#if   (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110) 
         
        Lcd_SetBox(x*16,y*32,16,32,x_offset,y_offset);
         
#else
         
        Lcd_SetBox(x*32,y*16,32,16,x_offset,y_offset);
         
#endif
         
        Lcd_WR_Start();
//        CatchASCII(ASCIICode,ASCII_Offset);//��??�ݬ��?����?
//        ASCIIPointer=CharBuffer;
        for(RowCounter=0;RowCounter<16;RowCounter++)
        {
  
          for(BitCounter=0;BitCounter<8;BitCounter++)
          {
 
            if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
            {
 
                
               Set_Rs;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;
 
             
}
            else
            {
 
               Set_Rs;
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
             
}
             
           
}
           
        for(BitCounter=0;BitCounter<8;BitCounter++)
          {
 
            if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
            {
 
                
               Set_Rs;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;
               DataToWrite(CharBackColor);
               Clr_nWr;
               Set_nWr;
 
             
}
            else
            {
 
               Set_Rs;
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
               DataToWrite(CharColor);
               Clr_nWr;
               Set_nWr; 
             
}
             
           
}
           ASCIIPointer++;
         
}
         Set_Cs;
 
}
void Lcd_Write32X16ASCIIClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u8 ASCIICode)
{
 
 
  GPIO_InitTypeDef GPIO_InitStructure;
  u8 RowCounter,BitCounter;
  u8 *ASCIIPointer;
  u8 ASCIIBuffer[16];
  u16 Temp;
 
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)
  Lcd_SetBox(x*16,y*32,16,32,x_offset,y_offset);    
#else
  Lcd_SetBox(x*32,y*16,32,16,x_offset,y_offset);    
#endif
     
  Lcd_WR_Start();
  GetASCIICode(ASCIIBuffer,ASCIICode,ASCII_Offset);//��??�ݬ��?����?
  ASCIIPointer=ASCIIBuffer;
   
  for(RowCounter=0; RowCounter<16; RowCounter++)
  {
  
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
 
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                      
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}    
     
}
           
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if((*ASCIIPointer  & (0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
               
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;       
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;          
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;     
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}            
     
}
    ASCIIPointer++;
   
}
  // Set_Cs;
 
}
/************************************************************
��?٣��Lcd?�ݬ����?
���������������?����?����?�ݬ������??�??������
��Ϣ??��x,?���?����������?��0~29 
          y,?���?���߾������??0~19
          CharColaor,�ݬ��?�� 
          CharBackColor,�ݬ����?�� 
          *s ����?���ݬ��
����?����
*************************************************************/
void Lcd_WriteString(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{
 
 
  u8 databuff;
  Set_Rs;
 
  do
  {
 
    databuff=*s++;  
    Lcd_WriteASCII(x,y,x_offset,y_offset,CharColor,CharBackColor,databuff);
     
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<29)
    {
 
      x++;
     
} 
    else if (y<19)
    {
 
      x=0;
      y++;
     
}   
    else
    {
 
      x=0;
      y=0;
     
}
        
#else
    if (y<39)
    {
 
      y++;
     
}
    else if (x<14)
    {
 
      y=0;
      x++;
     
}
    else
    {
 
      x=0;
      y=0;
     
}
#endif 
        
   
}
  while (*s!=0);
   
  Set_Cs;
   
 
}
void Lcd_WriteStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{
 
 
  u8 databuff;
  Set_Rs;
 
  do
  {
 
    databuff=*s++;  
    Lcd_WriteASCIIClarity(x,y,x_offset,y_offset,CharColor,databuff);
     
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<29)
    {
 
      x++;
     
} 
    else if (y<19)
    {
 
      x=0;
      y++;
     
}   
    else
    {
 
      x=0;
      y=0;
     
}       
#else
    if (y<39)
    {
 
      y++;
     
}
    else if (x<14)
    {
 
      y=0;
      x++;
     
}
    else
    {
 
      x=0;
      y=0;
     
}
#endif
     
   
}
  while(*s!=0);
   
  Set_Cs;
   
 
}
/****************************************
?��32X16��ASCII�ݬ��
*****************************************/
void Lcd_Write32X16String(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{
 
 
  u8 databuff;
  Set_Rs;
   
  do
  {
 
    databuff=*s++;  
    Lcd_Write32X16ASCII(x,y,x_offset,y_offset,CharColor,CharBackColor,databuff);
     
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<14)
    {
 
      x++;
     
} 
    else if (y<9)
    {
 
      x=0;
      y++;
     
}   
    else
    {
 
      x=0;
      y=0;
     
}   
#else
    if (y<6)
    {
 
      y++;
     
}
    else if (x<19)
    {
 
      y=0;
      x++;
     
}
    else
    {
 
      x=0;
      y=0;
     
}   
#endif
       
   
}
  while (*s!=0);
     
  Set_Cs;
   
 
}
void Lcd_Write32X16StringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{
 
 
  u8 databuff;
  Set_Rs;
   
  do
  {
 
    databuff=*s++;  
    Lcd_Write32X16ASCIIClarity(x,y,x_offset,y_offset,CharColor,databuff);
 
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<14)
    {
 
      x++;
     
} 
    else if (y<9)
    {
 
      x=0;
      y++;
     
}   
    else
    {
 
      x=0;
      y=0;
     
}     
#else
    if (y<6)
    {
 
      y++;
     
}
    else if (x<19)
    {
 
      y=0;
      x++;
     
}
    else
    {
 
      x=0;
      y=0;
     
}
#endif
     
   
}
  while(*s!=0);
   
  Set_Cs;
   
 
}
/****************************************************************
��?٣��Lcd?1?������?
��Ϣ??��x,?���?����������?��0~15
          y,?���?���߾������??0~19
          CharColaor,�ݬ��?�� 
          CharBackColor,�ݬ����?�� 
         ASCIICode,��?������??
�����?��320240��ܩ����?��?��?����?��15�����ݬ��?����?��20��
����?����
 
��򣡣�����������??�������????ʥ߾Lcd_Rs_H()��Set_Cs;?��?��ͧ?������
??�ֵ��ʣ?���??����߾��??ϣ��?٤ֵ����RS_L��?��??��?��?
����?ILI9320???RS_L??������٤ֵ
*****************************************************************/
void Lcd_WriteChinese(u8 x,u8 y,u16 x_offset,u16 y_offset,u16 CharColor,u16 CharBackColor,u16 ChineseCode)
{
 
 
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
         
  Lcd_SetBox(x*16,y*16,16,16,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;
         
  for(ByteCounter=0; ByteCounter<32; ByteCounter++)
  {
  
    for(BitCounter=0;BitCounter<8;BitCounter++)
    {
 
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}     
     
}
    ChinesePointer++;
   
}
  //Set_Cs;
   
 
}
void Lcd_WriteChineseClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 ChineseCode)
{
 
 
  GPIO_InitTypeDef GPIO_InitStructure;
  u16 Temp;
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
   
  Lcd_SetBox(x*16,y*16,16,16,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;
   
  for(ByteCounter=0; ByteCounter<32; ByteCounter++)
  {
  
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if((*ChinesePointer & (0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}     
     
}
  ChinesePointer++;
   
}
  //Set_Cs;
   
 
}
/****************************************************************
��?٣��Lcd?1?32X32������?
��Ϣ??��x,?���?����������?��0~7
          y,?���?���߾������??0~9
          CharColaor,�ݬ��?�� 
          CharBackColor,�ݬ����?�� 
         ASCIICode,��?������??
�����?��320240��ܩ����?��?��?����?��7�����ݬ��?����?��10��
����?����
 
��򣡣�����������??�������????ʥ߾Lcd_Rs_H()��Set_Cs;?��?��ͧ?������
??�ֵ��ʣ?���??����߾��??ϣ��?٤ֵ����RS_L��?��??��?��?
����?ILI9320???RS_L??������٤ֵ
*****************************************************************/
void Lcd_Write32X32Chinese(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,u16 ChineseCode)
{
 
         
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
   
  Lcd_SetBox(x*32,y*32,32,32,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;
   
  for(ByteCounter=0; ByteCounter<16; ByteCounter++)
  {
  
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if (((*ChinesePointer)&(0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}
    for (BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if (((*(ChinesePointer+1)) & (0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}          
    for (BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if (((*ChinesePointer) & (0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}        
    for (BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if (((*(ChinesePointer+1)) & (0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharBackColor);
        Clr_nWr;
        Set_nWr; 
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}
    ChinesePointer+=2;
   
}
  //Set_Cs;
  
}
void Lcd_Write32X32ChineseClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 ChineseCode)
{
 
         
  GPIO_InitTypeDef GPIO_InitStructure;
  u8 ByteCounter,BitCounter;
  u8 *ChinesePointer;
  u8 ChineseBuffer[32];
  u16 Temp;
 
  Lcd_SetBox(x*32,y*32,32,32,x_offset,y_offset);         
  Lcd_WR_Start();
  GetChineseCode(ChineseBuffer,ChineseCode,Chinese_Offset);
  ChinesePointer=ChineseBuffer;       
   
  for(ByteCounter=0; ByteCounter<16; ByteCounter++)
  {
  
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if (((*ChinesePointer)&(0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                      
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}
           
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if (((*(ChinesePointer+1))&(0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                      
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}
 
           
           
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if (((*ChinesePointer)&(0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                      
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}
           
    for(BitCounter=0; BitCounter<8; BitCounter++)
    {
 
      if (((*(ChinesePointer+1))&(0x80 >> BitCounter)) == 0x00)
      {
 
        //Set_Rs;
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr;
                      
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        Clr_nRd;
        Set_nRd;
                
        Temp=GPIO_ReadInputData(GPIOE);
                
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_Init(GPIOE, &GPIO_InitStructure);
                
        DataToWrite(Temp);
        Clr_nWr;
        Set_nWr; 
       
}
      else
      {
 
        //Set_Rs;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr;
        DataToWrite(CharColor);
        Clr_nWr;
        Set_nWr; 
       
}
     
}
      ChinesePointer+=2;
   
}
  //Set_Cs;
   
 
}
void Lcd_WriteChineseString(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{
 
    
  u8 databuffer;
  u16 ChineseCode;
  Set_Rs;
   
  do
  {
 
    databuffer=*s++;
    ChineseCode=databuffer<<8;
    ChineseCode=ChineseCode|*s++;
    Lcd_WriteChinese(x,y,x_offset,y_offset,CharColor,CharBackColor,ChineseCode);
     
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<14)
    {
 
      x++;
     
}
    else if (y<19)
    {
 
      x=0;
      y++;
     
}
    else
    {
 
      x=0;
      y=0;
     
}     
#else
    if (y<19)
    {
 
      y++;
     
}
    else if (x<14)
    {
 
      y=0;
      x++;
     
}
    else
    {
 
      x=0;
      y=0;
     
}  
#endif
       
   
}
  while(*s!=0);
     
  Set_Cs;
 
 
}
void Lcd_WriteChineseStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{
 
   u8 databuffer;
   u16 ChineseCode;
   Set_Rs;
    do
   {
 
       databuffer=*s++;
       ChineseCode=databuffer<<8;
       ChineseCode=ChineseCode|*s++;
       Lcd_WriteChineseClarity(x,y,x_offset,y_offset,CharColor,ChineseCode);
        
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
       if (x<14)
       {
 
         x++;
        
}
       else if (y<19)
       {
 
         x=0;
         y++;
        
}
       else
       {
 
         x=0;
         y=0;
        
}     
#else
       if (y<19)
       {
 
         y++;
        
}
       else if (x<14)
       {
 
         y=0;
         x++;
        
}
       else
       {
 
         x=0;
         y=0;
        
}  
#endif
        
    
}
     while(*s!=0);
    Set_Cs;
 
}
void Lcd_Write32X32ChineseString(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,u16 CharBackColor,char *s)
{
 
    
  u8 databuffer;
  u16 ChineseCode;
    
  Set_Rs;
  do
  {
 
    databuffer=*s++;
    ChineseCode=databuffer<<8;
    ChineseCode=ChineseCode|*s++;
    Lcd_Write32X32Chinese(x,y,x_offset,y_offset,CharColor,CharBackColor,ChineseCode);
        
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
    if (x<6)
    {
 
      x++;
     
}
    else if (y<9)
    {
 
      x=0;
      y++;
     
}
    else
    {
 
      x=0;
      y=0;
     
}     
#else
    if (y<9)
    {
 
      y++;
     
}
    else if (x<6)
    {
 
      y=0;
      x++;
     
}
    else
    {
 
      x=0;
      y=0;
     
}  
#endif
      
   
}
  while(*s!=0);
 
  Set_Cs;
 
 
}
void Lcd_Write32X32ChineseStringClarity(u8 x,u8 y,u8 x_offset,u8 y_offset,u16 CharColor,char *s)
{
 
u8 databuffer;
u16 ChineseCode;
Set_Rs;
 
do
{
 
databuffer=*s++;
ChineseCode=databuffer<<8;
ChineseCode=ChineseCode|*s++;
Lcd_Write32X32ChineseClarity(x,y,x_offset,y_offset,CharColor,ChineseCode);
     
#if (ID_AM==000)|(ID_AM==010)|(ID_AM==100)|(ID_AM==110)  
 
if (x<6)
{
 
x++;
 
}
else if (y<9)
{
 
x=0;
y++;
 
}
else
{
 
x=0;
y=0;
 
}     
#else
if (y<9)
{
 
y++;
 
}
else if (x<6)
{
 
y=0;
x++;
 
}
else
{
 
x=0;
y=0;
 
}  
#endif
     
 
}
while (*s!=0);
 
Set_Cs;
 
}
/*********************************************************
?��??��?
**********************************************************/
void LcdWritePictureFromSPI(u8 xStart,u16 yStart,u8 xLong,u16 yLong,u32 BaseAddr)
{
 
ColorTypeDef color;
u32 pixels;
 
Lcd_SetBox(xStart,yStart,xLong,yLong,0,0);
Lcd_WR_Start();
Set_Rs;
 
//SPI_FLASH_CS_LOW();
//SPI_FLASH_SendByte(0x0B);//FSTREAD
//SPI_FLASH_SendByte((BaseAddr & 0xFF0000) >> 16);
//SPI_FLASH_SendByte((BaseAddr& 0xFF00) >> 8);
//SPI_FLASH_SendByte(BaseAddr & 0xFF);
//SPI_FLASH_SendByte(0);//Dummy_Byte
 
SPI2->DR = 0;//Dummy_Byte
while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (u16) RESET);
color.U8[1] = SPI2->DR;
 
SPI2->DR = 0;//Dummy_Byte
 
for (pixels=0; pixels<(xLong*yLong); pixels++)
{
 
 
while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (u16) RESET);
color.U8[0] = SPI2->DR;
 
SPI2->DR = 0;//Dummy_Byte
 
DataToWrite(color.U16);
Clr_nWr;
Set_nWr;
 
while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (u16) RESET);
color.U8[1] = SPI2->DR;
 
SPI2->DR = 0;//Dummy_Byte
 
}
 
//SPI_FLASH_CS_HIGH();
Set_Cs;  
 
}
/*********************************************************
��?٣��SPI��ASCII?�����
?��??��u8 ASCII ?����ASCII?����'A'
          BaseAddr ��� ?ASCII?����?�FLASH����?������
����?����
?٥��?����?ASCII?������?�SPI FLASH����16Byte?����?
??����ۯ����?16byte��ASCII?��??CharBuffer[]��
**********************************************************/
void GetASCIICode(u8* pBuffer,u8 ASCII,u32 BaseAddr)
{
  u8 i;
   for(i=0;i<16;i++)
    *(pBuffer+i)=*(AsciiLib + (ASCII - 32)*16 + i);//    OffSet = (*pAscii - 32)*16;
 
 // SPI_FLASH_BufferRead(pBuffer,BaseAddr+16*ASCII,16);
 
 
}
/*********************************************************
��?٣��SPI����?��?�����
?��??��u16 ASCII ?������������"�"
          BaseAddr ��� ??����?�FLASH�����������
����?����
?٥��?����?����������?�SPI FLASH����32Byte?����?
??����ۯ����?32byte��?��??ChineseBuffer[]
**********************************************************/
void GetChineseCode(u8* pBuffer,u16 ChineseCode,u32 BaseAddr)
{
 
   
  u8 High8bit,Low8bit,i;
  u16 temp;
   
 // temp=ChineseCode-0xA0A0;//ߩ��?�?��?
  temp=ChineseCode;
  High8bit=(temp>>8);
  Low8bit=(temp&0x00FF);
   
 // for(i=0;i<32;i++)
 //   *(pBuffer+i)= *(HzLib + 32*((High8bit-0xb0)*94+Low8bit-0xa1) + i);
 // SPI_FLASH_BufferRead(pBuffer,BaseAddr+32*((High8bit-1)*94+Low8bit-1),32);
 
 
}
void Get320240PictureCode(u8* pBuffer,u32 BufferCounter,u32 BaseAddr)
{
 
 //   SPI_FLASH_BufferRead(pBuffer,BaseAddr+BufferCounter*32,32);
 
}
 
void Delay_nms(int n)
{
 
   
  u32 f=n,k;
  for (; f!=0; f--)
  {
 
    for(k=0xFFF; k!=0; k--);
   
}
   
 
}
void DrawPixel(u8 x, u8 y, int Color)
{
 
Lcd_SetCursor(x,y);
    Lcd_WR_Start(); 
Set_Rs;
    DataToWrite(Color);
Clr_nWr;
Set_nWr;
Set_Cs;
 
}
void DispPic240_320(const unsigned char *str)
{
 
 
u32 temp;
ColorTypeDef color;
Lcd_SetCursor(0x00, 0x0000);
LCD_WR_REG(0x0050,0x00);//��� GRAM�������
LCD_WR_REG(0x0051,239);//���GRAM?�����
LCD_WR_REG(0x0052,0);//����GRAM�������
LCD_WR_REG(0x0053,319);//����GRAM?�����   
Lcd_WR_Start();
Set_Rs;
   
for (temp = 0; temp < 240*320; temp++)
{
   
color.U8[1] =*(unsigned short *)(&str[ 2 * temp]);
color.U8[0]=*(unsigned short *)(&str[ 2 * temp+1]);
//DataToWrite(i);
 
DataToWrite(color.U16);
Clr_nWr;
Set_nWr;
 
}
 
//==============================  
 
}
void test_color(){
 
  u8  R_data,G_data,B_data,i,j;
 
Lcd_SetCursor(0x00, 0x0000);
LCD_WR_REG(0x0050,0x00);//��� GRAM�������
LCD_WR_REG(0x0051,239);//���GRAM?�����
LCD_WR_REG(0x0052,0);//����GRAM�������
LCD_WR_REG(0x0053,319);//����GRAM?�����   
Lcd_WR_Start();
Set_Rs;
    R_data=0;G_data=0;B_data=0;     
    for(j=0;j<50;j++)//?��?˭?
    {
 
        for(i=0;i<240;i++)
            {
R_data=i/8;DataToWrite(R_data<<11|G_data<<5|B_data);
Clr_nWr;Set_nWr;
}
     
}
    R_data=0x1f;G_data=0x3f;B_data=0x1f;
    for(j=0;j<50;j++)
    {
 
        for(i=0;i<240;i++)
            {
 
            G_data=0x3f-(i/4);
            B_data=0x1f-(i/8);
            DataToWrite(R_data<<11|G_data<<5|B_data);
Clr_nWr;
Set_nWr;
 
}
     
}
//----------------------------------
    R_data=0;G_data=0;B_data=0;
    for(j=0;j<50;j++)//?��?˭?
    {
 
        for(i=0;i<240;i++)
            {
G_data=i/4;
DataToWrite(R_data<<11|G_data<<5|B_data);
Clr_nWr;
Set_nWr;
}
     
}
 
    R_data=0x1f;G_data=0x3f;B_data=0x1f;
    for(j=0;j<50;j++)
    {
 
        for(i=0;i<240;i++)
            {
 
            R_data=0x1f-(i/8);
            B_data=0x1f-(i/8);
            DataToWrite(R_data<<11|G_data<<5|B_data);
Clr_nWr;
Set_nWr;
 
}
     
}
//----------------------------------
  
    R_data=0;G_data=0;B_data=0;
    for(j=0;j<60;j++)//?��?˭?
    {
 
        for(i=0;i<240;i++)
            {
B_data=i/8;DataToWrite(R_data<<11|G_data<<5|B_data);
Clr_nWr;
Set_nWr;
}
     
} 
 
    B_data=0; 
    R_data=0x1f;G_data=0x3f;B_data=0x1f;
 
    for(j=0;j<60;j++)
    {
 
        for(i=0;i<240;i++)
            {
 
            G_data=0x3f-(i/4);
            R_data=0x1f-(i/8);
            DataToWrite(R_data<<11|G_data<<5|B_data);
Clr_nWr;
Set_nWr;
 
}
     
}  
Set_Cs;
 
}
//====================================================================================
/*
void DrawSingleAscii(u16 x, u16 y, u8 *pAscii, u16 LineColor,u16 FillColor, u8 Mod)
{
 
    u8 i, j;
    u8 str;
    u16 OffSet;
 
Lcd_SetCursor(0x00, 0x0000);
LCD_WR_REG(0x0050,0x00);//��� GRAM�������
LCD_WR_REG(0x0051,239);//���GRAM?�����
LCD_WR_REG(0x0052,0);//����GRAM�������
LCD_WR_REG(0x0053,319);//����GRAM?����� 
Lcd_WR_Start();
Set_Rs;
 
    OffSet = (*pAscii - 32)*16;
    for (i=0;i<16;i++)
    {
 
        Lcd_SetCursor(x,y+i);
        Lcd_WR_Start();
Set_Rs;
 
        str = *(AsciiLib + OffSet + i);  
        for (j=0;j<8;j++)
        {
 
            if ( str & (0x80>>j) )     //0x80>>j
            {
 
                DataToWrite((u16)(LineColor&0xffff));
Clr_nWr;
Set_nWr;
             
}
            else
            {
 
                if (NORMAL == Mod) 
                    DataToWrite((u16)(FillColor&0xffff));
Clr_nWr;
Set_nWr;
                else
                {
 
                    Lcd_SetCursor(x+j+1,y+i);
                    Lcd_WR_Start();
Set_Rs; 
                 
}
             
}               
         
} 
     
}
 
}
*/