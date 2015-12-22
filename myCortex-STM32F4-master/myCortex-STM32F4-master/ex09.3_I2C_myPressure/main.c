
/*
  ex09.3_I2C_myPressure

  myPressure ��� �������̽� ����.
  I2C �������̽��� �̿��� myPressure�� ���� ��°��� ��� UART�� ����Ѵ�.
  CPAL ���̺귯���� ����� I2C�� �������̽� �Ѵ�.
 */

#include <stdio.h>
#include "stm32f4xx.h"
#include "led.h"
#include "llio.h"
#include "cpal.h"
#include "cpal_i2c.h"
#include "cpal_conf.h"

#include "myPressure.h"


// myPressure�� �������� �ʰ� ������ ������� ������ ����ü
// �������Ϳ��� ���� ���� �� ����ü�� ����ȯ �ϸ� ������ ��Ʈ ������ �ʿ���� �ٷ� ���� ���� ���� �� �ִ�.
// ���� �ѹ��� i2c ����� overhead�� ���� �� �ִ�.
typedef __packed struct
{
    short temp;
    long pressure;
    long altitude;
} pressure_t;



static CPAL_TransferTypeDef buff;
static unsigned char i2c_buff[32];


int main()
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;




    LED_Init();
    LED_R_ON();


    CPAL_I2C_StructInit(&I2C1_DevStructure);
    I2C1_DevStructure.CPAL_Dev = CPAL_I2C1;
    I2C1_DevStructure.pCPAL_I2C_Struct->I2C_ClockSpeed = 400000;
    I2C1_DevStructure.CPAL_ProgModel = CPAL_PROGMODEL_DMA;
    I2C1_DevStructure.wCPAL_Options = 0;

    CPAL_I2C_Init(&I2C1_DevStructure);




    // Interrupt input(PB5) configuration
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);


    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);



    LLIO_Init(115200);
    printf("\r\nmyPressure I2C example\r\n");




    // myPressure�� WHO_AM_I �������� ���� �о� ����Ѵ�.
    // read ������� ��ϵ� user callback�� ���� �޾ƿ´�.
    buff.pbBuffer = i2c_buff;
    buff.wAddr1 = PRESSURE_SLAVE_ADDRESS << 1;
    buff.wAddr2 = PRESSURE_REG_WHO_AM_I;
    buff.wNumData = 1;

    I2C1_DevStructure.pCPAL_TransferRx = &buff;
    printf("\r\n\r\nread : %d\r\n", CPAL_I2C_Read(&I2C1_DevStructure));


    while(1)
    {
        __WFI();    // ���ͷ�Ʈ�� �߻��� �� ���� �̰����� ���
    }

}







uint32_t CPAL_TIMEOUT_UserCallback(CPAL_InitTypeDef* pDevInitStruct)
{
    printf("\r\nCPAL timeout error\r\n");

    return CPAL_PASS;
}



void CPAL_I2C_RXTC_UserCallback(CPAL_InitTypeDef* pDevInitStruct)
{
    unsigned char reg;
    pressure_t * pres;

    float temp;
    double pressure, altitude;



    printf("\r\nread TC CB. len = %d\r\n", pDevInitStruct->pCPAL_TransferRx->wNumData);

    reg = pDevInitStruct->pCPAL_TransferRx->wAddr2;
    if (reg == PRESSURE_REG_WHO_AM_I)
    {
        // 0xB1�� ��µǾ�� �����̴�.
        printf("WHO AM I : 0x%02X\r\n", pDevInitStruct->pCPAL_TransferRx->pbBuffer[0]);
    }
    else if (reg == PRESSURE_REG_TEMP_LOW)
    {
        pres = (pressure_t *) pDevInitStruct->pCPAL_TransferRx->pbBuffer;

        temp = (float)pres->temp * 100. / 32767;

        pressure = (double)pres->pressure * 10000. / 2147483647.;
        altitude = (double)pres->altitude * 0.1;

        printf("Pressure[hPa]:%.4f\r\n", pressure);
        printf("Altitude[m]:%.4f\r\n", altitude);
        printf("TEMP[degC]:%.2f\r\n\r\n", temp);


        LED_G_TOGGLE();

    }


}

void CPAL_I2C_ERR_UserCallback(CPAL_DevTypeDef pDevInstance, uint32_t DeviceError)
{
    printf("\r\nCPAL Error. %d\r\n", DeviceError);
}



void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line5);

        buff.pbBuffer = i2c_buff;
        buff.wAddr1 = PRESSURE_SLAVE_ADDRESS << 1;
        buff.wAddr2 = PRESSURE_REG_TEMP_LOW;
        buff.wNumData = sizeof(pressure_t);
        I2C1_DevStructure.pCPAL_TransferRx = &buff;
        CPAL_I2C_Read(&I2C1_DevStructure);
    }
}




