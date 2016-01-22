/* ============================================================================ */
/*	Exp10_4_SysTick.c : �Ｚ����/LG���� ��� TV ������ �Է� ���α׷�	*/
/* ============================================================================ */
/*			                programmed by Seung-Yong Lee in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

void EXTI0_IRQHandler(void);			
void SysTick_Handler(void);			/* SysTick interrupt function */

// SS_Leader_Code	= 243243		// 9ms / 0.037us = 54000
// LG_Leader_Code	= 364864		// 13.5ms / 0.037us = 81000
// Low_Bit		= 30405			// 1.125ms / 0.037us = 6750
// High_Bit		= 60810			// 2.25ms / 0.037us = 13500

#define ARR_value	291891			// 120% of maximum Leader_Code
#define SS_Leader_Max	267567			// 110% of Leader_Code for Samsung
#define SS_Leader_Min	218918			//  90% of Leader_Code
#define LG_Leader_Max	401350			// 110% of Leader_Code for LG
#define LG_Leader_Min	328377			//  90% of Leader_Code
#define Low_Bit_Max	33445			// 110% of Low_Bit_Code
#define Low_Bit_Min	27364			//  90% of Low_Bit_Code
#define High_Bit_Max	66891			// 110% of High_Bit_Code
#define High_Bit_Min	54729			//  90% of High_Bit_Code

volatile unsigned char Remocon_state;		// step to recognize remocon signal
volatile unsigned int Remocon_count;		// 1us base time counter
volatile unsigned char Remocon_data;		// temporary byte data of remocon signal
volatile unsigned char Remocon_bit_count;	// bit count of data to recognize remocon signal
volatile unsigned char Remocon_OK_flag;		// remocon command receive OK
volatile unsigned char Remocon_command[4];	// remocon command string
volatile unsigned char format;			// data format('S' = Samsung, 'L' = LG)

/* ----- ���ͷ�Ʈ ó�� ���α׷� ----------------------------------------------- */

void EXTI0_IRQHandler(void)
{
  EXTI->PR = 0x00000001;			// clear pending bit of EXTI0 interrupt
  
  unsigned char x;

  Remocon_count = ARR_value - SysTick->VAL;	// read Remocon_count
  SysTick->VAL  = ARR_value;			// start new Remocon_count

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

}


void SysTick_Handler(void)			/* SysTick interrupt function */
{
  Remocon_state = 0;				// if overflow, the signal is noise
}

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");		// display title
  LCD_string(0xC0,"   TV remocon   ");
  LCD_string(0xC0,"   Exp10_4.c    ");
  Beep();

  TFT_string(10,10,White,Magenta," �Ｚ/LG  TV ������ ");
  TFT_string(11,16,Green,Black,  "�ڵ� : 00 00 00 00");
  TFT_string(11,19,Green,Black,  "��� :             ");
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

  SysTick->LOAD  = ARR_value;			// 120% of Leader_Code
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 216MHz/8 = 27MHz, 0.037us clock

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