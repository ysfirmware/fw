/* ============================================================================ */
/*		Digital_Piano_KHJ.c : D/A 컨버터를 이용한 전자 피아노		*/
/* ============================================================================ */
/*					programmed by Hyung-Jae Kim in 2016.    */

#include "stm32f746xx.h"
#include "OK-STM746.h"
#include "OK-STM746_large.h"

#define	F3		1			// note F of octave 3
#define	F3X		2			// note F# of octave 3
#define	G3		3			// note G of octave 3
#define	G3X		4			// note G# of octave 3
#define	A3		5			// note A of octave 3
#define	A3X		6			// note A# of octave 3
#define	B3		7			// note B of octave 3
#define	C4		8			// note C of octave 4
#define	C4X		9			// note C# of octave 4
#define	D4		10			// note D of octave 4
#define	D4X		11			// note D# of octave 4
#define	E4		12			// note E of octave 4
#define	F4		13			// note F of octave 4
#define	F4X		14			// note F# of octave 4
#define	G4		15			// note G of octave 4
#define	G4X		16			// note G# of octave 4
#define	A4		17			// note A of octave 4
#define	A4X		18			// note A# of octave 4
#define	B4		19			// note B of octave 4
#define	C5		20			// note C of octave 5
#define	C5X		21			// note C# of octave 5
#define	D5		22			// note D of octave 5
#define	D5X		23			// note D# of octave 5
#define	E5		24			// note E of octave 5
#define	F5		25			// note F of octave 5
#define	F5X		26			// note F# of octave 5
#define	G5		27			// note G of octave 5
#define	G5X		28                      // note G# of octave 5

void SysTick_Handler(void);                     /* SysTick interrupt function */
void EXTI0_IRQHandler(void);                    /* EXT0 interrupt function */
void EXTI1_IRQHandler(void);                    /* EXT1 interrupt function */
void Draw_keyboard(void);			/* draw keyboard screen */
void Draw_symbol(U16 xPos,U16 yPos,U16 colorfore,U16 colorback,U08 code); /* draw symbol */
void Check_keyboard(void);			/* check keyboard key or icon */
void Play_note(U08 sound, U32 type);		/* play note */
void Volume_control(void);                      /* voloume up or down */

unsigned char keyboard_key;		        // keyboard key
unsigned char delay_time = 5;                   // delay time(ms) for piano sound
unsigned char increse_value;                    // volume up(+2.5%)
unsigned char decrese_value;                    // volume down(-5.0%)
unsigned char roop_count;
volatile signed int volume, volume_out;	        // speaker volume(0~4000)
volatile unsigned char toggle_flag = 0;
volatile unsigned char interrupt_flag = 0;
volatile unsigned char redraw_flag = 0;         

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function */
{
  toggle_flag++;                                // making like a clock
  toggle_flag %= 2;
  
  if(toggle_flag == 1)
  { DAC->DHR12R1 = 0;				// clear DAC channel1 output data
    DAC->DHR12R2 = 0;				// clear DAC channel2 output data
  }
  else
  { DAC->DHR12R1 = volume_out;                  // DAC channel1 output data
    DAC->DHR12R2 = volume_out;                  // DAC channel2 output data
  }
}

void EXTI0_IRQHandler(void)                     /* EXT0 interrupt function */
{ EXTI->PR = 0x00000001;                        // clear pending bit of EXTI0
  if(interrupt_flag == 0)                       // if KEY1 pushed, volume down
  { volume -= 200;                              
    if(volume <= 0)                             // volume down limit
    { volume = 0;
      Beep_3times();
    }
    else
      Beep();
  
    while((GPIOC->IDR & 0x0000000F) != 0x0000000F);     // debouncing
    Delay_ms(100);
    interrupt_flag = 1;
  }
}

void EXTI1_IRQHandler(void)                     /* EXT1 interrupt function */
{ EXTI->PR = 0x00000002;                        // clear pending bit of EXTI1
  if(interrupt_flag == 0)                       // if KEY2 pushed, volume up
  { volume += 200;                              
    if(volume >= 4000)                          // volume up limit
    { volume = 4000;
      Beep_3times();
    }
    else
      Beep();
  
    while((GPIOC->IDR & 0x0000000F) != 0x0000000F);     // debouncing
    Delay_ms(100);
    interrupt_flag = 1;
  }
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Initialize_touch_screen();			// initialize touch screen

  LCD_string(0x80,"OK-STM32F7  V1.0");		// text_LCD display title
  LCD_string(0xC0," Digital Piano  ");
  Beep();

  GPIOA->MODER |= 0x00000F00;			// PA4 = DAC_OUT1, PA5 = DAC_OUT2
  RCC->APB1ENR |= 0x20000000;			// enable DAC clock
  
  DAC->CR = 0x00010001;				// DAC enable, buffer and trigger disable
  DAC->DHR12R1 = 0;				// clear DAC channel1 output data
  DAC->DHR12R2 = 0;				// clear DAC channel2 output data
  
  volume = 2000;
  
  GPIOC->MODER &= 0xFFFFFFF0;			// PC1, PC0 = Input mode
  RCC->AHB1ENR |= 0x00000004;			// enable GPIOC clock
  SYSCFG->EXTICR[0] = 0x00000022;               // EXTI1,0 = PC1,PC0
  EXTI->IMR = 0x00000003;			// enable EXTI1,0 interrupt
  EXTI->RTSR = 0x00000000;			// disable EXTI1,0 rising edge
  EXTI->FTSR = 0x00000003;			// enable EXTI1,0 falling edge
  NVIC->ISER[0] = 0x000003C0;

  Draw_keyboard();			        // draw keyboard screen
  
  while(1)
  { Check_keyboard();				// check keyboard key or icon
    
    if(interrupt_flag == 1)                     // if KEY1 or KEY2 pushed, voloume up or down
    { Volume_control();                         
      Delay_ms(300);                            // delay for volume control display
      redraw_flag = 1;
      Block(49,80, 270,161, Black,Black);       // erase volume control display
      Draw_keyboard();                          // redraw keyboard just volume control display part
      interrupt_flag = 0;
    }
  }
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

void Draw_keyboard(void)			/* draw keyboard screen */
{
  unsigned int i, x;
  
  TFT_string( 0, 0,White,Magenta," OK-STM32F7 키트를 이용한 오르간/피아노 ");
  TFT_string(14, 2,LightGrey,Transparent,"Programmed by Hyung-Jae.K"); 
  TFT_string( 0, 2,White,Red    ,"전자 오르간");
  TFT_string( 0,16,Black,Green  ,"전자 피아노");
  TFT_string(19,16,Black,Yellow ,"볼륨조절 : KEY1/KEY2");
  
  Rectangle(0, 33, 319,125, Red);		// draw organ outline
  Rectangle(0,145, 319,237, Green);		// draw piano outline
  
  if(redraw_flag == 0)                          // draw all part of keyboard
  { for(i = 0; i < 16; i++)			// draw white key
    { x = 20*i + 1;
      Block(x, 34, x+17,124, White,White);
      Block(x,146, x+17,236, White,White);
    }
  
    for(i = 0; i < 16; i++)			// draw black key
    { x = 20*i + 11;
      if((i != 3) && (i != 6) && (i != 10) && (i != 13) && (i != 15))
      { Block(x, 34, x+17, 91, Black,Black);
        Block(x,146, x+17,203, Black,Black);
      }
      else if(i == 15)
      { Block(x, 34, x+7, 91, Black,Black);
        Block(x,146, x+7,203, Black,Black);
      }
    }
  }
  else if(redraw_flag == 1)                     // draw keyboard just volume control display part
  { for(i = 2; i < 14; i++)			// draw white key
    { x = 20*i + 1;
      Block(x, 34, x+17,124, White,White);
      Block(x,146, x+17,236, White,White);
    }
  
    for(i = 1; i < 14; i++)			// draw black key
    { x = 20*i + 11;
      if((i != 3) && (i != 6) && (i != 10) && (i != 13) && (i != 15))
      { Block(x, 34, x+17, 91, Black,Black);
        Block(x,146, x+17,203, Black,Black);
      }
    }
  }
}

const unsigned short symbol[1][16] = {                                  
 { 0x00C0,0x00E0,0x00F0,0x00D8,0x00CC,0x00CC,0x00CC,0x00CC,             // note4 symbol
   0x00C8,0x00D0,0x03C0,0x0FC0,0x0F80,0x1F80,0x1F00,0x0C00 }};

void Draw_symbol(U16 xPos,U16 yPos, U16 colorfore,U16 colorback, U08 code) /* draw symbol */
{
  unsigned char  x, y;
  unsigned short font;

  for(y = 0; y < 16; y++)
  { TFT_GRAM_address(xPos,yPos+y);
    font = symbol[code][y];
    for(x = 0; x < 16; x++)
    { if((font << x) & 0x8000) TFT_data(colorfore);	
      else                     TFT_data(colorback);
    }
  }
}

unsigned char icon_flag = 0;			// icon touch flag

void Check_keyboard(void)			/* check keyboard key or icon */
{
  Touch_screen_input();				// input touch screen
 
  if((icon_flag == 0) && (x_touch >= 1) && (x_touch <= 18) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;		                // key F3
    keyboard_key = F3;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 11) && (x_touch <= 28) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key F3X
    keyboard_key = F3X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 21) && (x_touch <= 38) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key G3
    keyboard_key = G3;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 31) && (x_touch <= 48) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key G3X
    keyboard_key = G3X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 41) && (x_touch <= 58) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key A3
    keyboard_key = A3;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 51) && (x_touch <= 68) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key A3X
    keyboard_key = A3X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 61) && (x_touch <= 78) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key B3
    keyboard_key = B3;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 81) && (x_touch <= 98) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key C4
    keyboard_key = C4;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 91) && (x_touch <= 108) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key C4X
    keyboard_key = C4X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 101) && (x_touch <= 118) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key D4
    keyboard_key = D4;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 111) && (x_touch <= 128) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key D4X
    keyboard_key = D4X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 121) && (x_touch <= 138) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key E4
    keyboard_key = E4;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 141) && (x_touch <= 158) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key F4
    keyboard_key = F4;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 151) && (x_touch <= 168) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key F4X
    keyboard_key = F4X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 161) && (x_touch <= 178) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key G4
    keyboard_key = G4;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 171) && (x_touch <= 188) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key G4X
    keyboard_key = G4X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 181) && (x_touch <= 198) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key A4
    keyboard_key = A4;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 191) && (x_touch <= 208) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key A4X
    keyboard_key = A4X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 201) && (x_touch <= 218) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key B4
    keyboard_key = B4;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 221) && (x_touch <= 238) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key C5
    keyboard_key = C5;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 231) && (x_touch <= 248) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key C5X
    keyboard_key = C5X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 241) && (x_touch <= 258) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key D5
    keyboard_key = D5;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 251) && (x_touch <= 268) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key D5X
    keyboard_key = D5X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 261) && (x_touch <= 278) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key E5
    keyboard_key = E5;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 281) && (x_touch <= 298) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key F5
    keyboard_key = F5;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 291) && (x_touch <= 308) && (y_touch >= 34) && (y_touch <= 91))
  { icon_flag = 1;				// key F5X
    keyboard_key = F5X;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 301) && (x_touch <= 318) && (y_touch >= 91) && (y_touch <= 143))
  { icon_flag = 1;				// key G5
    keyboard_key = G5;
    Play_note(keyboard_key, 'O');
  }
  else if((icon_flag == 0) && (x_touch >= 1) && (x_touch <= 18) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key F3
    keyboard_key = F3;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 11) && (x_touch <= 28) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key F3X
    keyboard_key = F3X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 21) && (x_touch <= 38) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key G3
    keyboard_key = G3;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 31) && (x_touch <= 48) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key G3X
    keyboard_key = G3X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 41) && (x_touch <= 58) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key A3
    keyboard_key = A3;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 51) && (x_touch <= 68) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key A3X
    keyboard_key = A3X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 61) && (x_touch <= 78) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key B3
    keyboard_key = B3;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 81) && (x_touch <= 98) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key C4
    keyboard_key = C4;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 91) && (x_touch <= 108) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key C4X
    keyboard_key = C4X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 101) && (x_touch <= 118) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key D4
    keyboard_key = D4;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 111) && (x_touch <= 128) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key D4X
    keyboard_key = D4X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 121) && (x_touch <= 138) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key E4
    keyboard_key = E4;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 141) && (x_touch <= 158) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key F4
    keyboard_key = F4;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 151) && (x_touch <= 168) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key F4X
    keyboard_key = F4X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 161) && (x_touch <= 178) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key G4
    keyboard_key = G4;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 171) && (x_touch <= 188) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key G4X
    keyboard_key = G4X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 181) && (x_touch <= 198) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key A4
    keyboard_key = A4;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 191) && (x_touch <= 208) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key A4X
    keyboard_key = A4X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 201) && (x_touch <= 218) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key B4
    keyboard_key = B4;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 221) && (x_touch <= 238) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key C5
    keyboard_key = C5;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 231) && (x_touch <= 248) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key C5X
    keyboard_key = C5X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 241) && (x_touch <= 258) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key D5
    keyboard_key = D5;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 251) && (x_touch <= 268) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key D5X
    keyboard_key = D5X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 261) && (x_touch <= 278) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key E5
    keyboard_key = E5;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 281) && (x_touch <= 298) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key F5
    keyboard_key = F5;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 291) && (x_touch <= 308) && (y_touch >= 146) && (y_touch <= 203))
  { icon_flag = 1;				// key F5X
    keyboard_key = F5X;
    Play_note(keyboard_key, 'P');
  }
  else if((icon_flag == 0) && (x_touch >= 301) && (x_touch <= 318) && (y_touch >= 203) && (y_touch <= 235))
  { icon_flag = 1;				// key G5
    keyboard_key = G5;
    Play_note(keyboard_key, 'P');
  }
  else if((x_touch == 0) && (y_touch == 0))
  { icon_flag = 0;				// key off
    Play_note(keyboard_key, 0xFF);
  }
}

void Play_note(U08 sound, U32 type)		/* play note */
{
  float frequency;
  unsigned long period;

  if(type == 0xFF)                              // if ended key push, clear DAC output and erase symbol
  { SysTick->CTRL  = 0x00000001;		// SysTick no interrupt
    DAC->DHR12R1 = 0;				// clear DAC channel1 output data
    DAC->DHR12R2 = 0;				// clear DAC channel2 output data
    
    if(sound == F3)                             
    { Draw_symbol(   2,96, White,White, 0); 
      Draw_symbol(  2,208, White,White, 0);
    }
    else if(sound == F3X)
    { Draw_symbol( 12, 51, Black,Black, 0);
      Draw_symbol( 12,163, Black,Black, 0);
    }
    else if(sound == G3)
    { Draw_symbol( 22, 96, White,White, 0);
      Draw_symbol( 22,208, White,White, 0);
    }
    else if(sound == G3X)
    { Draw_symbol( 32, 51, Black,Black, 0);
      Draw_symbol( 32,163, Black,Black, 0);
    }
    else if(sound == A3)  
    { Draw_symbol( 42, 96, White,White, 0);
      Draw_symbol( 42,208, White,White, 0);
    }
    else if(sound == A3X) 
    { Draw_symbol( 52, 51, Black,Black, 0);
      Draw_symbol( 52,163, Black,Black, 0);
    }
    else if(sound == B3)  
    { Draw_symbol( 62, 96, White,White, 0);
      Draw_symbol( 62,208, White,White, 0);
    }
    else if(sound == C4)  
    { Draw_symbol( 82, 96, White,White, 0);
      Draw_symbol( 82,208, White,White, 0);
    }
    else if(sound == C4X) 
    { Draw_symbol( 92, 51, Black,Black, 0);
      Draw_symbol( 92,163, Black,Black, 0);
    }
    else if(sound == D4)  
    { Draw_symbol(102, 96, White,White, 0);
      Draw_symbol(102,208, White,White, 0);
    }
    else if(sound == D4X) 
    { Draw_symbol(112, 51, Black,Black, 0);
      Draw_symbol(112,163, Black,Black, 0);
    }
    else if(sound == E4)  
    { Draw_symbol(122, 96, White,White, 0);
      Draw_symbol(122,208, White,White, 0);
    }
    else if(sound == F4)  
    { Draw_symbol(142, 96, White,White, 0);
      Draw_symbol(142,208, White,White, 0);
    }
    else if(sound == F4X) 
    { Draw_symbol(152, 51, Black,Black, 0);
      Draw_symbol(152,163, Black,Black, 0);
    }
    else if(sound == G4)  
    { Draw_symbol(162, 96, White,White, 0);
      Draw_symbol(162,208, White,White, 0);
    }
    else if(sound == G4X) 
    { Draw_symbol(172, 51, Black,Black, 0);
      Draw_symbol(172,163, Black,Black, 0);
    }
    else if(sound == A4)  
    { Draw_symbol(182, 96, White,White, 0);
      Draw_symbol(182,208, White,White, 0);
    }
    else if(sound == A4X) 
    { Draw_symbol(192, 51, Black,Black, 0);
      Draw_symbol(192,163, Black,Black, 0);
    }
    else if(sound == B4)  
    { Draw_symbol(202, 96, White,White, 0);
      Draw_symbol(202,208, White,White, 0);
    }
    else if(sound == C5)  
    { Draw_symbol(222, 96, White,White, 0);
      Draw_symbol(222,208, White,White, 0);
    }
    else if(sound == C5X) 
    { Draw_symbol(232, 51, Black,Black, 0);
      Draw_symbol(232,163, Black,Black, 0);
    }
    else if(sound == D5)  
    { Draw_symbol(242, 96, White,White, 0);
      Draw_symbol(242,208, White,White, 0);
    }
    else if(sound == D5X) 
    { Draw_symbol(252, 51, Black,Black, 0);
      Draw_symbol(252,163, Black,Black, 0);
    }
    else if(sound == E5)  
    { Draw_symbol(262, 96, White,White, 0); 
      Draw_symbol(262,208, White,White, 0);
    }
    else if(sound == F5)  
    { Draw_symbol(282, 96, White,White, 0);
      Draw_symbol(282,208, White,White, 0);
    }
    else if(sound == F5X) 
    { Draw_symbol(292, 51, Black,Black, 0);
      Draw_symbol(292,163, Black,Black, 0);
    }
    else if(sound == G5)  
    { Draw_symbol(302, 96, White,White, 0);
      Draw_symbol(302,208, White,White, 0);
    }
    else if(sound == G5X) 
    { Draw_symbol(312, 51, Black,Black, 0);
      Draw_symbol(312,163, Black,Black, 0);
    }
  } 
  else if(sound == F3)                          // if key inputted, determine note frequency
  { frequency = 174.614;                        // and draw symbol
    if(type == 'P') Draw_symbol(  2,208, Red,White, 0);
    else            Draw_symbol(  2, 96, Red,White, 0);
  }
  else if(sound == F3X) 
  { frequency = 184.997;
    if(type == 'P') Draw_symbol( 12,163, Red,Black, 0);
    else            Draw_symbol( 12, 51, Red,Black, 0);
  }
  else if(sound == G3)
  { frequency = 195.998;
    if(type == 'P') Draw_symbol( 22,208, Red,White, 0);
    else            Draw_symbol( 22, 96, Red,White, 0);
  }
  else if(sound == G3X)
  { frequency = 207.652;
    if(type == 'P') Draw_symbol( 32,163, Red,Black, 0);
    else            Draw_symbol( 32, 51, Red,Black, 0);
  }
  else if(sound == A3)
  { frequency = 220.000;
    if(type == 'P') Draw_symbol( 42,208, Red,White, 0);
    else            Draw_symbol( 42, 96, Red,White, 0);
  }
  else if(sound == A3X) 
  { frequency = 233.082;
    if(type == 'P') Draw_symbol( 52,163, Red,Black, 0);
    else            Draw_symbol( 52, 51, Red,Black, 0);
  }
  else if(sound == B3)  
  { frequency = 246.942;
    if(type == 'P') Draw_symbol( 62,208, Red,White, 0); 
    else            Draw_symbol( 62, 96, Red,White, 0);
  }
  else if(sound == C4)  
  { frequency = 261.626; 
    if(type == 'P') Draw_symbol( 82,208, Red,White, 0);  
    else            Draw_symbol( 82, 96, Red,White, 0);
  }
  else if(sound == C4X) 
  { frequency = 277.183;
    if(type == 'P') Draw_symbol( 92,163, Red,Black, 0);  
    else            Draw_symbol( 92, 51, Red,Black, 0);
  }
  else if(sound == D4)  
  { frequency = 293.665;
    if(type == 'P') Draw_symbol(102,208, Red,White, 0);  
    else            Draw_symbol(102, 96, Red,White, 0);
  }
  else if(sound == D4X) 
  { frequency = 311.127;
    if(type == 'P') Draw_symbol(112,163, Red,Black, 0);  
    else            Draw_symbol(112, 51, Red,Black, 0);
  }
  else if(sound == E4)  
  { frequency = 329.628;
    if(type == 'P') Draw_symbol(122,208, Red,White, 0);  
    else            Draw_symbol(122, 96, Red,White, 0);
  }
  else if(sound == F4)  
  { frequency = 349.228;
    if(type == 'P') Draw_symbol(142,208, Red,White, 0);  
    else            Draw_symbol(142, 96, Red,White, 0);
  }
  else if(sound == F4X) 
  { frequency = 369.994;
    if(type == 'P') Draw_symbol(152,163, Red,Black, 0);  
    else            Draw_symbol(152, 51, Red,Black, 0);
  }
  else if(sound == G4)  
  { frequency = 391.995;
    if(type == 'P') Draw_symbol(162,208, Red,White, 0);  
    else            Draw_symbol(162, 96, Red,White, 0);
  }
  else if(sound == G4X) 
  { frequency = 415.305;
    if(type == 'P') Draw_symbol(172,163, Red,Black, 0);  
    else            Draw_symbol(172, 51, Red,Black, 0);
  }
  else if(sound == A4)  
  { frequency = 440.000;
    if(type == 'P') Draw_symbol(182,208, Red,White, 0);  
    else            Draw_symbol(182, 96, Red,White, 0);
  }
  else if(sound == A4X) 
  { frequency = 466.164;
    if(type == 'P') Draw_symbol(192,163, Red,Black, 0);  
    else            Draw_symbol(192, 51, Red,Black, 0);
  }
  else if(sound == B4)  
  { frequency = 493.883;
    if(type == 'P') Draw_symbol(202,208, Red,White, 0);  
    else            Draw_symbol(202, 96, Red,White, 0);
  }
  else if(sound == C5)  
  { frequency = 523.251;
    if(type == 'P') Draw_symbol(222,208, Red,White, 0);  
    else            Draw_symbol(222, 96, Red,White, 0);
  }
  else if(sound == C5X) 
  { frequency = 554.365;
    if(type == 'P') Draw_symbol(232,163, Red,Black, 0);  
    else            Draw_symbol(232, 51, Red,Black, 0);
  }
  else if(sound == D5)  
  { frequency = 587.330;
    if(type == 'P') Draw_symbol(242,208, Red,White, 0);  
    else            Draw_symbol(242, 96, Red,White, 0);
  }
  else if(sound == D5X) 
  { frequency = 622.254;
    if(type == 'P') Draw_symbol(252,163, Red,Black, 0);  
    else            Draw_symbol(252, 51, Red,Black, 0);
  }
  else if(sound == E5)  
  { frequency = 659.255;
    if(type == 'P') Draw_symbol(262,208, Red,White, 0);  
    else            Draw_symbol(262, 96, Red,White, 0);
  }
  else if(sound == F5)  
  { frequency = 698.456;
    if(type == 'P') Draw_symbol(282,208, Red,White, 0);  
    else            Draw_symbol(282, 96, Red,White, 0);
  }
  else if(sound == F5X) 
  { frequency = 739.989;
    if(type == 'P') Draw_symbol(292,163, Red,Black, 0);  
    else            Draw_symbol(292, 51, Red,Black, 0);
  }
  else if(sound == G5)  
  { frequency = 783.991;
    if(type == 'P') Draw_symbol(302,208, Red,White, 0);  
    else            Draw_symbol(302, 96, Red,White, 0);
  }
  else if(sound == G5X) 
  { frequency = 830.609;
    if(type == 'P') Draw_symbol(312,163, Red,Black, 0);  
    else            Draw_symbol(312, 51, Red,Black, 0);
  }

  toggle_flag = 0;
  period = (unsigned long)(27000000./2./frequency + 0.5);
  SysTick->LOAD  = period - 1;			// N-1 for frequecny = 27MHz/2/frequency - 1
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 216MHz/8 = 27MHz, SysTick enable and interrupt enable
  
  volume_out = volume;
  increse_value = volume/40;                    // volume up percentage(+2.5%)
  decrese_value = volume/20;                    // volume down percentage(-5.0%)

  if(type == 'P')                               // piano type
  { for(roop_count = 0; roop_count <= 39; roop_count++)
    { volume_out += increse_value;
      Delay_ms(delay_time); 
      volume_out -= decrese_value;
      Delay_ms(delay_time);
    }	
  }
}

void Volume_control(void)                       /* voloume up or down */
{ 
  unsigned int i;
  
  Block(50,80, 270,160, Black, Yellow);         // draw display outline
  Rectangle(49,79, 271,161, Black);
  
  TFT_string_large(10,11, Black,Transparent, "Volume");
  TFT_string( 8,14, Black,Transparent, "KEY1");
  TFT_string(28,14, Black,Transparent, "KEY2");
  Rectangle(60,130, 260,150, Black);            // draw volume outline
  Rectangle(59,129, 261,151, Black);
   
  for(i = 0; i <= 19; i++)                      // draw volume grid
    Line(70 + 10*i,130, 70 + 10*i,150, Black);  
  
  Block(60,130,60 + volume/20,150, Black, Red); // draw volume bar
}