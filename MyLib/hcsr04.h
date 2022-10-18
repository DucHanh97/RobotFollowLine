/*
	*Echo			- PA0:	Input pull-up/pull-down
	*Trigger	- PA1:	Output push-pull, max speed 50 MHz
*/



#ifndef HCSR04_H
#define HCSR04_H
#include <stdint.h>

typedef enum
{
	IDLE_STATE,
	WAIT_RISING_STATE,
	WAIT_FALLING_STATE,
	COMPLETE_STATE
}HCSR04_State;

void EXTI0_Config(void);
void EXTI0_IRQHandler(void);
void HCSR04_Start(void);
void HCSR04_Handler(void);
void HCSR04_Complete_Callback(float distan);
void TIM2_Delay_us(uint16_t us);
void HCSR04_Init(void);

#endif
