/* ============================================================================ */
/*		Exp10_1_Samsung.c : 삼성전자 TV 리모컨 입력 프로그램		*/
/* ============================================================================ */
/*			                programmed by Seung-Yong Lee in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

void EXTI0_IRQHandler(void);		        /* EXTI4_15 interrupt function */
void TIM6_DAC_IRQHandler(void);			/* TIM6 interrupt function */

// Leader_Code	= 9000				// 9ms / 1us = 9000
// Low_Bit	= 1125				// 1.125ms / 1us = 1125
// High_Bit	= 2250				// 2.25ms / 1us = 2250

#define ARR_value	10800			// 120% of Leader_Code
#define Leader_Max	9900			// 110% of Leader_Code
#define Leader_Min	8100			//  90% of Leader_Code
#define Low_Bit_Max	1238			// 110% of Low_Bit_Code
#define Low_Bit_Min	1013			//  90% of Low_Bit_Code
#define High_Bit_Max	2475			// 110% of High_Bit_Code
#define High_Bit_Min	2025			//  90% of High_Bit_Code

volatile unsigned char Remocon_state;		// step to recognize remocon signal
volatile unsigned int Remocon_count;		// 1us base time counter
volatile unsigned char Remocon_data;		// temporary byte data of remocon signal
volatile unsigned char Remocon_bit_count;	// bit count of data to recognize remocon signal
volatile unsigned char Remocon_OK_flag;		// remocon command receive OK
volatile unsigned char Remocon_command[4];	// remocon command string

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void EXTI0_IRQHandler(void)
{
  EXTI->PR = 0x00000001;			// clear pending bit of EXTI0 interrupt

  unsigned char x;  

  Remocon_count = TIM6->CNT;			// read Remocon_count
  TIM6->CNT = 0;				// start new Remocon_count

  switch(Remocon_state)
    { case 0 :					// if ready state, go to step 1(leader state)
               Remocon_state = 1;
               Remocon_count = 0;           
               break;

      case 1 : 					// if leader state, check leader signal
               if((Remocon_count >= Leader_Min) && (Remocon_count <= Leader_Max))
                 { Remocon_state = 2;		// if lead signal, go to step 2(data state)
                   Remocon_data = 0;
	           Remocon_bit_count = 0;
                 }
               else
		 Remocon_state = 0;		// if not lead signal, go to step 0(ready state)
	       break;

      case 2: 					// if data state, check data signal 0 or 1
	       Remocon_data >>= 1;
               if((Remocon_count >= Low_Bit_Min) && (Remocon_count <= Low_Bit_Max))
                 Remocon_data &= 0x7F;		// if data 0, add data bit 0
               else if((Remocon_count >= High_Bit_Min) && (Remocon_count <= High_Bit_Max))
                 Remocon_data |= 0x80;
               else
	         Remocon_state = 0;		// if not 0 and not 1, go to step 0(ready state)
               Remocon_bit_count++;
	       if((Remocon_bit_count % 8) == 0) // if a character complete, store it
	         { x = Remocon_data;
		   Remocon_command[(Remocon_bit_count/8)-1] = x;
	           Remocon_data = 0;
                 }
               if(Remocon_bit_count == 32)	// if remocon OK, check custom code and checksum
                 { x = Remocon_command[3];
		   if((~Remocon_command[2] & 0xFF) == x)
		     { Remocon_state = 0;
		       Remocon_OK_flag = 1;
		     }
                   else
		     Remocon_state = 0;
	 	 }
               break;

      default : break;
    }

  EXTI->PR = 0x00000001;			// clear pending bit of EXTI0 interrupt
}

void TIM6_DAC_IRQHandler(void)			/* TIM6 interrupt function */
{
  
  Remocon_state = 0;				// if overflow, the signal is noise
  TIM6->SR = 0x0000;				// clear pending bit of TIM6 interrupt
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F7  V1.0");		// display title
  LCD_string(0xC0,"   Exp10_1.c    ");
  Beep();

  TFT_string(10,10,White,Magenta," 삼성전자 TV 리모컨 ");
  TFT_string(11,16,Green,Black,  "코드 : 00 00 00 00");
  TFT_string(11,19,Green,Black,  "기능 :             ");
  TFT_color(White,Black);

  Remocon_state = 0;				// remocon ready state
  Remocon_OK_flag = 0;				// remocon not character

  GPIOB->MODER &= 0xFFFFFFF0;			// PB0(REMOCON) = input mode			
  RCC->AHB1ENR |= 0x00000002;			// enable SYSCFG
  SYSCFG->EXTICR[0] = 0x00000001;		// EXTI0 = PB0(REMOCON)
  EXTI->IMR |= 0x00000001;			// enable EXT0 interrupt
  EXTI->RTSR = 0x00000000;			// disable EXTI0 rising edge
  EXTI->FTSR = 0x00000001;			// enable EXTI0 falling edge
  NVIC->ISER[0] |= 0x00000040;

  RCC->APB1ENR |= 0x00000010;			// enable TIM6 clock
  TIM6->PSC = 107;				// 108MHz/(107+1) = 1MHz = 1us base time
  TIM6->ARR = ARR_value;			// 120% of Leader_Code
  TIM6->CNT = 0;				// clear counter
  TIM6->DIER = 0x0001;				// enable update interrupt
  TIM6->CR1 = 0x0005;				// enable TIM6
  NVIC->ISER[1] |= 0x00400000;			// enable TIM6 interrupt

  while(1)
    { if(Remocon_OK_flag == 1)
        { Remocon_OK_flag = 0;
          TFT_color(White,Black);
          TFT_xy(18,16); TFT_hexadecimal(Remocon_command[0],2);		// code
          TFT_xy(21,16); TFT_hexadecimal(Remocon_command[1],2);
          TFT_xy(24,16); TFT_hexadecimal(Remocon_command[2],2);
          TFT_xy(27,16); TFT_hexadecimal(Remocon_command[3],2);
          Beep();

          if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x02)) // function
	    TFT_string(18,19,White,Black,"전원     ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x01))
	    TFT_string(18,19,White,Black,"TV/비디오");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x27))
	    TFT_string(18,19,White,Black,"TV/유선  ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x11))
	    TFT_string(18,19,White,Black,"0        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x04))
	    TFT_string(18,19,White,Black,"1        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x05))
	    TFT_string(18,19,White,Black,"2        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x06))
	    TFT_string(18,19,White,Black,"3        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x08))
	    TFT_string(18,19,White,Black,"4        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x09))
	    TFT_string(18,19,White,Black,"5        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x0A))
	    TFT_string(18,19,White,Black,"6        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x0C))
	    TFT_string(18,19,White,Black,"7        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x0D))
	    TFT_string(18,19,White,Black,"8        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x0E))
	    TFT_string(18,19,White,Black,"9        ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x1A))
	    TFT_string(18,19,White,Black,"메뉴     ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x12))
	    TFT_string(18,19,White,Black,"채널+    ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x10))
	    TFT_string(18,19,White,Black,"채널-    ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x07))
	    TFT_string(18,19,White,Black,"음량+    ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x0B))
	    TFT_string(18,19,White,Black,"음량-    ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x1D))
	    TFT_string(18,19,White,Black,"조정+    ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x1C))
	    TFT_string(18,19,White,Black,"조정-    ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x0F))
	    TFT_string(18,19,White,Black,"조용히   ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x3A))
	    TFT_string(18,19,White,Black,"자동채널 ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x13))
	    TFT_string(18,19,White,Black,"앞채널   ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x19))
	    TFT_string(18,19,White,Black,"기억     ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x18))
	    TFT_string(18,19,White,Black,"지움     ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x3E))
	    TFT_string(18,19,White,Black,"화면크기 ");
          else if((Remocon_command[0] == 0x07) && (Remocon_command[2] == 0x03))
	    TFT_string(18,19,White,Black,"취침예약 ");
          else
	    TFT_string(18,19,White,Black,"         ");
        }
    }
}
