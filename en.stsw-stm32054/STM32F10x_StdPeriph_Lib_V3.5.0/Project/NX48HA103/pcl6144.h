/******************************************************************************
  * @file       : pcl6144.c 
  * @author     : KIM YS, EETECH
  * @version    : Ver 0.1
  * @date       : 05/17/2016
  * @brief      : PCL6144. 
  * @pjt model  : NX48HA
  ******************************************************************************/
/* @Story*/
/* @2016-05-17 make*/
/**/
/**/
/*******************************************************************************/


#define TMC562_MODE_VELPOS     1
#define TMC562_MODE_VELNEG     2	  
#define TMC562_MODE_VELHOLD    3

#define TMC562_RAMPMODE     0x00
#define TMC562_XACTUAL      0x01
#define TMC562_VACTUAL      0x02
#define TMC562_VSTART       0x03
#define TMC562_A1           0x04
#define TMC562_V1           0x05
#define TMC562_AMAX         0x06
#define TMC562_VMAX         0x07
#define TMC562_DMAX         0x08
#define TMC562_D1           0x0A
#define TMC562_VSTOP        0x0B
#define TMC562_TZEROWAIT    0x0C
#define TMC562_XTARGET      0x0D
#define TMC562_IHOLD_IRUN   0x10
#define TMC562_VCOOLTHRS    0x11
#define TMC562_VHIGH        0x12
#define TMC562_VDCMIN       0x13
#define TMC562_SWMODE       0x14
#define TMC562_RAMPSTAT     0x15
#define TMC562_XLATCH       0x16
#define TMC562_ENCMODE      0x18
#define TMC562_XENC         0x19
#define TMC562_ENC_CONST    0x1A
#define TMC562_ENC_STATUS   0x1B
#define TMC562_ENC_LATCH    0x1C


	  
#define MOTOR_ADDR(m) (0x20 << m )

#define	ZCS_SET(i)		GPIO_WriteBit(GPIOA, GPIO_Pin_4, (i)?Bit_RESET:Bit_SET)
#define	ZDRV_SET(i)		GPIO_WriteBit(GPIOB, GPIO_Pin_0, (i)?Bit_RESET:Bit_SET)
#define	DCS_SET(i)		GPIO_WriteBit(GPIOB, GPIO_Pin_12, (i)?Bit_RESET:Bit_SET)
#define	DDRV_SET(i)		GPIO_WriteBit(GPIOB, GPIO_Pin_1, (i)?Bit_RESET:Bit_SET)

u8 ReadWriteSPI(u8 DeviceNumber, u8 Data, u8 LastTransfer);
void WriteTMC562Datagram(u8 Address, u8 x1, u8 x2, u8 x3, u8 x4);
void WriteTMC562Int(u8 Address, u32 Value);
int ReadTMC562Int(u8 Address);
