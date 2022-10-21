#include "hcsr04.h"
#include "gpio_driver.h"

void TIM_Delay_us(HCSR04_TypeDef *hcsr04_x, uint16_t us)
{
	hcsr04_x->hc04_tim->CNT = 0;
	hcsr04_x->hc04_tim->CR1 |= TIM_CR1_CEN;
	while(hcsr04_x->hc04_tim->CNT < us);
	hcsr04_x->hc04_tim->CR1 &= ~TIM_CR1_CEN;
}

void EXTI_HCSR04_Callback(HCSR04_TypeDef *hcsr04_x)
{

	{
		switch(hcsr04_x->hc04_state)
		{
			case WAIT_RISING_STATE:
				if(GPIO_Read_Pin(hcsr04_x->hc04_port, hcsr04_x->hc04_echo_pin) == 1)
				{
					hcsr04_x->hc04_tim->CNT = 0;
					hcsr04_x->hc04_state = WAIT_FALLING_STATE;
					hcsr04_x->hc04_tim->CR1 |= TIM_CR1_CEN;
				}
				else
				{
					hcsr04_x->hc04_state = IDLE_STATE;
				}
				break;
			case WAIT_FALLING_STATE:
				if(GPIO_Read_Pin(hcsr04_x->hc04_port, hcsr04_x->hc04_echo_pin) == 0)
				{
					hcsr04_x->hc04_tim->CR1 &= ~TIM_CR1_CEN;
					hcsr04_x->hc04_state = COMPLETE_STATE;
				}
				else
				{
					hcsr04_x->hc04_state = IDLE_STATE;
				}
				break;
			case IDLE_STATE:
				break;
			case COMPLETE_STATE:
				break;
		}

	}
}

void HCSR04_Start(HCSR04_TypeDef *hcsr04_x)
{
	GPIO_Write_Pin(hcsr04_x->hc04_port, hcsr04_x->hc04_trig_pin, PIN_SET);	
	TIM_Delay_us(hcsr04_x, 20);
	GPIO_Write_Pin(hcsr04_x->hc04_port, hcsr04_x->hc04_trig_pin, PIN_RESET);

	hcsr04_x->hc04_state = WAIT_RISING_STATE;
}

void HCSR04_Handler(HCSR04_TypeDef *hcsr04_x)
{
	if(hcsr04_x->hc04_state == COMPLETE_STATE)
	{
		hcsr04_x->distan = (float)(0.017*(hcsr04_x->hc04_tim->CNT));
		HCSR04_Complete_Callback(hcsr04_x);
		hcsr04_x->hc04_state = IDLE_STATE;
	}
}

void HCSR04_Init(HCSR04_TypeDef *hcsr04_x, TIM_TypeDef *TIMx, GPIO_TypeDef *GPIOx, uint16_t trig_pin, uint16_t echo_pin)
{
	hcsr04_x->hc04_tim = TIMx;
	hcsr04_x->hc04_port = GPIOx;
	hcsr04_x->hc04_trig_pin = trig_pin;
	hcsr04_x->hc04_echo_pin = echo_pin;
	hcsr04_x->hc04_state = IDLE_STATE;
	hcsr04_x->distan = 0;
}
