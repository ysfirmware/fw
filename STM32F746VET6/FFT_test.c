/* ============================================================================ */
/*	   FFT_test.c : 고속푸리에변환(fast Fourier transform) 프로그램         */
/* ============================================================================ */
/*					   programmed by Dong-Ki Kim in 2016.   */

#include "stm32f746xx.h"
#include "arm_math.h"
#include "OK-STM746.h"

#define Ts              5.e-6                   // sampling time
#define freq            1000.                   // frequency of waveforms (1kHz)

void SysTick_Handler(void);			/* SysTick interrupt function */
void TIM1_UP_TIM10_IRQHandler(void);	        /* TIM1 interrupt function */

void Display_wave(void);			/* display wave */
void Display_FFT(void);			        /* display FFT */
void Graphic_clear(void);			/* clear graphic screen */
void Graphic_line(void);			/* draw line of graphic screen */
void Graphic_bar(U16 x1, U16 y1, float max_h, float max_val, float val, U16 colorfore, U16 colorback);

unsigned char wave_mode, wave_flag, fft_mode, fft_flag, dir;
unsigned short wave_cnt, square_cnt, fft_cnt;
unsigned short DAC1_out;
unsigned int cnt_1s;
float sin_theta, theta;
double triangle;

float DAC_buffer[800], ADC_buffer[800];

float Input[512];
float Output[256];
float FFT_buffer[512];

arm_cfft_radix4_instance_f32 S;
float max_value;
unsigned int maxIndex;

/* ----- SysTick 인터럽트 처리 프로그램 ------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function (200kHz) */
{
  if(wave_mode == 1)                            // calculate of sin wave
  { DAC1_out = (U16)(2047.*sin_theta + 2048.);
    
    theta += 2.*PI*freq*Ts;
    if(theta >= 2.*PI) theta = 0.;
    sin_theta = arm_sin_f32(theta);
  }
  else if(wave_mode == 2)                       // calculate of square wave
  { if(square_cnt <= 99) DAC1_out = 4095;
    else DAC1_out = 0;

    square_cnt++;
    if(square_cnt >= 200) square_cnt = 0;
  }
  else if(wave_mode == 3)                       // calculate of triangular wave
  { DAC1_out = (U16)(2047.*triangle + 2048.);
    
    if(dir == 0) 
    { triangle += 4.*freq*Ts;
      if(triangle >= 1.)
      { triangle = 1.;
        dir = 1;
      }
    }
    else
    { triangle -= 4.*freq*Ts;
      if(triangle <= -1.)
      { triangle = -1.;
        dir = 0;
      }
    }
  }
  DAC->DHR12R1 = DAC1_out;

  if(fft_mode == 0)                             //  fft_mode = 0, wave mode
  { ADC1->CR2 |= 0x40000000;			// right alignment, ADON = 1
    while((ADC1->SR & 0x00000002) != 0x00000002); // ADC1 end of conversion(?)
    
    DAC_buffer[wave_cnt] = DAC->DOR1;
    ADC_buffer[wave_cnt] = ADC1->DR;
    
    wave_cnt++;
    if(wave_cnt >= 800) wave_cnt = 0;

    cnt_1s++;
    if(cnt_1s >= 200000)
    { cnt_1s = 0;
      wave_flag = 1;
    }
  }
}

/* ----- TIM1 인터럽트 처리 프로그램 ------------------------------------------- */

void TIM1_UP_TIM10_IRQHandler(void)	        /* TIM1 interrupt function (25.6kHz) */
{
  TIM1->SR = 0x00000000;			// clear pending bit of TIM1 interrupt
  
  if(fft_mode == 1)                             //  fft_mode = 1, FFT mode
  { ADC1->CR2 |= 0x40000000;			// right alignment, ADON = 1
    while((ADC1->SR & 0x00000002) != 0x00000002); // ADC1 end of conversion(?)
      
    FFT_buffer[fft_cnt] = ((float)ADC1->DR - 2048.)/2048.;
    FFT_buffer[fft_cnt+1] = 0.;

    fft_cnt += 2;
    if(fft_cnt >= 512)
    { fft_cnt = 0;
      fft_flag = 1;
    }
  }
}

int main(void)
{
  unsigned char key=no_key;
  unsigned short i;
  unsigned short DACy, DACyo=0, ADCy, ADCyo=0;  // graphic variable         

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   FFT_test.c   ");

  /* Systick timer configuration */
  SysTick->LOAD  = 1079;			// 216MHz/(1079+1) = 200kHz
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000007;			// SysTickCLK = HCLK, counter and interrupt enable
  
  /* TIM1 configuration */
  RCC->APB2ENR |= 0x00000001;			// enable TIM1 clock
  RCC->DCKCFGR1 |= 0x01000000;                  // TIM1CLK = HCLK
  TIM1->PSC = 0;
  TIM1->ARR = 8436;				// 216MHz / (8436+1) = 25.6kHz
  TIM1->CNT = 0;				// clear counter
  TIM1->DIER = 0x0001;				// enable update interrupt
  TIM1->CR1 = 0x0005;				// overflow interrupt, enable TIM1

  /* ADC1 configuration */
  RCC->APB2ENR |= 0x00000100;			// enable ADC1 clock
  GPIOA->MODER |= 0x00003000;			// PA6 = analog mode(ADC12_IN6)
  ADC->CCR = 0x00800000;	                // 54MHz / 2 = 27MHz

  ADC1->CR1 = 0x04000800;			// 8-bit resolution, single discontinuous mode
  ADC1->SMPR2 &= 0xFFE3FFF;			// sampling time of channel 6 =  3cycle
  ADC1->SMPR2 |= 0x00000000;
  ADC1->SQR1 = 0x00000000;                      // 1 conversion
  ADC1->SQR3 = 0x00000006;                      // channel 6 conversion
  ADC1->CR2 = 0x00000001;			// right alignment, ADON = 1

  /* DAC1 configuration */
  RCC->APB1ENR |= 0x20000000;			// enable DAC clock
  GPIOA->MODER |= 0x00000300;			// PA4 = DAC_OUT1

  DAC->CR = 0x00000001;				// DAC1 enable, output buffer

  /* initialize display */
  TFT_clear_screen();                           // clear TFT screen
  Display_wave();
  Graphic_line();
  
  arm_cfft_radix4_init_f32(&S, 256, 0, 1);      // Initialize Radix-4 CFFT & CIFFT function
  
  Beep();                                       // complete Initialization
  
  NVIC->ISER[0] |= 0x02000000;			// enable (25)TIM1 interrupt

  while(1)
    { key = Key_input();
      
      if(key == KEY1)				// if KEY1, sin wave mode
       { key = no_key;
         TFT_clear_screen();
         TFT_string(11,0,White,Magenta," 사인파 입/출력파형 ");
         Display_wave();
         Graphic_line();
         theta = 0.;
         sin_theta = 0.;
         wave_cnt = 0;
         fft_mode = 0;
         wave_mode = 1;
       }
      else if(key == KEY2)			// if KEY2, square wave mode
       { key = no_key;
         TFT_clear_screen();  
         TFT_string(11,0,White,Magenta," 구형파 입/출력파형 ");
         Display_wave();
         Graphic_line();
         square_cnt = 0;
         wave_cnt = 0;
         fft_mode = 0;
         wave_mode = 2;
       }
      else if(key == KEY3)			// if KEY3, triangular wave mode
       { key = no_key;
         TFT_clear_screen();
         TFT_string(11,0,White,Magenta," 삼각파 입/출력파형 ");
         Display_wave();
         Graphic_line();
         triangle = 0.;
         dir = 0;
         wave_cnt = 0;
         fft_mode = 0;
         wave_mode = 3;
       }
      else if(key == KEY4)			// if KEY4, fast Fourier transform mode
       { key = no_key;
         TFT_clear_screen();
         if(wave_mode == 1)      
           TFT_string(11,0,White,Magenta," 사인파 FFT 분석기 ");
         else if(wave_mode == 2) 
           TFT_string(11,0,White,Magenta," 구형파 FFT 분석기 ");
         else if(wave_mode == 3) 
           TFT_string(11,0,White,Magenta," 삼각파 FFT 분석기 ");
         Display_FFT();
         fft_mode = 1;
       }

      if(fft_mode == 0)                         // display waveforms
      { if(wave_flag == 1)
        { Graphic_clear();
          Graphic_line();
          for(i = 0; i <= 699; i++)
          { DACy = (U16)(120. - DAC_buffer[i]*83./4095. + 0.5);
            if(i == 0) DACyo = DACy;
            Line(30+(U16)(i*2./5.), DACyo, 31+(U16)(i*2./5.), DACy, Red);
            DACyo = DACy;
          
            ADCy = (U16)(220. - ADC_buffer[i]*83./4095. + 0.5);
            if(i == 0) ADCyo = ADCy;
            Line(30+(U16)(i*2./5.), ADCyo, 31+(U16)(i*2./5.), ADCy, Blue);
            ADCyo = ADCy;
          }
          wave_flag = 0;
        }
      }
      else                                      // display FFT result
      { if(fft_flag == 1)
        { for (i = 0; i < 512; i++)
            Input[i] = FFT_buffer[i];

          arm_cfft_radix4_f32(&S, Input);
          arm_cmplx_mag_f32(Input, Output, 256);
          arm_max_f32(Output, 256, &max_value, &maxIndex);

          for (i = 1; i < 128; i++)
          Graphic_bar(30+2*i, 219, 180., max_value, Output[i], Blue, Black);

          fft_flag = 0;
        }
      }
    }
}

/* ============================================================================ */
/*	                       파형 디스플레이 화면		                */
/* ============================================================================ */

void Display_wave(void)			        /* display wave */
{
  TFT_color(Green,Black);
  TFT_English_pixel(2,  18, '[');               // [V]
  TFT_English_pixel(10, 18, 'V');
  TFT_English_pixel(18, 18, ']');
  
  TFT_color(Blue,Black);
  TFT_English_pixel(2,  188, '1');		// 1.0
  TFT_English_pixel(10, 188, '.');
  TFT_English_pixel(18, 188, '0');
  TFT_English_pixel(2,  163, '2');		// 2.0
  TFT_English_pixel(10, 163, '.');
  TFT_English_pixel(18, 163, '0');
  TFT_English_pixel(2,  138, '3');		// 3.0
  TFT_English_pixel(10, 138, '.');
  TFT_English_pixel(18, 138, '0');
  
  TFT_color(Red,Black);
  TFT_English_pixel(2,  88,  '1');		// 1.0
  TFT_English_pixel(10, 88,  '.');
  TFT_English_pixel(18, 88,  '0');
  TFT_English_pixel(2,  63,  '2');		// 2.0
  TFT_English_pixel(10, 63,  '.');
  TFT_English_pixel(18, 63,  '0');
  TFT_English_pixel(2,  38,  '3');		// 3.0
  TFT_English_pixel(10, 38,  '.');
  TFT_English_pixel(18, 38,  '0');
  
  TFT_color(Green,Black);
  TFT_English_pixel(285, 223, '[');		// [ms]
  TFT_English_pixel(293, 223, 'm');		
  TFT_English_pixel(301, 223, 's');		
  TFT_English_pixel(309, 223, ']');
  TFT_English_pixel(58,  222, '0');		// 0.5
  TFT_English_pixel(66,  222, '.');		
  TFT_English_pixel(74,  222, '5');		
  TFT_English_pixel(98,  222, '1');		// 1.0
  TFT_English_pixel(106, 222, '.');		
  TFT_English_pixel(114, 222, '0');		
  TFT_English_pixel(138, 222, '1');		// 1.5
  TFT_English_pixel(146, 222, '.');		
  TFT_English_pixel(154, 222, '5');		
  TFT_English_pixel(178, 222, '2');		// 2.0
  TFT_English_pixel(186, 222, '.');		
  TFT_English_pixel(194, 222, '0');		
  TFT_English_pixel(218, 222, '2');		// 2.5
  TFT_English_pixel(226, 222, '.');		
  TFT_English_pixel(234, 222, '5');		
  TFT_English_pixel(258, 222, '3');		// 3.0
  TFT_English_pixel(266, 222, '.');		
  TFT_English_pixel(274, 222, '0');
}

void Graphic_line(void)				/* draw line of graphic screen */
{ 
  unsigned short x, y;
  
  Line(30,  220, 310, 220, White);		// x-axis line
  Line(305, 215, 310, 220, White);
  Line(305, 225, 310, 220, White);
  Line(30,  130, 30,  220, White);		// y-axis line
  Line(35,  135, 30,  130, White);
  Line(25,  135, 30,  130, White);

  Line(30,  120, 310, 120, White);		// x-axis line
  Line(305, 115, 310, 120, White);
  Line(305, 125, 310, 120, White);
  Line(30,  30,  30,  120, White);		// y-axis line
  Line(35,  35,  30,  30,  White);
  Line(25,  35,  30,  30,  White);
  
  for(x = 70; x <= 300; x += 40)		// x-axis scale
    { Line(x,218, x,222, White);
      Line(x,118, x,122, White);
      for(y = 35; y <= 110; y += 5)		// vertical line
        TFT_pixel(x,y, White);
      for(y = 135; y <= 220; y += 5)		// vertical line
        TFT_pixel(x,y, White);
    }
  for(y = 95; y >= 45; y -= 25)		        // y-axis scale
    { Line(28,y, 32,y, White);
      for(x = 30; x <= 310; x += 5)		// horizontal line
        TFT_pixel(x,y, White);
    }
  for(y = 195; y >= 145; y -= 25)		// y-axis scale
    { Line(28,y, 32,y, White);
      for(x = 30; x <= 310; x += 5)		// horizontal line
        TFT_pixel(x,y, White);
    }
}

void Graphic_clear(void)			/* clear graphic screen */
{ 
  unsigned short x, y;

  for(y = 30; y <= 120; y++)
  { TFT_GRAM_address(30,y);
    for(x = 30; x <= 310; x++)
      TFT_data(Black);
  }
  for(y = 130; y <= 220; y++)
  { TFT_GRAM_address(30,y);
    for(x = 30; x <= 310; x++)
      TFT_data(Black);
  }
}

/* ============================================================================ */
/*	                       FFT 디스플레이 화면                              */
/* ============================================================================ */

void Display_FFT(void)			        /* display FFT */
{
  unsigned short x, y;
  
  TFT_color(Blue,Black);
  TFT_English_pixel(10,194, '1');               // 10
  TFT_English_pixel(18,194, '0');
  TFT_English_pixel(10,176, '2');               // 20
  TFT_English_pixel(18,176, '0');
  TFT_English_pixel(10,158, '3');               // 30
  TFT_English_pixel(18,158, '0');
  TFT_English_pixel(10,140, '4');               // 40
  TFT_English_pixel(18,140, '0');
  TFT_English_pixel(10,122, '5');               // 50
  TFT_English_pixel(18,122, '0');
  TFT_English_pixel(10,104, '6');               // 60
  TFT_English_pixel(18,104, '0');
  TFT_English_pixel(10, 86, '7');               // 70
  TFT_English_pixel(18, 86, '0');
  TFT_English_pixel(10, 68, '8');               // 80
  TFT_English_pixel(18, 68, '0');
  TFT_English_pixel(10, 50, '9');               // 90
  TFT_English_pixel(18, 50, '0');
  TFT_English_pixel(2,  32, '1');		// 100
  TFT_English_pixel(10, 32, '0');
  TFT_English_pixel(18, 32, '0');
  TFT_English_pixel(2,  18, '[');               // [%]
  TFT_English_pixel(10, 18, '%');
  TFT_English_pixel(18, 18, ']');

  TFT_color(Green,Black);
  TFT_English_pixel(277,223, '[');		// [kHz]
  TFT_English_pixel(285,223, 'k');
  TFT_English_pixel(293,223, 'H');
  TFT_English_pixel(301,223, 'z');
  TFT_English_pixel(309,223, ']');
  TFT_English_pixel(46, 222, '1');		// 1
  TFT_English_pixel(66, 222, '2');		// 2
  TFT_English_pixel(86, 222, '3');		// 3
  TFT_English_pixel(106,222, '4');		// 4
  TFT_English_pixel(126,222, '5');		// 5
  TFT_English_pixel(146,222, '6');		// 6
  TFT_English_pixel(166,222, '7');		// 7
  TFT_English_pixel(186,222, '8');		// 8
  TFT_English_pixel(206,222, '9');		// 9
  TFT_English_pixel(221,222, '1');		// 10
  TFT_English_pixel(229,222, '0');		
  TFT_English_pixel(241,222, '1');		// 11
  TFT_English_pixel(249,222, '1');		
  TFT_English_pixel(261,222, '1');		// 12
  TFT_English_pixel(269,222, '2');
  
  Line(30, 220, 310, 220, White);		// x-axis line
  Line(305,215, 310, 220, White);
  Line(305,225, 310, 220, White);
  Line(30,  30,  30, 220, White);		// y-axis line
  Line(35,  35,  30,  30, White);
  Line(25,  35,  30,  30, White);

  for(x = 50; x <= 309; x += 20)		// x-axis scale
    Line(x,218, x,222, White);
  for(y = 40; y <= 202; y += 18)		// y-axis scale
    Line(28,y, 32,y, White);
}

/* ====== 사용자 정의함수 ===================================================== */

void Graphic_bar(U16 x1, U16 y1, float max_h, float max_val, float val, U16 colorfore, U16 colorback)
{
  unsigned short h;

  h = (U16)(val / max_val * max_h + 0.5);
  if(max_h == h)
    Line(x1, y1, x1, y1 - h, colorfore);
  else if (max_h > h)
  { Line(x1, y1, x1, y1 - h, colorfore);
    Line(x1, y1 - h, x1, y1 - (U16)max_h, colorback);
  }
}