/*	Timer driver writen by NGUYEN DUC HANH
	Timer Mode: Up Counter
	
	PWM PIN:
								TIM1			TIM2		TIM3			TIM4
			*	CH1:		PA8				PA0			PA6				PB6
			*	CH2:		PA9				PA1			PA7				PB7
			*	CH3:		PA10			PA2			PB0				PB8
			*	CH4:		PA11			PA3			PB1				PB9
			In PWM Mode, GPIO PINs need to configure to Floating Input
*/

#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include "stm32f10x.h"

#define MAX_ARR_VALUE		0xFFFF
#define MAX_PSC_VALUE 	0xFFFF

#define CH1		1
#define CH2		2
#define CH3		3
#define CH4		4

void TIM_Config(TIM_TypeDef *TIMx, uint16_t psc, uint16_t arr);

/*-------------------- INTERRUPT FUNCTIONS ---------------------------*/
void TIM_IT_Config(TIM_TypeDef *TIMx, uint16_t psc, uint16_t arr, uint8_t Priority);
void TIM1_UP_IRQHandler(void);
void TIM2_IRQHandler(void);
void TIM3_IRQHandler(void);
void TIM4_IRQHandler(void);

/*	Callback function when have been timer interrupt event	*/
void TIM_IRQ_Callback(TIM_TypeDef *TIMx);

/*-------------------- TIMER PWM FUNCTIONS ---------------------------*/
void TIM_PWM_Config(TIM_TypeDef *TIMx, uint16_t Channel, uint16_t psc, uint16_t arr);
void TIM_PWM_SetDuty(TIM_TypeDef *TIMx, uint16_t Channel, uint16_t duty);

//void TIM3_PWM_Init(uint16_t psc, uint16_t arr);
//void TIM3_PWM_SetDuty(uint8_t Channel, uint8_t duty);

#endif
