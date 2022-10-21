/*
	*Echo			- Input pull-up/pull-down
	*Trigger	- Output push-pull, max speed 50 MHz
	
	*EXTIx
	*TIMx
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
	uint16_t			hc04_trig_pin;
	uint16_t			hc04_echo_pin;
	HCSR04_State	hc04_state;
	float					distan;
}HCSR04_TypeDef;

void HCSR04_Start(HCSR04_TypeDef *hcsr04_x);
void HCSR04_Handler(HCSR04_TypeDef *hcsr04_x);
void HCSR04_Complete_Callback(HCSR04_TypeDef *hcsr04_x);
void TIM_Delay_us(HCSR04_TypeDef *hcsr04_x, uint16_t us);
void EXTI_HCSR04_Callback(HCSR04_TypeDef *hcsr04_x);
void HCSR04_Init(HCSR04_TypeDef *hcsr04_x, TIM_TypeDef *TIMx, GPIO_TypeDef *GPIOx, uint16_t trig_pin, uint16_t echo_pin);

#endif
