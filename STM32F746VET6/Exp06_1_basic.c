/* ============================================================================ */
/*		Exp06_1_basic.c : TFT-LCD �ѱ� ��� ���α׷�			*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");          // display title
  LCD_string(0xC0,"   Exp06_1.c    ");

  while(1)
    { TFT_clear_screen();			// clear screen
      TFT_landscape_mode();			// landscape mode
      Beep();

      TFT_string( 0, 0,Green,  Black,"****************************************");
      TFT_string( 0, 2,Magenta,Black,"  Cortex-M7 �ø��� OK-STM746 ŰƮ V1.0  ");
      TFT_string( 0, 4,Cyan,   Black,"        (TFT-32A ���� V1.0 ����)        ");
      TFT_string( 0, 6,Green,  Black,"****************************************");
      TFT_string( 0, 8,Black,  Black,"                                        ");
      TFT_string( 0,10,Magenta,Black,"      (���÷��� ��� = Landscape)     ");
      TFT_string( 0,14,Cyan,   Black,"    3.2��ġ, 320x240 �ȼ�, 65536 �÷�   ");
      TFT_string( 0,16,White,  Black," ��Ʈ�ѷ�/����̹� HX8347-A(�븸 Himax) ");
      TFT_string( 0,20,Red,    Black," ���� Red    ");
      TFT_string(13,20,Green,  Black," ��� Green  ");
      TFT_string(26,20,Blue,   Black," �Ķ� Blue    ");
      TFT_string( 0,22,Yellow, Black," ��� Yellow ");
      TFT_string(13,22,Cyan,   Black," ��û Cyan   ");
      TFT_string(26,22,Magenta,Black," ��ȫ Magenta ");
      TFT_string( 0,24,Black,  Black,"                                        ");
      TFT_string( 0,26,White,  Blue, "     �������� ����ũ�����μ��� ����     ");
      TFT_string( 0,28,Green,  Black,"    (http://cafe.naver.com/mcu32bit)    ");
      Delay_ms(5000);

      TFT_clear_screen(); 			// clear screen
      TFT_portrait_mode();			// portrait mode
      Beep();

      TFT_string( 0, 0,Green,  Black,"******************************");
      TFT_string( 0, 2,Magenta,Black,"     OK-STM746 ŰƮ  V1.0     ");
      TFT_string( 0, 4,Cyan,   Black,"   (TFT-32A ���� V1.0 ����)   ");
      TFT_string( 0, 6,Green,  Black,"******************************");
      TFT_string( 0, 8,Black,  Black,"                              ");
      TFT_string( 0,10,Magenta,Black," (���÷��� ��� = Portrait) ");
      TFT_string( 0,14,Cyan,   Black,"    3.2��ġ, 240x320 �ȼ�     ");
      TFT_string( 0,16,Yellow, Black,"          65536 �÷�          ");
      TFT_string( 0,20,Red,    Black,"    16��Ʈ CPU �������̽�     ");
      TFT_string( 0,24,White,  Black,"  ��Ʈ�ѷ�/����̹� HX8347-A  ");
      TFT_string( 0,26,White,  Black,"         (�븸 Himax)         ");
      TFT_string( 0,30,Red,    Black," ���� Red ");
      TFT_string(10,30,Green,  Black," ��� Gre ");
      TFT_string(20,30,Blue,   Black," �Ķ� Blu ");
      TFT_string( 0,32,Yellow, Black," ��� Yel ");
      TFT_string(10,32,Cyan,   Black," ��û Cya ");
      TFT_string(20,32,Magenta,Black," ��ȫ Mag ");
      TFT_string( 0,36,White,  Blue, "�������� ����ũ�����μ��� ����");
      TFT_string( 0,38,Green,  Black,"http://cafe.naver.com/mcu32bit");
      Delay_ms(5000);
    }
}
