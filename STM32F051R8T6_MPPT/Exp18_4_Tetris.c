/* ============================================================================ */
/*		Exp18_3_Tetris.c : 테트리스(Tetris) 게임			*/
/* ============================================================================ */
/*                          Designed and programmed by Duck-Yong Yoon in 2013.  */

#include <stdlib.h>
#include "stm32f0xx.h"
#include "OK-STM32F0.h"

void Draw_block(U16 xPos,U16 yPos,U16 color,U08 type); // draw a block
void Draw_board(void);				// draw outline of game board
void Display_score(U16 score);			// display score
void Draw_game_tetris(void);			// draw tetromino of game
void Draw_next_tetris(S08 next);		// draw next tetromino
void Add_tetris(S08 (*src)[4],S08 *xPos,S08 *yPos,U08 number); // add tetromino
void Delete_tetris(S08 (*src)[4],S08 *xPos,S08 *yPos);	// delete tetromino
void Rotate_tetris(S08 (*src)[4],U08 size,S08 *width,S08 *height); // rotate tetromino
signed char Drop_tetris(S08 (*src)[4],S08 xPos,S08 yPos,S08 width,S08 height); // drop tetromino
signed char Check_line(S08 yPos,S08 height);	// check if line is OK
void Check_limit(S08 (*src)[4],S08 *x,S08 *y,S08 *tetris_end,S08 *left_limit,S08 *right_limit,S08 *y_limit,S08 *rotate_limit,S08 current);

/* ---------------------------------------------------------------------------- */

#define	Xbase		 9			// x base position of board
#define	Ybase		18			// y base position of board

#define Left		KEY1			// KEY1 for left
#define Down		KEY2			// KEY2 for down
#define Rotate		KEY3			// KEY3 for rotate or game start
#define Right		KEY4			// KEY4 for right

signed char screen[20][10];			// game screen data
const signed char board[2][12] =		// constant data for board outline
                { "333333333333",
                  "322222222223" };

const signed char tetris[7][4][4] = {		/* define tetromino */
		{ "1100",			// 0 = O type (White)
		  "1100",
		  "0000",
		  "0000" },
		{ "1000",			// 1 = I type (Blue)
		  "1000",
		  "1000",
		  "1000" },
		{ "1300",			// 2 = T type (Yellow)
		  "1100",
		  "1300",
		  "0000" },
		{ "3100",			// 3 = Z type (Magenta)
		  "1100",
		  "1300",
		  "0000" },
		{ "1300",			// 4 = S type (Red)
		  "1100",
		  "3100",
		  "0000" },
		{ "1100",			// 5 = L type (Green)
		  "3100",
		  "3100",
		  "0000" },
		{ "1100",			// 6 = J type (Cyan)
		  "1300",	
		  "1300",
		  "0000" }};

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  unsigned int random, score = 0;
  unsigned char  x, y, key = 0, t_ch = 0, current_tetris = 0, next_tetris;
  signed char pos_x, pos_y = 0, del_pos_x, del_pos_y;
  signed char tetris_width = 0, tetris_height = -1, del_line;

  signed char tetris_end_flag = 0, game_end_flag = 0, time_move_flag = 0, key_move_flag = 0;
  signed char left_limit_flag = 0, right_limit_flag = 0, y_limit_flag = 0, rotate_limit_flag = 0;
  signed char temp_tetris[4][4], del_tetris[4][4];

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  TFT_portrait_mode();				// portrait mode

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp18_4.c    ");
  Beep();

  SysTick->LOAD  = 0x00FFFFFF;			// maximum value
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000001;			// SysTick enable and interrupt no request

  Draw_board();					// draw game board

  TFT_string( 6,18,Green,  Black, "Press ");	// wait KEY1 to start
  TFT_string(12,18,Magenta,Black, "KEY1");
  TFT_string( 6,20,Green,  Black, "to start !");
  while(key != KEY1) key = Key_input();

  random = SysTick->VAL;
  srand(random);				// initialize random number

  while(1)
    { for(y = 0; y < 20; y++)			// initialize game
        for(x = 0; x < 10; x++) screen[y][x] = '2';

      score = 0;
      del_line = tetris_height = -1;
      game_end_flag = tetris_end_flag = y_limit_flag = rotate_limit_flag = 0;
      next_tetris = rand() % 7;	

      Draw_board();				// draw game board

      do {
	if(tetris_end_flag == 1)
	  Delete_tetris(del_tetris,&del_pos_x,&del_pos_y);
	else
	  tetris_end_flag = left_limit_flag = right_limit_flag = y_limit_flag = rotate_limit_flag = 0;

	if(y_limit_flag < 2) left_limit_flag = right_limit_flag = rotate_limit_flag = 0;

	if(tetris_end_flag == 0)
	  { del_line = Check_line(pos_y,tetris_height);
	    score += (del_line < 0) ? 0 : (del_line == 0) ? 10 : (del_line*del_line)*100;
	    Display_score(score);		// display current score

	    pos_x = 4;				// initialize start of current tetromino
	    pos_y = 0;
	    tetris_width = 0;
	    tetris_height = -1;
	    current_tetris = next_tetris;

	    next_tetris = rand() % 7;		// get next tetromino number
            if(next_tetris == current_tetris) next_tetris = rand() % 7;
	    Draw_next_tetris(next_tetris);

	    for(y = 0; y < 4; y++)		// get tetromino
	      for(x = 0; x < 4; x++)
		{ temp_tetris[y][x] = tetris[current_tetris][y][x];
		  if(temp_tetris[y][x] == '1')
		    { if(tetris_width < x) tetris_width = x;
		      if(tetris_height < y) tetris_height = y;
		    }
		}
	  }
	Check_limit(temp_tetris,&pos_x,&pos_y,&tetris_end_flag,&left_limit_flag,&right_limit_flag,&y_limit_flag,&rotate_limit_flag,current_tetris);

	if(tetris_end_flag == 0 && pos_y == 0)
	  { game_end_flag = 1;
	    break;
	  }
	if(y_limit_flag > 0 && key_move_flag == 1 && time_move_flag == 0) y_limit_flag = 1;

	if(pos_x+tetris_width > 9) pos_x = 9-tetris_width;
	Add_tetris(temp_tetris,&pos_x,&pos_y,current_tetris);

	del_pos_x = pos_x;
	del_pos_y = pos_y;
	for(y = 0; y < 4; y++)
	  for(x = 0; x < 4; x++)
	    del_tetris[y][x] = temp_tetris[y][x];

	Draw_game_tetris();			// draw game status

	time_move_flag = 0;
	key_move_flag = 0;

	while(time_move_flag == 0 && key_move_flag == 0)	
	  { Delay_ms(10);
	    if(t_ch < 12) t_ch++;		// check time move
	    else
	      {	time_move_flag = 1;
		t_ch = 0;
		if(y_limit_flag == 0) pos_y++;
	      }
	    if(time_move_flag == 0 && key_move_flag == 0 )	// check key move
	      {	key = Key_input_silent();
		switch(key)
		  { case Left:  if(left_limit_flag == 0)
				  { pos_x--;
				    key_move_flag = 1;
				  }
				break;
		    case Right:	if(right_limit_flag == 0)
		    		  { pos_x++;
				    key_move_flag = 1;
				  }
				break;
		    case Down:  if(y_limit_flag == 0)
				  { pos_y = Drop_tetris(temp_tetris,pos_x,pos_y,tetris_width,tetris_height);
				    Beep();
				  }
				key_move_flag = 1; 
				break;
		    case Rotate:if(rotate_limit_flag == 0)
		    		  { if(current_tetris == 1)						
				      Rotate_tetris(temp_tetris,4,&tetris_width,&tetris_height);	
				    else
				      Rotate_tetris(temp_tetris,3,&tetris_width,&tetris_height);
				    key_move_flag = 1;
				  }	
				break;
		    default:	key_move_flag = 0;
				break;
		  }
	      }
	  }
      } while(game_end_flag == 0);

      TFT_string(5,38,Magenta,Black, "GAME OVER !!!");	// display game over
      Beep_3times();

      TFT_string( 6,18,Green,  Black, "Press ");	// wait KEY1 to start
      TFT_string(12,18,Magenta,Black, "KEY1");
      TFT_string( 6,20,Green,  Black, "to start !");
      key = 0;
      while(key != KEY1) key = Key_input();
    }
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

const unsigned short block[2][12] = 
	{{ 0xFFF0,0xC030,0xA050,0x9090,0x8910,0x8610,	// block □
	   0x8610,0x8910,0x9090,0xA050,0xC030,0xFFF0 },
	 { 0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,	// block ■
	   0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0,0xFFF0 }};

void Draw_block(U16 xPos,U16 yPos,U16 color,U08 type)	/* draw a block */
{
  unsigned char x, y;

  for(y = 0; y < 12; y++)
    { TFT_GRAM_address(xPos+1, yPos+y+1);
      for(x = 0; x < 12; x++) 
        { if((block[type][y] << x) & 0x8000) TFT_data(color);
          else                               TFT_data(Black);
        }
    }
}

void Draw_board(void)				/* draw outline of game board */
{
  unsigned char x, y, array;

  TFT_color_screen(Black);			// clear screen
  TFT_string(2,0,White,Blue, "       TETRIS 게임        "); // display title

  for(y = 0; y < 22; y++)			// main board box
    { if(y == 0 || y == 21) array = 0;
      else                  array = 1;

      for(x = 0; x < 12; x++)
	if(board[array][x] == '3')
	  Draw_block(Xbase+x*13, Ybase+y*13, White, 0);
    }

  for(x = 0; x < 4; x++)			// next tetromino box
    { Draw_block(Xbase+12*13+x*13, Ybase+0,    White, 0);
      Draw_block(Xbase+12*13+x*13, Ybase+7*13, White, 0);
    }
  for(y = 0; y < 8; y++)
    Draw_block(Xbase+16*13, Ybase+y*13, White, 0);

  TFT_string(22,19,White, Magenta," 점수 ");	// display score message

  TFT_string(22,28,Green,  Black, "KEY1");	// display key
  TFT_string(27,28,Magenta,Black, "<-");
  Line(217,231, 230,231, Magenta);
  TFT_string(22,30,Green,  Black, "KEY2");
  TFT_string(27,30,Magenta,Black, "Dn");
  TFT_string(22,32,Green,  Black, "KEY3");
  TFT_string(27,32,Magenta,Black, "Ro");
  TFT_string(22,34,Green,  Black, "KEY4");
  TFT_string(27,34,Magenta,Black, "->");
  Line(217,279, 230,279, Magenta);
}

void Display_score(U16 score)			// display score
{
  TFT_xy(22, 22);  TFT_color(Yellow,Black);
  TFT_unsigned_decimal(score, 0, 5);
}

void Draw_game_tetris(void)			/* draw tetromino of game */
{
  unsigned char  x, y;
  unsigned short color;

  for(y = 0; y < sizeof(screen)/sizeof(*screen); y++)
    for(x = 0; x < 10; x++)
      {	switch(screen[y][x])
          { case '4'+0: color = White;		break;
	    case '4'+1: color = Blue;		break;
	    case '4'+2: color = Yellow;		break;
	    case '4'+3: color = Magenta;	break;
	    case '4'+4: color = Red;		break;
	    case '4'+5: color = Green;		break;
	    case '4'+6: color = Cyan;		break;
	    default:    color = Black;		break;
	  }
	Draw_block(Xbase+13+x*13, Ybase+13+y*13, color, 1);
      }
}

void Draw_next_tetris(S08 next)			/* draw next tetromino */
{
  unsigned char  x, y, xcenter, ycenter;
  unsigned short color = 0;

  for(y = 0; y < 5*13; y++)			// clear space for next tetromino
    { TFT_GRAM_address(Xbase+12*13, Ybase+2*13+y);
      for(x = 0; x < 4*13; x++)
        TFT_data(Black);
    }

  switch(next)					// get position of next tetromino
    { case 0:	xcenter = 13*13;   ycenter = 3*13;	break;
      case 1:	xcenter = 13*13+6; ycenter = 2*13;	break;
      default:	xcenter = 13*13;   ycenter = 2*13+6;	break;
    }

  switch(next)					// get color of next tetromino
    { case 0:	color = White;		break;
      case 1:	color = Blue;		break;
      case 2:	color = Yellow;		break;
      case 3:	color = Magenta;	break;
      case 4:	color = Red;		break;
      case 5:	color = Green;		break;
      case 6:	color = Cyan;		break;
    }

  for(y = 0; y < 4; y++)			// draw next tetromino
    for(x = 0; x < 4; x++)
      if(tetris[next][y][x] == '1')
	Draw_block(Xbase+xcenter+x*13, Ybase+ycenter+y*13, color, 1);
}

/* ----- 게임 알고리즘 함수 --------------------------------------------------- */

void Add_tetris(S08 (*src)[4],S08 *xPos,S08 *yPos,U08 number)	/* add tetromino */
{
  signed char x, y, x1, y1 = -1, block_cnt = 0;

  for(y = 3; y >= 0; y--)
    { if(block_cnt < 4)	y1++;
      if(*yPos-y1 < 0)  break;

      block_cnt = 0;
      x1 = 0;

      for(x = 0; x < 4; x++)
	{ switch(src[y][x])
	    { case '0':	block_cnt++;
	    		break;
	      case '3':	x1++;
	      		break;
	      default:	screen[*yPos-y1][(*xPos)+x1] = '4'+number;
			x1++;
			break;
	    }
	}
    }
}

void Delete_tetris(S08 (*src)[4],S08 *xPos,S08 *yPos)	/* delete tetromino */
{
  signed char x, y, x1, y1 = -1, block_cnt = 0;

  for(y = 3; y >= 0; y--)
    { if(block_cnt < 4) y1++;
      if(*yPos-y1 < 0)  break;

      block_cnt = 0;
      x1 = 0;

      for(x = 0; x < 4; x++)
	{ switch(src[y][x])
	    { case '0': block_cnt++;
	    		break;
	      case '3':	x1++;
	      		break;
	      default:	screen[*yPos-y1][(*xPos)+x1] = '2';
			x1++;
			break;
	    }
	}
    }
}

void Rotate_tetris(S08 (*src)[4],U08 size,S08 *width,S08 *height)	/* rotate tetromino */
{
  unsigned char y, x, block_cnt, w_temp, h_temp, temp[4][4];

  w_temp = h_temp = 0;

  for(y = 0; y < size; y++)
    { block_cnt = 0;
      for(x = 0; x < size; x++)
	{ temp[y][size-1-x] = src[x][y];
	  if(temp[y][size-1-x] == '1' || temp[y][size-1-x] == '3') 
	    { block_cnt++;
	      if(h_temp < y) h_temp = y;
	    }
	}
      if(w_temp < block_cnt-1) w_temp = block_cnt-1;
    }

  for(y = 0; y < size; y++)
    for(x = 0; x < size; x++) src[y][x] = temp[y][x];

  *width = w_temp;
  *height = h_temp;
}

signed char Drop_tetris(S08 (*src)[4],S08 xPos,S08 yPos,S08 width,S08 height)	/* drop tetromino */
{
  signed char x, y, block_cnt, xb_cnt = 0, return_y = 20, y_pos, y_pos_arr[4];

  for(x = 0; x < 4; x++)
    { block_cnt = 0;
      for(y = height; y >= 0; y--)
	{ if(src[y][x] == '1')	
	    { y_pos_arr[xb_cnt++] = yPos-(height-(y+block_cnt));
	      break;
	    }
	  else if(src[y][x] == '0') block_cnt++;	
	}
    }

  for(x = xPos; x <= xPos+width; x++)
    { if(y_pos_arr[x-xPos] < 0) y_pos = yPos;
      else y_pos = y_pos_arr[x-xPos];

      for(y = y_pos+1; y < 20; y++)
	{ if(screen[y][x] > '3')
	    { y_pos_arr[x-xPos] = y-1+(yPos-y_pos_arr[x-xPos]);
	      break;
	    }
	}
      if(y > 19) y_pos_arr[x-xPos] = 19;
    }

  for(x = 0; x <= width; x++)
    if(return_y > y_pos_arr[x]) return_y = y_pos_arr[x];

  return return_y;
}

signed char Check_line(S08 yPos,S08 height)	/* check if line is OK */
{
  signed char x, y, line, no_blank_flag = 0, del_line_count = 0;
  signed char del_line[4], temp[4][10];

  if(height < 0) return -1;

  for(y = yPos; y >= yPos-height; y--)
    { no_blank_flag = 0;
      for(x = 0; x < 10; x++)			// check if horizontal line is OK
	{ if(screen[y][x] == '2')
	    no_blank_flag = 1;
	}
      if(no_blank_flag == 0)			// if yes, delete and sound
	{ del_line[del_line_count++] = y;
          Beep_3times();
        }
    }

  if(del_line_count > 0)	
    { for(y = 0; y < del_line_count; y++)
        for(x = 0; x < 10; x++) temp[y][x] = screen[del_line[y]][x];

      for(line = 0; line < 3; line++)
	{ for(y = 0; y < del_line_count; y++)
	    { if(!(line % 2))
                for(x = 0; x < 10; x++) screen[del_line[y]][x] = '2';
	      else 
                for(x = 0; x < 10; x++) screen[del_line[y]][x] = temp[y][x];
	    }
	  Draw_game_tetris();
          Delay_ms(100);
	}
	
      for(line = 0; line < del_line_count; line++)
	{ for(y = del_line[line]+line; y > 0; y--)
            for(x = 0; x < 10; x++) screen[y][x] = screen[y-1][x];
          for(x = 0; x < 10; x++) screen[0][x] = '2';			
	}
    }

  return del_line_count;
}

void Check_limit(S08 (*src)[4],S08 *xPos,S08 *yPos,S08 *tetris_end,S08 *left_limit,S08 *right_limit,S08 *y_limit,S08 *rotate_limit,S08 current)
{
  signed char x, y, x_pos, y_pos = -1, width, height, size, block_cnt = 0;
  signed char default_y_limit = 0, blank_y_limit = 0, y_pos_limit = 0;
  signed char temp[4][4];

  if(current != 0)
    { if(current == 1) size = 4;
      else             size = 3;

      for(y = 0; y < size; y++)	
	for(x = 0; x < size; x++) temp[y][x] = src[y][x];

      Rotate_tetris(temp,size,&width,&height);

      if(*xPos+width > 9) x_pos = 9-width;
      else                x_pos = *xPos/2;

      for(y = height; y >= 0; y--)	
	{ if(*yPos+y-height < 0) break;
	  for(x = 0; x <= width; x++)
	    if(temp[y][x] == '1' && screen[*yPos+y-height][x_pos+x] > '3') *rotate_limit = 1;
	  if(*rotate_limit == 1) break;
	}
    }
  else *rotate_limit = 1;

  for(y = 3; y >= 0; y--)
    { if(block_cnt < 4) y_pos++;
      if(*yPos-y_pos < 0) break; 
      block_cnt = 0;
      x_pos = 0;
      for(x = 0; x < 4; x++)
	{ switch(src[y][x])
	    { case '0': block_cnt++;
			break;
	      case '1':	if(*left_limit == 0)
			  { if((*xPos)+x_pos-1 >= 0) 
			      {	if(screen[*yPos-y_pos][(*xPos)+x_pos-1] > '3')
				  *left_limit = 1;	
			      }
			    else *left_limit = 1;
			  }
			if(*right_limit == 0)
			  { if((*xPos)+x_pos+1 <= 9)
			      { if(screen[*yPos-y_pos][(*xPos)+x_pos+1] > '3')
				  *right_limit = 1;
			      }
			    else *right_limit = 1;
			  }
			if(*yPos+1 < 20)
			  { if(screen[*yPos-y_pos+1][(*xPos)+x_pos] > '3')
			      { y_pos_limit = 1;
				default_y_limit = 1;
			      }
			    if(screen[*yPos-y_pos+1][(*xPos)+x_pos] == '2' && default_y_limit == 0)
			      blank_y_limit = 1;
			  }
			else y_pos_limit = 1;
			x_pos++;
			break;
	      default:	x_pos++;
			break;
	    }
	}
    }

  if(blank_y_limit == 1 && default_y_limit == 0) *y_limit = 0;
  if(y_pos_limit == 1) (*y_limit)++;
  if(*y_limit < 2) *tetris_end = 1;
  else             *tetris_end = 0;
}
