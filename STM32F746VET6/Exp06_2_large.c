/* ============================================================================ */
/*		Exp06_2_large.c : TFT-LCD ū �۾� ��� ���α׷�			*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"
#include "OK-STM746_large.h"
#include "OK-STM746_xlarge.h"

int main(void)
{
  unsigned char i;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80," OK-STM746 V1.0 ");          // display title
  LCD_string(0xC0,"   Exp06_2.c    ");

  while(1)
    { TFT_clear_screen(); 			// clear screen
      TFT_landscape_mode();			// landscape mode
      Beep();

      TFT_string(0, 0,Green,  Black,"****************************************");
      TFT_string(0, 2,Magenta,Black,"      OK-STM746 Ʈ���̴� ŰƮ V1.0      ");
      TFT_string(0, 4,Cyan,   Black,"        (TFT-32A ���� V1.0 ����)        ");
      TFT_string(0, 6,Green,  Black,"****************************************");
      TFT_string(0, 9,Magenta,Black,"      (���÷��� ��� = Landscape)     ");

      TFT_string(0,13,Cyan,   Black,"        8x16 English ASCII Normal       ");
      TFT_string(0,15,Yellow, Black,"       16x16 �ѱ� ���� �۾� ����ü      ");

      TFT_string_large(1,18, Cyan,  Black, "12x24 English ASCII Large");
      TFT_string_large(1,21, Yellow,Black, "24x24 �ѱ� ū �۾� ����ü");

      TFT_string(0,26,White,  Blue, "     �������� ����ũ�����μ��� ����     ");
      TFT_string(0,28,Green,  Black,"    (http://cafe.naver.com/mcu32bit)    ");
      Delay_ms(3000);

      Beep();
      Kfont_type = 'G';
      TFT_string(0,15,Yellow, Black,"       16x16 �ѱ� ���� �۾� ���ü      ");
      Kfont_type_large = 'G';
      TFT_string_large(1,21, Yellow,Black, "24x24 �ѱ� ū �۾� ���ü");
      Kfont_type = 'M';
      Kfont_type_large = 'M';
      Delay_ms(3000);

      Beep();
      Kfont_type = 'P';
      TFT_string(0,15,Yellow, Black,"       16x16 �ѱ� ���� �۾� �ʱ�ü      ");
      Kfont_type_large = 'P';
      TFT_string_large(1,21, Yellow,Black, "24x24 �ѱ� ū �۾� �ʱ�ü");
      Kfont_type = 'M';
      Kfont_type_large = 'M';
      Delay_ms(3000);

      Beep();
      TFT_string(0, 9,Cyan,Black,"       16x16 �ѱ� ���� �۾� ����ü      ");
      Kfont_type = 'G';
      TFT_string(0,11,Cyan,Black,"       16x16 �ѱ� ���� �۾� ���ü      ");
      Kfont_type = 'P';
      TFT_string(0,13,Cyan,Black,"       16x16 �ѱ� ���� �۾� �ʱ�ü      ");
      Kfont_type = 'M';
      TFT_string_large(1,15, Yellow,Black, "24x24 �ѱ� ū �۾� ����ü");
      Kfont_type_large = 'G';
      TFT_string_large(1,18, Yellow,Black, "24x24 �ѱ� ū �۾� ���ü");
      Kfont_type_large = 'P';
      TFT_string_large(1,21, Yellow,Black, "24x24 �ѱ� ū �۾� �ʱ�ü");
      Kfont_type_large = 'M';
      Delay_ms(3000);

      TFT_string(0, 9,Cyan,Black,"                                        ");
      TFT_string(0,11,Cyan,Black,"                                        ");
      TFT_string(0,13,Cyan,Black,"                                        ");
      TFT_string_large(1,15, Yellow,Black, "                         ");
      TFT_string_large(1,18, Yellow,Black, "                         ");
      TFT_string_large(1,21, Yellow,Black, "                         ");
      Beep();
      TFT_string(8, 8,Magenta,Black,"32x48 ���� ū �۾� ����");
      TFT_xy(0,10);
      TFT_color(Cyan,Black);
      for(i = 0; i < 10; i++)
        TFT_English_vlarge(i + '0');
      TFT_string(8,17,Magenta,Black,"40x56 ���� ū �۾� ����");
      TFT_xy(0,19);
      TFT_color(Yellow,Black);
      for(i = 0; i < 8; i++)
        TFT_English_xlarge(i + '0');
      Delay_ms(3000);

      TFT_clear_screen(); 			// clear screen
      TFT_portrait_mode();			// portrait mode
      Beep();

      TFT_string(0, 0,Green,  Black,"******************************");
      TFT_string(0, 2,Magenta,Black," OK-STM746 Ʈ���̴� ŰƮ V1.0 ");
      TFT_string(0, 4,Cyan,   Black,"   (TFT-32A ���� V1.0 ����)   ");
      TFT_string(0, 6,Green,  Black,"******************************");
      TFT_string(0,10,Magenta,Black," (���÷��� ��� = Portrait) ");

      TFT_string(0,16,Cyan,   Black,"     8x16 English Normal      ");
      TFT_string(0,19,Yellow, Black,"    16x16 �ѱ� ���� ����ü    ");

      TFT_string_large(1,24, Cyan,  Black, "12x24 English Large");
      TFT_string_large(1,28, Yellow,Black, "24x24 �ѱ� ū ����");

      TFT_string(0,36,White,  Blue, "�������� ����ũ�����μ��� ����");
      TFT_string(0,38,Green,  Black,"http://cafe.naver.com/mcu32bit");
      Delay_ms(3000);

      Beep();
      Kfont_type = 'G';
      TFT_string(0,19,Yellow, Black,"    16x16 �ѱ� ���� ���ü    ");
      Kfont_type_large = 'G';
      TFT_string_large(1,28, Yellow,Black, "24x24 �ѱ� ū ���");
      Kfont_type = 'M';
      Kfont_type_large = 'M';
      Delay_ms(3000);

      Beep();
      Kfont_type = 'P';
      TFT_string(0,19,Yellow, Black,"    16x16 �ѱ� ���� �ʱ�ü    ");
      Kfont_type_large = 'P';
      TFT_string_large(1,28, Yellow,Black, "24x24 �ѱ� ū �ʱ�");
      Kfont_type = 'M';
      Kfont_type_large = 'M';
      Delay_ms(3000);

      Beep();
      TFT_string(0,15,Cyan,Black,"    16x16 �ѱ� ���� ����ü    ");
      Kfont_type = 'G';
      TFT_string(0,17,Cyan,Black,"    16x16 �ѱ� ���� ���ü    ");
      Kfont_type = 'P';
      TFT_string(0,19,Cyan,Black,"    16x16 �ѱ� ���� �ʱ�ü    ");
      Kfont_type = 'M';
      TFT_string_large(1,23, Yellow,Black, "24x24 �ѱ� ū ���� ");
      Kfont_type_large = 'G';
      TFT_string_large(1,26, Yellow,Black, "24x24 �ѱ� ū ��� ");
      Kfont_type_large = 'P';
      TFT_string_large(1,29, Yellow,Black, "24x24 �ѱ� ū �ʱ� ");
      Kfont_type_large = 'M';
      Delay_ms(3000);

      TFT_string_large(1,23, Yellow,Black, "                   ");
      TFT_string_large(1,26, Yellow,Black, "                   ");
      TFT_string_large(1,29, Yellow,Black, "                   ");
      Beep();
      TFT_string(3,15,Magenta,Black,"32x48 ���� ū �۾� ����");
      TFT_xy(1,17);
      TFT_color(Cyan,Black);
      for(i = 0; i < 7; i++)
        TFT_English_vlarge(i + '0');
      TFT_string(3,24,Magenta,Black,"40x56 ���� ū �۾� ����");
      TFT_xy(0,26);
      TFT_color(Yellow,Black);
      for(i = 0; i < 6; i++)
        TFT_English_xlarge(i + '0');
      Delay_ms(3000);
    }
}
