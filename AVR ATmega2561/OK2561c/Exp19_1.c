/* ========================================================================== */
/*                Exp19_1.c : English/Korean Language Display                 */
/* ========================================================================== */
/*                        Designed and programmed by Duck-Yong Yoon in 2006.  */

#include <avr/io.h>
#include "OK2561.h"
#include "GLCD2561.h"

int main(void)
{ unsigned char i;

  MCU_initialize();                             // initialize MCU and kit
  Delay_ms(50);                                 // wait for system stabilization
  LCD_initialize();                             // initialize text LCD module
  Beep();

  GLCD_clear();                                 // initialize GLCD screen
  cursor_flag = 0;                              // cursor off

  LCD_string(0x80,"Graphic LCD Test");          // display title on text LCD
  LCD_string(0xC0," English/Korean ");

  while(1)
    { GLCD_string(0,0,"OK-2561키트 V1.0");      // display screen 1
      GLCD_string(1,0,"   2006/10/01   ");
      GLCD_string(2,0," 설계 : 윤덕용  ");
      GLCD_string(3,0," 판매 : Ohm사   ");
      Delay_ms(5000);

      GLCD_string(0,0,"    현대 LCD    ");      // display screen 2
      GLCD_string(1,0,"  HG12605NY-LY  ");
      GLCD_string(2,0,"                ");
      GLCD_string(3,0,"128x64 그래픽LCD");
      Delay_ms(5000);

      GLCD_string(0,0," 영문/한글 표시 ");      // display screen 3
      GLCD_string(1,0,"                ");
      GLCD_string(2,0," 8x16도트 ASCII ");
      GLCD_string(3,0,"16x16도트 조합형");
      Delay_ms(5000);

      GLCD_string(0,0,"   * 배우자 *   ");      // display screen 4
      GLCD_string(1,0,"마이크로프로세서");
      GLCD_string(2,0,"   * 익히자 *   ");
      GLCD_string(3,0,"마이크로콘트롤러");
      Delay_ms(5000);

      GLCD_xy(0,0);
      for(i=0x00; i<=0x3F; i++)                 // from 0x00 to 0x3F
        GLCD_English(i);
      Delay_ms(5000);
      GLCD_xy(0,0);
      for(i=0x40; i<=0x7F; i++)                 // from 0x40 to 0x7F
        GLCD_English(i);
      Delay_ms(5000);
    }
}
