/***************************************************************************//**
 *   @file   AD7792.c
 *   @brief  Implementation of AD7792 Driver.
 *   @author Bancisor MIhai
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
 *   SVN Revision: 501
*******************************************************************************/

/******************************************************************************/
/* Include Files                                                              */
/******************************************************************************/
#include "AD7792.h"				// AD7792 definitions.

/***************************************************************************//**
 * @brief Initializes the SPI communication peripheral.
 *
 * @param lsbFirst - Transfer format (0 or 1).
 *                   Example: 0x0 - MSB first.
 *                            0x1 - LSB first.
 * @param clockFreq - SPI clock frequency (Hz).
 *                    Example: 1000 - SPI clock frequency is 1 kHz.
 * @param clockPol - SPI clock polarity (0 or 1).
 *                   Example: 0x0 - idle state for SPI clock is low.
 *	                          0x1 - idle state for SPI clock is high.
 * @param clockPha - SPI clock phase (0 or 1).
 *                   Example: 0x0 - data is latched on the leading edge of SPI
 *                                  clock and data changes on trailing edge.
 *                            0x1 - data is latched on the trailing edge of SPI
 *                                  clock and data changes on the leading edge.
 *
 * @return 0 - Initialization failed, 1 - Initialization succeeded.
*******************************************************************************/
#if 0  //[[ YSKim_160530
unsigned char SPI_Init(unsigned char lsbFirst,
                       unsigned long clockFreq,
                       unsigned char clockPol,
                       unsigned char clockPha)
{
	// Add your code here.
	
    return(1);
}
#endif //]] YSKim_160530

/**************************************************************************************
 * spiWriteReg
 *
 * Writes to an 8-bit register with the SPI port
 **************************************************************************************/

void spiWriteReg(const unsigned char regAddr,const unsigned char regData)
{

  unsigned char SPICount;                                       // Counter used to clock out the data

  unsigned char SPIData;                                        // Define a data structure for the SPI data

  ACS1_SET(1);                                        		// Make sure we start with active-low CS high
  ASCLKn(0);                                        		// and CK low

  SPIData = regAddr;                                            // Preload the data to be sent with Address
  ACS1_SET(0);                                                   // Set active-low CS low to start the SPI cycle 
                                                                // Although SPIData could be implemented as an "int", 
                                                                // resulting in one
                                                                // loop, the routines run faster when two loops 
                                                                // are implemented with
                                                                // SPIData implemented as two "char"s.
  
  for (SPICount = 0; SPICount < 8; SPICount++)                  // Prepare to clock out the Address byte
  {
    if (SPIData & 0x80)                                         // Check for a 1
      ADINn(1);                                             // and set the MOSI line appropriately
    else
      ADINn(0);
    ASCLKn(1);                                                 // Toggle the clock line
    ASCLKn(0);
    SPIData <<= 1;                                              // Rotate to get the next bit
  }                                                             // and loop back to send the next bit
                                                        
                                                                // Repeat for the Data byte
  SPIData = regData;                                            // Preload the data to be sent with Data
  for (SPICount = 0; SPICount < 8; SPICount++)
  {
    if (SPIData & 0x80)
      ADINn(1);
    else
      ADINn(0);
    ASCLKn(1);
    ASCLKn(0);
    SPIData <<= 1;
  }          
  ACS1_SET(1);
  ADINn(0);
}

/**************************************************************************************
 * spiReadReg
 *
 * Reads an 8-bit register with the SPI port.
 * Data is returned. 
 **************************************************************************************/

unsigned char spiReadReg (const unsigned char regAddr)
{

  unsigned char SPICount;                                       // Counter used to clock out the data
  
  unsigned char SPIData;                  
  
  ACS1_SET(1);                                                   // Make sure we start with active-low CS high
  ASCLKn(0);                                                   // and CK low
  SPIData = regAddr;                                            // Preload the data to be sent with Address and Data

  ACS1_SET(0);                                                   // Set active-low CS low to start the SPI cycle
  for (SPICount = 0; SPICount < 8; SPICount++)                  // Prepare to clock out the Address and Data
  {
    if (SPIData & 0x80)
      ADINn(1);
    else
      ADINn(0);
    ASCLKn(1);
    ASCLKn(0);
    SPIData <<= 1;
  }                                                             // and loop back to send the next bit
  ADINn(0);		                                                  // Reset the MOSI data line
  
  SPIData = 0;
  for (SPICount = 0; SPICount < 8; SPICount++)                  // Prepare to clock in the data to be read
  {
    SPIData <<=1;                                               // Rotate the data
    ASCLKn(1);                                                 // Raise the clock to clock the data out of the MAX7456
    SPIData += ADOUT1_SET;                                        // Read the data bit
    ASCLKn(0);                                                 // Drop the clock ready for the next bit
  }                                                             // and loop back
  ACS1_SET(1);                                                   // Raise CS
                      
  return ((unsigned char)SPIData);                              // Finally return the read data
}


/***************************************************************************//**
 * @brief Writes data to SPI.
 *
 * @param data - Write data buffer:
 *               - first byte is the chip select number;
 *               - from the second byte onwards are located data bytes to write.
 * @param bytesNumber - Number of bytes to write.
 *
 * @return Number of written bytes.
*******************************************************************************/
unsigned char SPI_Write(unsigned char* data,
                        unsigned char bytesNumber)
{
	
#if 1  //[[ YSKim_160530
  	unsigned char chipSelect    = data[0];
		unsigned char writeData[4]  = {0, 0, 0, 0};
    //unsigned char readData[4]	= {0, 0, 0, 0};
    unsigned char byte          = 0;
    
    for(byte = 0;byte < bytesNumber;byte ++)
    {
        writeData[byte] = data[byte + 1];
    }
    if(chipSelect == 1)
    {
        ADI_PART_CS_LOW;
    }
    if(chipSelect == 2)
    {
        //ST7579_CS_LOW;
    }
    for(byte = 0;byte < bytesNumber;byte ++)
    {
			spiWriteReg((unsigned char)data[byte], writeData[byte]);
    }
    if(chipSelect == 1)
    {
        ADI_PART_CS_HIGH;
    }
    if(chipSelect == 2)
    {
        //ST7579_CS_HIGH;
    }
	
#endif //]] YSKim_160530
	return(bytesNumber);

}

/***************************************************************************//**
 * @brief Reads data from SPI.
 *
 * @param data - As an input parameter, data represents the write buffer:
 *               - first byte is the chip select number;
 *               - from the second byte onwards are located data bytes to write.
 *               As an output parameter, data represents the read buffer:
 *               - from the first byte onwards are located the read data bytes. 
 * @param bytesNumber - Number of bytes to write.
 *
 * @return Number of written bytes.
*******************************************************************************/
unsigned char SPI_Read(unsigned char* data,
                        unsigned char bytesNumber)
{	
#if 1  //[[ YSKim_160530
  	unsigned char chipSelect    = data[0];
		unsigned char writeData[4]  = {0, 0, 0, 0};
    unsigned char readData[4]	= {0, 0, 0, 0};
    unsigned char byte          = 0;
    
    for(byte = 0;byte < bytesNumber;byte ++)
    {
        writeData[byte] = data[byte + 1];
        data[byte + 1] = 0;
    }
    if(chipSelect == 1)
    {
        ADI_PART_CS_LOW;
    }
    if(chipSelect == 2)
    {
        //ST7579_CS_LOW;
    }
    for(byte = 0;byte < bytesNumber;byte ++)
    {
				spiWriteReg((unsigned char)data[byte], writeData[byte]);
        spiReadReg((unsigned char)data[byte]);
    }
    if(chipSelect == 1)
    {
        ADI_PART_CS_HIGH;
    }
    if(chipSelect == 2)
    {
        //ST7579_CS_HIGH;
    }
    for(byte = 0;byte < bytesNumber;byte ++)
    {
        data[byte] = readData[byte];
    }
#else //]] YSKim_160530

	ADI_PART_CS_LOW;
	
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE)==RESET);
	SPI_I2S_SendData(SPI2, data);
	while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE)==RESET);
	
	if(bytesNumber) ADI_PART_CS_HIGH;
	
#endif //]] YSKim_160530   
	return(bytesNumber);
}


/***************************************************************************//**
 * @brief Initializes the AD7792 and checks if the device is present.
 *
 * @return status - Result of the initialization procedure.
 *                  Example: 1 - if initialization was successful (ID is 0x0B).
 *                           0 - if initialization was unsuccessful.
*******************************************************************************/
unsigned char AD7792_Init(void)
{ 
	unsigned char status = 0x1;
    
    //SPI_Init(0, 1000000, 1, 1);
    if((AD7792_GetRegisterValue(AD7792_REG_ID, 1, 1) & 0x0F) != AD7792_ID)
	{
		status = 0x0;
	}
    
	return(status);
}

/***************************************************************************//**
 * @brief Sends 32 consecutive 1's on SPI in order to reset the part.
 *
 * @return  None.    
*******************************************************************************/
void AD7792_Reset(void)
{
	unsigned char dataToSend[5] = {0x03, 0xff, 0xff, 0xff, 0xff};
	
  ADI_PART_CS_LOW;	    
	SPI_Write(dataToSend,4);
	ADI_PART_CS_HIGH;	
}
/***************************************************************************//**
 * @brief Reads the value of the selected register
 *
 * @param regAddress - The address of the register to read.
 * @param size - The size of the register to read.
 *
 * @return data - The value of the selected register register.
*******************************************************************************/
unsigned long AD7792_GetRegisterValue(unsigned char regAddress, 
                                      unsigned char size,
                                      unsigned char modifyCS)
{
	unsigned char data[5]      = {0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned long receivedData = 0x00;
    unsigned char i            = 0x00; 
    
	data[0] = 0x01 * modifyCS;
	data[1] = AD7792_COMM_READ |  AD7792_COMM_ADDR(regAddress); 
	SPI_Read(data,(1 + size));
	for(i = 1;i < size + 1;i ++)
    {
        receivedData = (receivedData << 8) + data[i];
    }
    
    return (receivedData);
}
/***************************************************************************//**
 * @brief Writes the value to the register
 *
 * @param -  regAddress - The address of the register to write to.
 * @param -  regValue - The value to write to the register.
 * @param -  size - The size of the register to write.
 *
 * @return  None.    
*******************************************************************************/
void AD7792_SetRegisterValue(unsigned char regAddress,
                             unsigned long regValue, 
                             unsigned char size,
                             unsigned char modifyCS)
{
	unsigned char data[5]      = {0x00, 0x00, 0x00, 0x00, 0x00};	
	unsigned char* dataPointer = (unsigned char*)&regValue;
    unsigned char bytesNr      = size + 1;
    
    data[0] = 0x01 * modifyCS;
    data[1] = AD7792_COMM_WRITE |  AD7792_COMM_ADDR(regAddress);
    while(bytesNr > 1)
    {
        data[bytesNr] = *dataPointer;
        dataPointer ++;
        bytesNr --;
    }	    
	SPI_Write(data,(1 + size));
}
/***************************************************************************//**
 * @brief  Waits for RDY pin to go low.
 *
 * @return None.
*******************************************************************************/
void AD7792_WaitRdyGoLow(void)
{
#if 0  //[[ YSKim_160601
    while( AD7792_RDY_STATE )
    {
        //;
    }
#endif //]] YSKim_160601
}

/***************************************************************************//**
 * @brief Sets the operating mode of AD7792.
 *
 * @param mode - Mode of operation.
 *
 * @return  None.    
*******************************************************************************/
void AD7792_SetMode(unsigned long mode)
{
    unsigned long command;
    
    command = AD7792_GetRegisterValue(AD7792_REG_MODE,
                                      2,
                                      1); // CS is modified by SPI read/write functions.
    command &= ~AD7792_MODE_SEL(0xFF);
    command |= AD7792_MODE_SEL(mode);
    AD7792_SetRegisterValue(
            AD7792_REG_MODE,
            command,
            2, 
            1); // CS is modified by SPI read/write functions.
}
/***************************************************************************//**
 * @brief Selects the channel of AD7792.
 *
 * @param  channel - ADC channel selection.
 *
 * @return  None.    
*******************************************************************************/
void AD7792_SetChannel(unsigned long channel)
{
    unsigned long command;
    
    command = AD7792_GetRegisterValue(AD7792_REG_CONF,
                                      2,
                                      1); // CS is modified by SPI read/write functions.
    command &= ~AD7792_CONF_CHAN(0xFF);
    command |= AD7792_CONF_CHAN(channel);
    AD7792_SetRegisterValue(
            AD7792_REG_CONF,
            command,
            2,
            1); // CS is modified by SPI read/write functions.
}

/***************************************************************************//**
 * @brief  Sets the gain of the In-Amp.
 *
 * @param  gain - Gain.
 *
 * @return  None.    
*******************************************************************************/
void AD7792_SetGain(unsigned long gain)
{
    unsigned long command;
    
    command = AD7792_GetRegisterValue(AD7792_REG_CONF,
                                      2,
                                      1); // CS is modified by SPI read/write functions.
    command &= ~AD7792_CONF_GAIN(0xFF);
    command |= AD7792_CONF_GAIN(gain);
    AD7792_SetRegisterValue(
            AD7792_REG_CONF,
            command,
            2,
            1); // CS is modified by SPI read/write functions.
}
/***************************************************************************//**
 * @brief Sets the reference source for the ADC.
 *
 * @param type - Type of the reference.
 *               Example: AD7792_REFSEL_EXT	- External Reference Selected
 *                        AD7792_REFSEL_INT	- Internal Reference Selected.
 *
 * @return None.    
*******************************************************************************/
void AD7792_SetIntReference(unsigned char type)
{
    unsigned long command = 0;
    
    command = AD7792_GetRegisterValue(AD7792_REG_CONF,
                                      2,
                                      1); // CS is modified by SPI read/write functions.
    command &= ~AD7792_CONF_REFSEL(AD7792_REFSEL_INT);
    command |= AD7792_CONF_REFSEL(type);
    AD7792_SetRegisterValue(AD7792_REG_CONF,
							command,
							2,
                            1); // CS is modified by SPI read/write functions.
}

/***************************************************************************//**
 * @brief Performs the given calibration to the specified channel.
 *
 * @param mode - Calibration type.
 * @param channel - Channel to be calibrated.
 *
 * @return none.
*******************************************************************************/
void AD7792_Calibrate(unsigned char mode, unsigned char channel)
{
    unsigned short oldRegValue = 0x0;
    unsigned short newRegValue = 0x0;
    
    AD7792_SetChannel(channel);
    oldRegValue &= AD7792_GetRegisterValue(AD7792_REG_MODE, 2, 1); // CS is modified by SPI read/write functions.
    oldRegValue &= ~AD7792_MODE_SEL(0x7);
    newRegValue = oldRegValue | AD7792_MODE_SEL(mode);
    ADI_PART_CS_LOW; 
    AD7792_SetRegisterValue(AD7792_REG_MODE, newRegValue, 2, 0); // CS is not modified by SPI read/write functions.
    AD7792_WaitRdyGoLow();
    ADI_PART_CS_HIGH;
    
}

/***************************************************************************//**
 * @brief Returns the result of a single conversion.
 *
 * @return regData - Result of a single analog-to-digital conversion.
*******************************************************************************/
unsigned long AD7792_SingleConversion(void)
{
    unsigned long command = 0x0;
    unsigned long regData = 0x0;
    
    command  = AD7792_MODE_SEL(AD7792_MODE_SINGLE);
    ADI_PART_CS_LOW;
    AD7792_SetRegisterValue(AD7792_REG_MODE, 
                            command,
                            2,
                            0);// CS is not modified by SPI read/write functions.
    AD7792_WaitRdyGoLow();
    regData = AD7792_GetRegisterValue(AD7792_REG_DATA, 2, 0); // CS is not modified by SPI read/write functions.
    ADI_PART_CS_HIGH;

    return(regData);
}

/***************************************************************************//**
 * @brief Returns the average of several conversion results.
 *
 * @return samplesAverage - The average of the conversion results.
*******************************************************************************/
unsigned long AD7792_ContinuousReadAvg(unsigned char sampleNumber)
{
    unsigned long samplesAverage = 0x0;
    unsigned long command        = 0x0;
    unsigned char count          = 0x0;
    
    command = AD7792_MODE_SEL(AD7792_MODE_CONT);
    ACS1_SET(0);
    AD7792_SetRegisterValue(AD7792_REG_MODE,
                            command, 
                            2,
                            0);// CS is not modified by SPI read/write functions.
    for(count = 0;count < sampleNumber;count ++)
    {
        AD7792_WaitRdyGoLow();
        samplesAverage += AD7792_GetRegisterValue(AD7792_REG_DATA, 2, 0);  // CS is not modified by SPI read/write functions.
    }
    ADI_PART_CS_HIGH;
    samplesAverage = samplesAverage / sampleNumber;
    
    return(samplesAverage);
}

