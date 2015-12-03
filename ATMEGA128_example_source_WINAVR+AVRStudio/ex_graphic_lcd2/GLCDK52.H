typedef unsigned char byte;
typedef unsigned int  word;

#define		LCD_OUTP	PORTC
#define		LCD_INP		PINC
#define		LCD_CTRL	PORTA

#define		DISPON		0x3f
#define		DISPOFF		0x3e

#define	  	DI			0x01
#define		RW			0x02 
#define		E			0x04 
#define		CS1			0x08 
#define		CS2			0x10 
#define		CS_ALL		0x18 

#define		SetDI(m)		m?(LCD_CTRL|=DI): (LCD_CTRL&=~DI)
#define		SetRW(m)		m?(LCD_CTRL|=RW): (LCD_CTRL&=~RW)
#define 	SetE(m)			m?(LCD_CTRL|=E):  (LCD_CTRL&=~E)
#define		SetCS1(m)		m?(LCD_CTRL|=CS1):(LCD_CTRL&=~CS1)
#define		SetCS2(m)		m?(LCD_CTRL|=CS2):(LCD_CTRL&=~CS2)

byte   xcoordinate, ycoordinate;			/* x character(0-3), y character(0-15) */
byte   cursor_val, cursor_x, cursor_y;		/* x and y cursor position(0-3, 0-15) */
byte   Korean_buffer[32];			/* 32 byte Korean font buffer */
/*-------------------------------------------------------------------------*/
/* 시간 지연 루틴 */
void    delay( word del )
{
	while( del-- )
	{
	  asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle +
	}
}

void delay_us(word time_us)
{
   register word i;
   for(i=0; i<time_us; i++){   // 4 cycle +
      asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle +
      asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle +
      asm("PUSH   R0");        // 2 cycle +
      asm("POP    R0");        // 2 cycle   = 16 cycle = 1us for 16MHz
   }
}

void delay_ms(word time_ms)
{
   register word i;
   for(i=0;i<time_ms;i++) delay_us(450);
}


/* lcd 명령 출력 */
//cmd	 = instruction
void    write_cmd( byte  cs, byte  cmd )
{
	//check_bf();
	LCD_CTRL = cs & CS_ALL;	
	SetE(1);	
	LCD_OUTP = cmd;	
	delay(1);
	SetE(0);
	delay(1);
	LCD_CTRL=0x00;
	delay(10);
}

/* 1 문자 출력  */
//cmd	 = data
void    write_data( byte cs, byte ch )
{	
	//check_bf();
	LCD_CTRL = (cs & CS_ALL) | DI;	
	SetE(1);
	LCD_OUTP = ch;	
	delay(1);
	SetE(0);
	delay(1);
	LCD_CTRL=0x00;
	delay(10);	
}


/* GLCD Clear */
void	lcd_clear(void)
{
	byte i, j, x, y;
	LCD_CTRL=0x00;
	write_cmd(CS_ALL,0x3f);
	write_cmd(CS_ALL,0xc0);
	x = 0xB8;					/* X start address */
	y = 0x40;					/* Y start address */
	for(i = 0; i <= 7; i++)
	{ 
		write_cmd(CS_ALL,x);
		write_cmd(CS_ALL,y);
		for(j = 0; j < 64; j++)
			write_data(CS_ALL,0x00);		/* clear CS1 and CS2 */
		x++;
	}
}

void 	lcd_init(void)
{
	write_cmd( CS_ALL, DISPON );
	write_cmd( CS_ALL, 0xc0 );	/* 디스플레이 시작라인	*/
	write_cmd( CS_ALL, 0xb8 );	/* X 어드레스 셋 = 0	*/
	write_cmd( CS_ALL, 0x40 );	/* Y 어드레스 셋 = 0	*/  
}

/* 문자 위치 세트 */
void lcd_xy( byte x, byte y )	/* set English character position on GLCD */
{
	xcoordinate = x;				/* x = 0 - 3  */
	ycoordinate = y;				/* y = 0 - 15 */
}

/*-------------------------------------------------------------------------*/

void lcd_xy_row(byte row,byte x,byte y)	/* set character upper/lower row on GLCD */
{ 
  lcd_xy(x,y);						/* x = 0 - 3, y = 0 - 15 */

  if(row == 0)						/* if upper row, set Y address */
    write_cmd(CS_ALL,0xB8 + x*2);
  else							/* if lower row, set Y address + 1 */
    write_cmd(CS_ALL,0xB8 + x*2 + 1);

  if(ycoordinate <= 7)					/* if y <= 7, CS1 Y address */
    write_cmd(CS1,0x40 + y*8);
  else							/* if y >= 8, CS2 Y address */
    write_cmd(CS2,0x40 + (y-8)*8);
}

void lcd_English(byte Ecode)			//* display a 8x16 Dot English(ASCII) on GLCD /
{ byte x, y, i, signal;

  x = xcoordinate;
  y = ycoordinate;

  lcd_xy_row(0,x,y);					//* display upper row /
  if(ycoordinate <= 7)					//* if y <= 7, CS1 /
    signal = CS1;
  else							//* if y >= 8, CS2 /
    signal = CS2;
  for(i = 0; i <= 7; i++)
    write_data(signal,pgm_read_byte(&English[Ecode][i]));

  lcd_xy_row(1,x,y);					//* display lower row /
  if(ycoordinate <= 7)					//* if y <= 7, CS1 /
    signal = CS1;
  else							//* if y >= 8, CS2 /
    signal = CS2;
  for(i = 8; i <= 15; i++)
    { if((cursor_val == 1) && (xcoordinate == cursor_x) && (ycoordinate == cursor_y))
        write_data(signal,pgm_read_byte(&English[Ecode][i]) | 0x80);
      else
        write_data(signal,pgm_read_byte(&English[Ecode][i]));
    }

  ycoordinate++;
  if(ycoordinate==16){
  	  ycoordinate = 0;
	  xcoordinate++;
	}
}

void lcd_Korean(word  Kcode)			//* display a 16x16 Dot Korean on GLCD /
{ byte  x, y, i, signal;
  byte  cho_5bit, joong_5bit, jong_5bit;
  byte  cho_bul, joong_bul, jong_bul, jong_flag;
  word   character;
  if(ycoordinate==15)
    lcd_English(0x20);
  cho_5bit = table_initial[(Kcode >> 10) & 0x001F];		//* get cho, joong, jong 5 bit /
  joong_5bit = table_medial[(Kcode >> 5) & 0x001F];
  jong_5bit = table_final[Kcode & 0x001F];

  if(jong_5bit == 0)					//* don't have jongsung /
    { jong_flag = 0;
      cho_bul = bul_initial1[joong_5bit];
      if((cho_5bit == 1) || (cho_5bit == 16)) joong_bul = 0;
      else                                    joong_bul = 1;
    }
  else							//* have jongsung /
    { jong_flag = 1;
      cho_bul = bul_initial2[joong_5bit];
      if((cho_5bit == 1) || (cho_5bit == 16)) joong_bul = 2;
      else                                    joong_bul = 3;
      jong_bul = bul_final[joong_5bit];
    }

  character = cho_bul*20 + cho_5bit;			//* copy chosung font /
  for(i = 0; i <= 31; i++)
    Korean_buffer[i] = pgm_read_byte(&Korean[character][i]);

  character = 8*20 + joong_bul*22 + joong_5bit;		//* OR joongsung font /
  for(i = 0; i <= 31; i++)
    Korean_buffer[i] |= pgm_read_byte(&Korean[character][i]);

  if(jong_flag == 1)					//* if jongsung, OR jongsung font /
    { character = 8*20 + 4*22 +jong_bul*28 + jong_5bit;
      for(i = 0; i <= 31; i++)
        Korean_buffer[i] |= pgm_read_byte(&Korean[character][i]);
    }

  x = xcoordinate;
  y = ycoordinate;
  lcd_xy_row(0,x,y);					//* display upper left row /
  if(ycoordinate <= 7)					//* if y <= 7, CS1 /
    signal = CS1;
  else							//* if y >= 8, CS2 /
    signal = CS2;
  for(i = 0; i <= 7; i++)
    write_data(signal,Korean_buffer[i]);

  lcd_xy_row(1,x,y);					//* display lower left row /
  if(ycoordinate <= 7)					//* if y <= 7, CS1 /
    signal = CS1;
  else							//* if y >= 8, CS2 /
    signal = CS2;
  for(i = 16; i <= 23; i++)
    { if((cursor_val == 1) && (xcoordinate == cursor_x) && (ycoordinate == cursor_y))
         write_data(signal,Korean_buffer[i] | 0x80);
      else
         write_data(signal,Korean_buffer[i]);
    }

  ycoordinate++;

  y = ycoordinate;
  lcd_xy_row(0,x,y);					///* display upper right row /
  if(ycoordinate <= 7)					///* if y <= 7, CS1 /
    signal = CS1;
  else							///* if y >= 8, CS2 /
    signal = CS2;
  for(i = 8; i <= 15; i++)
    write_data(signal,Korean_buffer[i]);

  lcd_xy_row(1,x,y);					//* display lower right row /
  if(ycoordinate <= 7)					//* if y <= 7, CS1 /
    signal = CS1;
  else							//* if y >= 8, CS2 /
    signal = CS2;
  for(i = 24; i <= 31; i++)
    { if((cursor_val == 1) && (xcoordinate == cursor_x) && (ycoordinate == (cursor_y+1)))
         write_data(signal,Korean_buffer[i] | 0x80);
      else
         write_data(signal,Korean_buffer[i]);
    }

  ycoordinate++;
   if(ycoordinate==16){
  	  ycoordinate = 0;
	  xcoordinate++;
	}
}

/*******************************************************************************
* 완성형 한글을 조합형으로 바꾸어 주는 부분입니다.
*******************************************************************************/

word kssm_convert(word kssm)
{
	byte high,low;
	word index;
    word return_data;

	high = (kssm>>8)&0xff;
	low  =  kssm    &0xff;

	if ((kssm >= 0xb0a1)&&(kssm <= 0xc8fe))
        {
		index  = (high - 0xb0) * 94 + low - 0xa1;
		return_data  =  pgm_read_byte(&KSTable[index][0])*256;
		return_data  |= pgm_read_byte(&KSTable[index][1]);
		return return_data;
	}
        return 0;
}


void lcd_string(byte x, byte y, byte *string)	//* display a string on GLCD /
{ byte character1;
  word character2;

  lcd_xy(x,y);						//* x = 0 - 3, y = 0 - 15 /

	while(*string != '\0')
	{ 
		character1 = *string;
		string++;
		if(((character1 >> 7) & 0x01) == 0)
			lcd_English(character1);			//* display English character /
		else
		{
			character2 = *string;
			character2 = 256*character1 + (character2 & 0xFF);
			string++;
			character2 = kssm_convert(character2);//완성형을 조합형으로 변환
			lcd_Korean(character2);			//* display Korean character /
		}
	}
}

