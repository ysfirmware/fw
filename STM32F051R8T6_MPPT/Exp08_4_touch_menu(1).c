/* ============================================================================ */
/*	Exp08_4_touch_menu(1).c : ��ġ ��ũ���� �̿��� �޴� ó�� ���α׷�	*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void Display_menu(void);			/* display menu screen */
void Menu1(void);				/* menu1 : landscape mode test */
void Menu2(void);				/* menu2 : portrait mode test */
void Menu3(void);				/* menu3 : color bar in landscape mode */
void Menu4(void);				/* menu4 : color bar in portrait mode */

unsigned int menu;				// menu number

/* ----- ���� ���α׷� -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Initialize_touch_screen();			// initialize touch screen

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp08_4.c    ");
  Beep();

  menu = 1;					// initial value

  while(1)
    { Display_menu();				// update screen menu

      Touch_screen_input();			// input touch screen
      if((x_touch >= 32) && (x_touch <= 288) && (y_touch >= 59) && (y_touch <= 82))
        { Beep();				// menu1
          Delay_ms(200);
          if(menu == 1) Menu1();
          else          menu = 1;
        }
      else if((x_touch >= 32) && (x_touch <= 288) && (y_touch >= 83) && (y_touch <= 106))
        { Beep();				// menu2
          Delay_ms(200);
          if(menu == 2) Menu2();
          else          menu = 2;
        }
      else if((x_touch >= 32) && (x_touch <= 288) && (y_touch >= 107) && (y_touch <= 130))
        { Beep();				// menu3
          Delay_ms(200);
          if(menu == 3) Menu3();
          else          menu = 3;
        }
      else if((x_touch >= 32) && (x_touch <= 288) && (y_touch >= 131) && (y_touch <= 154))
        { Beep();				// menu4
          Delay_ms(200);
          if(menu == 4) Menu4();
          else          menu = 4;
        }
      else if((x_touch >= 9) && (x_touch <= 64) && (y_touch >= 196) && (y_touch <= 235))
        { Beep();				// icon1
          Delay_ms(200);
          if(menu == 1)      Menu1();
          else if(menu == 2) Menu2();
          else if(menu == 3) Menu3();
          else if(menu == 4) Menu4();
        }
      else if((x_touch >= 176) && (x_touch <= 231) && (y_touch >= 196) && (y_touch <= 235))
        { menu--;				// icon3
          if(menu == 0) menu = 4;
  	  Beep();
          Delay_ms(200);
        }
      else if((x_touch >= 256) && (x_touch <= 311) && (y_touch >= 196) && (y_touch <= 235))
        { menu++;				// icon4
          if(menu == 5) menu = 1;
  	  Beep();
          Delay_ms(200);
        }
    }
}

/* ----- ����� ���� �Լ� ----------------------------------------------------- */

void Display_menu(void)				/* display menu screen */
{
  TFT_string(7, 3,White,Magenta," ��ġ��ũ�� �޴� ���α׷� ");	// display menu
  TFT_string(5, 8, Cyan,Black, "�޴�1 : ǳ��ȭ ��� ��ġ��ũ��");
  TFT_string(5,11, Cyan,Black, "�޴�2 : �ʻ�ȭ ��� ��ġ��ũ��");
  TFT_string(5,14,Green,Black, "�޴�3 : ǳ��ȭ ��� �÷���    ");
  TFT_string(5,17,Green,Black, "�޴�4 : �ʻ�ȭ ��� �÷���    ");

  if(menu == 1) Rectangle(32,59, 288,82, Magenta); // display selection bar
  else          Rectangle(32,59, 288,82, Black);
  if(menu == 2) Rectangle(32,83, 288,106, Magenta);
  else          Rectangle(32,83, 288,106, Black);
  if(menu == 3) Rectangle(32,107, 288,130, Magenta);
  else          Rectangle(32,107, 288,130, Black);
  if(menu == 4) Rectangle(32,131, 288,154, Magenta);
  else          Rectangle(32,131, 288,154, Black);

  Rectangle( 9,196, 64,235, Yellow);		// display touch icon
  Rectangle(10,197, 63,234, Yellow);
  TFT_string(2,25, Cyan,   Black, "ICON1");
  TFT_string(2,27, Magenta,Black, "(RUN)");
  Rectangle(176,196, 231,235, Yellow);
  Rectangle(177,197, 230,234, Yellow);
  TFT_string(23,25, Cyan,   Black, "ICON3");
  TFT_xy(24,27); TFT_color(Magenta,Black);
  TFT_English('('); TFT_English(0x18); TFT_English(')');
  Rectangle(256,196, 311,235, Yellow);
  Rectangle(257,197, 310,234, Yellow);
  TFT_string(33,25, Cyan,   Black, "ICON4");
  TFT_xy(34,27); TFT_color(Magenta,Black);
  TFT_English('('); TFT_English(0x19); TFT_English(')');
}

void Menu1(void)				/* menu1 : landscape mode test */
{
  unsigned int exit_flag;			// menu exit flag

  TFT_clear_screen();				// TFT-LCD clear screen
  TFT_landscape_mode();				// landscape mode

  Rectangle(0,0, 319,239, Green);		// display title
  Rectangle(1,1, 318,238, Green);

  TFT_string( 8,5,White,Magenta," ��ġ��ũ�� ��ǥ �׽�Ʈ ");

  TFT_string( 1, 1, Cyan,   Black, "(0,0)");
  TFT_string(32, 1, Cyan,   Black, "(319,0)");
  TFT_string( 1,27, Cyan,   Black, "(0,239)");
  TFT_string(30,27, Cyan,   Black, "(319,239)");

  TFT_string( 9, 9, Cyan,   Black, " ���� ������");
  TFT_string(21, 9, Magenta,Black, " x = 0000 ");
  TFT_string(21,11, Magenta,Black, " y = 0000 ");
  TFT_string( 9,15, Cyan,   Black, " �ȼ� ������");
  TFT_string(21,15, Magenta,Black, " x =  000 ");
  TFT_string(21,17, Magenta,Black, " y =  000 ");
  TFT_string(17,22, White,    Red, " ���� ");
  Rectangle(131,172, 188,195, White);
  Rectangle(132,173, 187,194, White);

  exit_flag = 0;
  while(exit_flag == 0)
    { Touch_screen_input();			// input touch screen

      TFT_color(Cyan,Black);
      TFT_xy(26,9);				// display 12-bit original X data
      TFT_unsigned_decimal(x_12bit,0,4);
      TFT_xy(26,11);				// display 12-bit original Y data
      TFT_unsigned_decimal(y_12bit,0,4);
      TFT_xy(27,15);				// display pixel X data
      TFT_unsigned_decimal(x_touch,0,3);
      TFT_xy(27,17);				// display pixel Y data
      TFT_unsigned_decimal(y_touch,0,3);

      if((x_touch >= 131) && (x_touch <= 188) && (y_touch >= 172) && (y_touch <= 195))
        { exit_flag = 1;
	  Beep();
          Delay_ms(200);
          TFT_clear_screen();			// TFT-LCD clear screen
          TFT_landscape_mode();			// landscape mode
        }
    }
}

void Menu2(void)				/* menu2 : portrait mode test */
{
  unsigned int exit_flag;			// menu exit flag

  TFT_clear_screen();				// TFT-LCD clear screen
  TFT_portrait_mode();				// portrait mode

  Rectangle(0,0, 239,319, Green);		// display title
  Rectangle(1,1, 238,318, Green);

  TFT_string( 3,8,White,Magenta," ��ġ��ũ�� ��ǥ �׽�Ʈ ");

  TFT_string( 1, 1, Cyan,   Black, "(0,0)");
  TFT_string(22, 1, Cyan,   Black, "(239,0)");
  TFT_string( 1,37, Cyan,   Black, "(0,319)");
  TFT_string(20,37, Cyan,   Black, "(239,319)");

  TFT_string( 4,15, Cyan,   Black, " ���� ������");
  TFT_string(16,15, Magenta,Black, " x = 0000 ");
  TFT_string(16,17, Magenta,Black, " y = 0000 ");
  TFT_string( 4,22, Cyan,   Black, " �ȼ� ������");
  TFT_string(16,22, Magenta,Black, " x =  000 ");
  TFT_string(16,24, Magenta,Black, " y =  000 ");
  TFT_string(12,30, White,    Red, " ���� ");
  Rectangle(92,236, 147,259, White);
  Rectangle(93,237, 146,258, White);

  exit_flag = 0;
  while(exit_flag == 0)
    { Touch_screen_input();			// input touch screen

      TFT_color(Cyan,Black);
      TFT_xy(21,15);				// display 12-bit original X data
      TFT_unsigned_decimal(x_12bit,0,4);
      TFT_xy(21,17);				// display 12-bit original Y data
      TFT_unsigned_decimal(y_12bit,0,4);
      TFT_xy(22,22);				// display pixel X data
      TFT_unsigned_decimal(x_touch,0,3);
      TFT_xy(22,24);				// display pixel Y data
      TFT_unsigned_decimal(y_touch,0,3);

      if((x_touch >= 92) && (x_touch <= 147) && (y_touch >= 236) && (y_touch <= 259))
        { exit_flag = 1;
	  Beep();
          Delay_ms(200);
          TFT_clear_screen();			// TFT-LCD clear screen
          TFT_landscape_mode();			// landscape mode
        }
    }
}

unsigned int Color[8] =				// 8-color data
   { White, Black, Red, Green, Blue, Yellow, Cyan, Magenta };

void Menu3(void)				/* menu3 : color bar in landscape mode */
{
  unsigned int i, j, k;

  TFT_clear_screen();				// TFT-LCD clear screen
  TFT_landscape_mode();				// landscape mode

  for(i = 0; i < 8; i++)
    { TFT_GRAM_address(0,i*30);			// display 8-color bar
      for(j = 0; j < 30; j++)
        for(k = 0; k < 320; k++)
          TFT_data(Color[i]);
      if(i == 0)				// display name of color bar
        TFT_string(14, 1, Black,Transparent, "���(White)");
      else if(i == 1)
        TFT_string(14, 5, White,Transparent, "���(Black)");
      else if(i == 2)
        TFT_string(14, 8, Green,Transparent, "����(Red)");
      else if(i == 3)
        TFT_string(14,12, Red,  Transparent, "���(Green)");
      else if(i == 4)
        TFT_string(14,16, Cyan, Transparent, "û��(Blue)");
      else if(i == 5)
        TFT_string(14,20, Black,Transparent, "���(Yellow)");
      else if(i == 6)
        TFT_string(14,24, Red,  Transparent, "��û(Cyan)");
      else if(i == 7)
        TFT_string(14,27, White,Transparent, "��ȫ(Magenta)");
      Delay_ms(500);
    }
  Delay_ms(2000);

  TFT_clear_screen();				// TFT-LCD clear screen
  TFT_landscape_mode();				// landscape mode
}

void Menu4(void)				/* menu4 : color bar in portrait mode */
{
  unsigned int i, j, k;

  TFT_clear_screen();				// TFT-LCD clear screen
  TFT_portrait_mode();				// portrait mode

  for(i = 0; i < 8; i++)
    { TFT_GRAM_address(0,i*40);			// display 8-color bar
      for(j = 0; j < 40; j++)
        for(k = 0; k < 240; k++)
          TFT_data(Color[i]);
      if(i == 0)				// display name of color bar
        TFT_string(9, 2, Black,Transparent, "���(White)");
      else if(i == 1)
        TFT_string(9, 7, White,Transparent, "���(Black)");
      else if(i == 2)
        TFT_string(9,12, Green,Transparent, "����(Red)");
      else if(i == 3)
        TFT_string(9,17, Red,  Transparent, "���(Green)");
      else if(i == 4)
        TFT_string(9,22, Cyan, Transparent, "û��(Blue)");
      else if(i == 5)
        TFT_string(9,27, Black,Transparent, "���(Yellow)");
      else if(i == 6)
        TFT_string(9,32, Red,  Transparent, "��û(Cyan)");
      else if(i == 7)
        TFT_string(9,37, White,Transparent, "��ȫ(Magenta)");
      Delay_ms(500);
    }
  Delay_ms(2000);

  TFT_clear_screen();				// TFT-LCD clear screen
  TFT_landscape_mode();				// landscape mode
}
