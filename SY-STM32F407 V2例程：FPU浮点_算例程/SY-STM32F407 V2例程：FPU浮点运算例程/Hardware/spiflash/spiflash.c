#include "spiflash.h"

/******************************************************************************************
*��������SPI_FLASH_Init()
* ������void
* ����ֵ��void
* ���ܣ�SPIFLASH��ʼ���������ⲿ����
*********************************************************************************************/
void SPI_FLASH_Init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
  	SPI_InitTypeDef  SPI_InitStructure;
  	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA ,ENABLE);
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  SPI_FLASH_CS_HIGH();
  	SPI_I2S_DeInit(SPI1);
  	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;           //ȫ˫��
  	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                            //8λ����ģʽ
  	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                                    //����ģʽ��SCKΪ1
  	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                                  //���ݲ����ӵ�2��ʱ����ؿ�ʼ
  	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                                     //NSS�������
  	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;           //������
  	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                             //���ģʽ
  	SPI_InitStructure.SPI_CRCPolynomial = 7;                                       //CRC����ʽ
  	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                                  //����ģʽ
  	SPI_Init(SPI1, &SPI_InitStructure);
  	SPI_Cmd(SPI1, ENABLE);
}
/******************************************************************************************
*��������SPI_FLASH_SectorErase()
* ������u32 SectorAddr   ���ַ
* ����ֵ��void
* ���ܣ�SPIFLASH���������������ⲿ����
*********************************************************************************************/
void SPI_FLASH_SectorErase(u32 SectorAddr)
{
  /*����д����ʹ��ָ��*/
  SPI_FLASH_WriteEnable();
	/*�ȴ�����д�꣬��֤д�����ǿ��е�*/
  SPI_FLASH_WaitForWriteEnd();
  /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();
  /*������������ָ��*/
  SPI_FLASH_SendByte(W25X_SectorErase);
  /*���Ϳ��ַ��8λ*/
  SPI_FLASH_SendByte((SectorAddr & 0xFF0000) >> 16);
  /*���Ϳ��ַ��8λ*/
  SPI_FLASH_SendByte((SectorAddr & 0xFF00) >> 8);
  /*���Ϳ��ַ��8λ*/
  SPI_FLASH_SendByte(SectorAddr & 0xFF);
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();
  /* �ȴ�д���*/
  SPI_FLASH_WaitForWriteEnd();
}

/******************************************************************************************
*��������SPI_FLASH_BulkErase()
* ������void
* ����ֵ��void
* ���ܣ�SPIFLASH��Ƭ�����������ⲿ����
*********************************************************************************************/
void SPI_FLASH_BulkErase(void)
{
  /*ʹ��д��*/
  SPI_FLASH_WriteEnable();
   /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();
  /*������Ƭ����ָ��*/
  SPI_FLASH_SendByte(W25X_ChipErase);
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();
  /* �ȴ�д���*/
  SPI_FLASH_WaitForWriteEnd();
}

/******************************************************************************************
*��������SPI_FLASH_PageWrite()
* ������u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite ����ָ�룬д���ַ��д��ĸ���
* ����ֵ��void
* ���ܣ�SPIFLASHҳд�����ݺ������ⲿ����
*********************************************************************************************/
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
   /*ʹ��д��*/
  SPI_FLASH_WriteEnable();
  /*ʹ��Ƭѡ*/
  SPI_FLASH_CS_LOW();
  /* ����ҳд��ָ��*/
  SPI_FLASH_SendByte(W25X_PageProgram);
  /*���͸�8λ���ݵ�ַ*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF0000) >> 16);
  /*������8λ���ݵ�ַ*/
  SPI_FLASH_SendByte((WriteAddr & 0xFF00) >> 8);
  /*���͵�8λ���ݵ�ַ*/
  SPI_FLASH_SendByte(WriteAddr & 0xFF);
  /*���д��������Ƿ񳬳�ҳ��������С*/
  if(NumByteToWrite > SPI_FLASH_PerWritePageSize)
  {
     NumByteToWrite = SPI_FLASH_PerWritePageSize;
  }
  /*ѭ��д������*/
  while (NumByteToWrite--)
  {
    /*��������*/
    SPI_FLASH_SendByte(*pBuffer);
    /* ָ���Ƶ���һ��д������ */
    pBuffer++;
  }
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();
  /* �ȴ�д���*/
  SPI_FLASH_WaitForWriteEnd();
}

/******************************************************************************************
*��������SPI_FLASH_BufferWrite()
* ������u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite ����ָ�룬д���ַ��д��ĸ���
* ����ֵ��void
* ���ܣ�SPIFLASH������ݺ������ⲿ����
*********************************************************************************************/
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite)
{
  u8 NumOfPage = 0, NumOfSingle = 0, Addr = 0, count = 0, temp = 0;
  Addr = WriteAddr % SPI_FLASH_PageSize;                           //����д���ҳ�Ķ�Ӧ��ʼ��ַ
  count = SPI_FLASH_PageSize - Addr;
  NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;                //�����ܹ�Ҫд��ҳ��
  NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize;               //����ʣ�൥��ҳд�����ݸ���
  if (Addr == 0) /* ���Ҫд���ҳ��ַΪ0��˵��������ҳд���ݣ�û��ƫ��*/
  {
    if (NumOfPage == 0) /* ��������д��ҳ��Ϊ0��˵����������һ��ҳ�ķ�Χ�ڣ���ֱ�ӽ���ҳ��д*/
    {
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);      //����ҳд����
    }
    else /* ���Ҫд��ҳ������0*/
    { 
			/*�Ƚ���ͷ���ݽ�����ҳд��*/
      while (NumOfPage--)
      { 
				//��ҳд��
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				//��ַƫ��
        WriteAddr +=  SPI_FLASH_PageSize;
				//����ָ��ƫ��
        pBuffer += SPI_FLASH_PageSize;
      }
       //��ʣ�����ݸ���д��
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
    }
  }
  else /*���д��ĵ�ַ����ҳ�Ŀ�ͷλ��*/
  {
    if (NumOfPage == 0) /*���д������ҳ�ĸ���Ϊ0��������С��һҳ����*/
    {
      if (NumOfSingle > count) /*���ʣ�����ݴ��ڵ�ǰҳ��ʣ������*/
      {
        temp = NumOfSingle - count;     //���㳬�������ݸ���
				/*д����ǰҳ*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
				/*���õ�ַƫ��*/
        WriteAddr +=  count;
				/*��������ָ��ƫ��*/
        pBuffer += count;
        /*��ʣ����д���µ�ҳ*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, temp);
      }
      else  /*���ʣ������С�ڵ�ǰҳ��ʣ������*/
      {
				/*ֱ��д�뵱ǰҳ*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumByteToWrite);
      }
    }
    else /*���д������ҳ�ĸ�������0�������ݴ���һҳ����*/
    {
      NumByteToWrite -= count;         //�����ݼ�ȥ��ǰҳʣ�������
      NumOfPage =  NumByteToWrite / SPI_FLASH_PageSize;  //����Ҫд����ҳ����
      NumOfSingle = NumByteToWrite % SPI_FLASH_PageSize; //����ʣ�����ݸ���
      /*����ͷ����д�뵱ǰҳʣ���ֽڸ���*/
      SPI_FLASH_PageWrite(pBuffer, WriteAddr, count);
			/*���õ�ַƫ��*/
      WriteAddr +=  count;
			/*��������ָ��ƫ��*/
      pBuffer += count;
       /*��ʼʣ�����ݵ���ҳд��*/
      while (NumOfPage--)
      {
				/*д��һ��ҳ���ֽ���*/
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, SPI_FLASH_PageSize);
				/*���õ�ַƫ��*/
        WriteAddr +=  SPI_FLASH_PageSize;
				/*����ָ��ƫ��*/
        pBuffer += SPI_FLASH_PageSize;
      }
      /*���ʣ�����ݴ���0����ʣ��ĸ���д����һ��ҳ*/
      if (NumOfSingle != 0)
      {
        SPI_FLASH_PageWrite(pBuffer, WriteAddr, NumOfSingle);
      }
    }
  }
}

/******************************************************************************************
*��������SPI_FLASH_BufferRead()
* ������u8* pBuffer, u32 ReadAddr, u16 NumByteToRead ����ָ�룬�����ĵ�ַ�������ĸ���
* ����ֵ��void
* ���ܣ�SPIFLASH������ݺ������ⲿ����
*********************************************************************************************/
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead)
{
   /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();
  /*���Ͷ�����ָ��*/
  SPI_FLASH_SendByte(W25X_ReadData);
  /*����24λ���ݵ�ַ*/
  /* ���͸�8λ���ݵ�ַ*/
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*������8λ���ݵ�ַ*/
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /*���͵�8λ���ݵ�ַ*/
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
  while (NumByteToRead--) /* ѭ����ȡ����*/
  {
    /*��ȡһ���ֽ�����*/
    *pBuffer = SPI_FLASH_SendByte(Dummy_Byte);
    /*����ָ���1*/
    pBuffer++;
  }
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();
}

/******************************************************************************************
*��������SPI_FLASH_ReadID()
* ������void
* ����ֵ��u32 ����ID
* ���ܣ�SPIFLASH��ȡID�������ⲿ����
*********************************************************************************************/
u32 SPI_FLASH_ReadID(void)
{
  u32 Temp = 0, Temp0 = 0, Temp1 = 0, Temp2 = 0;

  /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();

  /*����ʶ������ID��*/
  SPI_FLASH_SendByte(W25X_JedecDeviceID);
  /* ��ȡһ���ֽ�*/
  Temp0 = SPI_FLASH_SendByte(Dummy_Byte);
  /* ��ȡһ���ֽ�*/
  Temp1 = SPI_FLASH_SendByte(Dummy_Byte);
   /* ��ȡһ���ֽ�*/
  Temp2 = SPI_FLASH_SendByte(Dummy_Byte);
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();
  Temp = (Temp0 << 16) | (Temp1 << 8) | Temp2;
  return Temp;
}
/******************************************************************************************
*��������SPI_FLASH_ReadDeviceID()
* ������void
* ����ֵ��u32 �豸ID
* ���ܣ�SPIFLASH��ȡ�豸ID�������ⲿ����
*********************************************************************************************/
u32 SPI_FLASH_ReadDeviceID(void)
{
  u32 Temp = 0;
   /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();
  /*���Ͷ�ȡIDָ��*/
  SPI_FLASH_SendByte(W25X_DeviceID);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  SPI_FLASH_SendByte(Dummy_Byte);
  /*��ȡ8λ����*/
  Temp = SPI_FLASH_SendByte(Dummy_Byte);
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();
  return Temp;
}
/******************************************************************************************
*��������SPI_FLASH_StartReadSequence()
* ������u32 ReadAddr 24λ����ַ
* ����ֵ��void
* ���ܣ�SPIFLASH����ʼ�������ⲿ����
*********************************************************************************************/
void SPI_FLASH_StartReadSequence(u32 ReadAddr)
{
   /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();
  /* ���Ͷ�����ָ��*/
  SPI_FLASH_SendByte(W25X_ReadData);
  /*����24λ���ݵ�ַ*/
  /* ���͸�8λ���ݵ�ַ */
  SPI_FLASH_SendByte((ReadAddr & 0xFF0000) >> 16);
  /*������8λ���ݵ�ַ*/
  SPI_FLASH_SendByte((ReadAddr& 0xFF00) >> 8);
  /*���͵�8λ���ݵ�ַ*/
  SPI_FLASH_SendByte(ReadAddr & 0xFF);
}

/******************************************************************************************
*��������SPI_FLASH_ReadByte()
* ������void
* ����ֵ��u8 8λ����
* ���ܣ�SPIFLASH��һ���ֽں������ⲿ����
*********************************************************************************************/
u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

/******************************************************************************************
*��������SPI_FLASH_SendByte()
* ������u8 byte        д�������
* ����ֵ��u8 8λ����
* ���ܣ�SPIFLASH��дһ���ֽں������ⲿ����
*********************************************************************************************/
u8 SPI_FLASH_SendByte(u8 byte)
{
  /*�ȴ�SPI���Ϳ���*/
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  /*����8λ���� */
  SPI_I2S_SendData(SPI1, byte);
 /* �ȴ�����8λ���� */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  /* ���ؽ��յ�8λ���� */
  return SPI_I2S_ReceiveData(SPI1);
}

/******************************************************************************************
*��������SPI_FLASH_SendHalfWord()
* ������u16 HalfWord        д���16λ����
* ����ֵ��u16 16λ����
* ���ܣ�SPIFLASH��д16λ���ݺ������ⲿ����
*********************************************************************************************/
u16 SPI_FLASH_SendHalfWord(u16 HalfWord)
{
  /*�ȴ�SPI���Ϳ���*/
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  /*����16λ���� */
  SPI_I2S_SendData(SPI1, HalfWord);
  /* �ȴ�����16λ���� */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  /* ���ؽ��յ�16λ���� */
  return SPI_I2S_ReceiveData(SPI1);
}

/******************************************************************************************
*��������SPI_FLASH_WriteEnable()
* ������void
* ����ֵ��void
* ���ܣ�SPIFLASHдʹ�ܺ������ⲿ����
*********************************************************************************************/
void SPI_FLASH_WriteEnable(void)
{
   /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();
  /*����дʹ��ָ��*/
  SPI_FLASH_SendByte(W25X_WriteEnable);
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();
}

/******************************************************************************************
*��������SPI_FLASH_WaitForWriteEnd()
* ������void
* ����ֵ��void
* ���ܣ�SPIFLASH�ȴ�д��Ϻ������ⲿ����
*********************************************************************************************/
void SPI_FLASH_WaitForWriteEnd(void)
{
  u8 FLASH_Status = 0;
   /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();
  /*���Ͷ�״ָ̬�� */
  SPI_FLASH_SendByte(W25X_ReadStatusReg);
  /*ѭ�����Ϳ�����ֱ��FLASHоƬ����*/
  do
  {
    /* ���Ϳ��ֽ� */
    FLASH_Status = SPI_FLASH_SendByte(Dummy_Byte);
  }
  while ((FLASH_Status & WIP_Flag) == SET); /* ����Ƿ����*/
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();
}

/******************************************************************************************
*��������SPI_Flash_PowerDown()
* ������void
* ����ֵ��void
* ���ܣ�SPIFLASH�������ģʽ�������ⲿ����
*********************************************************************************************/
void SPI_Flash_PowerDown(void)   
{ 
  /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();
  /*���͵���ָ�� */
  SPI_FLASH_SendByte(W25X_PowerDown);
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();
}   

/******************************************************************************************
*��������SPI_Flash_WAKEUP()
* ������void
* ����ֵ��void
* ���ܣ�SPIFLASH���ѵ���ģʽ�������ⲿ����
*********************************************************************************************/
void SPI_Flash_WAKEUP(void)   
{
  /* ʹ��Ƭѡ */
  SPI_FLASH_CS_LOW();
  /* �����˳�����ģʽָ�� */
  SPI_FLASH_SendByte(W25X_ReleasePowerDown);
  /*ʧ��Ƭѡ*/
  SPI_FLASH_CS_HIGH();              
}  
