#include "modbus_comm.h"
#include "crc.h"
//#include "uartdef.h"
//#include "uartproc.h"
#include "comm.h"
//#include "i2c_eeprom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include "process.h"
//#include "heatproc.h"
S_COMM0 RX0, TX0;



void	ModbusRX0_Init( void )
{
	RX0.RdPtr	= 0;
	RX0.WrPtr	= 0;
	RX0.Step	= PKT_ADDR;
	RX0.PCnt	= 0;		// RX data counter in packet
	RX0.DPtr	= 0;

	RX0.PktTO	= 0;		// RX packet time out
}


void	ModbusRx_Protocol( void )
{
	int rxd;

	rxd = Getth465 ();
	
	if ( rxd >= 0)
	{
		
		switch (RX0.Step)
		{
			case PKT_ADDR:
				if( rxd == MODBUS_ADDR )
				{
					RX0.PktTO = TIMEOUT_RX_PKT;
					RX0.DPtr = 0;
					RX0.Pkt[RX0.DPtr++] = rxd;		// write ADDR into packet
					RX0.Step = PKT_FUNC;
				}
				break;
			//--------------------------------------------------------------------------
			case PKT_FUNC:
				RX0.Pkt[RX0.DPtr++] = rxd;		// write FUNC code into packet
				RX0.PCnt = 0;
				if( rxd == FUNC_RD_HOLDING_REG )	RX0.PCnt = N_DATA_SINGLE;
				if( rxd == FUNC_RD_INPUT_REG )	RX0.PCnt = N_DATA_SINGLE;
				if( rxd == FUNC_WR_SINGLE_REG )	RX0.PCnt = N_DATA_SINGLE;
				if( RX0.PCnt ) RX0.Step = PKT_DATA;
				else			 RX0.Step = PKT_ADDR;
				break;
			//--------------------------------------------------------------------------
			case PKT_DATA:
				RX0.Pkt[RX0.DPtr++] = rxd;		// write DATA into packet
				if( RX0.DPtr == (RX0.PCnt+2) ) 
				{
					RX0.Step = PKT_CRC_H;
					RX0.Crc16 = Update_CRC16(&RX0.Pkt[0], RX0.DPtr);
				}
				break;
			//--------------------------------------------------------------------------
			case PKT_CRC_H:
				if( rxd == 0xC1 ) { rxd = (byte)((RX0.Crc16 & 0xFF00)>>8); }
				if( rxd == ((byte)((RX0.Crc16 & 0xFF00)>>8)) ) {
					RX0.Pkt[RX0.DPtr++] = rxd;		// write CRC-H into packet
					RX0.Step = PKT_CRC_L;
				}
				else {
					//..TX0_Write( (UCHAR8)((RX0.Crc16 & 0xFF00)>>8) );
					//..TX0_Write( (UCHAR8)((RX0.Crc16 & 0x00FF)) );
					RX0.Step = PKT_ADDR; 
				}
				break;
			//--------------------------------------------------------------------------
			case PKT_CRC_L:
				if( rxd == 0xC2 ) { rxd = (byte)(RX0.Crc16 & 0x00FF); }
				if( rxd == ((byte)(RX0.Crc16 & 0x00FF)) ) {
				RX0.Pkt[RX0.DPtr++] = rxd;		// write CRC-L into packet
				RX0_DoRxCmd();
				}
				else {
				//..TX0_Write( (UCHAR8)((RX0.Crc16 & 0xFF00)>>8) );
				//..TX0_Write( (UCHAR8)((RX0.Crc16 & 0x00FF)) );
				}
				RX0.Step = PKT_ADDR;
				break;
			//--------------------------------------------------------------------------
			default:
				break;
		}
	}

}

//******************************************************************************
//		DO RX COMMAND
//******************************************************************************
void	RX0_DoRxCmd( void )
{
  	byte	func;
	word	addr, nreg;
	
	func = RX0.Pkt[1];
	addr = ((word)(RX0.Pkt[2])<<8) | (word)(RX0.Pkt[3]);
	nreg = ((word)(RX0.Pkt[4])<<8) | (word)(RX0.Pkt[5]);
	
  	switch( func )
	{
		case FUNC_RD_HOLDING_REG:
		  Send_ReadRegister( FUNC_RD_HOLDING_REG, addr, nreg );
		  break;
		case FUNC_RD_INPUT_REG:
		  Send_ReadRegister( FUNC_RD_INPUT_REG, addr, nreg );
		  break;
		case FUNC_WR_SINGLE_REG:
		  Send_WriteSingleRegister();
		  WriteParameter( addr, nreg );
		  break;
	}
}

void	Send_ReadRegister( byte func, word addr, word nreg )
{
	byte	i;
  	word	data;
	
  	TX0.DPtr = 0;
  	TX0.Pkt[TX0.DPtr++] = MODBUS_ADDR;
  	TX0.Pkt[TX0.DPtr++] = func;
  	TX0.Pkt[TX0.DPtr++] = (nreg<<1);
	for( i=0; i<nreg; i++ ) {
	  data = ReadParameter( addr++ );  // 전송할 데이터 
	  TX0.Pkt[TX0.DPtr++] = (byte)((data & 0xFF00)>>8);
	  TX0.Pkt[TX0.DPtr++] = (byte) (data & 0x00FF);
	}
	TX0.Crc16 = Update_CRC16(&TX0.Pkt[0], TX0.DPtr);
  	TX0.Pkt[TX0.DPtr++] = (byte)((TX0.Crc16 & 0xFF00)>>8);
  	TX0.Pkt[TX0.DPtr++] = (byte)((TX0.Crc16 & 0x00FF));
	
	//TX0_WrCmd( &TX0.Pkt[0], TX0.DPtr );
	///*
	for( i=0; i<TX0.DPtr; i++ ) {
		SendUart (TOUCHPORT,(uint8_t)TX0.Pkt[i]);
	
	}
	//*/
}

void	Send_WriteSingleRegister( void )
{
  	byte	i;
	
	for( i=0; i<RX0.DPtr; i++ ) {		// return with RX packet
		SendUart (TOUCHPORT,(uint8_t)RX0.Pkt[i]);
	 
	}
	
}


#if 0  //[[ YSKim_160202
//******************************************************************************
//		READ PARAMETERS
//******************************************************************************
word	ReadParameter( word addr )
{

	VALUE *v = &Val;
	word	data;
	data = 0x00;
	
	switch( addr )
	{
		case ADDR_RUNNING :  // Heat 시작 
			data = isheatflag&0x01;
			break;
		case ADDR_TEMP : 
			if (ReadThermocouple > 1600)
				data = 0x0000;
			else 	
			{
				byte i;
				float sumTemp=ReadModebusBuf[0];
				for ( i = 1 ; i < 10;i++)
						sumTemp += ReadModebusBuf[i];
				sumTemp /= 10;
				
				data = (word)( sumTemp-T_Offset ); 
			}
			break;
		case ADDR_HEATING : data = (word)isHeating ; break;
		case ADDR_VERSION : data = (word)0x0065; break;
		case ADDR_SETTEMP : data = (word) (v->Set); break;
		case ADDR_OFFSET:		data = T_Offset ; break; 
	
	}
	
	
	return( data );
	
}
// 적용 
void	WriteParameter( word addr, word data )
{
	VALUE *v = &Val;
	switch( addr )
	{
		case ADDR_RUNNING :  // Heat 시작 
			OutPut_Control(data&0x01); // debug test
			break;
		case ADDR_SET_TEMP:  // 온도설정 
			v->Set = data;
		 if ( v->Set > Temp_Hi ) v->Set = Temp_Hi;
				else if ( v->Set < Temp_Lo ) v->Set = Temp_Lo;
			break;
		
		case ADDR_AIR_UPDN: //UP /DN
						if ((data&0x01) ==1) // UP
						{
							SetIO(IO_TIP_UP_VALUE_PORT,IO_TIP_UP_VALUE_PIN, 1);
							SetIO(IO_TIP_DN_VALUE_PORT,IO_TIP_DN_VALUE_PIN, 0);
						}
						else if (Rxd3.DATA[1] ==0) // DN
						{
							SetIO(IO_TIP_UP_VALUE_PORT,IO_TIP_UP_VALUE_PIN, 0);
							SetIO(IO_TIP_DN_VALUE_PORT,IO_TIP_DN_VALUE_PIN, 1);
						}
						break;
					case ADDR_ENABEL: //EN
						if ((data&0x01) ==1) // UP
						{
							SetIO(IO_N2_EN_VALUE_PORT,IO_N2_EN_VALUE_PIN, 1);							
						}
						else if (Rxd3.DATA[1] ==0) // DN
						{
							SetIO(IO_N2_EN_VALUE_PORT,IO_N2_EN_VALUE_PIN, 0);
						}
						break;
					case ADDR_RESERVE: //EN
						if ((data&0x01) ==1) // UP
						{
							SetIO(IO_RESERVE_VOUT_PORT,IO_RESERVE_VOUT_PIN, 1);							
						}
						else if (Rxd3.DATA[1] ==0) // DN
						{
							SetIO(IO_RESERVE_VOUT_PORT,IO_RESERVE_VOUT_PIN, 0);
						}
						break;
			 case ADDR_OFFSET:	
						printf ("Read offset Data=[%d][%x]\n",(int)data, (word)data);
						
						if ( ((data>>15)&0x01) ==1)
							T_Offset =  ((~data)+1)*-1  ; 
						else 
							T_Offset = data ; 
						
						printf ("Read offset =[%d][%x]\n",(int)T_Offset, (word)T_Offset);
						
						
						INT_SW_i2c_WriteByte(0xA0, 0x0004, data );
						
						INT_SW_I2C_ReadBytes(0xA0 , 0x0004 , received_data ,2);
						
						
					
					
						printf ("Read offset =[%x][%x]\n",received_data[0], received_data[1]);
						
	
	}
}
#endif //]] YSKim_160202


