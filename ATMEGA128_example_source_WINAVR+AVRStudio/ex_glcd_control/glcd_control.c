// Target : M2560
// Crystal: 16.000Mhz
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define MOTOR PORTE
#define LED PORTF
#define GLCD_C PORTA
#define GLCD_D PORTC
#define T8 unsigned char

#define RS_ON GLCD_C = (GLCD_Control|=0x01)
#define RS_OFF GLCD_C = (GLCD_Control&=0xfe)
#define RW_ON GLCD_C = (GLCD_Control|=0x02)
#define RW_OFF GLCD_C = (GLCD_Control&=0xfd)
#define EN_ON GLCD_C = (GLCD_Control|=0x04)
#define EN_OFF GLCD_C = (GLCD_Control&=0xfb)
#define CS2_ON GLCD_C = (GLCD_Control|=0x10)
#define CS2_OFF GLCD_C = (GLCD_Control&=0xef)
#define CS1_ON GLCD_C = (GLCD_Control|=0x08)
#define CS1_OFF GLCD_C = (GLCD_Control&=0xf7)
#define RESET_ON GLCD_C = (GLCD_Control|=0x20)
#define RESET_OFF GLCD_C = (GLCD_Control&=0xdf)
#define CS_ALL GLCD_C = (GLCD_Control|=0x18)
#define ON 1
#define OFF 0
#define delaytime 1
#define delaytime1 2

char GLCD_Control;
const char x=0xb8, y=0x40;
int status=0  ;
char compare[8][128]={0x00,};

void DataInput(char data);
void delay(int cnt);

void port_init(void)
{
 PORTA = 0x00;
 DDRA  = 0xFF;
 PORTB = 0x00;
 DDRB  = 0xFF;
 PORTC = 0x00; //m103 output only
 DDRC  = 0xFF;
 PORTD = 0x00;
 DDRD  = 0xFF;
 PORTE = 0xFC;
 DDRE  = 0xFC;
 PORTF = 0x00;
 DDRF  = 0xFF;
 PORTG = 0x00;
 DDRG  = 0x00;
}


//TIMER0 initialize - prescale:64
// WGM: Normal
// desired value: 1KHz
// actual value:  1.000KHz (0.0%)
void timer0_init(void)
{
 TCCR0 = 0x00; //stop
 ASSR  = 0x00; //set async mode
 TCNT0 = 0x06; //set count
 OCR0  = 0xFA;
 TCCR0 = 0x04; //start timer
}

ISR(TIMER0_OVF_vect)
{
 TCNT0 = 0x06; //reload counter value 
 MOTOR ^= 0x04;
}



//call this routine to initialize all peripherals
void init_devices(void)
{
 //stop errant interrupts until set up
 cli(); //disable all interrupts
 XMCRA = 0x00; //external memory
 port_init();
 timer0_init();

 MCUCR = 0x00;
 EICRA = 0x00; //extended ext ints
 EICRB = 0x00; //extended ext ints
 EIMSK = 0x00;
 TIMSK = 0x01; //timer interrupt sources
 ETIMSK = 0x00; //extended timer interrupt sources
 
 sei(); //re-enable interrupts
 //all peripherals are now initialized
}

void E_Pulse(void){
EN_ON;
delay(delaytime);
EN_OFF;
}

void DisplayOnOff(int onoff){
RW_OFF;
RS_OFF;
if(onoff==ON){
DataInput(0x3F);
}else{
DataInput(0x3E);
}
}

void DataInput(char data){
RS_OFF;
RW_OFF;
GLCD_Control = data;
E_Pulse();
}
void DisplayStartLine(int displayline){ 
DataInput(displayline | 0xc0);
}
void SetX(int X){
DataInput(X | 0xb8);
}
void SetY(int Y){
DataInput(Y | 0x40);
}

void Reset(void){
DataInput(0x10);
}
void Init_Graph(void){
CS2_ON;
CS1_ON;
DisplayOnOff(ON);
Reset();
}
void delay(int cnt){
int i , j;
for(i=0; i<cnt; i++){
for(j=0; j<100; j++) ;
}
}

void GWrite_Control(char CS, char cmd)
{
delay(delaytime);
EN_OFF;
delay(delaytime);
RS_OFF;
delay(delaytime);
RW_OFF;
delay(delaytime);
GLCD_C = CS;
delay(delaytime);
EN_ON;
delay(delaytime);
GLCD_D = cmd;
delay(delaytime);
EN_OFF;
delay(delaytime);
}   

void GWrite_Data(char CS, char data){
delay(delaytime);
EN_OFF;
delay(delaytime);
RS_ON;
delay(delaytime);
RW_OFF;
delay(delaytime);
GLCD_C = CS;
delay(delaytime);
EN_ON;
delay(delaytime);
GLCD_D = data;
delay(delaytime);
EN_OFF;
delay(delaytime);
}
void GLcd_Clear(){
int i, j, x, y;
GLCD_Control = 0x00;
GWrite_Control(CS_ALL, 0x3f);
GWrite_Control(CS_ALL, 0xc0);
x=0xb8;
y=0x40;
for(i=0; i<=7; i++){
  GWrite_Control(CS_ALL, x);
  GWrite_Control(CS_ALL, y);
  for(j=0; j<64; j++){
   GWrite_Data(CS_ALL, 0x00);}
  x++;
  }  
}

void GLcd_init(){
RESET_OFF;
RESET_ON;
CS_ALL;
GWrite_Control(CS_ALL, 0x3f);
GWrite_Control(CS_ALL, 0xc0);
GWrite_Control(CS_ALL, 0xb8);
GWrite_Control(CS_ALL, 0x40);
}
      
void gotoxy(int i, int j, char data){
  int x1=i%128, y1=j%8;
  if(x1<64){
   GWrite_Control(CS_ALL, x+y1);
   GWrite_Control(CS_ALL, y+x1);
   GWrite_Data(CS2_OFF, data);
   } else {
   GWrite_Control(CS_ALL, x+y1);
   GWrite_Control(CS_ALL, y+x1-64);
   GWrite_Data(CS1_OFF, data);
   }
  }
void PointXY(int xpoint, int ypoint){
int point=0x01, value;
int x1, y1, y2, y3;
x1=xpoint%128;
y1=ypoint%64;
y2=y1/8;
y3=y1%8;
compare[y2][x1]|=(point<<y3);
value=compare[y2][x1];
if(x1<64){
  GWrite_Control(CS_ALL, x+y2);
  GWrite_Control(CS_ALL, y+x1);
  GWrite_Data(CS2_OFF, value);} else {
  GWrite_Control(CS_ALL, x+y2);
  GWrite_Control(CS_ALL, y+x1-64);
  GWrite_Data(CS1_OFF, value);}
  }
void Point_Del(int xpoint, int ypoint){
int point=0x01, value;
int x1, y1, y2, y3;
x1=xpoint%128;
y1=ypoint%64;
y2=y1/8;
y3=y1%8;
compare[y2][x1]&=(~(point<<y3));
value=compare[y2][x1];
if(x1<64){
  GWrite_Control(CS_ALL, x+y2);
  GWrite_Control(CS_ALL, y+x1);
  GWrite_Data(CS2_OFF, value);} else {
  GWrite_Control(CS_ALL, x+y2);
  GWrite_Control(CS_ALL, y+x1-64);
  GWrite_Data(CS1_OFF, value);}
  }
  
void Draw_Line(int X0, int Y0, int X1, int Y1){
  int x0=X0, y0=Y0, x1=X1, y1=Y1; 
  int dy = y1 - y0; 
  int dx = x1 - x0; 
  int stepx, stepy; 
  if (dy < 0) { 
   dy = -dy; 
   stepy = -1; 
   } else { 
    stepy = 1; 
    } 
  if (dx < 0) { 
   dx = -dx; 
   stepx = -1; 
   } else { 
    stepx = 1; 
   } 
  dy <<= 1;            
  dx <<= 1;            
  PointXY(x0, y0);     
  if (dx > dy) { 
   int fraction = dy - (dx >> 1);  
   while (x0 != x1) { 
    if (fraction >= 0) { 
      y0 += stepy; 
      fraction -= dx;                  
      } 
    x0 += stepx; 
    fraction += dy;                    
    PointXY(x0, y0); 
     } 
    } else { 
        int fraction = dx - (dy >> 1); 
        while (y0 != y1) { 
          if (fraction >= 0) { 
              x0 += stepx; 
              fraction -= dy; 
              } 
          y0 += stepy; 
          fraction += dx;      
          PointXY(x0, y0); 
            } 
        } 
    } 
void Line_Del(int X0, int Y0, int X1, int Y1){
  int x0=X0, y0=Y0, x1=X1, y1=Y1; 
  int dy = y1 - y0; 
  int dx = x1 - x0; 
  int stepx, stepy; 
  if (dy < 0) { 
   dy = -dy; 
   stepy = -1; 
   } else { 
    stepy = 1; 
    } 
  if (dx < 0) { 
   dx = -dx; 
   stepx = -1; 
   } else { 
    stepx = 1; 
   } 
  dy <<= 1;            
  dx <<= 1;            
  Point_Del(x0, y0);     
  if (dx > dy) { 
   int fraction = dy - (dx >> 1);  
   while (x0 != x1) { 
    if (fraction >= 0) { 
      y0 += stepy; 
      fraction -= dx;                  
      } 
    x0 += stepx; 
    fraction += dy;                    
    Point_Del(x0, y0); 
     } 
    } else { 
        int fraction = dx - (dy >> 1); 
        while (y0 != y1) { 
          if (fraction >= 0) { 
              x0 += stepx; 
              fraction -= dy; 
              } 
          y0 += stepy; 
          fraction += dx;      
          Point_Del(x0, y0); 
            } 
        } 
    }                
void Draw_Rect(int x1, int y1, int x2, int y2, int fill){
if(fill==1){
  if(x1<x2){
   for(x1; x1<=x2; x1++){
    Draw_Line(x1,y1,x1,y2);}
        } else {
           for(x1; x1>=x2; x1--){
            Draw_Line(x1,y1,x1,y2);}
                }} else { Draw_Line(x1,y1,x1,y2); 
                                 Draw_Line(x1,y1,x2,y1);
                                 Draw_Line(x2,y2,x2,y1);
                                 Draw_Line(x2,y2,x1,y2);
                                 }
     }      
void Rect_Del(int x1, int y1, int x2, int y2){
if(x1<x2){
  for(x1; x1<=x2; x1++){
   Line_Del(x1,y1,x1,y2);}
  } else {
          for(x1; x1>=x2; x1--){
            Line_Del(x1,y1,x1,y2);}
                }
     }   
void Draw_Circle(int x1, int y1, int radius, int fill){
int a, r, cond;
a = 0;
r = radius;
cond = 1 - r;
do{
  if(fill==1){
   Draw_Line(x1-a, y1+r, x1+a, y1+r);
   Draw_Line(x1-a, y1-r, x1+a, y1-r);
   Draw_Line(x1-r, y1+a, x1+r, y1+a);
   Draw_Line(x1-r, y1-a, x1+r, y1-a);
   } else {
     PointXY(a+x1, r+y1);
     PointXY(r+x1, a+y1);
     PointXY(x1-a, r+y1);
     PointXY(x1-r, a+y1);
     PointXY(x1+r, y1-a);
     PointXY(x1+a, y1-r);
     PointXY(x1-a, y1-r);
     PointXY(x1-r, y1-a);
     }
  if(cond < 0){
    cond += 3 + 2*a++;
    } else {cond+= 5 + 2*(a++ - r--);} 
    }while(a <= r);
  }

	void Circle_Del(int x1, int y1, int radius){
	int a, r, cond;
	a = 0;
	r = radius;
	cond = 1 - r;
	do{
   	Line_Del(x1-a, y1+r, x1+a, y1+r);
   	Line_Del(x1-a, y1-r, x1+a, y1-r);
   	Line_Del(x1-r, y1+a, x1+r, y1+a);
   	Line_Del(x1-r, y1-a, x1+r, y1-a);
  
 	if(cond < 0){
    cond += 3 + 2*a++;
    } else {cond+= 5 + 2*(a++ - r--);} 
    }while(a <= r);
  }  



void main(void)
{
int i, j, k, on=1, off=0;
T8 Ti, Tj, Li=0;
//T8 *str1 = "NewTC Module Test";
//T8 *str2 = "T.02-704-4774";
//T8 *str[4]={"www.NewTC.co.kr", "Tel.02-704-4774", "david@NewTC.co.kr", "FOR AVR & ARM Kit"};
char temp;
int arr[17][4]={{13,3,29,19},{55,24,71,40},{98,45,114,61},
{55,3,71,19},{98,3,114,19},{13,24,29,40},{98,24,114,40},
{13,45,29,61},{55,45,71,61},{55,24,71,40},{13,3,29,19},
{55,24,71,40},{98,45,114,61},{55,3,71,19},{98,3,114,19},
{13,24,29,40},{98,24,114,40}};
int arr1[15][2]={{21,11},{21,32},{21,53},{63,11},{63,32},{63,53},
{106,11},{106,32},{106,53},{21,11},{21,53},{63,11},{63,53},
{106,11},{106,53}};
init_devices();
GLcd_init();
GLcd_Clear();
Draw_Line(1,0,126,0);
Draw_Line(1,21,126,21);
Draw_Line(1,42,126,42);
Draw_Line(1,63,126,63);
Draw_Line(0,0,0,63);
Draw_Line(42,0,42,63);
Draw_Line(84,0,84,63);
Draw_Line(127,0,127,63);

Draw_Circle(106,32,8,off);
Draw_Circle(106,53,8,on);
Draw_Rect(55,4,71,18,on);
Draw_Rect(55,46,71,60,off);
Draw_Rect(98,4,114,18,off);
Draw_Circle(63,32,8,off);
Draw_Rect(55,24,71,40,off);
Draw_Circle(21,11,8,off);
Draw_Circle(21,32,8,on);
Draw_Rect(13,46,29,60,on);

while(1){

};
}

