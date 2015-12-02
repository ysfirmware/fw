/* ============================================================================ */
/*	Exp10_3_all.c : �Ｚ����/LG���� ��� TV ������ �Է� ���α׷�		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void EXTI4_15_IRQHandler(void);			/* EXTI4_15 interrupt function */
void TIM6_DAC_IRQHandler(void);			/* TIM6 interrupt function */

// SS_Leader_Code	= 9000			// 9ms / 1us = 9000
// LG_Leader_Code	= 13500			// 13.5ms / 1us = 13500
// Low_Bit		= 1125			// 1.125ms / 1us = 1125
// High_Bit		= 2250			// 2.25ms / 1us = 2250

#define ARR_value	16200			// 120% of maximum Leader_Code
#define SS_Leader_Max	9900			// 110% of Leader_Code for Samsung
#define SS_Leader_Min	8100			//  90% of Leader_Code
#define LG_Leader_Max	14850			// 110% of Leader_Code for LG
#define LG_Leader_Min	12150			//  90% of Leader_Code
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
volatile unsigned char format;			// data format('S' = Samsung, 'L' = LG)

/* ----- ���ͷ�Ʈ ó�� ���α׷� ----------------------------------------------- */

void EXTI4_15_IRQHandler(void)			/* EXTI4_15 interrupt function */
{
  unsigned char x;

  Remocon_count = TIM6->CNT;			// read Remocon_count
  TIM6->CNT = 0;				// start new Remocon_count

  switch(Remocon_state)
    { case 0 :					// if ready state, go to step 1(leader state)
               Remocon_state = 1;
               Remocon_count = 0;           
               break;

      case 1 : 					// if leader state, check leader signal
               if((Remocon_count >= SS_Leader_Min) && (Remocon_count <= SS_Leader_Max))
                 { format = 'S';		// if Samsung lead signal, go to step 2(data state)
                   Remocon_state = 2;
                   Remocon_data = 0;
	           Remocon_bit_count = 0;
                 }
               else if((Remocon_count >= LG_Leader_Min) && (Remocon_count <= LG_Leader_Max))
                 { format = 'L';		// if LG lead signal, go to step 2(data state)
                   Remocon_state = 2;
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

  EXTI->PR = 0x00000020;			// clear pending bit of EXTI5 interrupt
}

void TIM6_DAC_IRQHandler(void)			/* TIM6 interrupt function */
{
  Remocon_state = 0;				// if overflow, the signal is noise

  TIM6->SR = 0x0000;				// clear pending bit of TIM6 interrupt
}

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   TV remocon   ");
  LCD_string(0xC0,"   Exp10_3.c    ");
  Beep();

  TFT_string(10,10,White,Magenta," �Ｚ/LG  TV ������ ");
  TFT_string(11,16,Green,Black,  "�ڵ� : 00 00 00 00");
  TFT_string(11,19,Green,Black,  "��� :             ");
  TFT_color(White,Black);

  Remocon_state = 0;				// remocon ready state
  Remocon_OK_flag = 0;				// remocon not character

  GPIOA->MODER &= 0xFFFFF3FF;			// PA5(REMOCON) = input mode
  RCC->APB2ENR |= 0x00000001;			// enable SYSCFG
  SYSCFG->EXTICR[1] = 0x00000000;		// EXTI5 = PA5(REMOCON)
  EXTI->IMR |= 0x00000020;			// enable EXTI5 interrupt
  EXTI->RTSR = 0x00000000;			// disable EXTI5 rising edge
  EXTI->FTSR = 0x00000020;			// enable EXTI5 falling edge
  NVIC->ISER[0] |= 0x00000080;			// enable (7)EXTI4_15 interrupt

  RCC->APB1ENR |= 0x00000010;			// enable TIM6 clock
  TIM6->PSC = 47;				// 48MHz/(47+1) = 1MHz = 1us base time
  TIM6->ARR = ARR_value;			// 120% of Leader_Code
  TIM6->CNT = 0;				// clear counter
  TIM6->DIER = 0x0001;				// enable update interrupt
  TIM6->CR1 = 0x0005;				// enable TIM6
  NVIC->ISER[0] |= 0x00020000;			// enable (17)TIM6 interrupt

  while(1)
    { if(Remocon_OK_flag == 1)
        { Remocon_OK_flag = 0;
          TFT_color(White,Black);
	  TFT_xy(18,16); TFT_hexadecimal(Remocon_command[0],2);		// code
          TFT_xy(21,16); TFT_hexadecimal(Remocon_command[1],2);
          TFT_xy(24,16); TFT_hexadecimal(Remocon_command[2],2);
          TFT_xy(27,16); TFT_hexadecimal(Remocon_command[3],2);
          Beep();

          if((format == 'S') && (Remocon_command[0] == 0x07))
            { TFT_string(10,10,White,Magenta," �Ｚ���� TV ������ ");

              if(Remocon_command[2] == 0x02)				// function
	        TFT_string(18,19,White,Black,"����     ");
              else if(Remocon_command[2] == 0x01)
	        TFT_string(18,19,White,Black,"TV/����");
              else if(Remocon_command[2] == 0x27)
	        TFT_string(18,19,White,Black,"TV/����  ");
              else if(Remocon_command[2] == 0x11)
	        TFT_string(18,19,White,Black,"0        ");
              else if(Remocon_command[2] == 0x04)
	        TFT_string(18,19,White,Black,"1        ");
              else if(Remocon_command[2] == 0x05)
	        TFT_string(18,19,White,Black,"2        ");
              else if(Remocon_command[2] == 0x06)
	        TFT_string(18,19,White,Black,"3        ");
              else if(Remocon_command[2] == 0x08)
	        TFT_string(18,19,White,Black,"4        ");
              else if(Remocon_command[2] == 0x09)
	        TFT_string(18,19,White,Black,"5        ");
              else if(Remocon_command[2] == 0x0A)
	        TFT_string(18,19,White,Black,"6        ");
              else if(Remocon_command[2] == 0x0C)
	        TFT_string(18,19,White,Black,"7        ");
              else if(Remocon_command[2] == 0x0D)
	        TFT_string(18,19,White,Black,"8        ");
              else if(Remocon_command[2] == 0x0E)
	        TFT_string(18,19,White,Black,"9        ");
              else if(Remocon_command[2] == 0x1A)
	        TFT_string(18,19,White,Black,"�޴�     ");
              else if(Remocon_command[2] == 0x12)
	        TFT_string(18,19,White,Black,"ä��+    ");
              else if(Remocon_command[2] == 0x10)
	        TFT_string(18,19,White,Black,"ä��-    ");
              else if(Remocon_command[2] == 0x07)
	        TFT_string(18,19,White,Black,"����+    ");
              else if(Remocon_command[2] == 0x0B)
	        TFT_string(18,19,White,Black,"����-    ");
              else if(Remocon_command[2] == 0x1D)
	        TFT_string(18,19,White,Black,"����+    ");
              else if(Remocon_command[2] == 0x1C)
	        TFT_string(18,19,White,Black,"����-    ");
              else if(Remocon_command[2] == 0x0F)
	        TFT_string(18,19,White,Black,"������   ");
              else if(Remocon_command[2] == 0x3A)
	        TFT_string(18,19,White,Black,"�ڵ�ä�� ");
              else if(Remocon_command[2] == 0x13)
	        TFT_string(18,19,White,Black,"��ä��   ");
              else if(Remocon_command[2] == 0x19)
	        TFT_string(18,19,White,Black,"���     ");
              else if(Remocon_command[2] == 0x18)
	        TFT_string(18,19,White,Black,"����     ");
              else if(Remocon_command[2] == 0x3E)
	        TFT_string(18,19,White,Black,"ȭ��ũ�� ");
              else if(Remocon_command[2] == 0x03)
	        TFT_string(18,19,White,Black,"��ħ���� ");
              else
	        TFT_string(18,19,White,Black,"         ");
            }
          else if((format == 'L') && (Remocon_command[0] == 0x04))
	    { TFT_string(10,10,White,Magenta,"  LG���� TV ������  ");

              if(Remocon_command[2] == 0x08)				// function
	        TFT_string(18,19,White,Black,"����     ");
              else if(Remocon_command[2] == 0x0B)
	        TFT_string(18,19,White,Black,"TV/����");
              else if(Remocon_command[2] == 0x0F)
	        TFT_string(18,19,White,Black,"TV/����  ");
              else if(Remocon_command[2] == 0x10)
	        TFT_string(18,19,White,Black,"0        ");
              else if(Remocon_command[2] == 0x11)
	        TFT_string(18,19,White,Black,"1        ");
              else if(Remocon_command[2] == 0x12)
	        TFT_string(18,19,White,Black,"2        ");
              else if(Remocon_command[2] == 0x13)
	        TFT_string(18,19,White,Black,"3        ");
              else if(Remocon_command[2] == 0x14)
	        TFT_string(18,19,White,Black,"4        ");
              else if(Remocon_command[2] == 0x15)
	        TFT_string(18,19,White,Black,"5        ");
              else if(Remocon_command[2] == 0x16)
	        TFT_string(18,19,White,Black,"6        ");
              else if(Remocon_command[2] == 0x17)
	        TFT_string(18,19,White,Black,"7        ");
              else if(Remocon_command[2] == 0x18)
	        TFT_string(18,19,White,Black,"8        ");
              else if(Remocon_command[2] == 0x19)
	        TFT_string(18,19,White,Black,"9        ");
              else if(Remocon_command[2] == 0x43)
	        TFT_string(18,19,White,Black,"�޴�     ");
              else if(Remocon_command[2] == 0x00)
	        TFT_string(18,19,White,Black,"ä��+    ");
              else if(Remocon_command[2] == 0x01)
	        TFT_string(18,19,White,Black,"ä��-    ");
              else if(Remocon_command[2] == 0x02)
	        TFT_string(18,19,White,Black,"����+    ");
              else if(Remocon_command[2] == 0x03)
	        TFT_string(18,19,White,Black,"����-    ");
              else if(Remocon_command[2] == 0x06)
	        TFT_string(18,19,White,Black,"����+    ");
              else if(Remocon_command[2] == 0x07)
	        TFT_string(18,19,White,Black,"����-    ");
              else if(Remocon_command[2] == 0x09)
	        TFT_string(18,19,White,Black,"������   ");
              else if(Remocon_command[2] == 0x54)
	        TFT_string(18,19,White,Black,"�ڵ�ä�� ");
              else if(Remocon_command[2] == 0x1A)
	        TFT_string(18,19,White,Black,"��ä��   ");
              else if(Remocon_command[2] == 0x44)
	        TFT_string(18,19,White,Black,"���     ");
              else if(Remocon_command[2] == 0x55)
	        TFT_string(18,19,White,Black,"����     ");
              else if(Remocon_command[2] == 0x0E)
	        TFT_string(18,19,White,Black,"��ħ���� ");
              else
	        TFT_string(18,19,White,Black,"         ");
            }
          else if((format == 'S') && (Remocon_command[0] == 0x05))
	    { TFT_string(10,10,White,Magenta," �Ｚ���� TV ������ ");
	      TFT_string(18,19,Magenta,Black,"(Video��)");
            }
          else if((format == 'L') && (Remocon_command[0] == 0x6E))
	    { TFT_string(10,10,White,Magenta,"  LG���� TV ������  ");
	      TFT_string(18,19,Magenta,Black,"(Video��)");
            }
          else
	    { TFT_string(10,10,White,Magenta," ��Ȯ���� TV ������ ");
	      TFT_string(18,19,Magenta,Black,"         ");
            }
        }
    }
}
