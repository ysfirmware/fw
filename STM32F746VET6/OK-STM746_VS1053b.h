/* ============================================================================ */
/*	    OK-STM746 V1.0 ŰƮ��(STM32F746VET6) VS1053b ��� ����		*/
/* ============================================================================ */
/*					Programmed by Duck_Yong Yoon in 2015.   */

// -------------------------------------------------------------------------------
//   �� ��������� OK-STM746 ŰƮ V1.0�� ���� ���Դϴ�. �̰��� �Ʒ��� ���ǿ�����
//  �ùٸ� ������ �����մϴ�.
// -------------------------------------------------------------------------------
//	(1) �ý��� Ŭ�� : 216MHz
//	(2) C �����Ϸ�  : IAR EWARM V7.40.3
//	(3) ����ȭ �ɼ� : High/Speed
//	(4) CSTACK ũ�� : 0x2000
//      (5) ��Ŭ���    : �⺻ ��� ���� OK-STM746.h�� �ڿ� ��Ŭ����� ��.
// -------------------------------------------------------------------------------

void Initialize_VS1053b(void);			// initialize VS1053b
void VS1053b_SCI_Write(U08 address, U16 data);	// write VS1053b control register
unsigned short VS1053b_SCI_Read(U08 address);	// read VS1053b control register
void VS1053b_SCI_Read_Hexadecimal(U08 address);	// read and display VS1053b control register
void VS1053b_software_reset(void);		// VS1053b software reset to change music file
void VS1053b_SetVolume(U16 volume);		// set volume to 5~250(range : 0~254)
void VS1053b_SetBassTreble(U16 bass, S16 treble);// set bass(0~15) and treble(-8 ~ +7)

/* ---------------------------------------------------------------------------- */

unsigned short volume;				// volume value 5 ~ 250(range : 0~254)
unsigned short bass;				// bass value 0 ~ 15
signed short   treble;				// treble value -8 ~ +7

/* ---------------------------------------------------------------------------- */

void Initialize_VS1053b(void)			/* initialize VS1053b */
{
  unsigned short word;

  GPIOC->MODER = (GPIOC->MODER & 0xFFF000FF) | 0x00011400;  // GPIO mode
  GPIOC->BSRR = 0x00000160;			// -MP3_RESET = -MP3_CS = -MP3_DCS = 1

  GPIOC->BSRR = 0x01000000;			// -MP3_RESET = 0
  Delay_us(10);
  GPIOC->BSRR = 0x00000100;			// -MP3_RESET = 1
  Delay_ms(10);

  TFT_string(0,24,White,Magenta,"           (3) VS1053b �ʱ�ȭ           ");

  SPI3_medium_speed();				// must use below 12.288MHz / 7 = 1.755MHz

  TFT_string(9,27,Magenta,Black,"0x00=");	// read address 0(result = 0x4800)
  TFT_color(Cyan,Black);
  VS1053b_SCI_Read_Hexadecimal(0x00);
  Delay_ms(1);

  TFT_string(19,27,Magenta,Black,"0x01=");	// read address 1(result = 0x0048)
  word = VS1053b_SCI_Read(0x01);
  TFT_color(Cyan,Black);
  TFT_hexadecimal(word,4);
  word &= 0x00F0;
  if(word == 0x0000)      TFT_string(2,27, Cyan,Black, "VS1001");
  else if(word == 0x0010) TFT_string(2,27, Cyan,Black, "VS1011");
  else if(word == 0x0020) TFT_string(2,27, Cyan,Black, "VS1002");
  else if(word == 0x0030) TFT_string(2,27, Cyan,Black, "VS1003");
  else if(word == 0x0040) TFT_string(2,27, Cyan,Black, "VS1053");
  else if(word == 0x0050) TFT_string(2,27, Cyan,Black, "VS1033");
  else if(word == 0x0060) TFT_string(2,27, Cyan,Black, "VS1063");
  else if(word == 0x0070) TFT_string(2,27, Cyan,Black, "VS1103");
  Delay_ms(1);

  VS1053b_SCI_Write(0x03, 0xE000);		// multiply 5.0 to XTALI
  TFT_string(29,27,Magenta,Black,"0x03=");	// read address 3(result = 0xE000)
  TFT_color(Cyan,Black);
  VS1053b_SCI_Read_Hexadecimal(0x03);
  Delay_ms(1);

  SPI3_high_speed();				// must use below 12.288MHz x 5.0 / 7 = 8.777MHz
}

void VS1053b_SCI_Write(U08 address, U16 data)	/* write VS1053b control register */
{
  GPIOC->BSRR = 0x00200000;			// -MP3_CS = 0

  SPI3_write(0x02);				// write instruction
  SPI3_write(address);				// write address
  SPI3_write(data >> 8);			// write control register
  SPI3_write(data);

  GPIOC->BSRR = 0x00000020;			// -MP3_CS = 1
}

unsigned short VS1053b_SCI_Read(U08 address)	/* read VS1053b control register */
{
  unsigned short HB, LB;

  GPIOC->BSRR = 0x00200000;			// -MP3_CS = 0

  SPI3_write(0x03);				// read instruction
  SPI3_write(address);				// write address
  HB = SPI3_write(0xFF);			// read control regitser
  LB = SPI3_write(0xFF);

  GPIOC->BSRR = 0x00000020;			// -MP3_CS = 1

  return (HB << 8) + LB;			// return control register
}

void VS1053b_SCI_Read_Hexadecimal(U08 address)	/* read and display VS1053b control register */
{
  GPIOC->BSRR = 0x00200000;			// -MP3_CS = 0

  SPI3_write(0x03);				// read instruction
  SPI3_write(address);				// write address
  TFT_hexadecimal(SPI3_write(0xFF),2);		// read control regitser
  TFT_hexadecimal(SPI3_write(0xFF),2);

  GPIOC->BSRR = 0x00000020;			// -MP3_CS = 1
}

/* ---------------------------------------------------------------------------- */

void VS1053b_software_reset(void)		/* VS1053b software reset to change music file */
{
  unsigned short command;

  command = 0xFFFF;				// wait until HDAT1 =  HDAT0 = 0x0000
  while(command == 0x0000)
    { command = VS1053b_SCI_Read(0x09);
      command |= VS1053b_SCI_Read(0x08);
    }

  command = VS1053b_SCI_Read(0x00);		// software reset
  VS1053b_SCI_Write(0x00, command | 0x0004);
  Delay_ms(2);
  VS1053b_SCI_Write(0x04, 0);			// clear decode time
}

void VS1053b_SetVolume(U16 volume)		/* set volume(5~250) */
{
  unsigned short value;

  if(volume > 250) volume = 250;		// limit max volume
  if(volume < 5)   volume = 5;			// limit min volume

  value = (254 - volume);

  VS1053b_SCI_Write(0x0B, (value << 8) | value); // set left and right volume
}

void VS1053b_SetBassTreble(U16 bass, S16 treble) /* set bass(0~15) and treble(-8 ~ +7) */
{
  unsigned short value;

  value = 0x050A;				// treble limit = 5kHz, bass limit = 100Hz
  value = value + ((treble << 12) & 0xF000);	// treble = -8 ~ +7
  value = value + ((bass << 4) & 0x00F0);	// bass = 0 ~ 15

  VS1053b_SCI_Write(0x02, value);		// set bass and treble
}
