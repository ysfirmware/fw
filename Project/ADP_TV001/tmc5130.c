/******************************************************************************
  * @file       : tmc5031.c 
  * @author     : JUNG SH, EETECH
  * @version    : Ver 0.1
  * @date       : 12/07/2016
  * @brief      : tmc5031. 
  * @pjt model  : ADP
  ******************************************************************************/
/* @Story*/
/* @2015-12-07 make*/
/**/
/**/
/*******************************************************************************/

#include "main.h"
#include "bits.h"
#include "tmc5031.h"

#define SPI_DEV_TMC50xx		1
static const u8 TMC562RegisterReadable[128]={
1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //00..0f
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //10..1f
1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    //20..2f
0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0,    //30..3f
1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    //40..4f
0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0,    //50..5f
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1,    //60..6f
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1,    //70..7f
};
static u32 TMC562SoftwareCopy[128];    //Software-Kopien aller TMC562-Register (f? die nicht lesbaren Register)

//==========================================================================//
//  -> ReadWriteSPI
//  -> ReadWriteSPI
//  @ :
//===========================================================================//
u8 ReadWriteSPI2(u8 DeviceNumber, u8 Data, u8 LastTransfer)
{
  switch(DeviceNumber)
  {
  case 1://SPI_DEV_TMC50xx:
	  //en
      ZCS_SET(1);

      while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
      SPI_I2S_SendData(SPI1, Data);
      while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);

	  //dis
      if(LastTransfer) ZCS_SET(0);

      return SPI_I2S_ReceiveData(SPI1);
      break;

    default:
      return 0;
      break;
  }
}


//==========================================================================//
//  -> WriteTMC562Datagram
//  -> WriteTMC562Datagram
//  @ :
//===========================================================================//
void WriteTMC5130Datagram(u8 Address, u8 x1, u8 x2, u8 x3, u8 x4)
{
  int Value;

  //Register im TMC562 beschreiben
  ReadWriteSPI(SPI_DEV_TMC50xx, Address|0x80, FALSE);
  ReadWriteSPI(SPI_DEV_TMC50xx, x1, FALSE);
  ReadWriteSPI(SPI_DEV_TMC50xx, x2, FALSE);
  ReadWriteSPI(SPI_DEV_TMC50xx, x3, FALSE);
  ReadWriteSPI(SPI_DEV_TMC50xx, x4, TRUE);

  //Softwarekopie aktualisieren
  Value=x1;
  Value<<=8;
  Value|=x2;
  Value<<=8;
  Value|=x3;
  Value<<=8;
  Value|=x4;
  TMC562SoftwareCopy[Address & 0x7f]=Value;
}


//==========================================================================//
//  -> WriteTMC562Int
//  -> WriteTMC562Int
//  @ :
//===========================================================================//
void WriteTMC562Int(u8 Address, int Value)
{
  //Register im TMC562 beschreiben
  ReadWriteSPI(SPI_DEV_TMC50xx, Address|0x80, FALSE);
  ReadWriteSPI(SPI_DEV_TMC50xx, Value >> 24, FALSE);
  ReadWriteSPI(SPI_DEV_TMC50xx, Value >> 16, FALSE);
  ReadWriteSPI(SPI_DEV_TMC50xx, Value >> 8, FALSE);
  ReadWriteSPI(SPI_DEV_TMC50xx, Value & 0xff, TRUE);

  //Softwarekopie aktualisieren
  TMC562SoftwareCopy[Address & 0x7f]=Value;
}

//==========================================================================//
//  -> ReadTMC562Int
//  -> ReadTMC562Int
//  @ :
//===========================================================================//
int ReadTMC562Int(u8 Address)
{
  int Value;

  Address&=0x7f;
  if(TMC562RegisterReadable[Address])
  {
    //Register lesbar => aus dem TMC562 lesen.
    //Dazu sind zwei Lesezugriffe erforderlich.
    ReadWriteSPI(SPI_DEV_TMC50xx, Address, FALSE);
    ReadWriteSPI(SPI_DEV_TMC50xx, 0, FALSE);
    ReadWriteSPI(SPI_DEV_TMC50xx, 0, FALSE);
    ReadWriteSPI(SPI_DEV_TMC50xx, 0, FALSE);
    ReadWriteSPI(SPI_DEV_TMC50xx, 0, TRUE);

    ReadWriteSPI(SPI_DEV_TMC50xx, Address, FALSE);
    Value=ReadWriteSPI(SPI_DEV_TMC50xx, 0, FALSE);
    Value<<=8;
    Value|=ReadWriteSPI(SPI_DEV_TMC50xx, 0, FALSE);
    Value<<=8;
    Value|=ReadWriteSPI(SPI_DEV_TMC50xx, 0, FALSE);
    Value<<=8;
    Value|=ReadWriteSPI(SPI_DEV_TMC50xx, 0, TRUE);

    //Register VACTUAL hat nur 24 Bits => Vorzeichenbits erweitern
    if(Address==0x22 || Address==0x42)
    {
    	if(Value & BIT23) Value|=0xff000000;
    }

    return Value;
  }
  else
  {
    //Register nicht lesbar => Softwarekopie zur?kgeben
    return TMC562SoftwareCopy[Address];
  }
}

 