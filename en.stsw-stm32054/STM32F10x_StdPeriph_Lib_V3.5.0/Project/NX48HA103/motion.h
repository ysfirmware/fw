/******************************************************************************
  * @file       : motion.h 
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



typedef struct
{
u32 VSTART;
u32 A1;
u32 V1;
u32 AMAX;
u32 VMAX;
u32 DMAX;
u32 D1;
u32 VSTOP;
s32 XTARGET;

}MOT_PAM;

//typedef struct{
	

typedef union {
	u32 nValue;
	struct{
		u8 IHOLD;
	u8 IRUN;
	u8 IHOLDDELAY;
	u8 RESERVE;
	}byte;
}IHOLD_IRUN;


extern void Delay(__IO uint32_t nTime);
void Motion_main();
void Motion_Action(u8 cmd, u16 nValue);
		