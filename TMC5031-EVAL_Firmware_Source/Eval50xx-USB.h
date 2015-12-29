/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-USB.h
           USB-Funktionen

  Datum:   29.4.2010 OK
*****************************************************/

#ifndef __EVAL562_USB_H
#define __EVAL562_USB_H

void InitUSB(void);
UINT USBGetData(UCHAR *Buffer);
void USBSendData(UCHAR *Buffer, UINT Size);
UCHAR GetUSBCmd(UCHAR *Cmd);
void DetachUSB(void);

void USB_Istr(void);

#endif
