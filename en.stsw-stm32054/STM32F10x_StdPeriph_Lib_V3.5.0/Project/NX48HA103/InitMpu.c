/******************************************************************************
  * @file       : InitMpu.c 
  * @author     : JUNG SH, EETECH
  * @version    : Ver 0.1
  * @date       : 12/07/2016
  * @brief      : Initailize stm32F405 MPU. 
  * @pjt model  : ADP
  ******************************************************************************/
/* @Story*/
/* @2015-12-07 make*/
/**/
/**/
/*******************************************************************************/

#include "main.h"
#include "InitMpu.h"


GPIO_InitTypeDef GPIO_InitStructure;
USART_InitTypeDef USART_InitStructure;
ADC_InitTypeDef ADC_InitStructure;
//ADC_CommonInitTypeDef ADC_CommonInitStructure;

//==========================================================================//
//  -> Init_SPI1
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_SPI1()
{
	SPI_InitTypeDef SPI_InitStructure;
	
	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	/* Enable GPIO clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/* Enable DMA clock */
	//RCC_APB2PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// connect SPI1 pins to SPI alternate function
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; // set to full duplex mode, seperate MOSI and MISO lines
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;     // transmit in master mode, NSS pin has to be always high
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; // one packet of data is 8 bits wide
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;        // clock is low when idle
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;      // data sampled at first edge
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft | SPI_NSSInternalSoft_Set; // set the NSS management to internal and pull internal NSS high	
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4; // SPI frequency is APB1 frequency / 4    
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;// data is transmitted MSB first
//	SPI_InitStructure.SPI_CRCPolynomial=0;
	SPI_Init(SPI1, &SPI_InitStructure); 	
	SPI_Cmd(SPI1, ENABLE);
}
//==========================================================================//
//  -> Init_SPI2
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_SPI2()
{
	//SPI_InitTypeDef SPI_InitStructure;
	
	/* Peripheral Clock Enable -------------------------------------------------*/
	/* Enable the SPI clock */
	//RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	/* Enable GPIO clocks */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* Enable DMA clock */
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);
	
	/* configure pins used by SPI2
	 *PB5, PB6	= NCS
 	   PB13 		= SCK
       PB15		= MOSI
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_13 | GPIO_Pin_15; //ACS1n | ACS2n | ASCLKn | ADINn
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* configure pins used by SPI2
	 *PB8, PB9 = MISO
	 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9; //ADOUT1n | ADOUT2n
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// connect SPI2 pins to SPI alternate function
}
//==========================================================================//
//  -> Init_USART1
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_USART1()
{
	/* Enable USART1 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	/* Configure USART1 Tx (PA9) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART1 Rx (PA10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9 , GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10 , GPIO_AF_USART1);
	
	/* Port Configuration */
	USART_InitStructure.USART_BaudRate = 9600;
	
	/* data length 8bit */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		
	/* Configure USART2 */
	USART_Init(USART1, &USART_InitStructure);

	/* Enable the USART Receive interrupt
	: this interrupt is generated when the USART1 receive data register is not empty */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	/* Enable USART */
	USART_Cmd(USART1, ENABLE);
}
//==========================================================================//
//  -> Init_USART2
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_USART2()
{
	USART_InitTypeDef USART_InitStructure;
	
	/* Enable USART2 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	USART_DeInit(USART2); 
	
	/* Configure USART2 Tx (PA2) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART2 Rx (PA3) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2 , GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3 , GPIO_AF_USART2);
	
	/* Port Configuration */
	USART_InitStructure.USART_BaudRate = 9600;
    //Current_BPS = bps;
	/* data length 8bit */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	//USART_InitStructure.USART_WordLength = USART_WordLength_9b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		
	/* Configure USART2 */
	USART_Init(USART2, &USART_InitStructure);

	/* Enable the USART Receive interrupt
	: this interrupt is generated when the USART2 receive data register is not empty */
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	//USART_ITConfig(USART2, USART_IT_TXE, ENABLE);

	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);

	// 버퍼 초기화
	
}
//==========================================================================//
//  -> Init_USART3
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_USART3()
{
	//USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* Enable USART3 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
	/* Enable GPIO clocks */
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	/* Configure USART3 Tx (PB10) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Configure USART3 Rx (PB11) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10 , GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11 , GPIO_AF_USART3);
	
	//485 DR PB12
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* NVIC configuration */
	/* Configure the Priority Group to 2 bits */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the USARTx Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	/* Enable USART */
	USART_Cmd(USART3, ENABLE);	
}
//==========================================================================//
//  -> Init_USART6
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_USART6()
{
	/* Enable USART6 clocks */
	RCC_APB1PeriphClockCmd(RCC_APB2Periph_USART6, ENABLE);
	
	/* Configure USART6 Tx (PC6) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	/* Configure USART6 Rx (PC7) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource6 , GPIO_AF_USART6);
	GPIO_PinAFConfig(GPIOC, GPIO_PinSource7 , GPIO_AF_USART6);
	
	/* Port Configuration */
	USART_InitStructure.USART_BaudRate = 9600;
	
	/* data length 8bit */
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		
	/* Configure USART6 */
	USART_Init(USART6, &USART_InitStructure);

	/* Enable the USART Receive interrupt
	: this interrupt is generated when the USART6 receive data register is not empty */
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);

	/* Enable USART */
	USART_Cmd(USART6, ENABLE);
}

//==========================================================================//
//  -> Init_Output
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_Output()
{
	/* GPIO Periph Clock Enable */
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |  //EN_X | MS1_X | EXT_Q_X 
																GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |  //EN_Y | MS1_Y | EXT_Q_Y 
																GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 |  //EN_Z | MS1_Z | EXT_Q_Z 
	                              GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11; //EN_U | MS1_U | EXT_Q_U 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //LEDA | LEDB | LEDC | LEDD
	GPIO_Init(GPIOF, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | 
															//MOTION_ELLX |MOTION_ELLY |MOTION_ELLZ |MOTION_ELLU |MOTION_RST |
																GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
															//HEATERA 	  | HEATERB 		| EXO1 			  | EXO2
	GPIO_Init(GPIOG, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_13 | GPIO_Pin_15; //ACS1n | ACS2n | ASCLKn | ADINn
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}
//==========================================================================//
//  -> Init_Input
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_Input()
{
	/* GPIO Periph Clock Enable */
  RCC_APB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |  //DI1 | DI2 | DI3 | DI4 
																GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 |  //DI5 | DI6 | DI7 | DI8 
																GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11; //BSYX | BSYY | BSYZ |BSYU 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
}
//==========================================================================//
//  -> Init_LED
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_LED()
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3; //LEDA | LEDB | LEDC | LEDD
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}


void Init_ADC()
{
	/* Enable ADC1 and GPIOC clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	
	
	/* ADC Common Init */
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4; 		// ADC clock = PCLK2/prescaler. ADC freq = typical 30MHz, Max 36MHz. PCLK2 = 168/2=84MHz.
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	
	
	/* ADC channel 10 (PC0) configuration */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	/* ADC channel configuration */
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	
	/* ADC1 regular channel10 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
	
	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);
	
}

//==========================================================================//
//  -> Init_RCC
//  -> RCC configuration
//  @ :
//===========================================================================//
void Init_RCC(void)
{
    
    
}

//==========================================================================//
//  -> Init_NVIC
//  -> NVIC configuration
//  @ :
//===========================================================================//
void Init_NVIC(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	// Set the Vector Table base location at 0x08000000 
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   

	// 1 bits for pre-emption priority, 3 bits for subpriority 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); // 16

	//=====================================================================
	// Preemption Priority : 0

	/* Enable and configure RCC global IRQ channel */
	NVIC_InitStructure.NVIC_IRQChannel = RCC_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//=====================================================================
	// Preemption Priority : 1
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	/* UART1 RS232 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure); 

	/* UART5 RS485 */
	/*
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_Init(&NVIC_InitStructure);
	*/
	/* PCL6025B */
	/* Enable and set EXTI9_5 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	//NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}



//==========================================================================//
//  -> fputc
//  -> Retargets the C library printf function to the USART.
//  @ :
//===========================================================================//
int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the USART */

    if( ch == '\n') {
        USART_SendData(USART6, '\r');
        while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);
        USART_SendData(USART6, '\n');
    } else {
        USART_SendData(USART6, (uint8_t) ch);
    }

    /* Loop until the end of transmission */
        while(USART_GetFlagStatus(USART6, USART_FLAG_TXE) == RESET);

    return ch;
}

//==========================================================================//
//  -> Init_Mpu
//  -> .
//  @ :
//===========================================================================//
void Init_Mpu(void)
{
#if 0  //[[ YSKim_160519
	Init_SPI1();   // MOTION PCL6144 COMM
	Init_SPI2();   // HEATER TC AD7792 COMM
	Init_USART1(); // RS232
	Init_USART2(); // USB FT234XD
	Init_USART3(); // RS485
	Init_USART6(); // RS232		
	Init_Output();
	Init_Input();
	Init_LED();
	Init_ADC();

#endif //]] YSKim_160519
}
