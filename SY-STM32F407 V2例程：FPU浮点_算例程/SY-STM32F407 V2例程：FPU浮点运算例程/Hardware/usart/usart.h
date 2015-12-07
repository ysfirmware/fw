#ifndef __USART_H
#define __USART_H
#include "stm32f4xx.h"
#include "stdio.h"

/*******************************************************************************************************************************
 * 函数名：USART1_Conf()
 * 参数：无
 *返回值：无
 * 功能：配置USART1，并初始化
********************************************************************************************************************************/																		
void USART1_Conf(void);
/********************************************************************************************************************************
*函数名：USART_SendString()
* 参数：USART_TypeDef* USARTx 所要发送到的串口的指针，u8 *str 指向发送的字符串的指针
* 返回值：void
* 功能：向某个串口发送一个字符串
*********************************************************************************************************************************/
void USART_SendString(USART_TypeDef* USARTx,u8 *str);
/********************************************************************************************************************************
*函数名：fputc()
* 参数：int ch，FILE *f
* 返回值：int
* 功能：重新定义stdio.h中的fputc()函数，使printf()输出到USART1
*********************************************************************************************************************************/
int fputc(int ch,FILE *f);
#endif
