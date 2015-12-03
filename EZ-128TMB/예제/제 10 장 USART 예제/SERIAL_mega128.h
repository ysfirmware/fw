/***************************************************************************

���ϸ� :   SERIAL_mega128.H
 
           PE0 -> RXD
		   PE1 -> TXD
           
           16MHz���� 9600 bps

Device :  Atmega128-16AI
Clock  :  16MHz
���� ȯ�� : IAR EWAVR 2.28A
***************************************************************************/

#define		BEL		0x07
#define		BS		0x08
#define		LF		0x0a
#define		CR		0x0d
#define		ESC		0x1b


void init_SERIAL(void)
{

     DDRE=0xFE;     // PE1(TXD) ���(1), PE0(RXD) �Է�(0)

     UCSR0A=0x00;
     UCSR0B=0x18;   // Receive enable, Transmitte enable
     UCSR0C=0x06;   // �񵿱� ���, No parity bit, 1 stop bit   

     UBRR0H=0x00;
     UBRR0L=0x67;   // 16MHz���� 9600 bps  (10���� 103)
}

// �ø��� ��Ʈ�� �ѹ��ڸ� �۽��Ѵ�. 
void putch(unsigned char data)
{
	while(!(UCSR0A & 0x20));
	UDR0=data;
    
	UCSR0A |= 0x20;         // Ŭ���� UDRE0
}

// �ø��� ��Ʈ�κ��� �ѹ��ڸ� �����Ѵ�.  
char getch(void)
{
    unsigned char data;

	while(!(UCSR0A & 0x80));
    data=UDR0;

	UCSR0A |= 0x80;        // Ŭ���� RXC0
	
	return(data);
}

// ��簪�� �ƽ�Ű �ڵ�� �����Ͽ� �۽��Ѵ�.  
void put_hex_ascii(unsigned char ch)
{

	unsigned char hnibble, lnibble;

	lnibble=ch&0x0f;
	hnibble=ch>>4;

    if(hnibble<0x0a)  
		hnibble=hnibble+0x30;
    else 
		hnibble=hnibble+0x37;
    putch(hnibble);

	if(lnibble<0x0a)  
		lnibble=lnibble+0x30;
    else 
		lnibble=lnibble+0x37;
	putch(lnibble);
}

// ���带 �ƽ�Ű�ڵ�� �����Ͽ� �۽��Ѵ�.
void put_word_ascii(unsigned int ch)
{
        put_hex_ascii(ch>>8);
        put_hex_ascii(ch);
}

// ���ڿ��� �۽��Ѵ�.
void put_string(const char *str)
{
	int i;

    for(i=0;*(str+i)!=0;i++)
		{
    	 if(*(str+i)=='\n') 
		   putch(CR);
         putch(*(str+i));
        }
}

// �ѹ��ڸ� �����ϰ� �����Ѵ�.  
char getch_echo(void)
{
	char ch;

	ch=getch();
	putch(ch);
	return ch;
}

// �ƽ�Ű�ڵ带 ���� �����Ѵ�.  
unsigned char get_ascii_hex(void)
{
    unsigned char ch,ch1,ddata;
    ch = ch1 = ddata = 0;

	ch=getch();
	if( ch>='0' && ch<='9' )   ch=(ch-'0');
    else if( ch>='A' && ch<='F' )   ch=(ch-'A'+0x0a);
    ddata = ch<<4;

	ch1=getch();
	if( ch1>='0' && ch1<='9' )   ch1=(ch1-'0');
    else if( ch1>='A' && ch1<='F' )   ch1=(ch1-'A'+0x0a);
    ddata |= ch1;
    return(ddata);
}

// �ƽ�Ű�ڵ带 ���� �����ϰ� �����Ѵ�.  
unsigned char get_ascii_hex_echo(void)
{
       unsigned char ch, ddata=0x00;

       ch=getch_echo();

	   if( ch>='0' && ch<='9' )   ch=(ch-'0');
   	   else if( ch>='A' && ch<='F' )   ch=(ch-'A'+0x0a);
       ddata = ch<<4;

       ch=getch_echo();
	   if( ch>='0' && ch<='9' )   ch=(ch-'0');
  	   else if( ch>='A' && ch<='F' )   ch=(ch-'A'+0x0a);
       ddata = ddata | ch;
       return(ddata);
}

// �ƽ�Ű�ڵ带 ����� ��ȯ�ϰ� �����Ѵ�. 
unsigned int get_ascii_word_echo(void)
{
        unsigned int ch;
        unsigned int ddata=0x0000;

        ch=(unsigned int)get_ascii_hex_echo();
        ddata =(ch<<8);
        ch=(unsigned int)get_ascii_hex_echo();
        ddata = ddata | ch;

        return(ddata);
}
