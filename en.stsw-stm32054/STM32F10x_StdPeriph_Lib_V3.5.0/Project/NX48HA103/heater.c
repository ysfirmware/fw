/******************************************************************************
  * @file       : heater.c 
  * @author     : KIM YS, EETECH
  * @version    : Ver 0.1
  * @date       : 05/30/2016
  * @brief      : Heater procedure. 
  * @pjt model  : NX48HA
  ******************************************************************************/
/* @Story*/
/* @2016-05-30 make*/
/**/
/**/
/*******************************************************************************/

#include "main.h"
#include "heater.h"
#include "AD7792.h"		    // AD7792 definitions.
unsigned long 	value 		= 0;	// value read from the ADC


//==========================================================================//
//    H E A T E R _   C O N T
//==========================================================================//
void Heater_Cont(u16 nCh, u8 nStat)
{
	if(nStat==HEATER_ON)
	{
  	GPIO_WriteBit(GPIOG, nCh, Bit_SET);
	}
	else if(nStat==HEATER_OFF)
	{
		GPIO_WriteBit(GPIOG, nCh, Bit_RESET);
	}
}

#if 0  //[[ YSKim_160531
void Delay_ms(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}
#endif //]] YSKim_160531

//==========================================================================//
//    H E A T E R _ M A I N
//==========================================================================//
void Heater_main()
{
	AD7792_Reset();
	if(!AD7792_Init())
	{
		return;		
	}
	
	/* AD7792 setup */
    AD7792_SetGain(AD7792_GAIN_1);                // set gain to 1
    AD7792_SetChannel(AD7792_CH_AIN1P_AIN1M);     // use AIN1(+) - AIN1(-) 
    AD7792_SetIntReference(AD7792_REFSEL_INT);    // select internal 1.17V reference
    AD7792_Calibrate(AD7792_MODE_CAL_INT_ZERO,
                     AD7792_CH_AIN1P_AIN1M);      // Internal Zero-Scale Calibration
    value = AD7792_ContinuousReadAvg(20);
		while(1)
		{
			value = AD7792_SingleConversion();
			if(value > 0x8000)
				{
					value -= 0x8000;
					value = ((value * 1170) >> 15);
				}
			else
				{
					value = 0x8000 - value;
					value = ((value * 1170) >> 15);
				}
			//Delay_ms(250);                       
		}
}
