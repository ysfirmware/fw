/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-IO.h
           I/O-Funktionen

  Datum:   17.10.2011 OK
*****************************************************/

#ifndef __EVAL562_IO_H
#define __EVAL562_IO_H

void InitIO(void);
int GetMeasuredSpeed(UINT Axis);
void SystemControl(void);
void DisablePins(void);

#endif
