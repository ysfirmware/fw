/* ============================================================================ */
/*              Long_Homework.c : FFT 분석기(FFT analyzer)                      */
/* ============================================================================ */
/*					programmed by Ho-jun Lee in 2015.       */

#include "stm32f746xx.h"
#include "OK-STM746.h"
#include <arm_math.h>

void SysTick_Handler(void);			/* SysTick interrupt function */
void Draw_dualaxis(void);                      /* Dual waveform */
void Draw_singleaxis(void);                    /* FFT analysis */
void Single_graphic_scale(void);               /* scale drawing */
void Dual_graphic_scale(void);                 /* scale drawing */
void Graphic_clear(void);                      /* clear graphic*/
void DrawBar(unsigned short bottomX, unsigned short bottomY, unsigned short maxHeight, unsigned short maxValue, float value, unsigned short foreground, unsigned short background); /* Drawing bar*/

unsigned int tri_count = 0, sqaure_data=3722, sin_count, ADC_count;
unsigned int sqaure_cnt,clear_cnt;
volatile unsigned int Display_flag;
volatile unsigned int ADC_array[800], triangle_data[200]={0,}, sin_data[200]={0,},DAC_data[200]={0,};
volatile unsigned short ADC_step = 0;	
unsigned short y1, y1o = 0, y2, y2o = 0;	// graphic variable
float j = 0.000005;

unsigned int mode=0,FFT_mode=0;
unsigned short FFT_flag = 0, FFT_data = 0, KEY4_flag = 0;
float Input[512];
float Output[256];
float Input_tmp[512];
arm_cfft_radix4_instance_f32 S;    
float maxValue;
unsigned int maxIndex;

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */
void SysTick_Handler(void)			/* SysTick interrupt function */
{
  if(Display_flag == 0)
  {  clear_cnt++;
     if(clear_cnt>= 50000)
     { clear_cnt = 0;
       Display_flag = 1;
     }
  }
  if(mode == 1)
  {
    DAC->DHR12R1 = sin_data[sin_count];	        // output D/A data
    DAC_data[sin_count] = DAC->DHR12R1;
    ADC1->CR2 |= 0x00400000;                    // start convertion of regular group
    while(!(ADC1->SR & 0x00000004));	        // wait A/D conversion complete
    ADC_array[ADC_count] = ADC1->JDR1;
    sin_count++;
    ADC_count++;
    if(sin_count >= 200) sin_count = 0;
    if(ADC_count >= 800)
      ADC_count = 0;
    
  }
  else if(mode == 2)
  {
    DAC->DHR12R1 = sqaure_data;	                // output D/A data
    DAC_data[sqaure_cnt] = DAC->DHR12R1;
    ADC1->CR2 |= 0x00400000;                    // start convertion of regular group
    while(!(ADC1->SR & 0x00000004));	        // wait A/D conversion complete
    ADC_array[ADC_count] = ADC1->JDR1;
    sqaure_cnt++;
    ADC_count++;
    if((sqaure_cnt % 100) == 0)  sqaure_data = 0;
    if((sqaure_cnt % 200) == 0)
    {
      sqaure_data = 3722;
      sqaure_cnt = 0;
    } 
    if(ADC_count >= 800)
      ADC_count = 0;
    
    
  }
  else if(mode == 3)
  {
    DAC->DHR12R1 = triangle_data[tri_count];	// output D/A data
    DAC_data[tri_count] = DAC->DHR12R1;
    ADC1->CR2 |= 0x00400000;                    // start convertion of regular group
    while(!(ADC1->SR & 0x00000004));	        // wait A/D conversion complete
    ADC_array[ADC_count] = ADC1->JDR1;
    tri_count++;
    ADC_count++;
    if(tri_count >= 200) tri_count = 0;
    if(ADC_count >= 800)
      ADC_count = 0;
    
    
  }
}

void TIM6_DAC_IRQHandler(void)			/* TIM6 interrupt function */
{
  TIM6->SR = 0x0000;				// clear pending bit of TIM6 interrupt
  
  Input_tmp[FFT_data] = (float)((ADC1->JDR1 - 1861.)/1861.);
  Input_tmp[FFT_data+1] = 0.;
  FFT_data+=2;
  if(FFT_data > 511)
  {
    FFT_data = 0;
    FFT_flag = 1;
  }
}

int main(void)
{
  unsigned int key;
  unsigned int i, x, y;
  float a;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0," Long_Homework.c");
  Beep();
  Dual_graphic_scale();
  Draw_dualaxis();
  GPIOA->MODER |= 0x00003000;			// ADC6
  GPIOA->OSPEEDR &= 0xFFFFCFFF;			// 100MHz fast speed (MCO1 = 180MHz high speed)
  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000100;			// enable ADC1 clock

  ADC1->SMPR1 = 0x00000000;			// sampling time of channel 16~18 = 480 cycle
  ADC1->SMPR2 = 0x00040000;			// sampling time of channel 0~3 = 28 cycle, channel 6 = 15 cycle
  ADC1->JSQR = 0x00030000;                      // JL[1:0] = 0 : single conversion, cannel 6
  
  ADC->CCR = 0x00800000;			// TSVREFE = 1, VBATE = 0, ADCCLK = 54MHz/2 = 27MHz
  ADC1->CR1 = 0x00001000;                       // Discontinuous mode on Injected channels
  ADC1->CR2 = 0x00000001;                       // ADC ON
  
  a = 3722./100.;			        // calculate data of triangular wave
  for(x = 50; x < 100; x++)
  { y = (unsigned short)(a * x);
    triangle_data[x-50] = y;
  }
  i = 51;
  for(x = 100; x < 150; x++)
  { triangle_data[x-50] = triangle_data[100-i];
    i++;
  }
  for(x = 0; x < 50; x++)
  { y = (unsigned short)(a * x);
    triangle_data[x+150] = y;
  }
  i = 1;
  for(x = 150; x < 200; x++)
  { triangle_data[x-50] = triangle_data[200-i];
    i++;
  }
  for(x = 0; x < 200; x++)		        // calculate data of sinusoidal wave
  { y = (signed short)(1861. * arm_sin_f32(x * 1.8 * 3.141592 / 180.)) + 1861;
    sin_data[x] = y;
  }
  
  RCC->APB1ENR |= 0x00000010;			// enable TIM6 clock
  RCC->DCKCFGR1 |= 0x01000000;                  // TIM6CLK = HCLK
  TIM6->PSC = 0;				// 216MHz/(0+1) = 216MHz
  TIM6->ARR = 8436;				// 216MHz / (8436+1) = 25.6kHz
  TIM6->CNT = 0;				// clear counter
  TIM6->DIER = 0x0001;				// enable update interrupt
  TIM6->CR1 = 0x0085;				// use buffer of TIM6_ARR, enable TIM6

  NVIC->ISER[1] |= 0x00400000;			// enable (54)TIM6 interrupt
  
  GPIOA->MODER |= 0x00000F00;			// PA4 = DAC_OUT1
  RCC->APB1ENR |= 0x20000000;			// enable DAC clock

  DAC->CR = 0x00010001;				// DAC enable, output buffer and trigger disable
  
  SysTick->LOAD  = 134;			        // 27MHz/(134+1) = 200kHz
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 216MHz/8 = 27MHz, SysTick enable and interrupt disable
  
  while(1)
  { 
    key = Key_input();			        // key input
    switch(key)
    { case KEY1 : mode = 1;                     // sine wave mode
                   TFT_clear_screen();
                   Dual_graphic_scale();
                   Draw_dualaxis();
                   ADC_count = 0;
                   sin_count = 0;
                   Display_flag = 0;
                   FFT_mode = 0;
                   KEY4_flag = 0;               // waveform change flag
                   break;
      case KEY2 : mode = 2;                     // sqaure wave mode
                   TFT_clear_screen();
                   Dual_graphic_scale();
                   Draw_dualaxis();
                   ADC_count = 0;
                   sqaure_cnt = 0;
                   Display_flag = 0;
                   FFT_mode = 0;
                   KEY4_flag = 0;               // waveform change flag
                   break;             
      case KEY3 : mode = 3;
                   TFT_clear_screen();
                   Dual_graphic_scale();
                   Draw_dualaxis();
                   ADC_count = 0;
                   tri_count = 0;
                   Display_flag = 0;
                   FFT_mode = 0;
                   KEY4_flag = 0;               // waveform change flag
                   break;
      case KEY4 : if(KEY4_flag == 1)
                   {
                     if(mode == 1)      mode = 2;
                     else if(mode == 2) mode = 3;
                     else if(mode == 3) mode = 1;
                   }
                   FFT_mode = 1;
                   TFT_clear_screen();
                   Single_graphic_scale();
                   Draw_singleaxis();
                   ADC_count = 0;
                   Display_flag = 0;
                   KEY4_flag = 1;
                   break;
      default:    break;
    }
    if(FFT_mode == 1)
    {
      if(FFT_flag == 1)
      { FFT_flag = 0;
        for (x = 0; x < 512; x++)
          Input[x] = (float32_t)Input_tmp[x];
        arm_cfft_radix4_init_f32(&S, 256, 0, 1);
      
        arm_cfft_radix4_f32(&S, Input);
      
        arm_cmplx_mag_f32(Input, Output, 256);
      
        arm_max_f32(Output, 256, &maxValue, &maxIndex);
      }
      for (x = 1; x < 135; x++)
        DrawBar(30 +  2*x, 219, 180, (unsigned short)maxValue, (float)Output[(unsigned short)x], White, Black);
       
      if(Display_flag == 1) Display_flag = 0;
    }
    else
    { 
      if(Display_flag == 1)
      { 
	Graphic_clear();
        Dual_graphic_scale();
        Draw_dualaxis();
        Display_flag = 0;
        for(x=0;x<700;x++)
        {
          y1 = (unsigned int)(220. - ADC_array[x]*75./3722. + 0.5);	        // draw ADC_data
          if(x == 0) y1o = y1;
            Line((unsigned int)(30.+(float)x*280./700.),y1o, (unsigned int)(30.+(float)(x+1)*280./700.),y1, White);
          y1o = y1;
          if(x < 200)
            y2 =(unsigned int)(120. - DAC_data[x]*75./3722. + 0.5);	        // draw DAC_data
          else if((x >= 200)&&(x < 400))
            y2 =(unsigned int)(120. - DAC_data[x-200]*75./3722. + 0.5);	
          else if((x >= 400)&&(x < 600))
            y2 =(unsigned int)(120. - DAC_data[x-400]*75./3722. + 0.5);	
          else
            y2 =(unsigned int)(120. - DAC_data[x-600]*75./3722. + 0.5);	
          if(x == 0) y2o = y2;
           Line((unsigned int)(30.+(float)x*280./700.),y2o, (unsigned int)(30.+(float)(x+1)*280./700.),y2, White);
          y2o = y2;
        }
      }
    }
  }
}
/* ====== FFT Draw 정의함수 =================================================== */

void DrawBar(unsigned short bottomX, unsigned short bottomY, unsigned short maxHeight, unsigned short maxValue, float value, unsigned short foreground, unsigned short background)
{ uint16_t height;
  height = (unsigned short)(value / maxValue * maxHeight);
  if(height == maxHeight)
   Line(bottomX, bottomY, bottomX, bottomY - height, foreground);
  
  else if (height < maxHeight)
  { Line(bottomX, bottomY, bottomX, bottomY - height, foreground);
    Line(bottomX, bottomY - height, bottomX, bottomY - maxHeight, background);
  }
}

/* ====== 사용자 정의함수 ===================================================== */

void Draw_dualaxis(void)
{
  unsigned int x, y;

  Rectangle(0,17, 319,239, White);		// outline
  
  Line(30,220,310,220, Cyan);			// x-axis line
  Line(305,215,310,220, Cyan);			// x-axis
  Line(305,225,310,220, Cyan);			// x-axis
  Line(30,130, 30,220, Cyan);			// y-axis line
  Line(35,135, 30,130, Cyan);			// y-axis
  Line(25,135, 30,130, Cyan);			// y-axis
  
  Line(30,120, 310,120, Cyan);			// x-axis line
  Line(305,115, 310,120, Cyan);			// x-axis
  Line(305,125, 310,120, Cyan);			// x-axis
  Line(30,30, 30,120, Cyan);			// y-axis line
  Line(35,35, 30,30, Cyan);			// y-axis
  Line(25,35, 30,30, Cyan);			// y-axis
  
  for(x = 70; x <= 300; x += 40)		// x-axis scale
    { Line(x,218, x,222, Cyan);
      Line(x,118, x,122, Cyan);

      for(y = 35; y <= 110; y += 5)		// vertical line
        TFT_pixel(x,y, Silver);
      for(y = 135; y <= 220; y += 5)		// vertical line
        TFT_pixel(x,y, Silver);
    }

  for(y = 95; y > 30; y -= 25)		        // y-axis scale
    { Line(28,y, 32,y, Cyan);

      for(x = 30; x < 300; x += 5)		// horizontal line
        TFT_pixel(x,y, Silver);
    }
  for(y = 195; y > 120; y -= 25)		// y-axis scale
    { Line(28,y, 32,y, Cyan);

      for(x = 30; x < 300; x += 5)		// horizontal line
        TFT_pixel(x,y, Silver);
    }
}

void Draw_singleaxis(void)
{
  unsigned int x, y;

  Rectangle(0,17, 319,239, White);		// outline
  
  Line(30, 220, 310, 220, Cyan);		// x-axis line
  Line(305,215, 310, 220, Cyan);		// x-axis
  Line(305,225, 310, 220, Cyan);		// x-axis
  Line(30,  30,  30, 220, Cyan);		// y-axis line
  Line(35,  35,  30,  30, Cyan);		// y-axis
  Line(25,  35,  30,  30, Cyan);		// y-axis

  for(x = 70; x <= 300; x += 20)		// x-axis scale
    { Line(x,218, x,222, Cyan);

      for(y = 40; y <= 220; y += 5)		// vertical line
        TFT_pixel(x,y, Silver);
    }
  for(y = 40; y < 220; y += 18)		        // y-axis scale
    { Line(28,y, 32,y, Cyan);

      for(x = 30; x < 300; x += 5)		// horizontal line
        TFT_pixel(x,y, Silver);
    } 
}

void Dual_graphic_scale(void)
{
  TFT_string(7,0,White,Magenta,"        FFT 분석기          ");
  TFT_color(Green,Black);
  TFT_English_pixel(2, 188, '1');		// 1.0 V
  TFT_English_pixel(10,188, '.');
  TFT_English_pixel(18,188, '0');
  TFT_English_pixel(2, 163, '2');		// 2.0 V
  TFT_English_pixel(10,163, '.');
  TFT_English_pixel(18,163, '0');
  TFT_English_pixel(2, 138, '3');		// 3.0 V
  TFT_English_pixel(10,138, '.');
  TFT_English_pixel(18,138, '0');
  TFT_English_pixel(2, 88,  '1');		// 1.0 V
  TFT_English_pixel(10,88,  '.');
  TFT_English_pixel(18,88,  '0');
  TFT_English_pixel(2, 63,  '2');		// 2.0 V
  TFT_English_pixel(10,63,  '.');
  TFT_English_pixel(18,63,  '0');
  TFT_English_pixel(2, 38,  '3');		// 3.0 V
  TFT_English_pixel(10,38,  '.');
  TFT_English_pixel(18,38,  '0');
  TFT_English_pixel(2,  18, '[');              // [V]
  TFT_English_pixel(10, 18, 'V');
  TFT_English_pixel(18, 18, ']');
  
  TFT_English_pixel(285,223, '[');		// [ms]
  TFT_English_pixel(293,223, 'm');		
  TFT_English_pixel(301,223, 's');		
  TFT_English_pixel(309,223, ']');		
  TFT_English_pixel(58, 222, '0');		// 0.5 ms
  TFT_English_pixel(66, 222, '.');		
  TFT_English_pixel(74, 222, '5');		
  TFT_English_pixel(98, 222, '1');		//1.0 ms
  TFT_English_pixel(106,222, '.');		
  TFT_English_pixel(114,222, '0');		
  TFT_English_pixel(138,222, '1');		// 1.5 ms
  TFT_English_pixel(146,222, '.');		
  TFT_English_pixel(154,222, '5');		
  TFT_English_pixel(178,222, '2');		// 2.0 ms
  TFT_English_pixel(186,222, '.');		
  TFT_English_pixel(194,222, '0');		
  TFT_English_pixel(218,222, '2');		// 2.5 ms
  TFT_English_pixel(226,222, '.');		
  TFT_English_pixel(234,222, '5');		
  TFT_English_pixel(258,222, '3');		// 3.0 ms
  TFT_English_pixel(266,222, '.');		
  TFT_English_pixel(274,222, '0');		
}

void Single_graphic_scale(void)		// display graphic scale
{ if(mode == 1)      TFT_string(7,0,White,Magenta,"       정현파의 결과         ");
  else if(mode == 2) TFT_string(7,0,White,Magenta,"       구형파의 결과         ");
  else if(mode == 3) TFT_string(7,0,White,Magenta,"       삼각파의 결과         ");
  TFT_color(Green,Black);
  TFT_English_pixel(10,194, '1');               // 10%
  TFT_English_pixel(18,194, '0');
  TFT_English_pixel(10,176, '2');               // 20%
  TFT_English_pixel(18,176, '0');
  TFT_English_pixel(10,158, '3');               // 30%
  TFT_English_pixel(18,158, '0');
  TFT_English_pixel(10,140, '4');               // 40%
  TFT_English_pixel(18,140, '0');
  TFT_English_pixel(10,122, '5');               // 50%
  TFT_English_pixel(18,122, '0');
  TFT_English_pixel(10,104, '6');               // 60%
  TFT_English_pixel(18,104, '0');
  TFT_English_pixel(10, 86, '7');               // 70%
  TFT_English_pixel(18, 86, '0');
  TFT_English_pixel(10, 68, '8');               // 80%
  TFT_English_pixel(18, 68, '0');
  TFT_English_pixel(10, 50, '9');               // 90%
  TFT_English_pixel(18, 50, '0');
  TFT_English_pixel(2,  32, '1');		// 100 %
  TFT_English_pixel(10, 32, '0');
  TFT_English_pixel(18, 32, '0');
  TFT_English_pixel(2,  18, '[');              // [%]
  TFT_English_pixel(10, 18, '%');
  TFT_English_pixel(18, 18, ']');
  
  TFT_English_pixel(277,223, '[');		// [kHz]
  TFT_English_pixel(285,223, 'k');		
  TFT_English_pixel(293,223, 'H');		
  TFT_English_pixel(301,223, 'z');		
  TFT_English_pixel(309,223, ']');		
  TFT_English_pixel(46, 222, '1');		// 1 kHz
  TFT_English_pixel(66, 222, '2');		// 2 kHz
  TFT_English_pixel(86, 222, '3');		// 3 kHz
  TFT_English_pixel(106,222, '4');		// 4 kHz
  TFT_English_pixel(126,222, '5');		// 5 kHz
  TFT_English_pixel(146,222, '6');		// 6 kHz
  TFT_English_pixel(166,222, '7');		// 7 kHz
  TFT_English_pixel(186,222, '8');		// 8 kHz		
  TFT_English_pixel(206,222, '9');		// 9 kHz		
  TFT_English_pixel(221,222, '1');		// 10 kHz
  TFT_English_pixel(229,222, '0');		
  TFT_English_pixel(241,222, '1');		// 11 kHz		
  TFT_English_pixel(249,222, '1');		
  TFT_English_pixel(261,222, '1');		// 12 kHz
  TFT_English_pixel(269,222, '2');		
}

void Graphic_clear(void)			// clear graphic screen 
{ 
  unsigned int x, y;

  for(y = 21; y <= 119; y++)
    { TFT_GRAM_address(31,y);
      for(x = 31; x <= 310; x++)
        TFT_data(Black);
    }
  for(y = 130; y <= 219; y++)
    { TFT_GRAM_address(31,y);
      for(x = 31; x <= 310; x++)
        TFT_data(Black);
    }
}
