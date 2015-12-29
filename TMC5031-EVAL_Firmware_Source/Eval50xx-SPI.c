/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-SPI.c
           SPI-Funktionen

  Datum:   17.10.2011 OK
*****************************************************/

#include "stm32f10x_lib.h"
#include "bits.h"
#include "Eval50xx.h"


/*******************************************************************
   Funktion: InitSPI()
   Parameter: ---

   Rückgabewert: ---

   Zweck: Intialisierung des SPI.
********************************************************************/
void InitSPI(void)
{
  SPI_InitTypeDef SPIInit;
  GPIO_InitTypeDef GPIO_InitStructure;

  //Clock für SPI1 einschalten
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

  //PA5..PA7 mit SPI1 verbinden
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  //SPI initialisieren
  SPIInit.SPI_Direction=SPI_Direction_2Lines_FullDuplex;
  SPIInit.SPI_Mode=SPI_Mode_Master;
  SPIInit.SPI_DataSize=SPI_DataSize_8b;
  SPIInit.SPI_CPOL=SPI_CPOL_High;
  SPIInit.SPI_CPHA=SPI_CPHA_2Edge;
  SPIInit.SPI_NSS=SPI_NSS_Soft;
  SPIInit.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_16;
  SPIInit.SPI_FirstBit=SPI_FirstBit_MSB;
  SPIInit.SPI_CRCPolynomial=0;
  SPI_Init(SPI1, &SPIInit);
  SPI_Cmd(SPI1, ENABLE);
}


/*******************************************************************
   Funktion: ReadWriteSPI
   Parameter: DeviceNumber: Gerätenummer (noch nicht verwendet)
              Data: zu sendendes Datenbyte
              LastTransfer: TRUE: nach diesem Byte wird CS wieder
                                  auf High gesetzt
                            FALSE: CS bleibt low (weiteres Byte folgt).

   Rückgabewert: empfangenes Datenbyte

   Zweck: SPI-Datenübertragung durchführen.
********************************************************************/
UCHAR ReadWriteSPI(UCHAR DeviceNumber, UCHAR Data, UCHAR LastTransfer)
{
  switch(DeviceNumber)
  {
    case SPI_DEV_TMC50xx:
      SELECT_TMC50xx();

      while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE)==RESET);
      SPI_I2S_SendData(SPI1, Data);
      while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE)==RESET);

      if(LastTransfer) DESELECT_TMC50xx();

      return SPI_I2S_ReceiveData(SPI1);
      break;

    default:
      return 0;
      break;
  }
}
