/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V2.0.1
* Date               : 06/13/2008
* Description        : Main program body.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/
 
/* Includes ------------------------------------------------------------------*/
#include "main.h" 
#include "HzLib_65k.h"
//#include "ads7843drv.h"
 
/* Private typedef -----------------------------------------------------------*/
#define RED  0xf800
#define GREEN0x07e0
#define BLUE 0x001f
#define WHITE0xffff
#define BLACK0x0000
#define YELLOW   0xFFE0
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;
 
/* Private function prototypes -----------------------------------------------*/
void GPIO_Configuration(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
void SysTick_Config(void);
/* Private functions ---------------------------------------------------------*/
 
static vu32 TimingDelay;
void Delay(u32 nTime);
 
 
/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
 
u8 count=0,flow=0;
int x,y;
unsigned char  buffer[40];
#ifdef DEBUG
  debug();
#endif
 
  /* System Clocks Configuration */
  RCC_Configuration();   
 
  /* NVIC configuration */
  NVIC_Configuration();
  GPIO_Configuration();
 
  SysTick_Config();
     
  Lcd_Configuration();
  Lcd_Initialize();
  Delay(10);
 
  TP_Init();
  while (1)
  {
 
    TP_GetAdXY(&x,&y);
//Lcd_Clear(WHITE);
sprintf(buffer,"x:=%d \n y:=%d",x,y);
Lcd_WriteString(2,2,10,10,0x0000,0xffff,buffer);
 
    /* Toggle leds connected to GPIO_LED Pin 6, Pin 7, Pin 8 and Pin 9 */
//    GPIO_Write(GPIOA, 0);
//GPIO_SetBits(GPIOA, GPIO_Pin_0);
//GPIO_ResetBits(GPIOA, GPIO_Pin_1);
GPIO_SetBits(GPIOC, GPIO_Pin_4);
GPIO_ResetBits(GPIOC, GPIO_Pin_5);
 
    /* Insert 500 ms delay */
    Delay(100);
 
    /* Toggle leds connected to GPIO_LED Pin 6, Pin 7, Pin 8 and Pin 9 */
//    GPIO_Write(GPIOA, 1);
//GPIO_ResetBits(GPIOA, GPIO_Pin_0);
//GPIO_SetBits(GPIOA, GPIO_Pin_1);
GPIO_ResetBits(GPIOC, GPIO_Pin_4);
GPIO_SetBits(GPIOC, GPIO_Pin_5);
    /* Insert 300 ms delay */
    Delay(100);
if(++count==15)
{
 
 count = 0;
if(++flow>7) flow = 1;
switch(flow)
{
 
case 1:
test_color();
//Lcd_Clear(0xf800);
break;
case 2:
    Delay(40);
        Lcd_Clear(GREEN);
        Delay(40);
        Lcd_Clear(RED);
        Delay(40);
Lcd_Clear(BLACK);
        Delay(40);
        Lcd_Clear(WHITE);
      Delay(40);
Lcd_Clear(BLUE);
        Delay(40);
        Lcd_Clear(YELLOW);
break;
case 3:
Lcd_WriteChineseString(0,0,0,0,0xffff,0x0000,"ä²Ð£à÷Ù¥ýì?ß¾?Û°??ßæ?ÛöÞÞ?ãÀÓÞÛö?îÜ?áêÛ°îÜ");  
 
break;
case 4:
Lcd_Clear(WHITE);
sprintf(buffer,"x:=%d \n y:=%d",x,y);
Lcd_WriteString(2,2,10,10,0x0000,0xffff,buffer);
break;
case 5:
Lcd_ColorBox(100,100,100,100, 0x001f);
break;
case 6:
Lcd_WriteStringClarity(2,2,0,0,0xffff,"hello"); 
break;
case 7:
   //DispPic240_320(gImage_1);
break;
 
}
 
}
   
}
 
}
 
void GPIO_Configuration(void)
{
 
GPIO_InitTypeDef GPIO_InitStructure;
GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //õÏØ¸?õó
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
 
 
}
 
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
    
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();
 
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);      //??èâÝ»??8M
 
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp(); //ÔõÓâ???ïÒ
 
  if(HSEStartUpStatus == SUCCESS)
  {
 
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
 
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
  
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);          //AHB??
   
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);   //APB2??
 
    /* PCLK1 = HCLK/2 */ 
    RCC_PCLK1Config(RCC_HCLK_Div2);  //ABP1??
 
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);  //PLLX9
 
    /* Enable PLL */
    RCC_PLLCmd(ENABLE);
 
    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
 
     
}
 
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
 
    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
 
     
}
   
}
 
   
  /* Enable peripheral clocks --------------------------------------------------*/
  /* Enable DMA clock */
 // RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
 
  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
  /* Enable ADC1 and GPIOB clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOB, ENABLE);
 
}
 
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
 
//NVIC_InitTypeDef NVIC_InitStructure;
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif
 
 
}
 
/******************************************************************************
ÞÅéÄSTîÜùÞ??ÞÅéÄsystickîÜÛ°Ûö
1¡¢?éÄSysTick_CounterCmd() ã÷ÒöSysTick??Ðï
2¡¢?éÄSysTick_ITConfig () ã÷ÒöSysTickñé? 
3¡¢?éÄSysTick_CLKSourceConfig() ?öÇSysTick??ê¹¡£
4¡¢?éÄSysTick_SetReload() ?öÇSysTickñì???¡£
5¡¢?éÄSysTick_ITConfig ()     ÞÅÒöSysTickñé?
6¡¢?éÄSysTick_CounterCmd() ??SysTick??Ðï
SysTick   ÛÕöÇùÞ?
*******************************************************************************/
 
/*******************************************************************************
* Function Name  : SysTick_Config
* Description    : Configures SysTick
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//SysTick?öÇ
void SysTick_Config(void)
{
 
    /* Disable SysTick Counter */
    SysTick_CounterCmd(SysTick_Counter_Disable);
   
    /* Disable the SysTick Interrupt */
    SysTick_ITConfig(DISABLE);
   
    /* Configure HCLK clock as SysTick clock source */
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);
    /* SysTick interrupt each 1000 Hz with HCLK equal to 72MHz */
    SysTick_SetReload(9000);
    /* Enable the SysTick Interrupt */
    SysTick_ITConfig(ENABLE);
 
/* Enable the SysTick Counter */
 //   SysTick_CounterCmd(SysTick_Counter_Enable);
 
}
 
/*******************************************************************************
* Function Name  : Delay
* Description    : Inserts a delay time.
* Input          : nTime: specifies the delay time length, in milliseconds.
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(u32 nTime)
{
 
  /* Enable the SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Enable);
   
  TimingDelay = nTime;
 
  while(TimingDelay != 0);
 
  /* Disable SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Disable);
  /* Clear SysTick Counter */
  SysTick_CounterCmd(SysTick_Counter_Clear);
 
}
 
/*******************************************************************************
* Function Name  : TimingDelay_Decrement
* Description    : Decrements the TimingDelay variable.
* Input          : None
* Output         : TimingDelay
* Return         : None
*******************************************************************************/
void TimingDelay_Decrement(void)
{
 
  if (TimingDelay != 0x00)
  {
  
    TimingDelay--;
   
}
 
}
 
#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{
  
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
 
  /* Infinite loop */
  while (1)
  {
 
   
}
 
}
#endif
 
/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/