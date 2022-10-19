/*
	*Echo			- PA0:	Input pull-up/pull-down
	*Trigger	- PA1:	Output push-pull, max speed 50 MHz
	
	*EXTI0
	*TIM2
*/



#ifndef HCSR04_H
#define HCSR04_H
#include "stm32f10x.h"

typedef enum
{
	IDLE_STATE,
	WAIT_RISING_STATE,
	WAIT_FALLING_STATE,
	COMPLETE_STATE
}HCSR04_State;

typedef struct
{
	TIM_TypeDef 	*hc04_tim;
	GPIO_TypeDef	*hc04_port;
	uint16_t			hc04_pin;
	HCSR04_State	hc04_state;
}HCSR04_TypeDef;

void EXTI0_Config(void);
void EXTI0_IRQHandler(void);
void HCSR04_Start(void);
void HCSR04_Handler(void);
void HCSR04_Complete_Callback(float distan);
void TIM2_Delay_us(uint16_t us);
void HCSR04_Init(HCSR04_TypeDef *hcsr04_x, TIM_TypeDef *TIMx, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#endif
