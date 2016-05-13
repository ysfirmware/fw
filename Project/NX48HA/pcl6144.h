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

u8 ReadWriteSPI(u8 DeviceNumber, u8 Data, u8 LastTransfer);
void WriteTMC562Datagram(u8 Address, u8 x1, u8 x2, u8 x3, u8 x4);
void WriteTMC562Int(u8 Address, u32 Value);
int ReadTMC562Int(u8 Address);