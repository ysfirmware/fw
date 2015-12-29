/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-SPI.h
           SPI-Funktionen

  Datum:   17.10.2011 OK
*****************************************************/


#ifndef __EVAL562_SPI_H
#define __EVAL562_SPI_H

void InitSPI(void);
UCHAR ReadWriteSPI(UCHAR DeviceNumber, UCHAR Data, UCHAR LastTransfer);

#endif
