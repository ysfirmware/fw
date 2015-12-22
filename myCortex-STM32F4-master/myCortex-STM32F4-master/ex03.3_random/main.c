
/*
  ex03.3_random

  ���� ���� ����
  random number generator�� ����� ������ �߻���Ű�� �̸� ���
 */

#include <stdio.h>
#include "stm32f4xx.h"
#include "led.h"
#include "delay.h"
#include "llio.h"

int main()
{
    unsigned long r;


    LED_Init();
    LED_R_ON();

    LLIO_Init(115200);
    printf("\r\nRandom Number Generator test\r\n");


    // ���� ������ �ʱ�ȭ
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);

    RNG_Cmd(ENABLE);

    while(1)
    {
        delay_ms(500);         // 0.5�� ������

        // ���ο� ������ ��������� �ʾҴٸ� ������ �� ���� ���
        while(RNG_GetFlagStatus(RNG_FLAG_DRDY)== RESET);

        // �߻��� ������ ���
        r = RNG_GetRandomNumber();
        printf("%u\r\n", r);

        LED_G_TOGGLE();
    }
}



