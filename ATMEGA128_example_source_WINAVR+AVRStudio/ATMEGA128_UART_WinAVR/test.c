#include<avr/io.h>
#include<avr/interrupt.h>
#include<util/delay.h>
#include<stdio.h>


 

#define F_CPU   16000000UL      //16MHz



 

void uart0_init(unsigned long BaudRate);
int putch(char ch);
void putstr(char* str);



 

int main(void)
{
    cli();                  // clear interrupt. sei()�� ���Ҿ� ���. ũ��Ƽ�ü����� ����.
    uart0_init(38400);      // uart0 initialize. baudrate = 38400
    fdevopen(putch, NULL);  // �ݵ�� �Լ��� return���� int, arg�� char���̾�� ��
    DDRA=0xFF;              // PORTA ��������� ����
    PORTA=0x01;
    sei();                  // set enable interrupt


 

    while(1);


 

    return 0;
}


 

void uart0_init(unsigned long BaudRate)
{
    // Not Double mode, Not multi_communication
    UCSR0A=0x00;
    // 0b 1001 1000 RXCIE,TXCIE,UDRIE,RxEN,TxEN,xxx
    UCSR0B=0x98;
    UCSR0C=0x06;
    // Setting BaudRate
    UBRR0H=0x00;
    UBRR0L=(F_CPU/BaudRate/16 - 1);
}


 

int putch(char ch)
{
    if (ch == '\n')                     //���๮��..
        putch('\r');
    while((UCSR0A&0x20)==0x00);         // ��Ʈ�� ���۰� ���
    UDR0=ch;                            // ������ ������
    return 0;
}


 

void putstr(char* str)
{
    while(*str) putch(*str++);
}


 

ISR(USART0_RX_vect)
{
    char buf;
    buf=UDR0;


 

    switch(buf)
    {
        case ',' :
            PORTA=PORTA<<1;
            if(!PORTA)PORTA=0x80;
            break;
        case '.' :
            PORTA=PORTA>>1;
            if(!PORTA)PORTA=0x01;
            break;
        case 'a' :
            putstr("Hell'o World!!\n");
            break;
        case 's' :
            printf("Now PORTA is %#04x\n",PORTA);
            break;
        default :
            break;
    }
}
