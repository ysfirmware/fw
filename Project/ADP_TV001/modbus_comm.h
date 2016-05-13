#ifndef __MODBUS_COMM
#define __MODBUS_COMM
#include "custmodel.h"


enum	PKT_STEP {
  	PKT_NONE = 0,
	PKT_ADDR,
	PKT_FUNC,	
	PKT_DATA,
	PKT_CRC_H,
	PKT_CRC_L
};

#define	MODBUS_ADDR				0x01

#define	FUNC_RD_HOLDING_REG		0x03
#define	FUNC_RD_INPUT_REG		0x04

#define	FUNC_WR_SINGLE_REG		0x06
#define	FUNC_WR_MULTIPLE_REG	0x16


// Modbus Data  Address Define
#define ADDR_RUNNING	0x01
#define ADDR_SET_TEMP 0x02

#define ADDR_TEMP 0x05
#define ADDR_HEATING 0x06
#define ADDR_VERSION 0x07
#define ADDR_SETTEMP 0x08

#define ADDR_AIR_UPDN  0x10
#define ADDR_ENABEL		 0x11
#define ADDR_RESERVE	 0x12
#define ADDR_OFFSET		0x13

void	ModbusRX0_Init( void);

void	Send_WriteSingleRegister( void );
	void	Send_ReadRegister( byte func, word addr, word nreg );
	void	RX0_DoRxCmd( void );
	void	ModbusRx_Protocol( void );

word	ReadParameter( word addr );

void	WriteParameter( word addr, word data );

#endif //__MODBUS_COMM
