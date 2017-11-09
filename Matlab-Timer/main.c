#include "stm32f4xx.h"                  // Device header
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


char str[50];
uint32_t i;
 long map(long x,long in_min,long in_max,long out_min,long out_max)
{
return (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}
 

uint16_t Read_ADC(void)
{
  ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_56Cycles);
	ADC_SoftwareStartConv(ADC1);
	while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
	return ADC_GetConversionValue(ADC1);
}	
void USART_Puts(USART_TypeDef* USARTx,volatile char*s) // char karakter sayisi kadar döndürüyor
{
 while(*s)
 {
  while(!(USARTx ->SR & 0x00000040));
	 USART_SendData(USARTx,*s);
	 *s++;
 }	

}	
int main()
{
	
	uint16_t adc_data,adc_data2; //12 bitlik adc verisini saklar
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	USART_InitTypeDef USART_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE); // transmitter tx A2 GPIOya bagli usartta
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF; //Alternatif fonksiyonlar(input,output,adc disinda baska sey oldugu)
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz;
	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2);//USARTtan bilgiyi GPIOya atacaz,CPUya tanitmamiz lazim
	
	USART_InitStructure.USART_BaudRate=115200; // Saniye içinde hat üzerinden kaç tane bit gönderilmesi gerektigi
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode=USART_Mode_Tx;
	USART_InitStructure.USART_Parity=USART_Parity_No;
	USART_InitStructure.USART_StopBits=USART_StopBits_1;
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;
	USART_Init(USART2,&USART_InitStructure);
	USART_Cmd(USART2,ENABLE);
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	ADC_CommonInitStructure.ADC_Mode=ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler=ADC_Prescaler_Div4; // Kristal hizinin 1/4ü kadar
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	ADC_InitStructure.ADC_Resolution=ADC_Resolution_12b;
	ADC_Init(ADC1,&ADC_InitStructure);
	
	ADC_Cmd(ADC1,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12 ;
	GPIO_InitStructure.GPIO_Mode= GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType= GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed= GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd= GPIO_PuPd_UP;
	GPIO_Init(GPIOD,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource12,GPIO_AF_TIM4);
	
	TIM_TimeBaseStructure.TIM_Period=19999; //arr degeri
	TIM_TimeBaseStructure.TIM_Prescaler=84;//PrescalerValue; 84
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //0 'dan 20 bine kadar sayar Count 0 -> CNT
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);
	TIM_Cmd(TIM4,ENABLE);
 
	TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM1;  //Output compare or Output control mode Set pulse when CNT==CCRx
	TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable;
	
	TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High; // Active High, i.e 0 -> 1 starts duty cycle
	
	while(1)
	{
	
	  adc_data=Read_ADC();
		adc_data=map(adc_data,0,4095,0,750);	
		adc_data2=map(adc_data,0,750,0,180);	
		sprintf(str,"%d \n",adc_data2);
		USART_Puts(USART2,str);
		TIM_OCInitStructure.TIM_Pulse=adc_data;//CCR1_Val; Initial duty cycle
	  TIM_OC1Init(TIM4,&TIM_OCInitStructure);
	  TIM_OC1PreloadConfig(TIM4,TIM_OCPreload_Enable);
		i=400000;
		while(i)
			i--;
	}	


}	
