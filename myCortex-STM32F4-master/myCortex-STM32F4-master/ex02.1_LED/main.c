
/*
 * ex02.1_LED
 * Red/Green LED�� �ʱ�ȭ�ϰ� �����ϴ� ����
 */

#include "stm32f4xx.h"
#include "led.h"

int main()
{
    LED_Init();

    LED_R_ON();
    LED_G_ON();

}

