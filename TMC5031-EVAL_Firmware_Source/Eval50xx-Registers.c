/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-Registers.c
           Zugriffsfunktionen für den TMC562

  Datum:   26.01.2012 OK
*****************************************************/

#include <stdlib.h>
#include "stm32f10x_lib.h"
#include "bits.h"

#include "Eval50xx.h"
#include "Eval50xx-SPI.h"


//Tabelle, die für jedes TMC562-Register angibt, on dieses auch gelesen werden kann (0=nein, 1=ja).
static const UCHAR TMC562RegisterReadable[128]={
1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //00..0f
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //10..1f
1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    //20..2f
0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0,    //30..3f
1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    //40..4f
0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0,    //50..5f
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1,    //60..6f
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1,    //70..7f
};

static int TMC562SoftwareCopy[128];    //Software-Kopien aller TMC562-Register (für die nicht lesbaren Register)


/*******************************************************************
   Funktion: WriteTMC562Datagram()
   Parameter: Address: Registeradresse (0x00..0x7f)
              x1, x2, x3, x4: Einzelne Bytes des Registers

   Rückgabewert: ---

   Zweck: Schreiben eines TMC562-Registers, wobei der Wert in
          einzelnen Bytes angegeben wird.
          Die Softwarekopie des Registers wird ebenfalls
          aktualisiert.
********************************************************************/
void WriteTMC562Datagram(UCHAR Address, UCHAR x1, UCHAR x2, UCHAR x3, UCHAR x4)
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


/*******************************************************************
   Funktion: WriteTMC562Int()
   Parameter: Address: Registeradresse (0x00..0x7f)
              Value: zu schreibender Wert

   Rückgabewert: ---

   Zweck: Schreiben eines TMC562-Registers.
          Die Softwarekopie des Registers wird ebenfalls
          aktualisiert.
********************************************************************/
void WriteTMC562Int(UCHAR Address, int Value)
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


/*******************************************************************
   Funktion: ReadTMC562Int()
   Parameter: Address: Registeradresse (0x00..0x7f)

   Rückgabewert: gelesener Wert

   Zweck: Lesen eines TMC562-Registers.
          Falls das Register nicht lesbar ist wird die Softwarekopie
          zurückgegeben.
********************************************************************/
int ReadTMC562Int(UCHAR Address)
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
    //Register nicht lesbar => Softwarekopie zurückgeben
    return TMC562SoftwareCopy[Address];
  }
}
