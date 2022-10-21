#include "timer_driver.h"

/*-------------------TIMER CONFIGURE--------------------------*/
void TIM_Config(TIM_TypeDef *TIMx, uint16_t psc, uint16_t arr)
{
	if(TIMx == TIM1)
	{
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	}
	else if(TIMx == TIM2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(TIMx == TIM3)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(TIMx == TIM4)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	TIMx->PSC = psc - 1;
	TIMx->ARR = arr -1;
	
	TIMx->CR1 |= TIM_CR1_CEN;								//Enable Counter
	while(!(TIMx->SR & TIM_SR_UIF));				//Wait for Update Interrupt Flag is set
}

/*--------------------------------------------------------------*/
/****************************************************************/
/*------------------INTERRUPT TIMER CONFIGURE-------------------*/
void TIM_IT_Config(TIM_TypeDef *TIMx, uint16_t psc, uint16_t arr, uint8_t Priority)
{
	if(TIMx == TIM1)
	{
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	}
	else if(TIMx == TIM2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(TIMx == TIM3)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(TIMx == TIM4)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	TIMx->PSC = psc - 1;
	TIMx->ARR = arr -1;
	TIMx->CNT = 0;
	
//	TIMx->CR1 &= ~TIM_CR1_DIR;							// Up Counter
	TIMx->DIER |= TIM_DIER_UIE;								// Enable Update Interrupt Event
	TIMx->CR1 |= TIM_CR1_CEN;									// Enable Counter
	if(TIMx == TIM1)
	{
		NVIC_SetPriority(TIM1_UP_IRQn, Priority);
		NVIC_EnableIRQ(TIM1_UP_IRQn);
	}
	else if(TIMx == TIM2)
	{
		NVIC_SetPriority(TIM2_IRQn, Priority);
		NVIC_EnableIRQ(TIM2_IRQn);
	}
	else if(TIMx == TIM3)
	{
		NVIC_SetPriority(TIM3_IRQn, Priority);
		NVIC_EnableIRQ(TIM3_IRQn);
	}
	else if(TIMx == TIM4)
	{
		NVIC_SetPriority(TIM4_IRQn, Priority);
		NVIC_EnableIRQ(TIM4_IRQn);
	}
}

void TIM1_UP_IRQHandler(void)
{
	TIM1->SR &= ~TIM_SR_UIF;
	TIM_IRQ_Callback(TIM1);
}

void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;
	TIM_IRQ_Callback(TIM2);
}

void TIM3_IRQHandler(void)
{
	TIM3->SR &= ~TIM_SR_UIF;
	TIM_IRQ_Callback(TIM3);
}

void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF;
	TIM_IRQ_Callback(TIM4);
}

/*--------------------------------------------------------------*/
/****************************************************************/
/*------------------- TIMER PWM CONFIGURE ----------------------*/
void TIM_PWM_Config(TIM_TypeDef *TIMx, uint16_t Channel, uint16_t psc, uint16_t arr)
{
	if(TIMx == TIM1)
	{
		RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
		TIM1->BDTR |= TIM_BDTR_MOE;
	}
	else if(TIMx == TIM2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	}
	else if(TIMx == TIM3)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
	}
	else if(TIMx == TIM4)
	{
		RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
	}
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	TIMx->PSC = psc - 1;
	TIMx->ARR = arr - 1;
	TIMx->CNT = 0;
	
	if(Channel == CH1)
	{
		TIMx->CCER |= TIM_CCER_CC1E;
		TIMx->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2;																//PWM CH1 Mode 1
	}
	else if(Channel == CH2)
	{
		TIMx->CCER |= TIM_CCER_CC2E;
		TIMx->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;																//PWM CH2 Mode 1
	}
	else if(Channel == CH3)
	{
		TIMx->CCER |= TIM_CCER_CC3E;
		TIMx->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;																//PWM CH3 Mode 1
	}
	else if(Channel == CH4)
	{
		TIMx->CCER |= TIM_CCER_CC4E;
		TIMx->CCMR2 |= TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_2;																//PWM CH4 Mode 1
	}
	TIMx->CR1 |= TIM_CR1_CEN;
	while(!(TIMx->SR & TIM_SR_UIF));
}

void TIM_PWM_SetDuty(TIM_TypeDef *TIMx, uint16_t Channel, uint16_t duty)
{
	uint16_t arr, ccr;
	arr = TIMx->ARR;
	ccr = duty * arr / 100;
	switch(Channel)
	{
		case CH1:
			TIMx->CCR1 = ccr;
			break;
		case CH2:
			TIMx->CCR2 = ccr;
			break;
		case CH3:
			TIMx->CCR3 = ccr;
			break;
		case CH4:
			TIMx->CCR4 = ccr;
			break;
	}
}
