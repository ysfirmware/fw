/* ============================================================================ */
/*		Exp11_1_timer1.c : 타이머1을 사용한 주기적인 인터럽트		*/
/* ============================================================================ */
/*					programmed by Duck-Yong Yoon in 2013.   */

#include "stm32f0xx.h"
#include "OK-STM32F0.h"

#define I_offset 3050
#define I_gain 5.4045
#define V_gain 17.66667

void TIM1_BRK_UP_TRG_COM_IRQHandler(void);	/* TIM1 interrupt function (2Hz) */

volatile unsigned char Start_1,i,key,int_cnt=0;		// interrupt count
volatile signed int duty;		// interrupt count
volatile unsigned int result,result_1;

volatile signed short I;
volatile float Vout_m_avg,Iout_m_avg,P;
volatile float P_diff=0,P_pri=0,V_diff=0,V_pri=0;
/* ----- 인터럽트 처리 프로그램 ----------------------------------------------- */

void TIM1_BRK_UP_TRG_COM_IRQHandler(void){	/* TIM1 interrupt function (2Hz) */
  
  if(int_cnt==2){
   // TIM1->CCR4 = duty;
    TIM1->SR = 0x0000;				// clear pending bit of TIM1 interrupt
    int_cnt=0;
  }
  else int_cnt++;
}

/* ----- 메인 프로그램 -------------------------------------------------------- */

int main(void)
{
  Initialize_MCU();				// initialize MCU and kit
  Delay_ms(50);					// wait for system stabilization
  Initialize_LCD();				// initialize text LCD module
  Initialize_TFT_LCD();				// initialize TFT-LCD module
  Beep();

  GPIOA->MODER |= 0x000000FF;			// use ADC0~ADC3
  RCC->APB2ENR |= 0x00000200;			// enable ADC clock
  RCC->CFGR |= 0x00004000;			// ADC clock = PCLK/4 = 48MHz/4 = 12MHz
  RCC->CFGR3 |= 0x00000100;

  ADC1->CR = 0x00000001;			// enable ADC
  ADC->CCR = 0x01C00000;			// VBATEN = TSEN = VREFEN = 1
  ADC1->SMPR = 0x00000005;			// sampling time = 55.5 cycle
  ADC1->CFGR1 = 0x00000000;			// 12-bit resolution, single conversion mode
  ADC1->CFGR2 = 0x80000000;			// PCLK/4 = 12MHz
    
  GPIOA->MODER &= 0xFF3FFFFF;			// PA11 = alternate function mode
  GPIOA->MODER |= 0x00800000;
  GPIOA->AFR[1] |= 0x00002000;			// PA11 = TIM1_CH4
  
  RCC->APB2ENR |= 0x00000800;			// enable TIM1 clock
  TIM1->PSC = 0;				// 48MHz/(479+1) = 100kHz
  TIM1->ARR = 4799;				// 100kHz / (49999+1) = 2Hz
  
  TIM1->CCR4 = 2399;
  
  TIM1->CNT = 0;				// clear counter
  TIM1->DIER = 0x0001;				// enable update interrupt
  
  TIM1->CCMR2 = 0x6C00;				// OC4M = 110(PWM mode 1), CC4S = 00(output)
  TIM1->CCER = 0x1000;				// CC4E = 1(enable OC4 output)
  TIM1->BDTR = 0x8000;				// MOE = 1
  
  TIM1->CR1 = 0x0005;				// overflow interrupt, enable TIM1
  
  NVIC->ISER[0] |= 0x00002000;			// enable (13)TIM1 interrupt
  
  LCD_string(0x80,"V=       I=");      // display 8-bit integer
  LCD_string(0xC0,"P=     duty=");
  
  while(1){
    
    ADC1->CHSELR = 0x00000004;		// channel 2 (VR1)
    result=0;
    for(i = 0; i < 16; i++){
      ADC1->CR |= 0x00000004;		// start conversion by software
      while(!(ADC1->ISR & 0x00000004));	// wait for end of conversion
      result += ADC1->DR;
      Delay_us(50);
    }
    result >>= 4;				// calculate average
    Vout_m_avg = V_gain *(float)result*3.3/4096.;
  
    ADC1->CHSELR = 0x00000008;		// channel 3 (LM35DZ)
    result_1=0;
    for(i = 0; i < 16; i++){
      ADC1->CR |= 0x00000004;		// start conversion by software
      while(!(ADC1->ISR & 0x00000004));	// wait for end of conversion
      result_1 += ADC1->DR;
      Delay_us(50);
    }
    result_1 >>= 4;				// calculate average
    I = result_1 - I_offset;
    Iout_m_avg =  I_gain * (float)I*3.3/4096. ;
  
    P = Vout_m_avg*Iout_m_avg;
  

    if(Start_1==0){           
      duty = 0;
      Start_1=1;
    }

    //----------- I-V curve Scane -----------
    else if(Start_1==1){
      duty = duty+1;
      if(duty>4799){
        Start_1 = 2;
        duty=4000;
      }    
    }
    //------------ MPPT Algorit m------------
    else if(Start_1==2){

      P = Vout_m_avg*Iout_m_avg;
      P_diff = P-P_pri;
      P_pri = P;

      if(P_diff>0){
        V_diff=Vout_m_avg-V_pri;
        V_pri=Vout_m_avg;

        if(V_diff>0)      duty = duty-5;
        else if(V_diff<0) duty = duty+5;

        if(duty>=4799)    duty= 4799;
        if(duty<=0)       duty = 0;
      }
      else if(P_diff<0){
        V_diff = Vout_m_avg-V_pri;
        V_pri = Vout_m_avg;

        if(V_diff>0)      duty = duty+5;
        else if(V_diff<0) duty = duty-5;

        if(duty>=4799)   duty = 4799;
        if(duty<=0)      duty = 0;
      }
    }
    TIM1->CCR4 = duty;
    LCD_command(0x82);			// display multiplication
    LCD_unsigned_float( Vout_m_avg , 2, 2);
    
    LCD_command(0x8B);			// display multiplication
    LCD_unsigned_float( Iout_m_avg , 1, 3);
    
    LCD_command(0xC2);			// display multiplication
    LCD_unsigned_float( P , 1, 2);
  
    LCD_command(0xCC);			// display multiplication
    LCD_unsigned_float(duty/4799., 1, 2);
  }
}
