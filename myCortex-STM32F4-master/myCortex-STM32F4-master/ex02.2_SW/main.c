
/*
 * ex02.2_SW
 * GPIO �Է��� �̿��� ����ġ�Է��� �޾� LED�� �����ϴ� ����
 */

#include "stm32f4xx.h"
#include "led.h"
#include "switch.h"

int main()
{
    LED_Init();

    SW_INIT();

    while(1)
    {
        // SW1(PE0)�� ������ �ִ� ���� ������ LED�� �Ҵ�.
        if (SW_1_IS_PUSHED())
            LED_R_ON();
        else
            LED_R_OFF();

        // SW2(PE1)�� ������ �ִ� ���� ��� LED�� �Ҵ�.
        if (SW_2_IS_PUSHED())
            LED_G_ON();
        else
            LED_G_OFF();
    }

}

