/******************************************************************************
  * @file       : motion.c 
  * @author     : JUNG SH, EETECH
  * @version    : Ver 0.1
  * @date       : 12/07/2016
  * @brief      : tmc5031. 
  * @pjt model  : motion
  ******************************************************************************/
/* @Story*/
/* @2015-12-07 make*/
/**/
/**/
/*******************************************************************************/

#include "main.h"
#include "pcl6144.h"

//==========================================================================//
//  -> InitMotorDrivers
//  -> 
//  @ :
//===========================================================================//
void InitMotorDrivers()
{
	//Invert the direction of the motor 2 (so that both run the same)
	//WriteTMC562Datagram(0x00, 0x00, 0x00, 0x02, 0x00);
	
	WriteTMC562Datagram(0x00, 0x00, 0x00, 0x02, 0x00);
	//Setting the base configuration of the driver
	//Motor 1
	WriteTMC562Datagram(0x6c, 0x00, 0x01, 0x01, 0xd5);
	WriteTMC562Datagram(0x30, 0x00, 0x07, 0x17, 0x03);

	//Motor 2
	WriteTMC562Datagram(0x7c, 0x00, 0x01, 0x01, 0xd5);
	WriteTMC562Datagram(0x50, 0x00, 0x07, 0x17, 0x03);
}


