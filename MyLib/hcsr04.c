#include "hcsr04.h"
#include "stm32f10x.h"
#include "timer_driver.h"
#include "gpio_driver.h"

void EXTI0_Config(void)
{
	//PA0 - EXTI0
	__disable_irq();
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->EXTICR[0] &= (uint16_t)~AFIO_EXTICR1_EXTI0;		//Clear EXTI0 bits
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI0_PA;						//Set EXTI0 - PA0
	EXTI->IMR |= 1;																			//Disable the Mask on EXTI0
	EXTI->RTSR |= 1;																		//Enable Rising Edge Trigger for PA1
	EXTI->FTSR |= 1;																		//Enable Falling Edge Trigger for PA1
	NVIC_SetPriority(EXTI0_IRQn, 1);										//Set Priority
	NVIC_EnableIRQ(EXTI0_IRQn);													//Enable Interrupt
	__enable_irq();
}

void TIM2_Delay_us(uint16_t us)
{
	TIM2->CNT = 0;
	TIM2->CR1 |= TIM_CR1_CEN;
	while(TIM2->CNT <us);
	TIM2->CR1 &= ~TIM_CR1_CEN;
}

static HCSR04_State hc04_state = IDLE_STATE;

void EXTI0_IRQHandler(void)
{
	if(EXTI->PR & (1 << 0))
	{
		switch(hc04_state)
		{
			case WAIT_RISING_STATE:
				if((GPIOA->IDR & GPIO_IDR_IDR0) == 1)
				{
					TIM2->CNT = 0;
					hc04_state = WAIT_FALLING_STATE;
					TIM2->CR1 |= TIM_CR1_CEN;
				}
				else
				{
					hc04_state = IDLE_STATE;
				}
				break;
			case WAIT_FALLING_STATE:
				if((GPIOA->IDR & GPIO_IDR_IDR0) == 0)
				{
					TIM2->CR1 &= ~TIM_CR1_CEN;
					hc04_state = COMPLETE_STATE;
				}
				else
				{
					hc04_state = IDLE_STATE;
				}
				break;
			case IDLE_STATE:
				break;
			case COMPLETE_STATE:
				break;
		}
		EXTI->PR |= (1 << 0);
	}
}

void HCSR04_Start(void)
{
	GPIOA->ODR |= 1 << 1;
	TIM2_Delay_us(20);
	GPIOA->ODR &= (uint16_t)~(1 << 1);
	
	hc04_state = WAIT_RISING_STATE;
}

void HCSR04_Handler(void)
{
	if(hc04_state == COMPLETE_STATE)
	{
		float distan = (float)(0.017*TIM2->CNT);
		HCSR04_Complete_Callback(distan);
		hc04_state = IDLE_STATE;
	}
}

void HCSR04_Init(void)
{
	TIM_Config(TIM2, 72, MAX_ARR_VALUE);
	EXTI0_Config();
	GPIO_Config(GPIOA, PIN_0, INPUT, IN_PP);
	GPIO_Config(GPIOA, PIN_1, OUT50, O_GP_PP);
}
