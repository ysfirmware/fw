/* ============================================================================ */
/*	     FFT.c : 고속푸리에변환(Fast Fourier transform) 프로그램            */
/* ============================================================================ */
/*					 programmed by Kyung-Hwan Kim in 2016.  */

#include "stm32f746xx.h"
#include "OK-STM746.h"
#include "arm_math.h"

void SysTick_Handler(void);			
void TIM1_UP_TIM10_IRQHandler(void);	        
void ADC_DAC_graph_axis(void);
void ADC_DAC_graph_scale(void);
void FFT_graph_axis(void);
void FFT_graph_scale(void);
void FFT_bar_graph(unsigned short x1, unsigned short y1, unsigned short Max_Height, unsigned short Max_Value, float Value, unsigned short foreground, unsigned short background);
void Graphic_clear_top(void);
void Graphic_clear_bottom(void);

arm_cfft_radix4_instance_f32 S;    
float maxValue;              
unsigned int  maxIndex;
float Input[512];
float Output[256];
volatile float Input_buffer[512];
volatile float ADC_graph_data[600], DAC_graph_data[600];
volatile unsigned short DAC_data[200] = {0,}, graph_count = 0;
volatile unsigned short ADC_graph_flag = 0, ADC_flag = 0, DAC_flag = 0;
unsigned short ADC_count = 0, DAC_count = 0, data_count = 0, a = 0;	
unsigned short y1, y10 = 0, y2, y20 = 0, AD_DA_mode = 0, FFT_mode = 0;	
         
void SysTick_Handler(void)			
{
  DAC->DHR12R1 = DAC_data[data_count];		
  DAC_graph_data[ADC_count] = DAC_data[data_count];
  if(FFT_mode == 0)
  {
    ADC1->CR2 |= 0x40000000;		       
    while(!(ADC1->SR & 0x00000002));	       
    ADC_graph_flag = 1;
    ADC_graph_data[ADC_count] = ADC1->DR;
    ADC_count++;
    graph_count++;
    if(ADC_count >= 599)
      ADC_count = 0;
    if(graph_count >= 18000)
    { graph_count = 0;
      ADC_flag = 1;
    } 
  }
  data_count++;
  if(data_count >= 200)
    data_count = 0;  
}

void TIM1_UP_TIM10_IRQHandler(void)	       
{
  TIM1->SR = 0x00000000;			
  
  if(FFT_mode == 1)
  { ADC1->CR2 |= 0x40000000;		       
    while(!(ADC1->SR & 0x00000002));	       
    Input_buffer[a] = ((float)ADC1->DR - 2048.)/2048.;
    Input_buffer[a+1] = 0.;
    a+=2;
    if(a > 511)
      a = 0;
  }
}

int main(void)
{
  unsigned short x, i, j;
  unsigned char key;      

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"     FFT.c      ");
  TFT_color_screen(White);
  ADC_DAC_graph_scale();
  ADC_DAC_graph_axis();

  GPIOA->MODER |= 0x00000300;			// PA4 = DAC_OUT1
  RCC->APB1ENR |= 0x20000000;			// enable DAC clock
  DAC->CR = 0x00000001;				// DAC1 enable, output buffer and trigger disable
  
  GPIOA->MODER |= 0x00003000;			// use ADC6

  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000100;			// enable ADC1 clock

  ADC1->SMPR2 = 0x00080000;			// sampling time of channel 6 = 28 cycle
  ADC->CCR = 0x00000000;			// ADCCLK = 54MHz/2 = 27MHz
  ADC1->CR1 = 0x00000800;			// 12-bit resolution, single discontinuous mode
  ADC1->CR2 = 0x00000001;			// right alignment, ADON = 1
  ADC1->SQR3 = 0x00000006;			// channel 6
  
  SysTick->LOAD  = 134;				// 27MHz/(134+1) = 200kHz
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;	                // 216MHz/8 = 27MHz, SysTick enable and interrupt disable
   
  RCC->APB2ENR |= 0x00000001;			// enable TIM1 clock
  RCC->DCKCFGR1 |= 0x01000000;                  // TIM1CLK = HCLK
  TIM1->PSC = 39;				// 216MHz/(199+1) = 1080kHz
  TIM1->ARR = 26;				// 1080kHz / (41+1) = 27.14kHz
  TIM1->CNT = 0;				// clear counter
  TIM1->DIER = 0x0001;				// enable update interrupt
  TIM1->CR1 = 0x0005;				// overflow interrupt, enable TIM1

  NVIC->ISER[0] |= 0x02000000;			// enable (25)TIM1 interrupt
  
  arm_cfft_radix4_init_f32(&S, 256, 0, 1);

  while(1)
    { key = Key_input();                       
      switch(key)
        { case KEY1 : TFT_color_screen(White); 
                       ADC_DAC_graph_scale();
                       ADC_DAC_graph_axis();
                       TIM1->PSC = 39;				
                       TIM1->ARR = 26;
                       for(x = 0; x < 200; x++)			
                         DAC_data[x] = (signed short)(2047. * sin(x * 1.8 * 3.141592 / 180.)) + 2047;
                       FFT_mode = 0;
                       AD_DA_mode = 1;
                      break;
          case KEY2 : TFT_color_screen(White);
                       ADC_DAC_graph_scale();
                       ADC_DAC_graph_axis();
                       TIM1->PSC = 39;				
                       TIM1->ARR = 26;
                       for(i = 0; i <= 199; i++)
                       { if(i <= 100)
                           DAC_data[i] = 4095;
                         else
                           DAC_data[i] = 0;
                       }
                       FFT_mode = 0;
                       AD_DA_mode = 2;
                      break;
          case KEY3 : TFT_color_screen(White);
                       ADC_DAC_graph_scale();
                       ADC_DAC_graph_axis();
                       TIM1->PSC = 39;				
                       TIM1->ARR = 26;
                       i = 49;
                       for(x = 0; x < 200; x++)			
                       { if(x <= 50)
                         { i++;
                           DAC_data[x] = (unsigned short)(4095./100.*(float)i);
                         }
                         else if((x > 50) && x < 151)
                         { i--;
                           DAC_data[x] = (unsigned short)(4095./100.*(float)i);
                         }
                         else if(x >= 151)
                         { i++;
                           DAC_data[x] = (unsigned short)(4095./100.*(float)i);
                         }
                       }
                       FFT_mode = 0;
                       AD_DA_mode = 3; 
                      break;
          case KEY4 : TIM1->PSC = 0;				
                       TIM1->ARR = 8436;
                       TFT_color_screen(White);
                       FFT_graph_axis();
                       FFT_graph_scale();
                       FFT_mode = 1;
                      break;
          default:    break;
        }
      if(FFT_mode == 1)
      { for (j = 0; j < 512; j++)
          Input[j] = Input_buffer[j];
        arm_cfft_radix4_f32(&S, Input);
        arm_cmplx_mag_f32(Input, Output, 256);
        arm_max_f32(Output, 256, &maxValue, &maxIndex);
        FFT_graph_axis();
        for (j = 1; j < 129; j++)
          FFT_bar_graph(30 +  2*j, 219, 199, (unsigned short)maxValue, Output[j], Red, White);
      }
      if(FFT_mode == 0)
      { if(ADC_flag == 1)
        { ADC_flag = 0;
          Graphic_clear_top();
          Graphic_clear_bottom();
          ADC_DAC_graph_scale();
          ADC_DAC_graph_axis();
          for(j = 0; j <= 599; j++)
          { y2 = (unsigned short)(120. - DAC_graph_data[j]*90./4095. + 0.5);
            if(j == 0) y20 = y2;
            Line((unsigned short)(20.+(float)j/2.),y20, (unsigned short)(20.+(float)j/2.+1.),y2, Blue);
            y20 = y2;
            
            y1 = (unsigned short)(220. - ADC_graph_data[j]*90./4095. + 0.5);
            if(j == 0) y10 = y1;
            Line((unsigned short)(20.+(float)j/2.),y10, (unsigned short)(20+(float)j/2.+1.),y1, Blue);
            y10 = y1;
          }
        }
      }
    }
}

void ADC_DAC_graph_axis(void)
{
  unsigned int x, y;

  Rectangle(0,17, 319,239, White);		
  
  Line(20,220, 319,220, Black);			
  Line(314,215, 319,220, Black);			
  Line(314,225, 319,220, Black);			
  Line(20,130, 20,220, Black);			
  Line(15,135, 20,130, Black);			
  Line(25,135, 20,130, Black);			
  
  Line(20,120, 319,120, Black);			
  Line(314,115, 319,120, Black);			
  Line(314,125, 319,120, Black);			
  Line(20,30, 20,120, Black);			
  Line(25,35, 20,30, Black);			
  Line(15,35, 20,30, Black);			
  
  for(x = 70; x <= 320; x += 50)		
    { Line(x,218, x,222, Black);
      Line(x,118, x,122, Black);

      for(y = 35; y <= 110; y += 5)		
        TFT_pixel(x,y, Black);
      for(y = 135; y <= 220; y += 5)		
        TFT_pixel(x,y, Black);
    }

  for(y = 45; y <= 95; y += 25)		        
    { Line(18,y, 22,y, Black);

      for(x = 20; x <= 315; x += 5)		
        TFT_pixel(x,y, Black);
    }
  for(y = 145; y <= 195; y += 25)		
    { Line(18,y, 22,y, Black);

      for(x = 20; x < 320; x += 5)		
        TFT_pixel(x,y, Black);
    }
}

void ADC_DAC_graph_scale(void)
{
  if(AD_DA_mode == 1)
    TFT_string(6,0,White,Magenta,"           정현파           ");
  else if(AD_DA_mode == 2)
    TFT_string(6,0,White,Magenta,"           구형파           ");
  else if(AD_DA_mode == 3)
    TFT_string(6,0,White,Magenta,"           삼각파           ");
  TFT_color(Red,Transparent);
  TFT_English_pixel(2, 188, '1');		
  TFT_English_pixel(10,188, '.');
  TFT_English_pixel(2, 163, '2');		
  TFT_English_pixel(10,163, '.');
  TFT_English_pixel(2, 138, '3');		
  TFT_English_pixel(10,138, '.');
  TFT_English_pixel(5,213,  '0');
  TFT_English_pixel(2, 88,  '1');		
  TFT_English_pixel(10,88,  '.');
  TFT_English_pixel(2, 63,  '2');		
  TFT_English_pixel(10,63,  '.');
  TFT_English_pixel(2, 38,  '3');		
  TFT_English_pixel(10,38,  '.');
  TFT_English_pixel(5,113,  '0');
  		
  TFT_English_pixel(58, 222, '0');		
  TFT_English_pixel(66, 222, '.');		
  TFT_English_pixel(74, 222, '5');		
  TFT_English_pixel(107, 222, '1');		
  TFT_English_pixel(115,222, '.');		
  TFT_English_pixel(123,222, '0');		
  TFT_English_pixel(158,222, '1');		
  TFT_English_pixel(166,222, '.');		
  TFT_English_pixel(174,222, '5');		
  TFT_English_pixel(207,222, '2');		
  TFT_English_pixel(215,222, '.');		
  TFT_English_pixel(223,222, '0');				
  TFT_English_pixel(258,222, '2');		
  TFT_English_pixel(266,222, '.');		
  TFT_English_pixel(274,222, '5');		
  
  TFT_color(Black,Transparent);
  TFT_English_pixel(2,  18, '[');            
  TFT_English_pixel(10, 18, 'V');
  TFT_English_pixel(18, 18, ']');
  
  TFT_English_pixel(285,223, '[');		
  TFT_English_pixel(293,223, 'm');		
  TFT_English_pixel(301,223, 's');		
  TFT_English_pixel(309,223, ']');
}

void FFT_graph_axis(void)
{
  unsigned int x, y;

  Rectangle(0,17, 319,239, White);		
  
  Line(30, 220, 320, 220, Black);		
  Line(314,215, 319, 220, Black);		
  Line(314,225, 319, 220, Black);		
  Line(30,  20,  30, 220, Black);		
  Line(35,25, 30,20, Black);			
  Line(25,25, 30,20, Black);			

  for(x = 50; x <= 320; x += 20)		
    Line(x,218, x,222, Black);
  for(y = 40; y <= 200; y += 20)		  
    { Line(28,y, 32,y, Black);

      for(x = 35; x <= 315; x += 10)	
        TFT_pixel(x,y, Black);
    }
}

void FFT_graph_scale(void)		
{ if(AD_DA_mode == 1)
    TFT_string(6,0,White,Magenta,"      정현파의 FFT 결과     ");
  else if(AD_DA_mode == 2)
    TFT_string(6,0,White,Magenta,"      구형파의 FFT 결과     ");
  else if(AD_DA_mode == 3)
    TFT_string(6,0,White,Magenta,"      삼각파의 FFT 결과     ");
  TFT_color(Red,Transparent);
  TFT_English_pixel(6,193, '1');
  TFT_English_pixel(14,193, '0');
  TFT_English_pixel(6,173, '2');
  TFT_English_pixel(14,173, '0');
  TFT_English_pixel(6,153, '3');
  TFT_English_pixel(14,153, '0');
  TFT_English_pixel(6,132, '4');
  TFT_English_pixel(14,132, '0');
  TFT_English_pixel(6,113, '5');
  TFT_English_pixel(14,113, '0');
  TFT_English_pixel(6, 93, '6');
  TFT_English_pixel(14, 93, '0');
  TFT_English_pixel(6, 73, '7');
  TFT_English_pixel(14, 73, '0');
  TFT_English_pixel(6, 53, '8');
  TFT_English_pixel(14, 53, '0');
  TFT_English_pixel(6, 33, '9');
  TFT_English_pixel(14, 33, '0');
  TFT_English_pixel(10,213,  '0');
  		
  TFT_English_pixel(46, 222, '1');			
  TFT_English_pixel(66,222, '2');				
  TFT_English_pixel(86,222, '3');				
  TFT_English_pixel(106,222, '4');					
  TFT_English_pixel(126,222, '5');
  TFT_English_pixel(146,222, '6');
  TFT_English_pixel(166,222, '7');
  TFT_English_pixel(186,222, '8');
  TFT_English_pixel(206,222, '9');
  TFT_English_pixel(222,222, '1');
  TFT_English_pixel(230,222, '0');
  TFT_English_pixel(242,222, '1');
  TFT_English_pixel(250,222, '1');
  TFT_English_pixel(262,222, '1');
  TFT_English_pixel(270,222, '2');
  
  TFT_color(Black,Transparent);
  TFT_English_pixel(2,  18, '[');              
  TFT_English_pixel(10, 18, '%');
  TFT_English_pixel(18, 18, ']');
  
  TFT_English_pixel(277,223, '[');		
  TFT_English_pixel(285,223, 'k');		
  TFT_English_pixel(293,223, 'H');		
  TFT_English_pixel(301,223, 'z');		
  TFT_English_pixel(309,223, ']');
}

void FFT_bar_graph(unsigned short x1, unsigned short y1, unsigned short Max_Height, unsigned short Max_Value, float Value, unsigned short foreground, unsigned short background)
{ unsigned short Height;
  Height = (unsigned short)(Value / (float)Max_Value * (float)Max_Height);
  if(Height == Max_Height)
    Line(x1, y1, x1, y1 - Height, foreground);
  else if (Height < Max_Height)
  { Line(x1, y1, x1, y1 - Height, foreground);
    Line(x1, y1 - Height, x1, y1 - Max_Height, background);
  }
}

void Graphic_clear_top(void)			
{ 
  unsigned int x, y;

  for(y = 120; y <= 221; y++)
    { TFT_GRAM_address(19,y);
      for(x = 19; x <= 319; x++)
        TFT_data(White);
    }
}
void Graphic_clear_bottom(void)		
{
  unsigned int x, y;

  for(y = 20; y <= 119; y++)
    { TFT_GRAM_address(19,y);
      for(x = 19; x <= 319; x++)
        TFT_data(White);
    }
}
