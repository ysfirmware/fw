/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-Commands.h
           Kommandointerpreter

  Datum:   23.4.2009 OK
*****************************************************/

#ifndef __EVAL562_COMMANDS_H
#define __EVAL562_COMMANDS_H

#define TM_IDLE 0
#define TM_RUN 1
#define TM_STEP 2
#define TM_RESET 3   //nicht verwendet
#define TM_DOWNLOAD 4
#define TM_DEBUG 5  //wie TM_IDLE, es wird jedoch der Akku nicht modifiziert bei GAP etc.

#define TCS_IDLE  0
#define TCS_CAN7  1
#define TCS_CAN8  2
#define TCS_UART  3
#define TCS_UART_ERROR 4
#define TCS_UART_II 5
#define TCS_UART_II_ERROR 6
#define TCS_USB   7
#define TCS_USB_ERROR 8
#define TCS_MEM   9


//TMCL-Befehle
#define TMCL_ROR 1
#define TMCL_ROL 2
#define TMCL_MST 3
#define TMCL_MVP 4
#define TMCL_SAP 5
#define TMCL_GAP 6
#define TMCL_STAP 7
#define TMCL_RSAP 8
#define TMCL_SGP 9
#define TMCL_GGP 10
#define TMCL_STGP 11
#define TMCL_RSGP 12
#define TMCL_RFS 13
#define TMCL_SIO 14
#define TMCL_GIO 15
#define TMCL_CALC 19
#define TMCL_COMP 20
#define TMCL_JC   21
#define TMCL_JA   22
#define TMCL_CSUB 23
#define TMCL_RSUB 24
#define TMCL_WAIT 27
#define TMCL_STOP 28
#define TMCL_SAC 29
#define TMCL_SCO 30
#define TMCL_GCO 31
#define TMCL_CCO 32
#define TMCL_CALCX 33
#define TMCL_AAP 34
#define TMCL_AGP 35
#define TMCL_CLE 36
#define TMCL_VECT 37
#define TMCL_RETI 38
#define TMCL_ACO 39
#define TMCL_UF0 64
#define TMCL_UF1 65
#define TMCL_UF2 66
#define TMCL_UF3 67
#define TMCL_UF4 68
#define TMCL_UF5 69
#define TMCL_UF6 70
#define TMCL_UF7 71

#define TMCL_ApplStop 128
#define TMCL_ApplRun 129
#define TMCL_ApplStep 130
#define TMCL_ApplReset 131
#define TMCL_DownloadStart 132
#define TMCL_DownloadEnd 133
#define TMCL_ReadMem 134
#define TMCL_GetStatus 135
#define TMCL_GetVersion 136
#define TMCL_FactoryDefault 137
#define TMCL_SetEvent 138
#define TMCL_SetASCII 139
#define TMCL_SecurityCode 140
#define TMCL_Breakpoint 141

#define TMCL_Boot 0xf2
#define TMCL_SoftwareReset 0xff


#define MVP_ABS 0
#define MVP_REL 1
#define MVP_COORD 2
#define MVP_RELCOORD 3

#define RFS_START 0
#define RFS_STOP 1
#define RFS_STATUS 2

#define WAIT_TICKS 0
#define WAIT_POS 1
#define WAIT_REFSW 2
#define WAIT_LIMSW 3
#define WAIT_RFS 4

#define JC_ZE 0
#define JC_NZ 1
#define JC_EQ 2
#define JC_NE 3
#define JC_GT 4
#define JC_GE 5
#define JC_LT 6
#define JC_LE 7
#define JC_ETO 8
#define JC_EAL 9
#define JC_EDV 10
#define JC_EPO 11
#define JC_ESD 12

#define CALC_ADD 0
#define CALC_SUB 1
#define CALC_MUL 2
#define CALC_DIV 3
#define CALC_MOD 4
#define CALC_AND 5
#define CALC_OR 6
#define CALC_XOR 7
#define CALC_NOT 8
#define CALC_LOAD 9
#define CALC_SWAP 10

#define CLE_ALL 0
#define CLE_ETO 1
#define CLE_EAL 2
#define CLE_EDV 3
#define CLE_EPO 4
#define CLE_ESD 5


//Statuscodes
#define REPLY_OK 100
#define REPLY_CMD_LOADED 101
#define REPLY_CHKERR 1
#define REPLY_INVALID_CMD 2
#define REPLY_WRONG_TYPE 3
#define REPLY_INVALID_VALUE 4
#define REPLY_EEPROM_LOCKED 5
#define REPLY_CMD_NOT_AVAILABLE 6
#define REPLY_CMD_LOAD_ERROR 7
#define REPLY_WRITE_PROTECTED 8
#define REPLY_MAX_EXCEEDED 9
#define REPLY_DELAYED 128
#define REPLY_ACTIVE_COMM 129
#define REPLY_AVR_NOT_READY 32

//Format der Antwort
#define RF_STANDARD 0
#define RF_SPECIAL 1

//"CPU-Flags"
#define FLAG_EQUAL           0x00000001
#define FLAG_LOWER           0x00000002
#define FLAG_GREATER         0x00000004
#define FLAG_GREATER_EQUAL   0x00000005
#define FLAG_LOWER_EQUAL     0x00000003
#define FLAG_ZERO            0x00000008
#define FLAG_ERROR_TIMEOUT   0x00000010
#define FLAG_ERROR_EXT_ALARM 0x00000020
#define FLAG_ERROR_DEVIATION 0x00000040
#define FLAG_ERROR_POSITION  0x00000080
#define FLAG_ERROR_SHUTDOWN  0x00000100

//Schreib-/Leseschutzbits
#define PB_READ  0x01
#define PB_WRITE 0x02

//Motor-Fehler-Flags
#define ME_STALLGUARD 0x01
#define ME_DEVIATION  0x02

//Andere Konstanten
#define TMCL_MEM_SIZE 2048
#define TMCL_STACK_DEPTH 8
#define TMCL_USER_VARS 56
#define TMCL_COORDINATES 21
#define TMCL_BREAKPOINTS 10

//Modusbits für den ASCII-Kommandointerpreter
#define ASCII_NORMAL_ECHO 0x01
#define ASCII_FULL_LINE_ECHO 0x11
#define ASCII_NO_ECHO 0x21

//Modi der Einheitenumrechnung (nur beim TMC457)
#define UNIT_MODE_INTERNAL 0
#define UNIT_MODE_PPS 1


//Typedefinitionen

//TMCL-Befehl
typedef struct
{
  UCHAR Opcode;
  UCHAR Type;
  UCHAR Motor;
  union
  {
    int Int32;
    UCHAR Byte[4];
  } Value;
} TTMCLCommand;

//TMCL-Antwort
typedef struct
{
  UCHAR Status;
  UCHAR Opcode;
  union
  {
    int Int32;
    UCHAR Byte[4];
  } Value;
} TTMCLReply;


//Exportierte Funktionen
void InitCommunication(void);
void ProcessCommands(void);

#endif
