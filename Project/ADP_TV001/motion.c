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
#include "tmc5031.h"
#include "motion.h"


MOT_PAM Motion_Param;
//==========================================================================//
//  -> InitMotorDrivers
//  -> 
//  @ :
//===========================================================================//
void InitMotorDrivers()
{
	//Invert the direction of the motor 2 (so that both run the same)
	//WriteTMC562Datagram(0x00, 0x00, 0x00, 0x02, 0x00);
	
	//WriteTMC562Datagram(0x00, 0x00, 0x00, 0x02, 0x00);
	//Setting the base configuration of the driver
	//Motor 1
	WriteTMC562Datagram(0x6c, 0x00, 0x01, 0x01, 0xd5); //12MHz
	WriteTMC562Datagram(0x30, 0x00, 0x07, 0x05, 0x01);

	//Motor 2
	WriteTMC562Datagram(0x7c, 0x00, 0x01, 0x01, 0xd5);
	WriteTMC562Datagram(0x50, 0x00, 0x07, 0x05, 0x01);
}

//==========================================================================//
//  -> Param set
//  -> 
//  @ :
//===========================================================================//
/*static void SetAxisParameter(void)
{
	
}
*/
void Motion_Set(u8 nAxis)
{
	if(nAxis)
	{
	WriteTMC562Int(TMC562_VSTART|MOTOR_ADDR(nAxis), 500);
	WriteTMC562Int(TMC562_VSTOP|MOTOR_ADDR(nAxis), 500);
	WriteTMC562Int(TMC562_AMAX|MOTOR_ADDR(nAxis), 5000);
	}
	else
	{
	WriteTMC562Int(TMC562_VSTART|MOTOR_ADDR(nAxis), 500);
	WriteTMC562Int(TMC562_VSTOP|MOTOR_ADDR(nAxis), 500);
	WriteTMC562Int(TMC562_AMAX|MOTOR_ADDR(nAxis), 5000);
	}
	//WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(nAxis), 0, 0, 0, TMC562_MODE_VELPOS);
}

//JOG
void Jog_Right(u8 nAxis)
{
	if(nAxis)
		WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(nAxis), 20000);
	else
		WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(nAxis), 40000);
	WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(nAxis), 0, 0, 0, TMC562_MODE_VELPOS);
}
void Jog_Left(u8 nAxis)
{
	if(nAxis)
		WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(nAxis), 1000);
	else
		WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(nAxis), 500);
	WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(nAxis), 0, 0, 0, TMC562_MODE_VELNEG);
}
void MotorStop(u8 nAxis)
{
	WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(nAxis), 0);
	WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(nAxis), 0, 0, 0, TMC562_MODE_VELNEG);
}
//INC

void MotionInc(u8 nAxis, MOT_PAM Motion_Par)
{
	
	WriteTMC562Int(TMC562_VSTART|MOTOR_ADDR(nAxis), Motion_Par.VSTART);
	WriteTMC562Int(TMC562_A1|MOTOR_ADDR(nAxis), Motion_Par.A1);
	WriteTMC562Int(TMC562_V1|MOTOR_ADDR(nAxis), Motion_Par.V1);
	WriteTMC562Int(TMC562_AMAX|MOTOR_ADDR(nAxis), Motion_Par.AMAX);
	WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(nAxis), Motion_Par.VMAX);
	WriteTMC562Int(TMC562_DMAX|MOTOR_ADDR(nAxis), Motion_Par.DMAX);
	WriteTMC562Int(TMC562_D1|MOTOR_ADDR(nAxis), Motion_Par.D1);
	WriteTMC562Int(TMC562_VSTOP|MOTOR_ADDR(nAxis), Motion_Par.VSTOP);
	WriteTMC562Int(TMC562_XTARGET|MOTOR_ADDR(nAxis), Motion_Par.XTARGET);
	/*
	WriteTMC562Int(TMC562_VSTART|MOTOR_ADDR(nAxis), 500);
	WriteTMC562Int(TMC562_A1|MOTOR_ADDR(nAxis),20000);
	WriteTMC562Int(TMC562_V1|MOTOR_ADDR(nAxis), 20000);
	WriteTMC562Int(TMC562_AMAX|MOTOR_ADDR(nAxis), 10000);
	WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(nAxis), 51200);
	WriteTMC562Int(TMC562_DMAX|MOTOR_ADDR(nAxis), 10000);
	WriteTMC562Int(TMC562_D1|MOTOR_ADDR(nAxis), 20000);
	WriteTMC562Int(TMC562_VSTOP|MOTOR_ADDR(nAxis), 500);
	WriteTMC562Int(TMC562_XTARGET|MOTOR_ADDR(nAxis), 0);
	WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(nAxis), 0, 0, 0, TMC562_RAMPMODE);
	*/
}

//ABS
void MotionAbs(u8 nAxis, MOT_PAM Motion_Par)
{
	
}
//HOM
void MotionHom(u8 nAxis, MOT_PAM Motion_Par)
{
	
	WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(nAxis), 0, 0, 0, TMC562_MODE_VELHOLD);
}

//



//drive current control
void CurCont(u8 nAxis, u32 Value)
{
	//addr 0x30, 0x50
	WriteTMC562Int(TMC562_IHOLD_IRUN|MOTOR_ADDR(nAxis), Value);
}


u32 GenInput;
u32 ReadInput()
{
	GenInput = ReadTMC562Int(0x04);
	
	return GenInput;
}

int ReadPos(u8 nAxis)
{
	return ReadTMC562Int(TMC562_XACTUAL|MOTOR_ADDR(nAxis));
}

u32 ReadSwMode(u8 nAxis)
{
	return ReadTMC562Int(TMC562_SWMODE|MOTOR_ADDR(nAxis));
}
u32 ReadRamStat(u8 nAxis)
{
	return ReadTMC562Int(TMC562_RAMPSTAT|MOTOR_ADDR(nAxis));
}

void WriteGconf(u32 Value)
{
	
}


u32 ReadGconf()
{
	return ReadTMC562Int(0x00);
}

u32 ReadGstat()
{
	return ReadTMC562Int(0x01);
}

void WriteShaft(u8 nAxis)
{
	WriteTMC562Int(0x00 , ReadGconf() | (1 << (nAxis+8)));
}

void MotionPgm()
{
	

}

u32 Test, Test1;

typedef struct{
	u8 status;
	u8 next;
	u8 pre;
}MOTION_STAUTS;

MOTION_STAUTS MotionStatus;		
IHOLD_IRUN CurValue;
u32 error;
s32 Posx, Posy;
u32 RDRVENN;
u32 SWVER;
u32 xxx;
u32 swm, rmst, rmst1;
void Motion_main()
{
	switch(MotionStatus.status)
	{
	case 0: // initialize
		ZDRV_SET(0);
		InitMotorDrivers();
		Test1 = ReadInput();
		ZDRV_SET(1);
		//DDRV_SET(1);
		/*
		CurValue.byte.IHOLD = 10;
		CurValue.byte.IRUN = 15;
		CurValue.byte.IHOLDDELAY = 1;
		Test = CurValue.nValue;
		
		
		CurCont(0, Test); // 
		CurCont(1, Test);
		*/
		Test1 = ReadInput();
		MotionStatus.status = 1;
		//Motion_Set(0);
		//Motion_Set(1);
		
		break;
	case 1: // monitoring
		
		if(ReadGstat())
		{
			//err define
		error= ReadGstat();//xAxis err
		//= ReadGstat() & 0x02;//yAxis err
		}
		Motion_Param.A1 = 20000;
		Motion_Param.AMAX = 10000;
		Motion_Param.D1 = 20000;
		Motion_Param.DMAX = 10000;
		Motion_Param.V1 = 20000;
		Motion_Param.VMAX = 25600;
		Motion_Param.VSTART = 500;
		Motion_Param.VSTOP = 500;
		Motion_Param.XTARGET =-30000;
		
		
		MotionInc(0, Motion_Param);
		MotionInc(1, Motion_Param);
		
		MotionStatus.status = 2;
		
		break;
	case 2:
		Posx = ReadPos(0);
		Posy = ReadPos(1);
		RDRVENN = ReadInput() & 0x00000080;
		SWVER = ReadInput() & 0xFF000000;
		rmst = ReadRamStat(0);
		rmst1 = ReadRamStat(1);
		swm = ReadSwMode(0);
		break;
	default:
		break;
		
	}	
}

void Motion_Auto()
{

}
/*
WriteTMC562Int(TMC562_VSTART|MOTOR_ADDR(nAxis), 500);
	WriteTMC562Int(TMC562_A1|MOTOR_ADDR(nAxis),20000);
	WriteTMC562Int(TMC562_V1|MOTOR_ADDR(nAxis), 20000);
	WriteTMC562Int(TMC562_AMAX|MOTOR_ADDR(nAxis), 10000);
	WriteTMC562Int(TMC562_VMAX|MOTOR_ADDR(nAxis), 51200);
	WriteTMC562Int(TMC562_DMAX|MOTOR_ADDR(nAxis), 10000);
	WriteTMC562Int(TMC562_D1|MOTOR_ADDR(nAxis), 20000);
	WriteTMC562Int(TMC562_VSTOP|MOTOR_ADDR(nAxis), 500);
	WriteTMC562Int(TMC562_XTARGET|MOTOR_ADDR(nAxis), 0);
	WriteTMC562Datagram(TMC562_RAMPMODE|MOTOR_ADDR(nAxis), 0, 0, 0, TMC562_RAMPMODE);
	*/
u16 nSVL;


void Motion_Action(u8 cmd, u16 nValue)
{
	u16 c=0xf000;
	switch(cmd)
	{
		case 'i':
			Motion_Param.A1 = 20000;
			Motion_Param.AMAX = 10000;
			Motion_Param.D1 = 20000;
			Motion_Param.DMAX = 10000;
			Motion_Param.V1 = 20000;
			Motion_Param.VMAX = 51200;
			Motion_Param.VSTART = 500;
			Motion_Param.VSTOP = 500;
			Motion_Param.XTARGET = 0;
			MotionInc(1, Motion_Param);
			
			MotionInc(0, Motion_Param);
			while(c--);
			while(!(Motion_Param.XTARGET == ReadPos(0)));
			nSVL = nValue;
			if(nValue==0)
				break;
			switch(nValue)
			{
			case 1:
				Motion_Param.XTARGET = -8000; //20000 - 11600 = 8400
				break;
			case 2:
				Motion_Param.XTARGET = -16000; //20000 - 11600 = 8400
				break;
			case 3:
				Motion_Param.XTARGET = -24000; //20000 - 11600 = 8400
				break;
			case 4:
				Motion_Param.XTARGET = -32000; //20000 - 11600 = 8400
				break;
			case 5:
				Motion_Param.XTARGET = -40000; //20000 - 11600 = 8400
				break;
			default:
				Motion_Param.XTARGET = 0;
				break;
			}
			Motion_Param.A1 = 4000;
			Motion_Param.AMAX = 3000;
			Motion_Param.D1 = 4000;
			Motion_Param.DMAX = 3000;
			Motion_Param.V1 = 4000;
			Motion_Param.VMAX = 6400;
			Motion_Param.VSTART = 200;
			Motion_Param.VSTOP = 200;
			
			MotionInc(0, Motion_Param);
			
			while(!(Motion_Param.XTARGET == ReadPos(0)));
			
			Motion_Param.A1 = 10000;
			Motion_Param.AMAX = 5000;
			Motion_Param.D1 = 10000;
			Motion_Param.DMAX = 5000;
			Motion_Param.V1 = 10000;
			Motion_Param.VMAX = 6400;
			Motion_Param.VSTART = 200;
			Motion_Param.VSTOP = 200;
			Motion_Param.XTARGET -= 4000;
			MotionInc(0, Motion_Param);
			Motion_Param.XTARGET = -4000;
			MotionInc(1, Motion_Param);
			
			
			
			//MotionInc(1, Motion_Param);
			//while(!(Motion_Param.XTARGET == ReadPos(0)))
			/*
			Motion_Param.A1 = 10000;
			Motion_Param.AMAX = 5000;
			Motion_Param.D1 = 10000;
			Motion_Param.DMAX = 5000;
			Motion_Param.V1 = 10000;
			Motion_Param.VMAX = 12800;
			Motion_Param.VSTART = 200;
			Motion_Param.VSTOP = 200;
			Motion_Param.XTARGET = -5000;
			
			MotionInc(0, Motion_Param);
			Motion_Param.A1 = 10000;
			Motion_Param.AMAX = 5000;
			Motion_Param.D1 = 10000;
			Motion_Param.DMAX = 5000;
			Motion_Param.V1 = 10000;
			Motion_Param.VMAX = 12800;
			Motion_Param.VSTART = 200;
			Motion_Param.VSTOP = 200;
			Motion_Param.XTARGET = 15000;
			
			MotionInc(1, Motion_Param);
			*/
			/*
			Motion_Param.A1 = 10000;
			Motion_Param.AMAX = 15000;
			Motion_Param.D1 = 50000;
			Motion_Param.DMAX = 15000;
			Motion_Param.V1 = 50000;
			Motion_Param.VMAX = 256000;
			Motion_Param.VSTART = 1000;
			Motion_Param.VSTOP = 1500;
			Motion_Param.XTARGET = 20000;
			
			*/
			break;
		case 'd':
			Motion_Param.A1 = 60000;
			Motion_Param.AMAX = 60000;
			Motion_Param.D1 = 60000;
			Motion_Param.DMAX = 60000;
			Motion_Param.V1 = 100000;
			Motion_Param.VMAX = 400000;
			Motion_Param.VSTART = 50000;
			Motion_Param.VSTOP = 50000;
			switch(nValue)
			{
			case 1:
				if(nSVL==5)
					Motion_Param.XTARGET = -36000; //20000 - 11600 = 8400
				else if(nSVL==4)
					Motion_Param.XTARGET = -28000; //20000 - 11600 = 8400
				else if(nSVL==3)
					Motion_Param.XTARGET = -20000;
				else if(nSVL==2)
					Motion_Param.XTARGET = -12000;
				else if(nSVL==1)
					Motion_Param.XTARGET = -4000;
				else
				{
					Motion_Param.XTARGET = -4000;
					nSVL+=1;
				}
				nSVL-=1;
				break;
			case 2:
				if(nSVL==5)
					Motion_Param.XTARGET = -28000; //20000 - 11600 = 8400
				else if(nSVL==4)
					Motion_Param.XTARGET = -20000; //20000 - 11600 = 8400
				else if(nSVL==3)
					Motion_Param.XTARGET = -12000;
				else if(nSVL==2)
					Motion_Param.XTARGET = -4000;
				else
				{
					Motion_Param.XTARGET = -4000;
					nSVL+=1;
				}
				nSVL-=2;
				break;
			case 3:
				if(nSVL==5)
					Motion_Param.XTARGET = -20000; //20000 - 11600 = 8400
				else if(nSVL==4)
					Motion_Param.XTARGET = -12000; //20000 - 11600 = 8400
				else if(nSVL==3)
					Motion_Param.XTARGET = -4000;
				else
				{
					Motion_Param.XTARGET = -4000;
					nSVL+=1;
				}
				nSVL-=3;
				break;
			case 4:
				if(nSVL==5)
					Motion_Param.XTARGET = -12000; //20000 - 11600 = 8400
				else if(nSVL==4)
					Motion_Param.XTARGET = -4000; //20000 - 11600 = 8400
				else
				{
					Motion_Param.XTARGET = -4000;
					nSVL+=1;
				}
				nSVL-=4;
				break;
			case 5:
				Motion_Param.XTARGET = -4000; //20000 - 11600 = 8400
				nSVL-=5;
				break;
				
			default:
				Motion_Param.XTARGET = -4000;
				break;
			}
			MotionInc(0, Motion_Param);
			
			break;
	}

}