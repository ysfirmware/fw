/* ============================================================================ */
/*	Exp06_5_string(1).c : TFT-LCD�� �� ���ڿ��� ����ϴ� ���α׷�		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");          // display title
  LCD_string(0xC0,"   Exp06_5.c    ");

  while(1)
    { TFT_clear_screen();			// display screen 1
      Beep();

      TFT_landscape_mode();			// landscape mode
      TFT_string(11, 2,White, Magenta," ���⿡ ���� �ͼ� ");
      Delay_ms(1000);
      TFT_string(17, 6,Red,   Black,"(����)");
      Delay_ms(1000);
      TFT_string( 0, 9,Green, Black,"  ���� �ڶ������� �±ر� �տ� �����Ӱ� ���Ƿο� ���ѹα��� ������ ������ ���Ͽ� �漺�� ���� ���� ���� �����մϴ�.");
      Delay_ms(1000);
      TFT_string(15,18,Red,   Black,"(�� ����)");
      Delay_ms(1000);
      TFT_string( 0,21,Yellow,Black,"  ���� �ڶ����� �±ر� �տ� ������ ������ ������ ������ ���Ͽ� ���� ������ ���� �漺�� ���� ���� ���� �����մϴ�.");
      Delay_ms(5000);

      TFT_clear_screen();			// display screen 2
      Beep();

      TFT_portrait_mode();			// portrait mode
      TFT_string( 9, 1,White, Magenta," ���޷� �� ");
      Delay_ms(1000);
      TFT_string(18, 5,Green, Black,"��ҿ�");
      Delay_ms(1000);
      TFT_string( 0, 9,Cyan,  Black,"�����Ⱑ ���ܿ�");
      Delay_ms(1000);
      TFT_string( 0,11,Cyan,  Black,"���� ������");
      Delay_ms(1000);
      TFT_string( 0,13,Cyan,  Black,"������ ���� �����帮������.");
      Delay_ms(1000);
      TFT_string( 0,17,Yellow,Black,"������ ���");
      Delay_ms(1000);
      TFT_string( 0,19,Yellow,Black,"���޷���");
      Delay_ms(1000);
      TFT_string( 0,21,Yellow,Black,"�Ƹ����� ���� �濡 �Ѹ�������.");
      Delay_ms(1000);
      TFT_string( 0,25,Cyan,  Black,"���ô� ���� ����");
      Delay_ms(1000);
      TFT_string( 0,27,Cyan,  Black,"���� �� ����");
      Delay_ms(1000);
      TFT_string( 0,29,Cyan,  Black,"����� ������ ���ÿɼҼ�.");
      Delay_ms(1000);
      TFT_string( 0,33,Red,   Black,"�����Ⱑ ���ܿ�");
      Delay_ms(1000);
      TFT_string( 0,35,Red,   Black,"���� ������");
      Delay_ms(1000);
      TFT_string( 0,37,Red,   Black,"�׾ �ƴ� ���� �긮������.");
      Delay_ms(3000);
    }
}
