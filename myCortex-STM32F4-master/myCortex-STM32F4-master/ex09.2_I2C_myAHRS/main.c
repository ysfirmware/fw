
/*
  ex09.2_I2C_myAHRS

  myAHRS ��� �������̽� ����.
  I2C �������̽��� �̿��� myAHRS�� ���� ��°��� ��� UART�� ����Ѵ�.
  CPAL ���̺귯���� ����� I2C�� �������̽� �Ѵ�.
 */

#include <stdio.h>
#include "stm32f4xx.h"
#include "led.h"
#include "llio.h"
#include "cpal.h"
#include "cpal_i2c.h"
#include "cpal_conf.h"

#include "myAHRS.h"


// myAHRS�� �������� �ʰ� ������ ������� ������ ����ü
// �������Ϳ��� ���� ���� �� ����ü�� ����ȯ �ϸ� ������ ��Ʈ ������ �ʿ���� �ٷ� ���� ���� ���� �� �ִ�.
// ���� �ѹ��� i2c ����� overhead�� ���� �� �ִ�.
typedef struct
{
    short acc_x;
    short acc_y;
    short acc_z;
    short gyro_x;
    short gyro_y;
    short gyro_z;
    short magnet_x;
    short magnet_y;
    short magnet_z;
    unsigned char dummy[6];     // magnet_z_high �������Ϳ� roll_low �������� ������ 6 ����Ʈ ��ŭ �� ���� ����
    short roll;
    short pitch;
    short yaw;
} ahrs_t;



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
    printf("\r\nmyAHRS I2C example\r\n");




    // myAHRS�� WHO_AM_I �������� ���� �о� ����Ѵ�.
    // read ������� ��ϵ� user callback�� ���� �޾ƿ´�.
    buff.pbBuffer = i2c_buff;
    buff.wAddr1 = AHRS_SLAVE_ADDRESS << 1;
    buff.wAddr2 = AHRS_REG_WHO_AM_I;
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
    ahrs_t *ahrs;

    float acc_x, acc_y, acc_z;
    float gyro_x, gyro_y, gyro_z;
    float magnet_x, magnet_y, magnet_z;
    float roll, pitch, yaw;



    printf("\r\nread TC CB. len = %d\r\n", pDevInitStruct->pCPAL_TransferRx->wNumData);

    reg = pDevInitStruct->pCPAL_TransferRx->wAddr2;
    if (reg == AHRS_REG_WHO_AM_I)
    {
        // 0xB1�� ��µǾ�� �����̴�.
        printf("WHO AM I : 0x%02X\r\n", pDevInitStruct->pCPAL_TransferRx->pbBuffer[0]);
    }
    else if (reg == AHRS_REG_ACC_X_LOW)
    {
        ahrs = (ahrs_t *) pDevInitStruct->pCPAL_TransferRx->pbBuffer;



        acc_x = (float)ahrs->acc_x * 16.0 / 32767.;
        acc_y = (float)ahrs->acc_y * 16.0 / 32767.;
        acc_z = (float)ahrs->acc_z * 16.0 / 32767.;

        gyro_x = (float)ahrs->gyro_x * 2048. / 32767.;
        gyro_y = (float)ahrs->gyro_y * 2048. / 32767.;
        gyro_z = (float)ahrs->gyro_z * 2048. / 32767.;

        magnet_x = (float)ahrs->magnet_x * 8.0 / 32767.;
        magnet_y = (float)ahrs->magnet_y * 8.0 / 32767.;
        magnet_z = (float)ahrs->magnet_z * 8.0 / 32767.;

        roll = (float)ahrs->roll * 0.01;
        pitch = (float)ahrs->pitch * 0.01;
        yaw = (float)ahrs->yaw * 0.01;

        printf("ACC[m/s/s]:%.2f,%.2f,%.2f\r\n", acc_x, acc_y, acc_z);
        printf("GYRO[deg/s]:%.2f,%.2f,%.2f\r\n", gyro_x, gyro_y, gyro_z);
        printf("MAG[Gauss]:%.2f,%.2f,%.2f\r\n", magnet_x, magnet_y, magnet_z);
        printf("R/P[deg]:%.2f,%.2f,%.2f\r\n\r\n", roll, pitch, yaw);


        LED_G_TOGGLE();

    }


}

void CPAL_I2C_ERR_UserCallback(CPAL_DevTypeDef pDevInstance, uint32_t DeviceError)
{
    printf("\r\nCPAL Error. %d\r\n", DeviceError);
}


static int int_cnt;

void EXTI9_5_IRQHandler(void)
{
    if (EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line5);

        if (int_cnt++ % 10 == 0)
        {
            buff.pbBuffer = i2c_buff;
            buff.wAddr1 = AHRS_SLAVE_ADDRESS << 1;
            buff.wAddr2 = AHRS_REG_ACC_X_LOW;
            buff.wNumData = sizeof(ahrs_t);
            I2C1_DevStructure.pCPAL_TransferRx = &buff;
            CPAL_I2C_Read(&I2C1_DevStructure);
        }
    }
}




