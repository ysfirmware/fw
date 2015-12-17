/* ============================================================================ */
/*	   OK-STM746 V1.0 키트용(STM32F746VET6) SD 카드 헤더 파일		*/
/* ============================================================================ */
/*					Programmed by Duck-Yong Yoon in 2015.   */

// -------------------------------------------------------------------------------
//   이 헤더파일은 OK-STM746 키트 V1.0을 위한 것입니다. 이것은 아래의 조건에서만
//  올바른 동작을 보장합니다.
// -------------------------------------------------------------------------------
//	(1) 시스템 클록 : 216MHz
//	(2) C 컴파일러  : IAR EWARM V7.50.1
//	(3) 최적화 옵션 : High/Size
//	(4) CSTACK 크기 : 0x1000
//      (5) 인클루드    : 기본 헤더 파일 OK-STM746.h의 뒤에 인클루드할 것.
// -------------------------------------------------------------------------------

void SPI3_low_speed(void);			// initialize SPI3(210.9375kHz)
void SPI3_medium_speed(void);			// initialize SPI3(1.6875MHz)
void SPI3_high_speed(void);			// initialize SPI3(6.75MHz)
unsigned char SPI3_write(U08 data);		// send a byte to SPI3 and receive

void Initialize_SD(void);			// initialize SD card
void SD_command(U08 command, U32 sector);	// send SD card command
void SD_read_sector(U32 sector, U08* buffer);	// read a sector of SD card

/* ---------------------------------------------------------------------------- */

#define CMD0		0			// GO_IDLE_STATE
#define CMD8		8			// SEND_IF_COND

#define CMD17		17			// READ_SINGLE_BLOCK
#define CMD55		55			// APP_CMD
#define CMD58		58			// READ_OCR
#define ACMD41		41			// SD_SEND_OP_COND

#define VER1X		10			// Ver1.X SD Memory Card
#define VER2X		20			// Ver2.X SD Memory Card
#define VER2X_SC	21			// Ver2.X Standard Capacity SD Memory Card
#define VER2X_HC	22			// Ver2.X High Capacity SD Memory Card(SDHC)

unsigned char SD_type;				// SD card version(Ver1.X or Ver2.0)

/* ---------------------------------------------------------------------------- */

void SPI3_low_speed(void)			/* initialize SPI3(210.9375kHz) */
{
  RCC->APB1ENR |= 0x00008000;			// enable SPI3 clock

  GPIOC->MODER &= 0xC00C03FF;			// alternate and input/output mode
  GPIOC->MODER |= 0x06A11400;
  GPIOC->AFR[1] |= 0x00066600;			// PC12 = SPI3_MOSI, PC11 = SPI3_MISO, PC10 = SPI3_SCK
  GPIOC->ODR |= 0x00002160;			// -SD_CS = -MP3_RESET = -MP3_DCS = -MP3_CS = 1
  GPIOC->OSPEEDR |= 0x0AA02800;			// -SD_CS = SPI3 = -MP3_DCS/CS = 100MHz fast speed

  SPI3->CR1 = 0x037C;				// master mode, 54MHz/256 = 210.9375kHz (Max 400kHz)
  SPI3->CR2 = 0x1700;				// 8-bit data, disable SS output, CPOL = CPHA = 0
}

void SPI3_medium_speed(void)			/* initialize SPI3(1.6875MHz) */
{
  RCC->APB1ENR |= 0x00008000;			// enable SPI3 clock

  GPIOC->MODER &= 0xC00C03FF;			// alternate and input/output mode
  GPIOC->MODER |= 0x06A11400;
  GPIOC->AFR[1] |= 0x00066600;			// PC12 = SPI3_MOSI, PC11 = SPI3_MISO, PC10 = SPI3_SCK
  GPIOC->ODR |= 0x00002160;			// -SD_CS = -MP3_RESET = -MP3_DCS = -MP3_CS = 1
  GPIOC->OSPEEDR |= 0x0AA02800;			// -SD_CS = SPI3 = -MP3_DCS/CS = 100MHz fast speed

  SPI3->CR1 = 0x0364;				// master mode, 54MHz/32 = 1.6875MHz
  SPI3->CR2 = 0x1700;				// 8-bit data, disable SS output, CPOL = CPHA = 0
}

void SPI3_high_speed(void)			/* initialize SPI3(6.75MHz) */
{
  RCC->APB1ENR |= 0x00008000;			// enable SPI3 clock

  GPIOC->MODER &= 0xC00C03FF;			// alternate and input/output mode
  GPIOC->MODER |= 0x06A11400;
  GPIOC->AFR[1] |= 0x00066600;			// PC12 = SPI3_MOSI, PC11 = SPI3_MISO, PC10 = SPI3_SCK
  GPIOC->ODR |= 0x00002160;			// -SD_CS = -MP3_RESET = -MP3_DCS = -MP3_CS = 1
  GPIOC->OSPEEDR |= 0x0AA02800;			// -SD_CS = SPI3 = -MP3_DCS/CS = 100MHz fast speed

  SPI3->CR1 = 0x0354;				// master mode, 54MHz/8 = 6.75MHz (Max 25MHz)
  SPI3->CR2 = 0x1700;				// 8-bit data, disable SS output, CPOL = CPHA = 0
}

unsigned char SPI3_write(U08 data)		/* send a byte to SPI3 and receive */
{
  unsigned char byte = 0;
  unsigned int address;

  byte |= SPI3->DR;				// clear RXNE flag

  address = (uint32_t)SPI3 + 0x0C;
  *(__IO uint8_t *)address = data;

  while((SPI3->SR & 0x0003) != 0x0003);

  return SPI3->DR;
}

/* ---------------------------------------------------------------------------- */

void Initialize_SD(void)			/* initialize SD/SDHC card */
{
  unsigned char i, j;
  unsigned char R1, CMD_flag, R3[5], R7[5];

  TFT_string(0,2,White,Magenta,"           (1) SD 카드 초기화           ");
  Delay_ms(100);

  if(GPIOC->IDR & 0x00004000)
    { TFT_string(10,20,Red,Black,"SD 카드가 없습니다 !");
      Beep_3times();
      while(1);
    }

  SPI3_low_speed();				// initialize SPI3 (below 400kHz)
  GPIOC->BSRR = 0x00002000;			// -SD_CS = 1
  for(i = 0; i < 10; i++) SPI3_write(0xFF);	// set SPI mode(80 clocks)
  Delay_ms(1);

  GPIOC->BSRR = 0x20000000;			// -SD_CS = 0

  TFT_string(4,5,Magenta,Black,"CMD0 :  ");	// second CMD0
  TFT_color(Cyan, Black);
  SD_command(CMD0, 0);				// send CMD0(reset and go to SPI mode)
  CMD_flag = 0;
  for(i = 0; i < 5; i++)			// display R1
    { R1 = SPI3_write(0xFF);
      TFT_hexadecimal(R1,2);
      TFT_English(' ');
      if(R1 == 0x01)
        CMD_flag = 1;
    }
  if(CMD_flag == 1)
    TFT_string(30,5,Green,  Black,"/ OK !");
  else
    { TFT_string(30,5,Red,Black,"/ Error!");
      TFT_string(6,20,Red,Black,"SD 카드가 응답하지 않습니다 !");
      TFT_hexadecimal(SPI3->CR2,4);
      Beep_3times();
      while(1);
    }

  GPIOC->BSRR = 0x00002000;			// -SD_CS = 1

  SPI3_high_speed();				// initialize SPI3 (below 25MHz)
  GPIOC->BSRR = 0x20000000;			// -SD_CS = 0

  TFT_string(4,7,Magenta,Black,"CMD8 :  ");
  TFT_color(Cyan, Black);
  SD_command(CMD8, 0x000001AA);			// send CMD8(check version)
  CMD_flag = 0;
  for(i = 0; i < 5; i++)
    { TFT_xy(12,7);
      R7[0] =  SPI3_write(0xFF);
      TFT_hexadecimal(R7[0],2);
      TFT_English(' ');
      if(R7[0] == 0x05)				// if R1 = 0x05, Ver 1.X
        { SD_type = VER1X;
          CMD_flag = 1;
          TFT_string(30,7,Magenta,Black,"/ V1.X");
	  break;
	}
      else if(R7[0] == 0x01)			// if R1 = 0x01, Ver 2.X
        { for(j = 1; j <= 4; j++)
            { R7[j] = SPI3_write(0xFF);
              TFT_hexadecimal(R7[j],2);
              TFT_English(' ');
            }
          if((R7[3] == 0x01) && (R7[4] == 0xAA))
	    { SD_type = VER2X;
	      CMD_flag = 1;
              TFT_string(30,7,Magenta,Black,"/ V2.X");
	      break;
            }
	}
    }
  SPI3_write(0xFF);				// send dummy clocks
  if(CMD_flag == 0)
    { TFT_string(30,7,Red,Black,"/ Error!");
      TFT_string(5,20,Red,Black,"SD card with unknown format !");
      Beep_3times();
      while(1);
    }

  CMD_flag = 0;
  for(i = 0; i < 100; i++)			// send ACMD41(start initialization)
    { TFT_string(4,10,Magenta,Black,"CMD55:  ");
      TFT_color(Cyan, Black);
      SD_command(CMD55, 0);
      for(j = 0; j < 5; j++)			// receive R1 = 0x01
        { R1 = SPI3_write(0xFF);
          TFT_hexadecimal(R1,2);
          TFT_English(' ');
        }
      TFT_string(4,12,Magenta,Black,"ACMD41: ");
      TFT_color(Cyan, Black);
      SD_command(ACMD41, 0x40000000);		// HCS = 1
      for(j = 0; j < 5; j++)			// receive R1 = 0x00
        { R1 = SPI3_write(0xFF);
          TFT_hexadecimal(R1,2);
          TFT_English(' ');
          if(R1 == 0x00)
            CMD_flag = 1;
        }
      if(CMD_flag == 1)
        break;
    }
  if(CMD_flag == 1)
    TFT_string(30,12,Green,  Black,"/ OK !");
  else
    { TFT_string(30,12,Red,Black,"/ Error!");
      TFT_string( 9,20,Red,Black,"SD card is not ready !");
      Beep_3times();
      while(1);
    }

  if(SD_type == VER2X)
    { TFT_string(4,14,Magenta,Black,"CMD58:  "); // send CMD58(check SDHC)
      TFT_color(Cyan, Black);
      SD_command(CMD58, 0);
      for(i = 0; i < 5; i++)
        { TFT_xy(12,14);
          R3[0] =  SPI3_write(0xFF);
          TFT_hexadecimal(R3[0],2);
          TFT_English(' ');
          if(R3[0] == 0x00)
            { for(j = 1; j <= 4; j++)
                { R3[j] = SPI3_write(0xFF);
                  TFT_hexadecimal(R3[j],2);
                  TFT_English(' ');
	        }
              break;
	    }
        }
      if((R3[1] & 0xC0) == 0xC0)		// if CCS = 1, High Capacity(SDHC)
        { SD_type = VER2X_HC;
          TFT_string(30,14,Magenta,Black,"/ SDHC");
        }
      else					// if CCS = 0, Standard Capacity
        { SD_type = VER2X_SC;
          TFT_string(30,14,Magenta,Black,"/ SC");
        }
      SPI3_write(0xFF);				// send dummy clocks
    }

  GPIOC->BSRR = 0x00002000;			// -SD_CS = 1

  TFT_string(13,17,Green,Black,"초기화 완료 !");
}

void SD_command(U08 command, U32 sector)	/* send SD card command */
{
  unsigned char check;

  SPI3_write(0xFF);				// dummy
  SPI3_write(command | 0x40);			// send command index
  SPI3_write(sector >> 24);			// send 32-bit argument
  SPI3_write(sector >> 16);
  SPI3_write(sector >> 8);
  SPI3_write(sector);

  if(command == CMD0)      check = 0x95;	// CRC for CMD0
  else if(command == CMD8) check = 0x87;	// CRC for CMD8(0x000001AA)
  else                     check = 0xFF;	// no CRC for other CMD

  SPI3_write(check);				// send CRC
}

void SD_read_sector(U32 sector, U08 *buffer)	/* read a sector of SD card */
{
  unsigned int i;

  if((SD_type == VER1X) || (SD_type == VER2X_SC)) // if not SDHC, sector = sector*512
    sector <<= 9;

  GPIOC->BSRR = 0x20000000;			// -SD_CS = 0

  SD_command(CMD17, sector);			// send CMD17(read a block)

  while(SPI3_write(0xFF) != 0x00);		// wait for R1 = 0x00
  while(SPI3_write(0xFF) != 0xFE);		// wait for Start Block Token = 0xFE

  for(i = 0; i < 512; i++)			// receive 512-byte data
    buffer[i] = SPI3_write(0xFF);

  SPI3_write(0xFF);				// receive CRC(2 bytes)
  SPI3_write(0xFF);
  SPI3_write(0xFF);				// send dummy clocks

  GPIOC->BSRR = 0x00002000;			// -SD_CS = 1
}

