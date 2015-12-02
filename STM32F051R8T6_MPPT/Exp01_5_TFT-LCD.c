/* ============================================================================ */
/*		Exp01_5_TFT-LCD.c : �⺻ TFT-LCD ��� ���α׷�		        */
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"
#include "OK-STM32F0_large.h"

void TFT_8color_horizontal(void);		/* display 8-color horizontal */

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp01_5.c    ");

  while(1)
    { TFT_clear_screen(); 			// clear screen
      TFT_landscape_mode();			// landscape mode

      TFT_string( 0, 0,Green,  Black,"****************************************");
      TFT_string( 0, 2,Magenta,Black," Cortex-M0 �ø���  OK-STM32F0 ŰƮ V1.0 ");
      TFT_string( 0, 4,Cyan,   Black,"       (TFT-2432A ���� V2.0 ����)       ");
      TFT_string( 0, 6,Green,  Black,"****************************************");
      TFT_string( 0, 8,Black,  Black,"                                        ");
      TFT_string( 0,10,Magenta,Black,"      (���÷��� ��� = Landscape)     ");
      TFT_string( 0,13,Cyan,   Black,"    3.2��ġ, 320x240 �ȼ�, 65536 �÷�   ");
      TFT_string( 0,15,White,  Black," ��Ʈ�ѷ�/����̹� HX8347-G(�븸 Himax) ");
      TFT_string( 5,18,Green,  Black,"16x16 �ѱ�");
      TFT_string(24,18,Cyan,   Black,"8x16 ����");
      TFT_string_large( 2,21,Green,  Black,"24x24 �ѱ�");
      TFT_string_large(14,21,Cyan,   Black,"12x24 ����");
      TFT_string( 0,24,Black,  Black,"                                        ");
      TFT_string( 0,26,White,  Blue, "     �������� ����ũ�����μ��� ����     ");
      TFT_string( 0,28,Green,  Black,"       (http://cpu.kongju.ac.kr/)       ");
      Beep();
      Delay_ms(5000);

      TFT_clear_screen(); 			// clear screen
      TFT_portrait_mode();			// portrait mode

      TFT_string( 0, 0,Green,  Black,"******************************");
      TFT_string( 0, 2,Magenta,Black,"     OK-STM32F0 ŰƮ V1.0     ");
      TFT_string( 0, 4,Cyan,   Black,"  (TFT-2432A ���� V2.0 ����)  ");
      TFT_string( 0, 6,Green,  Black,"******************************");
      TFT_string( 0, 8,Black,  Black,"                              ");
      TFT_string( 0,10,Magenta,Black," (���÷��� ��� = Portrait) ");
      TFT_string( 0,14,Cyan,   Black,"    3.2��ġ, 240x320 �ȼ�     ");
      TFT_string( 0,16,Yellow, Black,"          65536 �÷�          ");
      TFT_string( 0,20,Red,    Black,"    16��Ʈ CPU �������̽�     ");
      TFT_string( 0,23,White,  Black,"  ��Ʈ�ѷ�/����̹� HX8347-G  ");
      TFT_string( 0,25,White,  Black,"         (�븸 Himax)         ");
      TFT_string( 2,28,Green,  Black,"16x16 �ѱ�");
      TFT_string(18,28,Cyan,   Black,"8x16 ����");
      TFT_string_large( 0,31,Green,  Black,"24x24");
      TFT_string_large( 5,31,Magenta,Black,"�ѱ�");
      TFT_string_large(11,31,Cyan,   Black,"12x24");
      TFT_string_large(16,31,Red,    Black,"����");
      TFT_string( 0,36,White,  Blue, "�������� ����ũ�����μ��� ����");
      TFT_string( 0,38,Green,  Black,"  (http://cpu.kongju.ac.kr/)  ");
      Beep();
      Delay_ms(5000);

      TFT_clear_screen(); 			// clear screen
      TFT_landscape_mode();			// landscape mode

      TFT_8color_horizontal();			// display 8-Color(horizontal)
      TFT_string(18, 1,Black,Transparent,"���");
      TFT_string(18, 5,White,Transparent,"���");
      TFT_string(18, 8,Green,Transparent,"����");
      TFT_string(18,12,Red,  Transparent,"���");
      TFT_string(18,16,Cyan, Transparent,"û��");
      TFT_string(18,20,Black,Transparent,"���");
      TFT_string(18,24,Red,  Transparent,"��û");
      TFT_string(18,27,White,Transparent,"��ȫ");
      Beep();
      Delay_ms(5000);
    }
}

/* ----- ����� ���� �Լ� ----------------------------------------------------- */

unsigned int Color[8] =				// 8-color data
   { White, Black, Red, Green, Blue, Yellow, Cyan, Magenta };

void TFT_8color_horizontal(void)		/* display 8-color horizontal */
{
  unsigned int i, j, k;

  TFT_GRAM_address(0,0);

  for(i = 0; i < 8; i++)
    for(j = 0; j < 30; j++)
      for(k = 0; k < 320; k++)
        TFT_data(Color[i]);
}
