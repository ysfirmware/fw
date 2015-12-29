/****************************************************
  Projekt: TMC562-EVAL

  Modul:   Eval562-Registers.h
           Zugriffsfunktionen und Registeradressen
           für den TMC562

  Datum:   26.01.2012 OK
*****************************************************/

//Registeradressen
#define TMC562_GCONF        0x00
#define TMC562_GSTAT        0x01
#define TMC562_IFCNT        0x02
#define TMC562_SLAVECONF    0x03
#define TMC562_INP_OUT      0x04
#define TMC562_X_COMPARE    0x05

#define TMC562_RAMPMODE_1   0x20
#define TMC562_XACTUAL_1    0x21
#define TMC562_VACTUAL_1    0x22
#define TMC562_VSTART_1     0x23
#define TMC562_A1_1         0x24
#define TMC562_V1_1         0x25
#define TMC562_AMAX_1       0x26
#define TMC562_VMAX_1       0x27
#define TMC562_DMAX_1       0x28
#define TMC562_D1_1         0x2A
#define TMC562_VSTOP_1      0x2B
#define TMC562_TZEROWAIT_1  0x2C
#define TMC562_XTARGET_1    0x2D
#define TMC562_IHOLD_IRUN_1 0x30
#define TMC562_VCOOLTHRS_1  0x31
#define TMC562_VHIGH_1      0x32
#define TMC562_VDCMIN_1     0x33
#define TMC562_SWMODE_1     0x34
#define TMC562_RAMPSTAT_1   0x35
#define TMC562_XLATCH_1     0x36
#define TMC562_ENCMODE_1    0x38
#define TMC562_XENC_1       0x39
#define TMC562_ENC_CONST_1  0x3A
#define TMC562_ENC_STATUS_1 0x3B
#define TMC562_ENC_LATCH_1  0x3C

#define TMC562_RAMPMODE_2   0x40
#define TMC562_XACTUAL_2    0x41
#define TMC562_VACTUAL_2    0x42
#define TMC562_VSTART_2     0x43
#define TMC562_A1_2         0x44
#define TMC562_V1_2         0x45
#define TMC562_AMAX_2       0x46
#define TMC562_VMAX_2       0x47
#define TMC562_DMAX_2       0x48
#define TMC562_D1_2         0x4A
#define TMC562_VSTOP_2      0x4B
#define TMC562_TZEROWAIT_2  0x4C
#define TMC562_XTARGET_2    0x4D
#define TMC562_IHOLD_IRUN_2 0x50
#define TMC562_VCOOLTHRS_2  0x51
#define TMC562_VHIGH_2      0x52
#define TMC562_VDCMIN_2     0x53
#define TMC562_SWMODE_2     0x54
#define TMC562_RAMPSTAT_2   0x55
#define TMC562_XLATCH_2     0x56
#define TMC562_ENCMODE_2    0x58
#define TMC562_XENC_2       0x59
#define TMC562_ENC_CONST_2  0x5A
#define TMC562_ENC_STATUS_2 0x5B
#define TMC562_ENC_LATCH_2  0x5C

#define TMC562_MSLUT0_1     0x60
#define TMC562_MSLUT1_1     0x61
#define TMC562_MSLUT2_1     0x62
#define TMC562_MSLUT3_1     0x63
#define TMC562_MSLUT4_1     0x64
#define TMC562_MSLUT5_1     0x65
#define TMC562_MSLUT6_1     0x66
#define TMC562_MSLUT7_1     0x67
#define TMC562_MSLUTSEL_1   0x68
#define TMC562_MSLUTSTART_1 0x69
#define TMC562_MSCNT_1      0x6A
#define TMC562_MSCURACT_1   0x6B
#define TMC562_CHOPCONF_1   0x6C
#define TMC562_COOLCONF_1   0x6D
#define TMC562_DCCTRL_1     0x6E
#define TMC562_DRVSTATUS_1  0x6F

#define TMC562_MSLUT0_2     0x70
#define TMC562_MSLUT1_2     0x71
#define TMC562_MSLUT2_2     0x72
#define TMC562_MSLUT3_2     0x73
#define TMC562_MSLUT4_2     0x74
#define TMC562_MSLUT5_2     0x75
#define TMC562_MSLUT6_2     0x76
#define TMC562_MSLUT7_2     0x77
#define TMC562_MSLUTSEL_2   0x78
#define TMC562_MSLUTSTART_2 0x79
#define TMC562_MSCNT_2      0x7A
#define TMC562_MSCURACT_2   0x7B
#define TMC562_CHOPCONF_2   0x7C
#define TMC562_COOLCONF_2   0x7D
#define TMC562_DCCTRL_2     0x7E
#define TMC562_DRVSTATUS_2  0x7F

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

//Motorbits und Write-Bit
#define TMC562_MOTOR0       0x20
#define TMC562_MOTOR1       0x40
#define TMC562_WRITE        0x80

//Rampenmodi (Register TMC562_RAMPMODE)
#define TMC562_MODE_POSITION   0
#define TMC562_MODE_VELPOS     1
#define TMC562_MODE_VELNEG     2
#define TMC562_MODE_HOLD       3

//Endschaltermodusbits (Register TMC562_SWMODE)
#define TMC562_SW_STOPL_ENABLE   0x0001
#define TMC562_SW_STOPR_ENABLE   0x0002
#define TMC562_SW STOPL_POLARITY 0x0004
#define TMC562_SW_STOPR_POLARITY 0x0008
#define TMC562_SW_SWAP_LR        0x0010
#define TMC562_SW_LATCH_L_ACT    0x0020
#define TMC562_SW_LATCH_L_INACT  0x0040
#define TMC562_SW_LATCH_R_ACT    0x0080
#define TMC562_SW_LATCH_R_INACT  0x0100
#define TMC562_SW_LATCH_ENC      0x0200
#define TMC562_SW_SG_STOP        0x0400
#define TMC562_SW_SOFTSTOP       0x0800


//Statusbitss (Register TMC562_RAMPSTAT)
#define TMC562_RS_STOPL          0x0001
#define TMC562_RS_STOPR          0x0002
#define TMC562_RS_LATCHL         0x0004
#define TMC562_RS_LATCHR         0x0008
#define TMC562_RS_EV_STOPL       0x0010
#define TMC562_RS_EV_STOPR       0x0020
#define TMC562_RS_EV_STOP_SG     0x0040
#define TMC562_RS_EV_POSREACHED  0x0080
#define TMC562_RS_VELREACHED     0x0100
#define TMC562_RS_POSREACHED     0x0200
#define TMC562_RS_VZERO          0x0400
#define TMC562_RS_ZEROWAIT       0x0800
#define TMC562_RS_SECONDMOVE     0x1000
#define TMC562_RS_SG             0x2000

#define MOTOR_ADDR(m) (0x20 << m )


void WriteTMC562Datagram(UCHAR Address, UCHAR x1, UCHAR x2, UCHAR x3, UCHAR x4);
void WriteTMC562Int(UCHAR Address, int Value);
int ReadTMC562Int(UCHAR Address);
