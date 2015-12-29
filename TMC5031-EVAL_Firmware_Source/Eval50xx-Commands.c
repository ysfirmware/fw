/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-Commands.c
           Kommandointerpreter
           BD: Hab ich geändert :-)

  Datum:   17.10.2011 OK
*****************************************************/

#include <stdlib.h>
#include "stm32f10x_lib.h"
#include "bits.h"

#include "Eval50xx.h"
#include "Eval50xx-Commands.h"
#include "Eval50xx-UART.h"
#include "Eval50xx-USB.h"
#include "Eval50xx-SPI.h"
#include "Eval50xx-SysTick.h"
#include "Eval50xx-IO.h"
#include "Eval50xx-Registers.h"


//Die Adressen sind hier fest eingestellt
#define SERIAL_HOST_ADDRESS 2
#define SERIAL_MODULE_ADDRESS 1

//Globale Variablen
static UCHAR TMCLCommandState;
TTMCLCommand ActualCommand;
TTMCLReply ActualReply;
UCHAR TMCLReplyFormat;
UCHAR SpecialReply[9];
static UCHAR UARTCmd[9];
static UCHAR UARTCount;
static UCHAR ResetRequested;
static UCHAR DemoMode;

static int VMax[N_O_MOTORS];
static UCHAR VMaxModified[N_O_MOTORS];


//Importierte Variablen
extern const char *VersionString;

//Prototypen für die TMCL-Funktionen
static void RotateRight(void);
static void RotateLeft(void);
static void MotorStop(void);
static void MoveToPosition(void);
static void SetAxisParameter(void);
static void GetAxisParameter(void);
static void SetGlobalParameter(void);
static void GetGlobalParameter(void);
static void UserFunc0(void);
static void UserFunc1(void);
static void UserFunc2(void);
static void UserFunc3(void);
static void UserFunc4(void);
static void UserFunc5(void);
static void UserFunc6(void);
static void UserFunc7(void);
static void GetVersion(void);
static void Boot(void);
static void SoftwareReset(void);


/*******************************************************************
   Funktion: ExecuteActualCommand()
   Parameter: ---

   Rückgabewert: ---

   Zweck: Ausführen des Befehls, der in der globalen Variablen
          ActualCommand steht.
********************************************************************/
void ExecuteActualCommand(void)
{
  //Antworttelegramm vorbelegen
  ActualReply.Opcode=ActualCommand.Opcode;
  ActualReply.Status=REPLY_OK;
  ActualReply.Value.Int32=ActualCommand.Value.Int32;

  //Befehl ausführen
  switch(ActualCommand.Opcode)
  {
    case TMCL_ROR:
      RotateRight();
      break;

    case TMCL_ROL:
      RotateLeft();
      break;

    case TMCL_MST:
      MotorStop();
      break;

    case TMCL_MVP:
      MoveToPosition();
      break;

    case TMCL_SAP:
      SetAxisParameter();
      break;

    case TMCL_GAP:
      GetAxisParameter();
      break;

    case TMCL_SGP:
      SetGlobalParameter();
      break;

    case TMCL_GGP:
      GetGlobalParameter();
      break;

    case TMCL_UF0:
      UserFunc0();
      break;

    case TMCL_UF1:
      UserFunc1();
      break;

    case TMCL_UF2:
      UserFunc2();
      break;

    case TMCL_UF3:
      UserFunc3();
      break;

    case TMCL_UF4:
      UserFunc4();
      break;

    case TMCL_UF5:
      UserFunc5();
      break;

    case TMCL_UF6:
      UserFunc6();
      break;

    case TMCL_UF7:
      UserFunc7();
      break;

    case TMCL_GetVersion:
      GetVersion();
      break;

    case TMCL_Boot:
      Boot();
      break;

    case TMCL_SoftwareReset:
      SoftwareReset();
      break;

    default:
      ActualReply.Status=REPLY_INVALID_CMD;
      break;
  }
}


/*******************************************************************
   Funktion: ProcessCommands()
   Parameter: ---

   Rückgabewert: ---

   Zweck: Holen und Ausführen von Befehlen über RS232 und USB.
          Diese Funktion muß periodisch aus der Hauptschleife
          heraus aufgerufen werden.
          Es werden 9-Byte-Kommandos ähnlich wie bei TMCL verwendet.
********************************************************************/
void ProcessCommands(void)
{
  UCHAR Byte;
  UCHAR Checksum;
  UINT i;
  UCHAR USBCmd[9];
  UCHAR USBReply[9];

  //**Antwort auf letzten Direktmodus-Befehl senden (wenn vorhanden)**
  if(DemoMode && (ActualCommand.Opcode==TMCL_GAP || ActualCommand.Opcode==TMCL_GGP
               || ActualCommand.Opcode==TMCL_SAP))
    ActualReply.Status=ActualCommand.Type;

  if(TMCLCommandState==TCS_UART)  //Antwort über UART
  {
    if(TMCLReplyFormat==RF_STANDARD)
    {
      Checksum=SERIAL_HOST_ADDRESS+SERIAL_MODULE_ADDRESS+
               ActualReply.Status+ActualReply.Opcode+
               ActualReply.Value.Byte[3]+
               ActualReply.Value.Byte[2]+
               ActualReply.Value.Byte[1]+
               ActualReply.Value.Byte[0];

      WriteUART(SERIAL_HOST_ADDRESS);
      WriteUART(SERIAL_MODULE_ADDRESS);
      WriteUART(ActualReply.Status);
      WriteUART(ActualReply.Opcode);
      WriteUART(ActualReply.Value.Byte[3]);
      WriteUART(ActualReply.Value.Byte[2]);
      WriteUART(ActualReply.Value.Byte[1]);
      WriteUART(ActualReply.Value.Byte[0]);
      WriteUART(Checksum);
    }
    else if(TMCLReplyFormat==RF_SPECIAL)
    {
      for(i=0; i<9; i++)
      {
        WriteUART(SpecialReply[i]);
      }
    }
  }
  else if(TMCLCommandState==TCS_UART_ERROR)  //letztes Kommando über UART hatte falsche Prüfsumme
  {
    ActualReply.Opcode=0;
    ActualReply.Status=REPLY_CHKERR;
    ActualReply.Value.Int32=0;

    Checksum=SERIAL_HOST_ADDRESS+SERIAL_MODULE_ADDRESS+
             ActualReply.Status+ActualReply.Opcode+
             ActualReply.Value.Byte[3]+
             ActualReply.Value.Byte[2]+
             ActualReply.Value.Byte[1]+
             ActualReply.Value.Byte[0];

    WriteUART(SERIAL_HOST_ADDRESS);
    WriteUART(SERIAL_MODULE_ADDRESS);
    WriteUART(ActualReply.Status);
    WriteUART(ActualReply.Opcode);
    WriteUART(ActualReply.Value.Byte[3]);
    WriteUART(ActualReply.Value.Byte[2]);
    WriteUART(ActualReply.Value.Byte[1]);
    WriteUART(ActualReply.Value.Byte[0]);
    WriteUART(Checksum);
  }
  else if(TMCLCommandState==TCS_USB)  //Antwort über USB
  {
    if(TMCLReplyFormat==RF_STANDARD)
    {
      Checksum=SERIAL_HOST_ADDRESS+SERIAL_MODULE_ADDRESS+
               ActualReply.Status+ActualReply.Opcode+
               ActualReply.Value.Byte[3]+
               ActualReply.Value.Byte[2]+
               ActualReply.Value.Byte[1]+
               ActualReply.Value.Byte[0];

      USBReply[0]=SERIAL_HOST_ADDRESS;
      USBReply[1]=SERIAL_MODULE_ADDRESS;
      USBReply[2]=ActualReply.Status;
      USBReply[3]=ActualReply.Opcode;
      USBReply[4]=ActualReply.Value.Byte[3];
      USBReply[5]=ActualReply.Value.Byte[2];
      USBReply[6]=ActualReply.Value.Byte[1];
      USBReply[7]=ActualReply.Value.Byte[0];
      USBReply[8]=Checksum;
    }
    else if(TMCLReplyFormat==RF_SPECIAL)
    {
      for(i=0; i<9; i++)
      {
        USBReply[i]=SpecialReply[i];
      }
    }

    USBSendData(USBReply, 9);
  }
  else if(TMCLCommandState==TCS_USB_ERROR)  //letztes Kommando über USB hatte falsche Prüfsumme
  {
    ActualReply.Opcode=0;
    ActualReply.Status=REPLY_CHKERR;
    ActualReply.Value.Int32=0;

    Checksum=SERIAL_HOST_ADDRESS+SERIAL_MODULE_ADDRESS+
             ActualReply.Status+ActualReply.Opcode+
             ActualReply.Value.Byte[3]+
             ActualReply.Value.Byte[2]+
             ActualReply.Value.Byte[1]+
             ActualReply.Value.Byte[0];

    USBReply[0]=SERIAL_HOST_ADDRESS;
    USBReply[1]=SERIAL_MODULE_ADDRESS;
    USBReply[2]=ActualReply.Status;
    USBReply[3]=ActualReply.Opcode;
    USBReply[4]=ActualReply.Value.Byte[3];
    USBReply[5]=ActualReply.Value.Byte[2];
    USBReply[6]=ActualReply.Value.Byte[1];
    USBReply[7]=ActualReply.Value.Byte[0];
    USBReply[8]=Checksum;

    USBSendData(USBReply, 9);
  }

  //Zustand zurücksetzen (Antwort ist nun gesendet)
  TMCLCommandState=TCS_IDLE;
  TMCLReplyFormat=RF_STANDARD;


  //Letzter Befehl war Reset-Befehl => Reset
  if(ResetRequested) ResetCPU(TRUE);

  //**Befehl holen (Prioritäten: UART, USB, Speicher)**
  if(ReadUART(&Byte))  //Befehl von RS232/RS485?
  {
    if(CheckUARTTimeout()) UARTCount=0;  //bei Timeout alles bisherige verwerfen
    UARTCmd[UARTCount++]=Byte;

    if(UARTCount==9)  //Neun Bytes wurden empfangen
    {
      UARTCount=0;

      if(UARTCmd[0]==SERIAL_MODULE_ADDRESS)  //Stimmt die Adresse?
      {
        Checksum=0;
        for(i=0; i<8; i++) Checksum+=UARTCmd[i];

        if(Checksum==UARTCmd[8])  //Stimmt die Prüfsumme?
        {
          ActualCommand.Opcode=UARTCmd[1];
          ActualCommand.Type=UARTCmd[2];
          ActualCommand.Motor=UARTCmd[3];
          ActualCommand.Value.Byte[3]=UARTCmd[4];
          ActualCommand.Value.Byte[2]=UARTCmd[5];
          ActualCommand.Value.Byte[1]=UARTCmd[6];
          ActualCommand.Value.Byte[0]=UARTCmd[7];

          TMCLCommandState=TCS_UART;

          UARTCount=0;
        }
        else TMCLCommandState=TCS_UART_ERROR;  //Prüfsumme war falsch
      }
    }
  }
  else if(GetUSBCmd(USBCmd))  //Befehl von USB?
  {
    if(USBCmd[0]==SERIAL_MODULE_ADDRESS)  //Stimmt die Adresse?
    {
      Checksum=0;
      for(i=0; i<8; i++) Checksum+=USBCmd[i];

      if(Checksum==USBCmd[8])  //Stimmt die Prüfsumme?
      {
        ActualCommand.Opcode=USBCmd[1];
        ActualCommand.Type=USBCmd[2];
        ActualCommand.Motor=USBCmd[3];
        ActualCommand.Value.Byte[3]=USBCmd[4];
        ActualCommand.Value.Byte[2]=USBCmd[5];
        ActualCommand.Value.Byte[1]=USBCmd[6];
        ActualCommand.Value.Byte[0]=USBCmd[7];

        TMCLCommandState=TCS_USB;
      } else TMCLCommandState=TCS_USB_ERROR;  //Prüfsumme war falsch
    }
  }

  //**Befehl ausführen (bzw. speichern wenn im Download-Modus)**
  //Befehl ausführen, wenn erfolgreich ein Befehl geholt wurde.
  if(TMCLCommandState!=TCS_IDLE && TMCLCommandState!=TCS_UART_ERROR &&
     TMCLCommandState!=TCS_USB_ERROR) ExecuteActualCommand();
}


//**************************** Funktionen für die einzelnen TMCL-Befehle *****************
// Für alle diese Funktionen gilt:
//   -die Parameter stehen in der globalen Variablen ActualCommand
//   -ein eventueller Rückgabewert wird in der globalen Variablen ActualReply abgelegt.
//****************************************************************************************

/*********************************
   Funktion: RotateRight()

   Zweck: TMCL-Befehl ROR
 *********************************/
static void RotateRight(void)
{
  if(ActualCommand.Motor<N_O_MOTORS)
  {
    VMaxModified[ActualCommand.Motor]=TRUE;
    WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(ActualCommand.Motor), abs(ActualCommand.Value.Int32));
    if(ActualCommand.Value.Int32>0)
      WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(ActualCommand.Motor), 0, 0, 0, TMC562_MODE_VELPOS);
    else
      WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(ActualCommand.Motor), 0, 0, 0, TMC562_MODE_VELNEG);
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}


/*********************************
   Funktion: RotateLeft()

   Zweck: TMCL-Befehl ROL
 *********************************/
static void RotateLeft(void)
{
  if(ActualCommand.Motor<N_O_MOTORS)
  {
    VMaxModified[ActualCommand.Motor]=TRUE;
    WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(ActualCommand.Motor), abs(ActualCommand.Value.Int32));
    if(ActualCommand.Value.Int32>0)
      WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(ActualCommand.Motor), 0, 0, 0, TMC562_MODE_VELNEG);
    else
      WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(ActualCommand.Motor), 0, 0, 0, TMC562_MODE_VELPOS);
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}


/*********************************
   Funktion: MotorStop()

   Zweck: TMCL-Befehl MST
 *********************************/
static void MotorStop(void)
{
  if(ActualCommand.Motor<N_O_MOTORS)
  {
    VMaxModified[ActualCommand.Motor]=TRUE;
    WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(ActualCommand.Motor), 0);
    WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(ActualCommand.Motor), 0, 0, 0, TMC562_MODE_VELPOS);
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}


/*********************************
   Funktion: MotorStop()

   Zweck: TMCL-Befehl MST
 *********************************/
static void MoveToPosition(void)
{
  int ActPos;

  if(ActualCommand.Motor<N_O_MOTORS)
  {
    switch(ActualCommand.Type)
    {
      case MVP_ABS:
        if(VMaxModified[ActualCommand.Motor])
        {
          WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(ActualCommand.Motor), VMax[ActualCommand.Motor]);
          VMaxModified[ActualCommand.Motor]=FALSE;
        }
        WriteTMC562Int(TMC562_XTARGET|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(ActualCommand.Motor), 0, 0, 0, TMC562_MODE_POSITION);
        break;

      case MVP_REL:
        ActPos=ReadTMC562Int(TMC562_XACTUAL|MOTOR_ADDR(ActualCommand.Motor));
        if(VMaxModified[ActualCommand.Motor])
        {
          WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(ActualCommand.Motor), VMax[ActualCommand.Motor]);
          VMaxModified[ActualCommand.Motor]=FALSE;
        }
        WriteTMC562Int(TMC562_XTARGET|MOTOR_ADDR(ActualCommand.Motor), ActPos+ActualCommand.Value.Int32);
        WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(ActualCommand.Motor), 0, 0, 0, TMC562_MODE_POSITION);
        break;

      default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}


/*********************************
   Funktion: SetAxisParameter()

   Zweck: TMCL-Befehl SAP
 *********************************/
static void SetAxisParameter(void)
{
  UINT Value;

  if(ActualCommand.Motor<N_O_MOTORS)
  {
    switch(ActualCommand.Type)
    {
      case 0:
        WriteTMC562Int(TMC562_XTARGET|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 1:
        WriteTMC562Int(TMC562_XACTUAL|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 2:
        VMaxModified[ActualCommand.Motor]=TRUE;
        WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(ActualCommand.Motor), abs(ActualCommand.Value.Int32));
        break;

      case 4:
        VMax[ActualCommand.Motor]=abs(ActualCommand.Value.Int32);
        if(ReadTMC562Int(TMC562_RAMPMODE|MOTOR_ADDR(ActualCommand.Motor))==TMC562_MODE_POSITION)
          WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(ActualCommand.Motor), abs(ActualCommand.Value.Int32));
        break;

      case 5:
        WriteTMC562Int(TMC562_AMAX|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 6:
        Value=ReadTMC562Int(TMC562_IHOLD_IRUN|MOTOR_ADDR(ActualCommand.Motor));
        WriteTMC562Datagram(TMC562_IHOLD_IRUN|MOTOR_ADDR(ActualCommand.Motor), 0, Value >> 16,
                            ActualCommand.Value.Byte[0], Value & 0xff);
        break;

      case 7:
        Value=ReadTMC562Int(TMC562_IHOLD_IRUN|MOTOR_ADDR(ActualCommand.Motor));
        WriteTMC562Datagram(TMC562_IHOLD_IRUN|MOTOR_ADDR(ActualCommand.Motor), 0, Value >> 16,
                            Value >> 8, ActualCommand.Value.Byte[0]);
        break;

      case 12:
        Value=ReadTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor));
        if(ActualCommand.Value.Int32==0)
          WriteTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor), Value|TMC562_SW_STOPR_ENABLE);
        else
          WriteTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor), Value & ~TMC562_SW_STOPR_ENABLE);
        break;

      case 13:
        Value=ReadTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor));
        if(ActualCommand.Value.Int32==0)
          WriteTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor), Value|TMC562_SW_STOPL_ENABLE);
        else
          WriteTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor), Value & ~TMC562_SW_STOPL_ENABLE);
        break;

      case 14:
        WriteTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 15:
        WriteTMC562Int(TMC562_A1|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 16:
        WriteTMC562Int(TMC562_V1|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 17:
        WriteTMC562Int(TMC562_DMAX|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 18:
        WriteTMC562Int(TMC562_D1|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 19:
        WriteTMC562Int(TMC562_VSTART|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 20:
        WriteTMC562Int(TMC562_VSTOP|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 21:
        WriteTMC562Int(TMC562_TZEROWAIT|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 22:
        WriteTMC562Int(TMC562_VCOOLTHRS|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 23:
        WriteTMC562Int(TMC562_VHIGH|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      case 24:
        WriteTMC562Int(TMC562_VDCMIN|MOTOR_ADDR(ActualCommand.Motor), ActualCommand.Value.Int32);
        break;

      default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }
  }
  else ActualReply.Status=REPLY_WRONG_TYPE;
}


/*********************************
   Funktion: GetAxisParameter()

   Zweck: TMCL-Befehl GAP
 *********************************/
static void GetAxisParameter(void)
{
  UINT Value;

  if(ActualCommand.Motor<N_O_MOTORS)
  {
    switch(ActualCommand.Type)
    {
      case 0:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_XTARGET|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 1:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_XACTUAL|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 2:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_VMAX|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 3:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_VACTUAL|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 4:
        ActualReply.Value.Int32=VMax[ActualCommand.Motor];
        break;

      case 5:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_AMAX|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 6:
        Value=ReadTMC562Int(TMC562_IHOLD_IRUN|MOTOR_ADDR(ActualCommand.Motor));
        ActualReply.Value.Int32=(Value>>8) & 0xff;
        break;

      case 7:
        Value=ReadTMC562Int(TMC562_IHOLD_IRUN|MOTOR_ADDR(ActualCommand.Motor));
        ActualReply.Value.Int32=Value & 0xff;
        break;

      case 8:
        ActualReply.Value.Int32=(ReadTMC562Int(TMC562_RAMPSTAT|MOTOR_ADDR(ActualCommand.Motor)) & TMC562_RS_POSREACHED) ? 1:0;
        break;

      case 10:
        ActualReply.Value.Int32=(ReadTMC562Int(TMC562_RAMPSTAT|MOTOR_ADDR(ActualCommand.Motor)) & TMC562_RS_STOPR) ? 0:1;
        break;

      case 11:
        ActualReply.Value.Int32=(ReadTMC562Int(TMC562_RAMPSTAT|MOTOR_ADDR(ActualCommand.Motor)) & TMC562_RS_STOPL) ? 0:1;
        break;

      case 12:
        ActualReply.Value.Int32=(ReadTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor)) & TMC562_SW_STOPR_ENABLE) ? 1:0;
        break;

      case 13:
        ActualReply.Value.Int32=(ReadTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor)) & TMC562_SW_STOPL_ENABLE) ? 1:0;
        break;

      case 14:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_SWMODE|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 15:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_A1|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 16:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_V1|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 17:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_DMAX|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 18:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_D1|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 19:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_VSTART|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 20:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_VSTOP|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 21:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_TZEROWAIT|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 22:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_VCOOLTHRS|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 23:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_VHIGH|MOTOR_ADDR(ActualCommand.Motor));
        break;

      case 24:
        ActualReply.Value.Int32=ReadTMC562Int(TMC562_VDCMIN|MOTOR_ADDR(ActualCommand.Motor));
        break;

      default:
        ActualReply.Status=REPLY_WRONG_TYPE;
        break;
    }
  }
  else ActualReply.Status=REPLY_INVALID_VALUE;
}


/*********************************
   Funktion: SetGlobalParameter()

   Zweck: TMCL-Befehl SGP
 *********************************/
static void SetGlobalParameter(void)
{
  switch(ActualCommand.Motor)
  {
    case 0:
      switch(ActualCommand.Type)
      {
        case 254:
          DemoMode=ActualCommand.Value.Byte[0];
          break;

        default:
          ActualReply.Status=REPLY_WRONG_TYPE;
          break;
      }
      break;

    default:
      ActualReply.Status=REPLY_INVALID_VALUE;
      break;
  }
}


/*********************************
   Funktion: GetGlobalParameter()

   Zweck: TMCL-Befehl GGP
 *********************************/
static void GetGlobalParameter(void)
{
  switch(ActualCommand.Motor)
  {
    case 0:
      switch(ActualCommand.Type)
      {
        case 132:
          ActualReply.Value.Int32=GetSysTimer();
          break;

        case 254:
          ActualReply.Value.Int32=DemoMode;
          break;

        default:
          ActualReply.Status=REPLY_WRONG_TYPE;
          break;
      }
      break;

    default:
      ActualReply.Status=REPLY_INVALID_VALUE;
      break;
  }
}


/*********************************************************
   Funktion: UserFunc0()

   Zweck: TMCL-Befehl UF0
 *********************************************************/
static void UserFunc0(void)
{
}


/***********************************************
   Funktion: UserFunc1()

   Zweck: TMCL-Befehl UF1
 ***********************************************/
static void UserFunc1(void)
{
}


/***********************************************
   Funktion: UserFunc2()

   Zweck: TMCL-Befehl UF2
   Motortreiber Ein- und Ausschalten.
 ***********************************************/
static void UserFunc2(void)
{
  switch(ActualCommand.Type)
  {
    case 0:
      if(ActualCommand.Value.Int32!=0)
        ENABLE_TMC50xx();
      else
        DISABLE_TMC50xx();
      break;

    default:
      ActualReply.Status=REPLY_WRONG_TYPE;
      break;
  }
}


/********************************************
   Funktion: UserFunc3()

   Zweck: TMCL-Befehl UF3
   Setzen der Taktfrequenz.
 ********************************************/
static void UserFunc3(void)
{
  TIM_TimeBaseInitTypeDef TIM_Base;
  TIM_OCInitTypeDef       TIM_Oc;


  switch(ActualCommand.Value.Int32)
  {
    case 0:  // 9MHz
      TIM_Base.TIM_Period=4;
      TIM_Oc.TIM_Pulse=2;
      break;

    case 1: // 12MHz
      TIM_Base.TIM_Period=3;
      TIM_Oc.TIM_Pulse=2;
      break;

    case 2: // 18MHz
      TIM_Base.TIM_Period=2;
      TIM_Oc.TIM_Pulse=1;
      break;

    default: // 18MHz
      TIM_Base.TIM_Period=2;
      TIM_Oc.TIM_Pulse=1;
      break;
  }

  TIM_Base.TIM_Prescaler = 0;
  TIM_Base.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_Base.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
  TIM_TimeBaseInit(TIM2, &TIM_Base);

  TIM_Oc.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_Oc.TIM_OutputState = TIM_OutputState_Enable;

  TIM_Oc.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC3Init(TIM2, &TIM_Oc);
}


/*********************************************
   Funktion: UserFunc4()

   Zweck: TMCL-Befehl UF4
   Abfrage der gemessenen Geschwindigkeit.
 *********************************************/
static void UserFunc4(void)
{
  ActualReply.Value.Int32=GetMeasuredSpeed(ActualCommand.Motor);
}


/*********************************
   Funktion: UserFunc5()

   Zweck: TMCL-Befehl UF5
   Beschreiben eines TMC50xx-Registers.
 *********************************/
static void UserFunc5(void)
{
  WriteTMC562Int(ActualCommand.Type, ActualCommand.Value.Int32);
}


/*********************************
   Funktion: UserFunc6()

   Zweck: TMCL-Befehl UF6
   Lesen eines TMC50xx-Registers
 *********************************/
static void UserFunc6(void)
{
  ActualReply.Value.Int32=ReadTMC562Int(ActualCommand.Type);
}


/*********************************
   Funktion: UserFunc7()

   Zweck: TMCL-Befehl UF7
 *********************************/
static void UserFunc7(void)
{
}


/*********************************
   Funktion: GetVersion

   Zweck: TMCL-Versionsabfrage
 *********************************/
static void GetVersion(void)
{
  UINT i;

  if(ActualCommand.Type==0)
  {
    TMCLReplyFormat=RF_SPECIAL;
    SpecialReply[0]=SERIAL_HOST_ADDRESS;
    for(i=0; i<8; i++)
      SpecialReply[i+1]=VersionString[i];
  }
  else if(ActualCommand.Type==1)
  {
    ActualReply.Value.Byte[3]=SW_TYPE_HIGH;
    ActualReply.Value.Byte[2]=SW_TYPE_LOW;
    ActualReply.Value.Byte[1]=SW_VERSION_HIGH;
    ActualReply.Value.Byte[0]=SW_VERSION_LOW;
  }
}


/****************************************
   Funktion: Boot()

   Zweck: Spezialbefehl zum Aufruf des
          Bootloaders.
 ***************************************/
static void Boot(void)
{
  UINT Delay;

  if(ActualCommand.Type==0x81 && ActualCommand.Motor==0x92 &&
     ActualCommand.Value.Byte[3]==0xa3 && ActualCommand.Value.Byte[2]==0xb4 &&
     ActualCommand.Value.Byte[1]==0xc5 && ActualCommand.Value.Byte[0]==0xd6)
  {
    DetachUSB();
    Delay=GetSysTimer();
    while(abs(GetSysTimer()-Delay)<1000) RESET_WATCHDOG();

    DisableInterrupts();
    NVIC_DeInit();
    SysTick_ITConfig(DISABLE);
    DMA_Cmd(DMA1_Channel1, DISABLE);
    DMA_DeInit(DMA1_Channel1);
    ADC_DeInit(ADC1);

    ResetCPU(FALSE);
  }
}


/*********************************
   Funktion: SoftwareReset()

   Zweck: TMCL-Reset-Befehl
 *********************************/
static void SoftwareReset(void)
{
  if(ActualCommand.Value.Int32==1234) ResetRequested=TRUE;
}
