/* ============================================================================ */
/*	Exp19_1_photo_album(1).c : SD 카드의 BMP 파일을 사용한 전자 앨범	*/
/* ============================================================================ */
/*                          Designed and programmed by Duck-Yong Yoon in 2013.  */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"
#include "OK-STM32F0_SD.h"
#include "OK-STM32F0_SD_FAT32.h"

void SysTick_Handler(void);			// SysTick interrupt function
void TFT_filenumber(void);			// display file number
void TFT_filename(U08 file_number);		// display BMP file name
void TFT_display_BMP(U32 sector);		// display BMP file of SD card

volatile unsigned char timer = 0;		// 1 sec timer

unsigned char total_file;			// total file number
unsigned char file_number = 0;			// current file number
unsigned char BMP_type;				// BMP image type(P=portrait, L=landscape)

/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void SysTick_Handler(void)			/* SysTick interrupt function */
{
  timer++;
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  unsigned char i;
  unsigned char display_flag = 1;		// new screen display flag
  unsigned int  BMP_start_sector[MAX_FILE];	// BMP file start sector

  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module

  LCD_string(0x80,"OK-STM32F0  V1.0");		// display title
  LCD_string(0xC0,"   Exp19_1.c    ");

  Initialize_SD();				// initialize SD card
  Initialize_FAT32();				// initialize FAT32 file system
  Beep();
  Delay_ms(1000);

  TFT_clear_screen();
  TFT_string(0,5,Green,  Black,"****************************************");
  TFT_string(0,7,White,Magenta,"           BMP 파일  전자앨범           ");
  TFT_string(0,9,Green,  Black,"****************************************");
  TFT_string(4,16,Cyan,   Black,"320x240 픽셀의 랜드스케이프 화면");
  TFT_string(4,19,White,  Black,"              또는              ");
  TFT_string(4,22,Green,  Black," 240x320 픽셀의 포트레이트 화면 ");
  Delay_ms(3000);

  SysTick->LOAD  = 5999999;			// N-1 for 1sec (6MHz/6000000 = 1Hz)
  SysTick->VAL   = 0;				// clear SysTick Counter
  SysTick->CTRL  = 0x00000003;			// 48MHz/8 = 6MHz, SysTick enable and interrupt request

  total_file = fatGetDirEntry(FirstDirCluster);	// get total file number

  for(i = 0; i < total_file; i++)		// get start address of all files
    BMP_start_sector[i] = fatClustToSect(file_start_cluster[i]);

  file_number = 0;

  while(1)
    { if(display_flag == 1)			// display BMP file, file number, and file name
        { Get_long_filename(file_number);	// check file name
	  if(extension != 0x00424D50)		// if not BMP file, skip
            { if(file_number < (total_file - 1))
	        file_number++;
	      else
	        file_number = 0;
	    }
          else					// if BMP file, display
	    { TFT_display_BMP(BMP_start_sector[file_number]);
	      TFT_filenumber();
	      TFT_filename(file_number);

	      display_flag = 0;
	    }
	}

      if(timer >= 5)				// go next file by 5 sec step
	{ timer = 0;
	  if(file_number < (total_file - 1))            
	    file_number++;
	  else
	    file_number = 0;

	  display_flag = 1;
        }
    }
}

/* ----- 사용자 정의 함수 ----------------------------------------------------- */

void TFT_filenumber(void)			/* display file number */
{
  if(BMP_type == 'P')				// screen mode(Portrait or Landscape)
    TFT_portrait_mode();
  else if(BMP_type == 'L')
    TFT_landscape_mode();

  TFT_xy(0,0);					// start position and color
  TFT_color(White,Black);

  TFT_English('(');
  TFT_unsigned_decimal(file_number+1,1,2);	// current file number
  TFT_English('/');
  TFT_unsigned_decimal(total_file,1,2);		// total file number
  TFT_English(')');
}

void TFT_filename(U08 file_number)		/* display BMP file name */
{
  unsigned char file_flag;

  file_flag = Get_long_filename(file_number);	// check file name

  if(file_flag == 0)				// short file name(8.3 format)
    TFT_short_filename(8,0,Yellow,Black);
  else if(file_flag == 1)			// long file name
    TFT_long_filename(8,0,Yellow,Black);
  else if(file_flag == 2)			// file name is longer than 195 characters
    TFT_string(8,0,White,Black,"* 파일명 길이 초과 *");
  else						// file name error
    TFT_string(8,0,White,Black,"*** 파일명 오류 ***");
}

/* ---------------------------------------------------------------------------- */

#pragma pack(1)					// byte(8 bit) align

struct BMP_header{				/* Header 14 bytes + InfoHeader 40 bytes */
 unsigned char  signature[2];			// 'BM'
 unsigned int   file_size;			// file size in bytes
 unsigned int   reserved;			// unused (= 0)
 unsigned int   data_offset;			// file offset to Raster Data

 unsigned int   size;				// size of InfoHeader (= 40)
 unsigned int   width;				// bitmap width
 unsigned int   height;				// bitmap height
 unsigned short planes;				// number of planes (= 1)
 unsigned short bit_count;			// bits per pixel 
 unsigned int   compression;			// type of compression   
 unsigned int   image_size;			// size of image
 unsigned int   x_pixels_per_m;			// horizontal resolution(pixels/meter)
 unsigned int   y_pixels_per_m;			// vertical resolution(pixels/meter)
 unsigned int   colors_used;			// number of actually used colors
 unsigned int   colors_important;		// number of important colors. 0 = all
};

#pragma pack()					// word(32 bit) align

// Description : SD 카드에 저장된 BMP 이미지 파일을 TFT-LCD에 출력하기
// sector = BMP 이미지가 들어있는 파일시작 섹터번호
//
//         [B]         [G]       [R]
//          5           6         5
//       11111000   11111100   11111000
//          &           &         &
//        buf[0]      buf[1]    buf[2]
//
//         >> 3        << 3      << 8 (BGR 형태를 RGB 형태로 다시 배열)
//
//          [R]         [G]        [B]
// pixel = ?????      ??????      ????? = 16 bit

void TFT_display_BMP(U32 sector)		/* display BMP file of SD card */
{
  unsigned char  buffer_control;
  unsigned short i, pixel, sector_cnt;
  unsigned char  BMPbuffer[512];   		// BMP data from SD card

  struct BMP_header *bmp_ptr;

  SD_read_sector(sector, BMPbuffer);		// read sector of BMP header
  bmp_ptr = (struct BMP_header *)BMPbuffer;

  if(bmp_ptr->width == 240)			// if 240*320, Portrait screen(bottom --> top)
    { BMP_type = 'P';
      TFT_write(0x16, 0x0088);			// MV=0, MX=0, MY=1, BGR=1
      TFT_write(0x02, 0x0000);			// x = 0~239
      TFT_write(0x03, 0x0000);
      TFT_write(0x04, 0x0000);
      TFT_write(0x05, 0x00EF);
      TFT_write(0x06, 0x0000);			// y = 0~319
      TFT_write(0x07, 0x0000);
      TFT_write(0x08, 0x0001);
      TFT_write(0x09, 0x003F);
      TFT_command(0x22);
    }
  else if(bmp_ptr->width == 320)		// if 320*240, Landscape screen(bottom --> top)
    { BMP_type = 'L';
      TFT_write(0x16, 0x0028);			// MV=1, MX=0, MY=0, BGR=1
      TFT_write(0x02, 0x0000);			// x = 0~319
      TFT_write(0x03, 0x0000);
      TFT_write(0x04, 0x0001);
      TFT_write(0x05, 0x003F);
      TFT_write(0x06, 0x0000);			// y = 0~239
      TFT_write(0x07, 0x0000);
      TFT_write(0x08, 0x0000);
      TFT_write(0x09, 0x00EF);
      TFT_command(0x22);
    }

  for(i = 0x36; i <= 509; i += 3)
    { pixel  = (BMPbuffer[i] & 0xf8) >> 3;	// Blue
      pixel |= (BMPbuffer[i+1] & 0xfc) << 3;	// Green
      pixel |= (BMPbuffer[i+2] & 0xf8) << 8;	// Red
      TFT_data(pixel);
    }
  sector++;
  buffer_control = 1;

  for(sector_cnt = 0; sector_cnt <= 449; sector_cnt++)	// read next sector and output RGB data
    {
      switch (buffer_control)			// rotate case 1, 2, 3
	{ case 1: pixel  = (BMPbuffer[510] & 0xF8) >> 3;
		  pixel |= (BMPbuffer[511] & 0xFC) << 3;
		  SD_read_sector(sector, BMPbuffer);
		  pixel |= (BMPbuffer[0] & 0xF8) << 8;
		  TFT_data(pixel);

		  for(i = 1; i <= 509; i += 3)
		    { pixel  = (BMPbuffer[i] & 0xf8) >> 3;
		      pixel |= (BMPbuffer[i+1] & 0xfc) << 3;
		      pixel |= (BMPbuffer[i+2] & 0xf8) << 8;
		      TFT_data(pixel);
		    }
		  sector++;
		  buffer_control = 2;
		  break;

	  case 2: pixel  = (BMPbuffer[511] & 0xF8) >> 3;
		  SD_read_sector(sector, BMPbuffer);
		  pixel |= (BMPbuffer[0] & 0xFC) << 3;
		  pixel |= (BMPbuffer[1] & 0xF8) << 8;
		  TFT_data(pixel);

		  for(i = 2; i <= 509; i += 3)
		    { pixel  = (BMPbuffer[i] & 0xF8) >> 3;
		      pixel |= (BMPbuffer[i+1] & 0xFC) << 3;
		      pixel |= (BMPbuffer[i+2] & 0xF8) << 8;
		      TFT_data(pixel);
		    }
		   sector++;
		   buffer_control = 3;
		   break;

	  case 3: SD_read_sector(sector, BMPbuffer);
		  if(sector_cnt != 449)
		    { for(i = 0; i <= 509; i += 3)
			{ pixel  = (BMPbuffer[i] & 0xF8) >> 3;
			  pixel |= (BMPbuffer[i+1] & 0xFC) << 3;
			  pixel |= (BMPbuffer[i+2] & 0xF8) << 8;
			  TFT_data(pixel);
			}
		    }
		  else				// if last sector, read 54 bytes only
		    { for(i = 0; i < 54; i += 3)
			{ pixel  = (BMPbuffer[i] & 0xF8) >> 3;
			  pixel |= (BMPbuffer[i+1] & 0xFC) << 3;
			  pixel |= (BMPbuffer[i+2] & 0xF8) << 8;
			  TFT_data(pixel);
			}
		    }
		  sector++;
		  buffer_control = 1;
		  break;
	}
    }

  TFT_landscape_mode();				// landscape mode
}
