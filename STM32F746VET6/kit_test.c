/* ============================================================================ */
/*	    kit_test.c : OK-STM746 키트 V1.0 하드웨어 진단 프로그램		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2015.   */

#include "stm32f746xx.h"
#include "OK-STM746.h"
#include "OK-STM746_large.h"
#include "OK-STM746_xlarge.h"

/* ----- 사용자 함수 ---------------------------------------------------------- */

void Display_screen(void);			// display test screen
void Display_test(U08 current);			// display test number
void Test1(void);				// Test 1
void TFT_8color_horizontal(void);		// display 8 color bars
void Test2(void);				// Test 2
void Test3(void);				// Test 3
void Test4(void);				// Test 4
void Test5(void);				// Test 5
void Test6(void);				// Test 6
void Test7(void);				// Test 7
void DS3234_initialize(void);			// check DS3234 RTC
unsigned char DS3234_read(U16 address);		// read DS3234
void DS3234_write(U16 address, U08 value);	// write DS3234 
void Digital_display_time(void);		// display DS3234 time
unsigned char BCD_increment(U08 number);	// BCD increment
unsigned char BCD_decrement(U08 number);	// BCD decrement
void Digital_cursor_left(void);			// go cursor left
void Digital_cursor_right(void);		// go cursor right
void Digital_increment(void);			// increment time
void Digital_decrement(void);			// decrement time
void Test8(void);				// Test 8
void Analog_cursor_left(void);			// go cursor left
void Analog_cursor_right(void);             	// go cursor right
void Analog_increment(void);	               	// increment time
void Analog_decrement(void);	               	// decrement time
void Draw_analog_clock(void);			// draw analog clock
void Analog_display_time(void);			// display analog and digital time
void Draw_pin(void);				// draw hour, minute, second pin
void Line_soft(S16 x1,S16 y1, S16 x2,S16 y2, U16 color); // draw a soft line


#ifdef ILI9320//[[ YSKim_151218
static void LCD_SetPos(U32 x0,U32 x1,U32 y0,U32 y1);
void ClearScreen(U32 bColor);


//===============================================================
//CLEAR SCREEN
void ClearScreen(U32 bColor)
{
 unsigned int i,j;
 LCD_SetPos(0,239,0,319);//320x240
 for (i=0;i<320;i++)
	{
	
	   for (j=0;j<240;j++)
	       TFT_data(bColor);
	}
}


//===============================================================
//Define the coordinate


static void LCD_SetPos(U32 x0,U32 x1,U32 y0,U32 y1)
{
  TFT_write(0x0050,x0);
  TFT_write(0x0051,x1);
  TFT_write(0x0052,y0);
  TFT_write(0x0053,y1);
  TFT_write(0x0020,x0);
  TFT_write(0x0021,y0);
  TFT_command(0x0022);//LCD_WriteCMD(GRAMWR);
}
#endif //]] YSKim_151218



/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  unsigned char key = no_key, test = 1;

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Beep();					// beep

  LCD_string(0x80," OK-STM746 V1.0 ");		// display title
  LCD_string(0xC0,"   kit_test.c   ");

  TFT_string_large(1, 5,Cyan,   Black,"  OK-STM746 키트  V1.0  ");
  TFT_string_large(1,10,White,Magenta," 하드웨어 진단 프로그램 ");
  TFT_string(6,19,White,  Black,"    2015년 11월 01일(일)    ");
  TFT_string(6,23,Green,  Black,"        윤덕용  작성        ");
  Delay_ms(3000);

  Display_screen();				// display test screen
  Display_test(test);				// display test number
  Beep();					// beep

  while(1){
		
#ifdef ILI9320  //[[ YSKim_151218
		ClearScreen(0x0000);	//CLEAR DISPLAY
		
		ClearScreen(0xf800);	//RED
		Delay_ms(100);
		ClearScreen(0x07e0);	//GREEN
		Delay_ms(100);
		ClearScreen(0x001f);	//BLUE
		Delay_ms(100);
		ClearScreen(0xffff);	//WHITE
		Delay_ms(100);
		
		ClearScreen(0x0eff);	//CYAN
		Delay_ms(100);
		ClearScreen(0xffe0);	//YELLOW
		Delay_ms(100);
		ClearScreen(0xf81f);	//PINK	
		Delay_ms(100);
#endif //]] YSKim_151218
		
#ifndef ILI9320  //[[ YSKim_151217
     while(key == no_key)			// wait key input
        key = Key_input();

      if(key == KEY3)				// if KEY3, decrenment test number
       { key = no_key;
         test--;
         if(test == 0) test = 8;
         Display_test(test);
       }
      else if(key == KEY4)			// if KEY4, increnment test number
       { key = no_key;
         test++;
         if(test > 8) test = 1;
         Display_test(test);
       }
      else if(key == KEY2)			// if KEY2, ignore and skip
       key = no_key;
      else if(key == KEY1)			// if KEY1, run test number
       { key = no_key;
         if(test == 1) Test1();
         else if(test == 2) Test2();
         else if(test == 3) Test3();
         else if(test == 4) Test4();
         else if(test == 5) Test5();
         else if(test == 6) Test6();
         else if(test == 7) Test7();
         else if(test == 8) Test8();

         Display_screen();			// display test screen
         Display_test(test);			// display test number
       }
#endif //]] YSKim_151217
    }
}

/* ---------------------------------------------------------------------------- */

void Display_screen(void)			/* display test screen */
{
  LCD_string(0xC0,"   kit_test.c   ");

  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode

  TFT_string( 0, 0,White,Magenta,"   OK-STM746 키트 V1.0  진단 프로그램   ");
  TFT_string( 0, 3,Cyan,   Black,"   (1) TFT-LCD 컬러 디스플레이 테스트");
  TFT_string( 0, 6,Green,  Black,"   (2) TFT-LCD 터치스크린 입력 테스트");
  TFT_string( 0, 9,Cyan,   Black,"   (3) MCO1/2 단자 클록 출력 테스트");
  TFT_string( 0,12,Green,  Black,"   (4) A/D 컨버터 입력 테스트");
  TFT_string( 0,15,Cyan,   Black,"   (5) D/A 컨버터 출력 테스트");
  TFT_string( 0,18,Green,  Black,"   (6) RS-232C 직렬통신 루프백 테스트");
  TFT_string( 0,21,Cyan,   Black,"   (7) DS3234를 이용한 디지털 시계");
  TFT_string( 0,24,Green,  Black,"   (8) DS3234를 이용한 아날로그 시계");
  TFT_string( 0,28,White,   Blue,"KEY1(    )");
  TFT_string(20,28,White,   Blue," KEY3( ) ");
  TFT_string(31,28,White,   Blue," KEY4( ) ");
  TFT_string( 5,28,Red,     Blue,"실행");
  TFT_xy(26,28); TFT_English(0x18);
  TFT_xy(37,28); TFT_English(0x19);
}

void Display_test(U08 current)			/* display test number */
{
  TFT_string(0, 3,Cyan,   Black,"  ");
  TFT_string(0, 6,Green,  Black,"  ");
  TFT_string(0, 9,Cyan,   Black,"  ");
  TFT_string(0,12,Green,  Black,"  ");
  TFT_string(0,15,Cyan,   Black,"  ");
  TFT_string(0,18,Green,  Black,"  ");
  TFT_string(0,21,Cyan,   Black,"  ");
  TFT_string(0,24,Green,  Black,"  ");
  TFT_string(0,current*3, Yellow,Black,">>");
}
/* ---------------------------------------------------------------------------- */

void Test1(void)				/* Test 1 */
{
  unsigned short i, x, y;

  LCD_string(0xC0,"     Test 1     ");

  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode

  TFT_string( 0, 0,Green,  Black,"****************************************");
  TFT_string( 0, 2,Magenta,Black,"  Cortex-M7 시리즈 OK-STM746 키트 V1.0  ");
  TFT_string( 0, 4,Cyan,   Black,"        (TFT-32A 보드 V1.0 포함)        ");
  TFT_string( 0, 6,Green,  Black,"****************************************");
  TFT_string( 0, 8,Black,  Black,"                                        ");
  TFT_string( 0,10,Magenta,Black,"      (디스플레이 모드 = Landscape)     ");
  TFT_string( 0,14,Cyan,   Black,"    3.2인치, 320x240 픽셀, 65536 컬러   ");
  TFT_string( 0,16,White,  Black," 컨트롤러/드라이버 HX8347-A(대만 Himax) ");
  TFT_string( 0,20,Red,    Black," 빨강 Red    ");
  TFT_string(13,20,Green,  Black," 녹색 Green  ");
  TFT_string(26,20,Blue,   Black," 파랑 Blue    ");
  TFT_string( 0,22,Yellow, Black," 노랑 Yellow ");
  TFT_string(13,22,Cyan,   Black," 녹청 Cyan   ");
  TFT_string(26,22,Magenta,Black," 자홍 Magenta ");
  TFT_string( 0,24,Black,  Black,"                                        ");
  TFT_string( 0,26,White,  Blue, "     윤교수의 마이크로프로세서 월드     ");
  TFT_string( 0,28,Green,  Black,"    (http://cafe.naver.com/mcu32bit)    ");
  Delay_ms(5000);

  TFT_clear_screen();				// clear screen

  TFT_string( 0, 0,Green,  Black,"****************************************");
  TFT_string( 0, 2,Magenta,Black,"  Cortex-M7 시리즈 OK-STM746 키트 V1.0  ");
  TFT_string( 0, 4,Cyan,   Black,"        (TFT-32A 보드 V1.0 포함)        ");
  TFT_string( 0, 6,Green,  Black,"****************************************");
  Kfont_type = 'M';
  TFT_string( 0, 9,Cyan,Black,"   8x16 영문 ASCII, 16x16 한글 명조체   ");
  Kfont_type = 'G';
  TFT_string( 0,11,Cyan,Black,"   8x16 영문 ASCII, 16x16 한글 고딕체   ");
  Kfont_type = 'P';
  TFT_string( 0,13,Cyan,Black,"   8x16 영문 ASCII, 16x16 한글 필기체   ");
  Kfont_type = 'M';
  Kfont_type_large = 'M';
  TFT_string_large( 0,16,Yellow,Black," 12x24 ASCII, 24x24 명조체 ");
  Kfont_type_large = 'G';
  TFT_string_large( 0,19,Yellow,Black," 12x24 ASCII, 24x24 고딕체 ");
  Kfont_type_large = 'P';
  TFT_string_large( 0,22,Yellow,Black," 12x24 ASCII, 24x24 필기체 ");
  Kfont_type_large = 'M';
  TFT_string( 0,26,White,  Blue, "     윤교수의 마이크로프로세서 월드     ");
  TFT_string( 0,28,Green,  Black,"    (http://cafe.naver.com/mcu32bit)    ");
  Beep();
  Delay_ms(5000);

  TFT_clear_screen();				// clear screen

  TFT_string( 0, 0,Green,  Black,"****************************************");
  TFT_string( 0, 2,Magenta,Black,"  Cortex-M7 시리즈 OK-STM746 키트 V1.0  ");
  TFT_string( 0, 4,Cyan,   Black,"        (TFT-32A 보드 V1.0 포함)        ");
  TFT_string( 0, 6,Green,  Black,"****************************************");
  TFT_string( 8, 8,Magenta,Black,"32x48 아주 큰 글씨 숫자");
  TFT_xy(0,10);
  TFT_color(Cyan,Black);
  for(i = 0; i < 10; i++)
    TFT_English_vlarge(i + '0');
  TFT_string( 8,17,Magenta,Black,"40x56 가장 큰 글씨 숫자");
  TFT_xy(0,19);
  TFT_color(Yellow,Black);
  for(i = 0; i < 8; i++)
    TFT_English_xlarge(i + '0');
  TFT_string( 0,26,White,  Blue, "     윤교수의 마이크로프로세서 월드     ");
  TFT_string( 0,28,Green,  Black,"    (http://cafe.naver.com/mcu32bit)    ");
  Beep();
  Delay_ms(5000);

  TFT_clear_screen(); 				// clear screen
  TFT_portrait_mode();				// portrait mode

  TFT_string( 0, 0,Green,  Black,"******************************");
  TFT_string( 0, 2,Magenta,Black,"     OK-STM746 키트  V1.0     ");
  TFT_string( 0, 4,Cyan,   Black,"   (TFT-32A 보드 V1.0 포함)   ");
  TFT_string( 0, 6,Green,  Black,"******************************");
  TFT_string( 0, 8,Black,  Black,"                              ");
  TFT_string( 0,10,Magenta,Black," (디스플레이 모드 = Portrait) ");
  TFT_string( 0,14,Cyan,   Black,"    3.2인치, 240x320 픽셀     ");
  TFT_string( 0,16,Yellow, Black,"          65536 컬러          ");
  TFT_string( 0,20,Red,    Black,"    16비트 CPU 인터페이스     ");
  TFT_string( 0,24,White,  Black,"  컨트롤러/드라이버 HX8347-A  ");
  TFT_string( 0,26,White,  Black,"         (대만 Himax)         ");
  TFT_string( 0,30,Red,    Black," 빨강 Red ");
  TFT_string(10,30,Green,  Black," 녹색 Gre ");
  TFT_string(20,30,Blue,   Black," 파랑 Blu ");
  TFT_string( 0,32,Yellow, Black," 노랑 Yel ");
  TFT_string(10,32,Cyan,   Black," 녹청 Cya ");
  TFT_string(20,32,Magenta,Black," 자홍 Mag ");
  TFT_string( 0,36,White,  Blue, "윤교수의 마이크로프로세서 월드");
  TFT_string( 0,38,Green,  Black,"http://cafe.naver.com/mcu32bit");
  Beep();
  Delay_ms(5000);

  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode

  TFT_8color_horizontal();			// display 8 color bars
  TFT_string(18, 1,Black,Transparent,"백색");
  TFT_string(18, 5,White,Transparent,"흑색");
  TFT_string(18, 8,Green,Transparent,"적색");
  TFT_string(18,12,Red,  Transparent,"녹색");
  TFT_string(18,16,Cyan, Transparent,"청색");
  TFT_string(18,20,Black,Transparent,"노랑");
  TFT_string(18,24,Red,  Transparent,"녹청");
  TFT_string(18,27,White,Transparent,"자홍");
  Beep();
  Delay_ms(5000);

  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode
  Beep();

  Rectangle(0,0, 319,239, Magenta);		// graphic show of line
  Rectangle(1,1, 318,238, Magenta);
  for(x = 10; x <= 310; x += 10)
    { Line(x,0, x,239, Green);
      Delay_ms(20);
    }
  for(y = 10; y <= 230; y += 10)
    { Line(0,y, 319,y, Green);
      Delay_ms(20);
    }
  Delay_ms(3000);
  TFT_clear_screen();

  Rectangle(0,0, 319,239, Magenta);		// graphic show of ramp
  Rectangle(1,1, 318,238, Magenta);
  for(i = 0,x = 20,y = 12; i <= 34; i++,x += 20,y += 12)
    { Line(x,0, 0,y, Red);
      Line(0,239-y, x,239, Red);
      Delay_ms(20);
    }
  for(i = 0,x = 700,y = 420; i <= 34; i++,x -= 20,y -= 12)
    { Line(x,0, 0,y, Cyan);
      Line(0,239-y, x,239, Cyan);
      Delay_ms(20);
    }
  Delay_ms(3000);
  TFT_clear_screen();

  Rectangle(0,0, 319,239, Magenta);		// graphic show of rectangle
  Rectangle(1,1, 318,238, Magenta);
  for(i = 0,x = 152,y = 114; i <= 18; i++,x -= 8,y -= 6)
    { Rectangle(x,y, 319-x,239-y, Cyan);
      Delay_ms(20);
    }
  Delay_ms(1000);
  for(i = 0,x = 8,y = 6; i <= 18; i++,x += 8,y += 6)
    { Rectangle(x,y, 319-x,239-y, Red);
      Delay_ms(20);
    }
  for(i = 0,x = 152,y = 114; i <= 18; i++,x -= 8,y -= 6)
    { Rectangle(x,y, 319-x,239-y, Green);
      Delay_ms(20);
    }
  Delay_ms(3000);
  TFT_clear_screen();

  Rectangle(0,0, 319,239, Magenta);		// graphic show of circle
  Rectangle(1,1, 318,238, Magenta);
  for(i = 10; i <= 160; i += 10)
    { Circle(160,120,i, Green);
      Delay_ms(20);
    }
  Delay_ms(1000);
  for(i = 160; i >= 10; i -= 10)
    { Circle(160,120,i, Red);
      Delay_ms(20);
    }
  for(i = 10; i <= 160; i += 10)
    { Circle(160,120,i, White);
      Delay_ms(20);
    }
  Delay_ms(3000);
  TFT_clear_screen();

  Rectangle(0,0, 319,239, Magenta);		// graphic show of 2 circles
  Rectangle(1,1, 318,238, Magenta);
  for(i = 10; i <= 120; i += 10)
    { Circle(80,120,i, Green);
      Circle(240,120,i, Green);
      Delay_ms(20);
    }
  Delay_ms(1000);
  for(i = 120; i >= 10; i -= 10)
    { Circle(80,120,i, Red);
      Circle(240,120,i, Red);
      Delay_ms(20);
    }
  for(i = 10; i <= 120; i += 10)
    { Circle(80,120,i, Yellow);
      Circle(240,120,i, Yellow);
      Delay_ms(20);
    }
  Delay_ms(3000);
  TFT_clear_screen();

  Rectangle(0,0, 319,239, Magenta);		// graphic show of sine curve
  Rectangle(1,1, 318,238, Magenta);
  Line(0,120, 319,120, Magenta);
  for(i = 10; i <= 120; i += 10)
    { Sine(i, 0, Green);
      Delay_ms(20);
    }
  Delay_ms(1000);
  for(i = 10; i <= 120; i += 10)
    { Sine(i, 1, Blue);
      Delay_ms(20);
    }
  Delay_ms(3000);
  Beep();
}

unsigned int Color[8] =				// 8-color data
   { White, Black, Red, Green, Blue, Yellow, Cyan, Magenta };

void TFT_8color_horizontal(void)		/* display 8 color bars */
{
  unsigned int i, j, k;

  TFT_GRAM_address(0,0);

  for(i = 0; i < 8; i++)
    for(j = 0; j < 30; j++)
      for(k = 0; k < 320; k++)
        TFT_data(Color[i]);
}

/* ---------------------------------------------------------------------------- */

void Test2(void)				/* Test 2 */
{
  LCD_string(0xC0,"     Test 2     ");

  Initialize_touch_screen();			// initialize touch screen
  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode

  Rectangle(0,0, 319,239, Green);		// display title
  Rectangle(1,1, 318,238, Green);

  TFT_string(6,5,White,Magenta," ADS7846  터치스크린 테스트 ");

  TFT_string( 1, 1,Cyan,   Black,"(0,0)");
  TFT_string(32, 1,Cyan,   Black,"(319,0)");
  TFT_string( 1,27,Cyan,   Black,"(0,239)");
  TFT_string(30,27,Cyan,   Black,"(319,239)");

  TFT_string( 9, 9,Cyan,   Black," 원시 데이터");
  TFT_string(21, 9,Magenta,Black,            " x = 0000 ");
  TFT_string(21,11,Magenta,Black,            " y = 0000 ");
  TFT_string( 9,15,Cyan,   Black," 픽셀 데이터");
  TFT_string(21,15,Magenta,Black,            " x =  000 ");
  TFT_string(21,17,Magenta,Black,            " y =  000 ");
  TFT_string( 8,22,White,   Blue," 비프음 ");
  TFT_string(19,22,White,    Red," 테스트 종료 ");
  Rectangle(61,173, 130,194, White);
  Rectangle(149,173, 258,194, White);

  while(1)
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

      if((x_touch >= 61) && (x_touch <= 130) && (y_touch >= 173) && (y_touch <= 194))
        { Beep();
          Delay_ms(200);
        }
      else if((x_touch >= 149) && (x_touch <= 258) && (y_touch >= 173) && (y_touch <= 194))
        { Beep();
          return;
	}
    }
}

/* ---------------------------------------------------------------------------- */

void Test3(void)				/* Test 3 */
{
  LCD_string(0xC0,"     Test 3     ");

  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode

  TFT_string(0,5,White,Magenta,"   MCO1/MCO2 단자  주파수 출력 테스트   ");

  GPIOA->MODER &= 0xFFFCFFFF;			// PA8 = MCO1(180MHz speed)
  GPIOA->MODER |= 0x00020000;
  GPIOA->AFR[1] &= 0xFFFFFFF0;
  GPIOA->OSPEEDR |= 0x00030000;

  GPIOC->MODER &= 0xFFF3FFFF;			// PC9 = MCO2(180MHz speed)
  GPIOC->MODER |= 0x00080000;
  GPIOC->AFR[1] &= 0xFFFFFF0F;
  GPIOC->OSPEEDR |= 0x000C0000;

  TFT_string(0,16,Cyan,Black,"      MCO1(TP8) = HSI      = 16MHz      ");
  TFT_string(0,19,Cyan,Black,"      MCO2(TP9) = SYSCLK/2 = 84MHz      ");
  RCC->CFGR &= 0x009FFFFF;			// MCO1 = HSI/1
  RCC->CFGR |= 0x20000000;			// MCO2 = SYSCLK/2
  Delay_ms(5000);
  Beep();

  TFT_string(0,16,Cyan,Black,"      MCO1(TP8) = HSI/2    =  8MHz      ");
  TFT_string(0,19,Cyan,Black,"      MCO2(TP9) = SYSCLK/4 = 42MHz      ");
  RCC->CFGR &= 0x009FFFFF;			// MCO1 = HSI/2
  RCC->CFGR |= 0x34000000;			// MCO2 = SYSCLK/4
  Delay_ms(5000);
  Beep();

  TFT_string(0,16,Cyan,Black,"      MCO1(TP8) = HSE      = 16MHz      ");
  TFT_string(0,19,Cyan,Black,"      MCO2(TP9) = PLL/2    = 84MHz      ");
  RCC->CFGR &= 0x009FFFFF;			// MCO1 = HSE/1
  RCC->CFGR |= 0xE0400000;			// MCO2 = PLL/2
  Delay_ms(5000);
  Beep();

  TFT_string(0,16,Cyan,Black,"      MCO1(TP8) = HSE/2    =  8MHz      ");
  TFT_string(0,19,Cyan,Black,"      MCO2(TP9) = PLL/4    = 42MHz      ");
  RCC->CFGR &= 0x009FFFFF;			// MCO1 = HSE/2
  RCC->CFGR |= 0xF4400000;			// MCO2 = PLL/4
  Delay_ms(5000);
  Beep();
}

/* ---------------------------------------------------------------------------- */

void Test4(void)				/* Test 4 */
{
  unsigned char i;
  unsigned short result;
  float x;

  LCD_string(0xC0,"     Test 4     ");

  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode

  TFT_string(0,3,White,Magenta,"        A/D 컨버터  입력 테스트         ");
  TFT_string(8, 7,White,Black,"ADC0  (+5.0V) = +0.00V");
  TFT_string(8,10,White,Black,"ADC1  (+3.3V) = +0.00V");
  TFT_string(8,13,White,Black,"ADC2  ( VR1 ) = +0.00V");
  TFT_string(8,16,White,Black,"ADC3 (LM35DZ) = +00.0 C");
  TFT_string(8,19,White,Black,"ADC18 (Temp.) = +00.0 C");
  TFT_string(8,22,White,Black,"ADC17 (1.21V) = +0.00V");
  TFT_xy(29,16);
  TFT_English(0x7F);
  TFT_xy(29,19);
  TFT_English(0x7F);
  TFT_string(30,28,White,   Blue,"KEY4(    )");
  TFT_string(35,28,Red,     Blue,"종료");

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3

  RCC->CFGR &= 0xFFFF1FFF;			// PCLK2 = AHB/4 = 216MHz/4 = 54MHz
  RCC->CFGR |= 0x0000A000;
  RCC->APB2ENR |= 0x00000100;			// enable ADC1 clock

  ADC1->SMPR1 = 0x00FC0000;			// sampling time of channel 16~17 = 480 cycle
  ADC1->SMPR2 = 0x00000492;			// sampling time of channel 0~3 = 28 cycle
  ADC->CCR = 0x00800000;			// enable channel 16~17, ADCCLK = 42MHz/2 = 21MHz
  ADC->CCR = 0x00830000;	                // TSVREFE = 1, VBATE = 0, 54Mhz / 8 = 6.75Mhz 
  ADC1->CR1 = 0x00000800;			// 12-bit resolution, single discontinuous mode
  ADC1->CR2 = 0x00000001;			// right alignment, ADON = 1

  while(1)
    { ADC1->SQR3 = 0x00000000;			// channel 0 (+5.0V)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,7);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096.,1,2);

      ADC1->SQR3 = 0x00000001;			// channel 1 (+3.3V)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,10);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*2.*3.3/4096.,1,2);

      ADC1->SQR3 = 0x00000002;			// channel 2 (VR1)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,13);				// display voltage
      TFT_color(Cyan,Black);
      TFT_signed_float((float)result*3.3/4096.,1,2);

      ADC1->SQR3 = 0x00000003;			// channel 3 (LM35DZ)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,16);				// display temperature
      TFT_color(Magenta,Black);
      TFT_signed_float((float)result*330./4096.,2,1);

      ADC1->SQR3 = 0x00000012;			// channel 18 (temperature sensor)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      x = (float)result*3.3/4096.;		// calculate temperature
      x = (x-0.76)/0.0025 + 25.;
      TFT_xy(24,19);				// display voltage
      TFT_color(Green,Black);
      TFT_signed_float(x,2,1);

      ADC1->SQR3 = 0x00000011;			// channel 17 (VREFINT = 1.21V)
      Delay_us(1);
      result = 0;
      for(i = 0; i < 16; i++)
        { ADC1->CR2 |= 0x40000000;		// start conversion by software
          while(!(ADC1->SR & 0x00000002));	// wait A/D conversion complete
          result += ADC1->DR;
          Delay_ms(1);
        }
      result >>= 4;				// calculate average
      TFT_xy(24,22);				// display voltage
      TFT_color(Green,Black);
      TFT_signed_float((float)result*3.3/4096.,1,2);

      Delay_ms(100);
      if(Key_input() == KEY4) return;
      Delay_ms(100);
      if(Key_input() == KEY4) return;
      Delay_ms(100);
      if(Key_input() == KEY4) return;
    }
}

/* ---------------------------------------------------------------------------- */

void Test5(void)				/* Test 5 */
{
  unsigned short i, j, x, y;
  float a;
  unsigned short DAC1_data[200], DAC2_data[200];

  LCD_string(0xC0,"     Test 5     ");

  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode

  TFT_string(0,5,White,Magenta,"        D/A 컨버터  출력 테스트         ");

  GPIOA->MODER |= 0x00000F00;			// PA4 = DAC_OUT1, PA5 = DAC_OUT2
  RCC->APB1ENR |= 0x20000000;			// enable DAC clock

  DAC->CR = 0x00010001;				// DAC1/2 enable, output buffer and trigger disable

  TFT_string(10,10,White,Black,"J1-3(DAC_OUT1) 단자");
  TFT_string( 6,13,Cyan, Black,"200Hz 삼각파  출력(0 ~ 3.3V)");
  TFT_string(10,18,White,Black,"J2-3(DAC_OUT2) 단자");
  TFT_string( 6,21,Cyan, Black,"200Hz 톱니파  출력(0 ~ 3.3V)");

  a = 4095./100.;				// calculate data of triangular wave
  for(x = 0; x <= 100; x++)
    { y = (unsigned short)(a * x);
      DAC1_data[x] = y;
    }
  i = 1;
  for(x = 101; x < 200; x++)
    { DAC1_data[x] = DAC1_data[100-i];
      i++;
    }

  a = 4095./200.;				// calculate data of sawtooth wave
  for(x = 0; x < 200; x++)
    { y = (unsigned short)(a * x);
      DAC2_data[x] = y;
    }

  for(i = 0; i < 1000; i++)			// display for 5 sec
    { for(j = 0; j < 200; j++)
        { DAC->DHR12R1 = DAC1_data[j];		// data for channel 1
	  DAC->DHR12R2 = DAC2_data[j];		// data for channel 2
          Delay_us(25);
        }
    }

  TFT_string(10,10,White,Black,"J1-3(DAC_OUT1) 단자");
  TFT_string( 6,13,Cyan, Black,"500Hz 삼각파  출력(0 ~ 3.3V)");
  TFT_string(10,18,White,Black,"J2-3(DAC_OUT2) 단자");
  TFT_string( 6,21,Cyan, Black,"500Hz 정현파  출력(0 ~ 3.3V)");
  Beep();

  for(x = 0; x < 200; x++)			// calculate data of sinusoidal wave
    { y = (signed short)(1638. * sin(x * 1.8 * 3.141592 / 180.)) + 1638;
      DAC2_data[x] = y;
    }

  for(i = 0; i < 2500; i++)			// display for 5 sec
    { for(j = 0; j < 200; j++)
        { DAC->DHR12R1 = DAC1_data[j];		// data for channel 1
	  DAC->DHR12R2 = DAC2_data[j];		// data for channel 2
          Delay_us(10);
        }
    }

  Beep();
}

/* ---------------------------------------------------------------------------- */

void Test6(void)				/* Test 6 */
{
  unsigned char i, test_flag, RXD;
  unsigned char string[10] = { "OK-STM746" };

  LCD_string(0xC0,"     Test 6     ");

  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode

  TFT_string(0,4,White,Magenta,"     RS-232C 직렬통신 루프백 테스트     ");

  TFT_string(1, 9,White,Black,"CN5 컨넥터에 RS-232C 케이블을 접속하고");
  TFT_string(6,12,White,Black,"2번핀과 3번핀을 연결하시오 !");
  TFT_string(5,18,Cyan, Black,"USART3 송신 문자 : ");
  TFT_string(5,21,Cyan, Black,"USART3 수신 문자 : ");
  Delay_ms(1000);

  GPIOB->MODER &= 0xFF0FFFFF;			// PB10 = USART3_TX, PB11 = USART3_RX
  GPIOB->MODER |= 0x00A00000;
  GPIOB->AFR[1] &= 0xFFFF00FF;
  GPIOB->AFR[1] |= 0x00007700;
  RCC->APB1ENR |= 0x00040000;			// enable USART3 clock

  USART3->CR1 = 0x0000000D;			// UE=TE=RE = 1, 8 data bit, oversampling by 16
  USART3->CR2 = 0x00000000;			// asynchronous mode, 1 stop bit
  USART3->CR3 = 0x00000000;			// 3 sampling bit
  USART3->BRR = 2812;				// 19200 bps = 54MHz/2812.5
  Delay_ms(1);
  i = USART3->RDR;				// dummy read

  test_flag = 0;
  for(i = 0; i < 9; i++)
    { USART3->TDR = string[i];	        	// transmit test data
      TFT_xy(24+i,18);				// display transmitted test data
      TFT_color(Green,Black);
      TFT_English(string[i]);
      Delay_ms(10);

      RXD = USART3->RDR;		        // receive test data
      TFT_xy(24+i,21);				// display transmitted test data
      TFT_color(Magenta,Black);
      TFT_English(RXD);

      if(RXD == string[i])		        // data OK ?
        test_flag++;

      Delay_ms(500);
    }

  if(test_flag == 9)
    { TFT_string(15,25,  Green,Black,"통신 OK !");
      Beep();
    }
  else
    { TFT_string(14,25,Magenta,Black,"통신 에러 !");
      Beep_3times();
    }

  Delay_ms(2000);
}

/* ---------------------------------------------------------------------------- */

void Test7(void)				/* Test 7 */
{
  unsigned int key;

  LCD_string(0xC0,"     Test 7     ");

  DS3234_initialize();				// initialize DS3234 RTC
  TFT_clear_screen();				// clear screen
  TFT_landscape_mode();				// landscape mode

  Rectangle(0,0, 319,239, Green);		// display title
  Rectangle(1,1, 318,238, Green);

  TFT_string( 5, 4,White,Magenta," DS3234를 이용한  디지털 시계 ");
  TFT_string(10,12,White,  Black,"2015년 11월 01일 (일)");
  TFT_string_large(7,15,White,Black,"오전 12:00:00");

  TFT_cursor(Red);				// display cursor
  Xcursor = 38;
  Ycursor = 16;

  while(1)
    { Digital_display_time();			// display clock

      key = Key_input();			// key input
      if((key == KEY4) && (Xcursor == 38) && (Ycursor == 16))
        { TFT_cursor(Transparent);
	  return;
        }

      switch(key)
        { case KEY1 : Digital_cursor_left();	// if KEY1, go cursor left
                      break;
          case KEY2 : Digital_cursor_right();	// if KEY2, go cursor right
                      break;
          case KEY3 : Digital_increment();	// if KEY3, increment time
                      break;
	  case KEY4 : Digital_decrement();	// if KEY4, decrement time
                      break;
	  default   : break;
        }
    }
}

unsigned char year, month, date, weekday, hour, minute, second;
unsigned char time_flag = 0;

void DS3234_initialize(void)			/* initialize DS3234 RTC */
{
  RCC->AHB1ENR |= 0x00000012;			// enable clock of port B, E
  RCC->APB1ENR |= 0x00004000;			// enable SPI2 clock

  GPIOB->MODER &= 0x03FFFFFF;			// alternate function
  GPIOB->MODER |= 0xA8000000;
  GPIOB->AFR[1] &= 0x000FFFFF;			// PB15 = SPI2_MOSI, PB14 = SPI2_MISO, PB13 = SPI2_SCK
  GPIOB->AFR[1] |= 0x55500000;

  GPIOE->MODER &= 0xFFFFCFFF;			// PE6 = -RTC_CS = 1
  GPIOE->MODER |= 0x00001000;
  GPIOE->ODR |= 0x00000040;

  SPI2->CR1 = 0x0B67;				// master mode, 54MHz/32 = 1.6875MHz (Max 4MHz)
  SPI2->CR2 = 0x0F00;				// 16-bit data, disable SS output, CPOL = CPHA = 1

  second =  DS3234_read(0x00);			// if not second, initialize
  minute =  DS3234_read(0x01);			// if not minute, initialize
  hour =  DS3234_read(0x02);			// if not 12 hour system, initialize
  weekday =  DS3234_read(0x03);			// if not weekday, initialize
  date =  DS3234_read(0x04);			// if not date, initialize
  month =  DS3234_read(0x05);			// if not month, initialize
  year =  DS3234_read(0x06);			// if not year, initialize

  if((second > 0x59) || (minute > 0x59) || (hour < 0x40) || (weekday > 0x07) || (date > 0x31) || (month > 0x12) || (year > 0x99))
    { DS3234_write(0x00, 0x00);			// second = 0x00
      DS3234_write(0x01, 0x00);			// minute = 0x00
      DS3234_write(0x02, 0x12 + 0x40);		// hour = 0x12 AM
      DS3234_write(0x03, 0x01);			// weekday = 0x01(Sunday)
      DS3234_write(0x04, 0x01);			// date = 0x01
      DS3234_write(0x05, 0x11);			// month = 0x11
      DS3234_write(0x06, 0x15);			// year = 0x2015
      DS3234_write(0x0E, 0x00);			// control register
      DS3234_write(0x0F, 0x00);			// control register
    }
}

unsigned char DS3234_read(U16 address)		/* read DS3234 */
{
  unsigned short word;

  word = SPI2->DR;				// clear RXNE flag
  GPIOE->BSRR = 0x00400000;			// -RTS_CS = 0
  SPI2->DR = address << 8;
  while((SPI2->SR & 0x0003) != 0x0003);
  word = SPI2->DR;
  GPIOE->BSRR = 0x00000040;			// -RTS_CS = 1

  return word & 0x00FF;
}

void DS3234_write(U16 address, U08 value)	/* write DS3234 */
{
  unsigned short word = 0;

  word = SPI2->DR;				// clear RXNE flag
  GPIOE->BSRR = 0x00400000;			// -RTS_CS = 0
  SPI2->DR = ((0x80 + address) << 8) + value;
  while((SPI2->SR & 0x0003) != 0x0003);
  word |= SPI2->DR;
  GPIOE->BSRR = 0x00000040;			// -RTS_CS = 1
}

void Digital_display_time(void)			/* display DS3234 time */
{
  TFT_xy(10,12); TFT_color(Cyan,Black);		// display year
  TFT_hexadecimal(0x20, 2);
  year = DS3234_read(0x06);
  TFT_hexadecimal(year,2);
  TFT_xy(17,12); TFT_color(Cyan,Black);		// display month
  month = DS3234_read(0x05);
  TFT_hexadecimal(month,2);
  TFT_xy(22,12); TFT_color(Cyan,Black);		// display date
  date = DS3234_read(0x04);
  TFT_hexadecimal(date,2);
  weekday = DS3234_read(0x03);			// display weekday
  if(weekday == 0x01)      TFT_string(28,12,Cyan,Black,"일");
  else if(weekday == 0x02) TFT_string(28,12,Cyan,Black,"월");
  else if(weekday == 0x03) TFT_string(28,12,Cyan,Black,"화");
  else if(weekday == 0x04) TFT_string(28,12,Cyan,Black,"수");
  else if(weekday == 0x05) TFT_string(28,12,Cyan,Black,"목");
  else if(weekday == 0x06) TFT_string(28,12,Cyan,Black,"금");
  else if(weekday == 0x07) TFT_string(28,12,Cyan,Black,"토");
  TFT_xy(12,15); TFT_color(Green,Black);	// display hour
  hour = DS3234_read(0x02);
  TFT_hexadecimal_large(hour & 0x1F,2);
  if((hour & 0x20) == 0x00) TFT_string_large(7,15,White,Black,"오전");
  else                      TFT_string_large(7,15,White,Black,"오후");
  TFT_xy(15,15); TFT_color(Green,Black);	// display minute
  minute = DS3234_read(0x01);
  TFT_hexadecimal_large(minute,2);
  TFT_xy(18,15); TFT_color(Green,Black);	// display second
  second = DS3234_read(0x00);
  TFT_hexadecimal_large(second & 0x7F,2);
  TFT_xy(38,16); TFT_color(Green,Black);	// display home
  TFT_English(' ');

  TFT_string( 1,25,White,Blue," KEY1 ");
  TFT_string(11,25,White,Blue," KEY2 ");
  TFT_string(23,25,White,Blue," KEY3 ");
  TFT_string(33,25,White,Blue," KEY4 ");
  if((Xcursor == 38) && (Ycursor == 16))
    TFT_string(2,27,Red,  Black,"좌측      우측                  종료");
  else
    TFT_string(2,27,Red,  Black,"좌측      우측        증가      감소");

  if((minute == 0x00) && (second == 0x00) && (time_flag == 0))
    { time_flag = 1;
      Beep();
    }
  else if(second != 0x00)
    time_flag = 0;
}

unsigned char BCD_increment(U08 number)		/* BCD increment */
{ unsigned char i;

  i = (number >> 4)*10 + (number & 0x0F);       // convert BCD to binary
  i++;                                          // incrment
  i = ((i/10) << 4) + (i % 10);                 // convert binary to BCD
  return i;
}

unsigned char BCD_decrement(U08 number)		/* BCD decrement */
{ unsigned char i;

  i = (number >> 4)*10 + (number & 0x0F);       // convert BCD to binary
  i--;                                          // decrement
  i = ((i/10) << 4) + (i % 10);                 // convert binary to BCD
  return i;
}

void Digital_cursor_left(void)		        /* go cursor left */
{
  if     ((Xcursor == 38) && (Ycursor == 16)) { Xcursor = 19; Ycursor = 15; }
  else if((Xcursor == 19) && (Ycursor == 15)) { Xcursor = 16; Ycursor = 15; }
  else if((Xcursor == 16) && (Ycursor == 15)) { Xcursor = 13; Ycursor = 15; }
  else if((Xcursor == 13) && (Ycursor == 15)) { Xcursor =  9; Ycursor = 15; }
  else if((Xcursor ==  9) && (Ycursor == 15)) { Xcursor = 28; Ycursor = 12; }
  else if((Xcursor == 28) && (Ycursor == 12)) { Xcursor = 23; Ycursor = 12; }
  else if((Xcursor == 23) && (Ycursor == 12)) { Xcursor = 18; Ycursor = 12; }
  else if((Xcursor == 18) && (Ycursor == 12)) { Xcursor = 13; Ycursor = 12; }
  else if((Xcursor == 13) && (Ycursor == 12)) { Xcursor = 38; Ycursor = 16; }
}

void Digital_cursor_right(void)			/* go cursor right */
{
  if     ((Xcursor == 38) && (Ycursor == 16)) { Xcursor = 13; Ycursor = 12; }
  else if((Xcursor == 13) && (Ycursor == 12)) { Xcursor = 18; Ycursor = 12; }
  else if((Xcursor == 18) && (Ycursor == 12)) { Xcursor = 23; Ycursor = 12; }
  else if((Xcursor == 23) && (Ycursor == 12)) { Xcursor = 28; Ycursor = 12; }
  else if((Xcursor == 28) && (Ycursor == 12)) { Xcursor =  9; Ycursor = 15; }
  else if((Xcursor ==  9) && (Ycursor == 15)) { Xcursor = 13; Ycursor = 15; }
  else if((Xcursor == 13) && (Ycursor == 15)) { Xcursor = 16; Ycursor = 15; }
  else if((Xcursor == 16) && (Ycursor == 15)) { Xcursor = 19; Ycursor = 15; }
  else if((Xcursor == 19) && (Ycursor == 15)) { Xcursor = 38; Ycursor = 16; }
}

void Digital_increment(void)			/* increment time */
{
  if((Xcursor == 13) && (Ycursor == 12))	// in case of year
    { year = DS3234_read(0x06);
      if(year == 0x99)      year = 0x00;
      else                  year = BCD_increment(year);
      DS3234_write(0x06,year);
    }
  else if((Xcursor == 18) && (Ycursor == 12))	// in case of month
    { month = DS3234_read(0x05);
      if(month == 0x12)     month = 0x01;
      else                  month = BCD_increment(month);
      DS3234_write(0x05,month);
    }
  else if((Xcursor == 23) && (Ycursor == 12))	// in case of date
    { date = DS3234_read(0x04);
      if(date == 0x31)      date = 0x01;
      else                  date = BCD_increment(date);
      DS3234_write(0x04,date);
    }
  else if((Xcursor == 28) && (Ycursor == 12))	// in case of weekday
    { weekday = DS3234_read(0x03);
      if(weekday == 0x07)   weekday = 0x01;
      else                  weekday++;
      DS3234_write(0x03,weekday);
    }
  else if((Xcursor == 9) && (Ycursor == 15))	// in case of AM/PM
    { hour = DS3234_read(0x02);
      if((hour & 0x20) == 0x00) hour |= 0x20;
      else                      hour &= 0xDF;
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 13) && (Ycursor == 15))	// in case of hour
    { hour = DS3234_read(0x02);
      if((hour & 0x1F) == 0x12) hour = (hour & 0xE0) + 0x01;
      else                      hour = BCD_increment(hour);
      DS3234_write(0x02,hour + 0x40);
    }
  else if((Xcursor == 16) && (Ycursor == 15))	// in case of minute
    { minute = DS3234_read(0x01);
      if(minute == 0x59)    minute = 0x00;
      else                  minute = BCD_increment(minute);
      DS3234_write(0x01,minute);
    }
  else if((Xcursor == 19) && (Ycursor == 15))	// in case of second
    { second = DS3234_read(0x00);
      if(second == 0x59)    second = 0x00;
      else                  second = BCD_increment(second);
      DS3234_write(0x00,second);
    }
}

void Digital_decrement(void)			/* decrement time */
{
  if((Xcursor == 13) && (Ycursor == 12))	// in case of year
    { year = DS3234_read(0x06);
      if(year == 0x00)      year = 0x99;
      else                  year = BCD_decrement(year);
      DS3234_write(0x06,year);
    }
  else if((Xcursor == 18) && (Ycursor == 12))	// in case of month
    { month = DS3234_read(0x05);
      if(month == 0x01)     month = 0x12;
      else                  month = BCD_decrement(month);
      DS3234_write(0x05,month);
    }
  else if((Xcursor == 23) && (Ycursor == 12))	// in case of date
    { date = DS3234_read(0x04);
      if(date == 0x01)      date = 0x31;
      else                  date = BCD_decrement(date);
      DS3234_write(0x04,date);
    }
  else if((Xcursor == 28) && (Ycursor == 12))	// in case of weekday
    { weekday = DS3234_read(0x03);
      if(weekday == 0x01)   weekday = 0x07;
      else                  weekday--;
      DS3234_write(0x03,weekday);
    }
  else if((Xcursor == 9) && (Ycursor == 15))	// in case of AM/PM
    { hour = DS3234_read(0x02);
      if((hour & 0x20) == 0x00) hour |= 0x20;
      else                      hour &= 0xDF;
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 13) && (Ycursor == 15))	// in case of hour
    { hour = DS3234_read(0x02);
      if((hour & 0x1F) == 0x01) hour = (hour & 0xE0) + 0x12;
      else                      hour = BCD_decrement(hour);
      DS3234_write(0x02,hour + 0x40);
    }
  else if((Xcursor == 16) && (Ycursor == 15))	// in case of minute
    { minute = DS3234_read(0x01);
      if(minute == 0x00)    minute = 0x59;
      else                  minute = BCD_decrement(minute);
      DS3234_write(0x01,minute);
    }
  else if((Xcursor == 19) && (Ycursor == 15))	// in case of second
    { second = DS3234_read(0x00);
      if(second == 0x00)    second = 0x59;
      else                  second = BCD_decrement(second);
      DS3234_write(0x00,second);
    }
}

/* ---------------------------------------------------------------------------- */

unsigned char second_old, start_flag;

void Test8(void)				/* Test 8 */
{
  unsigned char key = no_key;

  LCD_string(0xC0,"     Test 8     ");

  DS3234_initialize();				// initialize DS3234 RTC
  TFT_clear_screen();				// clear screen
  TFT_portrait_mode();				// portrait mode

  TFT_cursor(Magenta);				// display cursor by Magenta color
  Xcursor = 28;
  Ycursor = 37;

  start_flag = 0;

  TFT_string(0,1, White,Magenta," DS3234에 의한  아날로그 시계 "); // display title
  Draw_analog_clock();				// draw analog clock
  TFT_string( 8,37, Cyan,  Black, "오전");	// display digital clock
  TFT_string(13,37, Silver,Black, "12:00:00");

  while(1)
    { key = Key_input();			// key input
      if((key == KEY4) && (Xcursor == 28) && (Ycursor == 37))
        { TFT_cursor(Transparent);
  	  return;
        }

        switch(key)
        { case KEY1 : Analog_cursor_left();	// if KEY1, go cursor left
                      break;
          case KEY2 : Analog_cursor_right();	// if KEY2, go cursor right
                      break;
          case KEY3 : Analog_increment();	// if KEY3, increment time
                      break;
	  case KEY4 : Analog_decrement();	// if KEY4, decrement time
                      break;
	  default   : Analog_display_time();	// display time
	              break;
        }
    }
}

void Analog_cursor_left(void)			/* go cursor left */
{
  if     ((Xcursor == 28) && (Ycursor == 37)) { Xcursor = 20; Ycursor = 37; }
  else if((Xcursor == 20) && (Ycursor == 37)) { Xcursor = 17; Ycursor = 37; }
  else if((Xcursor == 17) && (Ycursor == 37)) { Xcursor = 14; Ycursor = 37; }
  else if((Xcursor == 14) && (Ycursor == 37)) { Xcursor = 10; Ycursor = 37; }
  else if((Xcursor == 10) && (Ycursor == 37)) { Xcursor = 19; Ycursor = 25; }

  else if((Xcursor == 19) && (Ycursor == 25)) { Xcursor = 17; Ycursor = 25; }
  else if((Xcursor == 17) && (Ycursor == 25)) { Xcursor = 14; Ycursor = 25; }
  else if((Xcursor == 14) && (Ycursor == 25)) { Xcursor = 11; Ycursor = 25; }
  else if((Xcursor == 11) && (Ycursor == 25)) { Xcursor = 28; Ycursor = 37; }
}

void Analog_cursor_right(void)			/* go cursor right */
{
  if     ((Xcursor == 28) && (Ycursor == 37)) { Xcursor = 11; Ycursor = 25; }
  else if((Xcursor == 11) && (Ycursor == 25)) { Xcursor = 14; Ycursor = 25; }
  else if((Xcursor == 14) && (Ycursor == 25)) { Xcursor = 17; Ycursor = 25; }
  else if((Xcursor == 17) && (Ycursor == 25)) { Xcursor = 19; Ycursor = 25; }
  else if((Xcursor == 19) && (Ycursor == 25)) { Xcursor = 10; Ycursor = 37; }

  else if((Xcursor == 10) && (Ycursor == 37)) { Xcursor = 14; Ycursor = 37; }
  else if((Xcursor == 14) && (Ycursor == 37)) { Xcursor = 17; Ycursor = 37; }
  else if((Xcursor == 17) && (Ycursor == 37)) { Xcursor = 20; Ycursor = 37; }
  else if((Xcursor == 20) && (Ycursor == 37)) { Xcursor = 28; Ycursor = 37; }
}

void Analog_increment(void)			/* increment time */
{
  if((Xcursor == 10) && (Ycursor == 37))	// in case of AM/PM
    { hour = DS3234_read(0x02);
      if((hour & 0x20) == 0x00) hour |= 0x20;
      else                      hour &= 0xDF;
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 14) && (Ycursor == 37))	// in case of hour
    { hour = DS3234_read(0x02);
      if((hour & 0x1F) == 0x12) hour = (hour & 0xE0) + 0x01;
      else                      hour = BCD_increment(hour);
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 17) && (Ycursor == 37))	// in case of minute
    { minute = DS3234_read(0x01);
      if(minute == 0x59)    minute = 0x00;
      else                  minute = BCD_increment(minute);
      DS3234_write(0x01,minute);
    }
  else if((Xcursor == 20) && (Ycursor == 37))	// in case of second
    { second = DS3234_read(0x00);
      if(second == 0x59)    second = 0x00;
      else                  second = BCD_increment(second);
      DS3234_write(0x00,second);
    }
  else if((Xcursor == 11) && (Ycursor == 25))	// in case of year
    { year = DS3234_read(0x06);
      if(year == 0x99)      year = 0x00;
      else                  year = BCD_increment(year);
      DS3234_write(0x06,year);
    }
  else if((Xcursor == 14) && (Ycursor == 25))	// in case of month
    { month = DS3234_read(0x05);
      if(month == 0x12)     month = 0x01;
      else                  month = BCD_increment(month);
      DS3234_write(0x05,month);
    }
  else if((Xcursor == 17) && (Ycursor == 25))	// in case of date
    { date = DS3234_read(0x04);
      if(date == 0x31)      date = 0x01;
      else                  date = BCD_increment(date);
      DS3234_write(0x04,date);
    }
  else if((Xcursor == 19) && (Ycursor == 25))	// in case of weekday
    { weekday = DS3234_read(0x03);
      if(weekday == 0x07)   weekday = 0x01;
      else                  weekday++;
      DS3234_write(0x03,weekday);
    }
}

void Analog_decrement(void)			/* decrement time */
{
  if ((Xcursor == 10) && (Ycursor == 37))	// in case of AM/PM
    { hour = DS3234_read(0x02);
      if((hour & 0x20) == 0x00) hour |= 0x20;
      else                      hour &= 0xDF;
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 14) && (Ycursor == 37))	// in case of hour
    { hour = DS3234_read(0x02);
      if((hour & 0x1F) == 0x01) hour = (hour & 0xE0) + 0x12;
      else                      hour = BCD_decrement(hour);
      DS3234_write(0x02,hour);
    }
  else if((Xcursor == 17) && (Ycursor == 37))	// in case of minute
    { minute = DS3234_read(0x01);
      if(minute == 0x00)    minute = 0x59;
      else                  minute = BCD_decrement(minute);
      DS3234_write(0x01,minute);
    }
  else if((Xcursor == 20) && (Ycursor == 37))	// in case of second
    { second = DS3234_read(0x00);
      if(second == 0x00)    second = 0x59;
      else                  second = BCD_decrement(second);
      DS3234_write(0x00,second);
    }
  else if((Xcursor == 11) && (Ycursor == 25))	// in case of year
    { year = DS3234_read(0x06);
      if(year == 0x00)      year = 0x99;
      else                  year = BCD_decrement(year);
      DS3234_write(0x06,year);
    }
  else if((Xcursor == 14) && (Ycursor == 25))	// in case of month
    { month = DS3234_read(0x05);
      if(month == 0x01)     month = 0x12;
      else                  month = BCD_decrement(month);
      DS3234_write(0x05,month);
    }
  else if((Xcursor == 17) && (Ycursor == 25))	// in case of date
    { date = DS3234_read(0x04);
      if(date == 0x01)      date = 0x31;
      else                  date = BCD_decrement(date);
      DS3234_write(0x04,date);
    }
  else if((Xcursor == 19) && (Ycursor == 25))	// in case of weekday
    { weekday = DS3234_read(0x03);
      if(weekday == 0x01)   weekday = 0x07;
      else                  weekday--;
      DS3234_write(0x03,weekday);
    }
}

unsigned short Xcenter = 120;			// center x,y of analog clock
unsigned short Ycenter = 160;

void Draw_analog_clock(void)			/* draw analog clock */
{
 unsigned short i; 
 unsigned short scale_out = 118;
 unsigned short scale_in1 = 112;
 unsigned short scale_in2 = 106;
 signed short Xstart, Ystart, Xend, Yend;

 for(i = 0; i < 60 ; i++)			// draw clock scale
   if((i % 5) != 0)
     { Xstart = Xcenter + (signed short)(scale_in1*sin(3.141592/180*(i*6)));
       Ystart = Ycenter - (signed short)(scale_in1*cos(3.141592/180*(i*6)));
       Xend   = Xcenter + (signed short)(scale_out*sin(3.141592/180*(i*6)));
       Yend   = Ycenter - (signed short)(scale_out*cos(3.141592/180*(i*6)));

       Line_soft(Xstart,Ystart, Xend,Yend, LightGrey);
     }
 for(i = 0; i < 12 ; i++)
   { Xstart = Xcenter + (signed short)(scale_in2*sin(3.141592/180*(i*30)));
     Ystart = Ycenter - (signed short)(scale_in2*cos(3.141592/180*(i*30)));
     Xend   = Xcenter + (signed short)(scale_out*sin(3.141592/180*(i*30)));
     Yend   = Ycenter - (signed short)(scale_out*cos(3.141592/180*(i*30)));

     Line_soft(Xstart,Ystart, Xend,Yend, White);
   }

  Circle(Xcenter,Ycenter, 119, Cyan);		// draw outline
  Circle(Xcenter,Ycenter, 118, Cyan);

  for(i = 4; i > 0; i--)			// draw center
    Circle(Xcenter,Ycenter, i, Yellow);
}

void Analog_display_time(void)			/* display analog and digital time */
{
  second = DS3234_read(0x00);			// new second ?
  if(second == second_old)
    return;
  else
    second_old = second;

  minute = DS3234_read(0x01);
  hour = DS3234_read(0x02);
  weekday = DS3234_read(0x03);
  date = DS3234_read(0x04);
  month = DS3234_read(0x05);
  year = DS3234_read(0x06);

  TFT_xy(13,37);				// display hour
  TFT_color(Green,Black);
  TFT_hexadecimal(hour & 0x1F,2);
  if((hour & 0x20) == 0x00) TFT_string(8,37, Cyan,Black, "오전");
  else                      TFT_string(8,37, Cyan,Black, "오후");
  TFT_xy(16,37);				// display minute
  TFT_color(Green,Black);
  TFT_hexadecimal(minute,2);
  TFT_xy(19,37);				// display second
  TFT_color(Green,Black);
  TFT_hexadecimal(second,2);

  TFT_xy(28,37); TFT_color(Green,Black);	// display home
  TFT_English(' ');

  Draw_pin();					// draw hour, minute, second pin
  Circle(Xcenter,Ycenter, 5, Green);

  if((minute == 0x00) && (second == 0x00) && (time_flag == 0))
    { time_flag = 1;
      Beep();
    }
  else if((minute != 0x00) || (second != 0x00))
    { time_flag = 0;
      GPIOC->BSRR = 0x00008000;			// buzzer on
      Delay_us(100);				// delay 100 us
      GPIOC->BSRR = 0x80000000;			// buzzer off
    }
}

signed short Xstart1_hour_old, Ystart1_hour_old, Xstart2_hour_old, Ystart2_hour_old, Xend_hour_old, Yend_hour_old;
signed short Xstart1_minute_old, Ystart1_minute_old, Xstart2_minute_old, Ystart2_minute_old, Xend_minute_old, Yend_minute_old;

signed short Xstart_second_old, Ystart_second_old, Xend_second_old, Yend_second_old;
signed short Xcircle_second_old, Ycircle_second_old;

void Draw_pin(void)				/* draw hour, minute, second pin */
{
  unsigned short i;
  unsigned short pin_hour, length_hour = 65;
  signed short Xstart1_hour, Ystart1_hour, Xstart2_hour, Ystart2_hour, Xend_hour, Yend_hour;

  unsigned short pin_minute, length_minute = 80;
  signed short Xstart1_minute, Ystart1_minute, Xstart2_minute, Ystart2_minute, Xend_minute, Yend_minute;

  unsigned short pin_second, length_second = 100, center_second = 80 ;
  signed short Xstart_second, Ystart_second, Xend_second, Yend_second;
  signed short Xcircle_second, Ycircle_second;

  pin_hour = ((hour & 0x10) >> 4)*10 + (hour & 0x0F);	// convert hour to binary
  pin_minute = (minute >> 4)*10 + (minute & 0x0F);	// convert minute to binary
  pin_second = (second >> 4)*10 + (second & 0x0F);	// convert second to binary

						// calculate hour variables
  Xstart1_hour = Xcenter + (signed short)(6*sin((pin_hour*30 + pin_minute/2. - 30)*3.141592/180.)+0.5);
  Ystart1_hour = Ycenter - (signed short)(6*cos((pin_hour*30 + pin_minute/2. - 30)*3.141592/180.)+0.5);

  Xstart2_hour = Xcenter + (signed short)(6*sin((pin_hour*30 + pin_minute/2. + 30)*3.141592/180.)+0.5);
  Ystart2_hour = Ycenter - (signed short)(6*cos((pin_hour*30 + pin_minute/2. + 30)*3.141592/180.)+0.5);

  Xend_hour = Xcenter + (signed short)(length_hour*sin((pin_hour*30 + pin_minute/2.)*3.141592/180.)+0.5);
  Yend_hour = Ycenter - (signed short)(length_hour*cos((pin_hour*30 + pin_minute/2.)*3.141592/180.)+0.5);

						 // calculate minute variables
  Xstart1_minute = Xcenter + (signed short)(6*sin((pin_minute*6 + pin_second/10. - 15)*3.141592/180.)+0.5);
  Ystart1_minute = Ycenter - (signed short)(6*cos((pin_minute*6 + pin_second/10. - 15)*3.141592/180.)+0.5);

  Xstart2_minute = Xcenter + (signed short)(6*sin((pin_minute*6 + pin_second/10. + 15)*3.141592/180.)+0.5);
  Ystart2_minute = Ycenter - (signed short)(6*cos((pin_minute*6 + pin_second/10. + 15)*3.141592/180.)+0.5);

  Xend_minute = Xcenter + (signed short)(length_minute*sin((pin_minute*6 + pin_second/10.)*3.141592/180.)+0.5);
  Yend_minute = Ycenter - (signed short)(length_minute*cos((pin_minute*6 + pin_second/10.)*3.141592/180.)+0.5);

						// calculate second variables
  Xstart_second = Xcenter + (signed short)(6*sin(3.141592/180.*(pin_second*6))+0.5);
  Ystart_second = Ycenter - (signed short)(6*cos(3.141592/180.*(pin_second*6))+0.5);

  Xend_second = Xcenter + (signed short)(length_second*sin(3.141592/180.*(pin_second*6))+0.5);
  Yend_second = Ycenter - (signed short)(length_second*cos(3.141592/180.*(pin_second*6))+0.5);
	
  Xcircle_second = Xcenter + (signed short)(center_second*sin(3.141592/180.*(pin_second*6))+0.5);
  Ycircle_second = Ycenter - (signed short)(center_second*cos(3.141592/180.*(pin_second*6))+0.5);

  if(start_flag == 0)				// initialize old value
    { start_flag = 1;

      Xstart1_hour_old = Xstart1_hour;
      Ystart1_hour_old = Ystart1_hour;
      Xstart2_hour_old = Xstart2_hour;
      Ystart2_hour_old = Ystart2_hour;
      Xend_hour_old = Xend_hour;
      Yend_hour_old = Yend_hour;

      Xstart1_minute_old = Xstart1_minute;
      Ystart1_minute_old = Ystart1_minute;
      Xstart2_minute_old = Xstart2_minute;
      Ystart2_minute_old = Ystart2_minute;
      Xend_minute_old = Xend_minute;
      Yend_minute_old = Yend_minute;

      Xstart_second_old = Xstart_second;
      Ystart_second_old = Ystart_second;
      Xend_second_old = Xend_second;
      Yend_second_old = Yend_second;
      Xcircle_second_old = Xcircle_second;
      Ycircle_second_old = Ycircle_second;
    }

						// delete old hour and minute pin
  if((Xend_hour != Xend_hour_old) || (Yend_hour != Yend_hour_old) || (Xend_minute != Xend_minute_old) || (Yend_minute != Yend_minute_old))
    { Line_soft(Xstart1_hour_old,Ystart1_hour_old, Xend_hour_old,Yend_hour_old, Black);
      Line_soft(Xstart2_hour_old,Ystart2_hour_old, Xend_hour_old,Yend_hour_old, Black);

      Line_soft(Xstart1_hour_old+1,Ystart1_hour_old, Xend_hour_old,Yend_hour_old, Black);
      Line_soft(Xstart1_hour_old-1,Ystart1_hour_old, Xend_hour_old,Yend_hour_old, Black);
      Line_soft(Xstart2_hour_old+1,Ystart2_hour_old, Xend_hour_old,Yend_hour_old, Black);
      Line_soft(Xstart2_hour_old-1,Ystart2_hour_old, Xend_hour_old,Yend_hour_old, Black);

      Line_soft(Xstart1_hour_old,Ystart1_hour_old+1, Xend_hour_old,Yend_hour_old, Black);
      Line_soft(Xstart1_hour_old,Ystart1_hour_old-1, Xend_hour_old,Yend_hour_old, Black);
      Line_soft(Xstart2_hour_old,Ystart2_hour_old+1, Xend_hour_old,Yend_hour_old, Black);
      Line_soft(Xstart2_hour_old,Ystart2_hour_old-1, Xend_hour_old,Yend_hour_old, Black);

      Line_soft(Xstart1_minute_old,Ystart1_minute_old, Xend_minute_old,Yend_minute_old, Black);
      Line_soft(Xstart2_minute_old,Ystart2_minute_old, Xend_minute_old,Yend_minute_old, Black);

      Line_soft(Xstart1_minute_old+1,Ystart1_minute_old, Xend_minute_old,Yend_minute_old, Black);
      Line_soft(Xstart1_minute_old-1,Ystart1_minute_old, Xend_minute_old,Yend_minute_old, Black);
      Line_soft(Xstart2_minute_old+1,Ystart2_minute_old, Xend_minute_old,Yend_minute_old, Black);
      Line_soft(Xstart2_minute_old-1,Ystart2_minute_old, Xend_minute_old,Yend_minute_old, Black);

      Line_soft(Xstart1_minute_old,Ystart1_minute_old+1, Xend_minute_old,Yend_minute_old, Black);
      Line_soft(Xstart1_minute_old,Ystart1_minute_old-1, Xend_minute_old,Yend_minute_old, Black);
      Line_soft(Xstart2_minute_old,Ystart2_minute_old+1, Xend_minute_old,Yend_minute_old, Black);
      Line_soft(Xstart2_minute_old,Ystart2_minute_old-1, Xend_minute_old,Yend_minute_old, Black);
    }

  for(i = 3; i > 0; i--)			// delete old second pin
    Circle(Xcircle_second_old,Ycircle_second_old, i, Black);
  Line_soft(Xstart_second_old,Ystart_second_old, Xend_second_old,Yend_second_old, Black);

  TFT_color(Green,Black);			// display hour number
  TFT_English_pixel(112,54, '1'); TFT_English_pixel(120,54, '2');
  TFT_English_pixel(62,68, '1'); TFT_English_pixel(70,68, '1');
  TFT_English_pixel(164,68, '1');
  TFT_English_pixel(28,104, '1'); TFT_English_pixel(36,104, '0');
  TFT_English_pixel(202,104, '2');
  TFT_English_pixel(18,152, '9');
  TFT_English_pixel(216,152, '3');
  TFT_English_pixel(32,202, '8');
  TFT_English_pixel(202,202, '4');
  TFT_English_pixel(68,238, '7');
  TFT_English_pixel(164,238, '5');
  TFT_English_pixel(117,252, '6');

  TFT_color(Silver,Black);			// display slash and ()
  TFT_xy(12,25);
  TFT_English('/');
  TFT_xy(15,25);
  TFT_English('/');
  TFT_xy(18,25);
  TFT_English('(');
  TFT_xy(21,25);
  TFT_English(')');
  TFT_string(8,25, Green,Black, "20");		// display year
  TFT_hexadecimal(year,2);
  TFT_xy(13,25);				// display month
  TFT_hexadecimal(month,2);
  TFT_xy(16,25);				// display date
  TFT_hexadecimal(date,2);
  if(weekday == 0x01)      TFT_string(19,25, Red,Black, "일");	// display weekday
  else if(weekday == 0x02) TFT_string(19,25, Red,Black, "월");
  else if(weekday == 0x03) TFT_string(19,25, Red,Black, "화");
  else if(weekday == 0x04) TFT_string(19,25, Red,Black, "수");
  else if(weekday == 0x05) TFT_string(19,25, Red,Black, "목");
  else if(weekday == 0x06) TFT_string(19,25, Red,Black, "금");
  else if(weekday == 0x07) TFT_string(19,25, Red,Black, "토");

  Rectangle(59,197, 179,217, Green);
  Rectangle(60,198, 178,216, Silver);

  Line_soft(Xstart1_hour,Ystart1_hour, Xend_hour, Yend_hour, Magenta); // draw new hour pin
  Line_soft(Xstart2_hour,Ystart2_hour, Xend_hour, Yend_hour, Magenta);

  Line_soft(Xstart1_minute,Ystart1_minute, Xend_minute,Yend_minute, Cyan); // draw new minute pin
  Line_soft(Xstart2_minute,Ystart2_minute, Xend_minute,Yend_minute, Cyan);

  for(i = 3; i > 0; i--)			// draw new second pin
    Circle(Xcircle_second,Ycircle_second, i, Yellow);
  Line_soft (Xstart_second,Ystart_second, Xend_second,Yend_second, Yellow);

  Xstart1_hour_old = Xstart1_hour;		// store old value
  Ystart1_hour_old = Ystart1_hour;
  Xstart2_hour_old = Xstart2_hour;
  Ystart2_hour_old = Ystart2_hour;
  Xend_hour_old = Xend_hour;
  Yend_hour_old = Yend_hour;

  Xstart1_minute_old = Xstart1_minute;
  Ystart1_minute_old = Ystart1_minute;
  Xstart2_minute_old = Xstart2_minute;
  Ystart2_minute_old = Ystart2_minute;
  Xend_minute_old = Xend_minute;
  Yend_minute_old = Yend_minute;

  Xstart_second_old = Xstart_second;
  Ystart_second_old = Ystart_second;
  Xend_second_old = Xend_second;
  Yend_second_old = Yend_second;
  Xcircle_second_old = Xcircle_second;
  Ycircle_second_old = Ycircle_second;
}

void Line_soft(S16 x1,S16 y1, S16 x2,S16 y2, U16 color)	/* draw a soft line */
{
  int x, y;

  if(y1 != y2)					// if y1 != y2, y is variable
    { if(y1 < y2)				//              x is function
        for(y = y1; y <= y2; y++)
          { x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
            TFT_pixel(x, y, color);
          }
      else
        for(y = y1; y >= y2; y--)
          { x = x1 + (y - y1)*(x2 - x1)/(y2 - y1);
            TFT_pixel(x, y, color);
          }
    }
  if(x1 != x2)					// if x1 != x2, x is variable
    { if(x1 < x2)				//              y is function
        for(x = x1; x <= x2; x++)
          { y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
            TFT_pixel(x, y, color);
          }
      else
        for(x = x1; x >= x2; x--)
          { y = y1 + (x - x1)*(y2 - y1)/(x2 - x1);
            TFT_pixel(x, y, color);
          }
    }
}
