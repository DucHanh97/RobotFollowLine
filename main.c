#include "stm32f10x.h"
#include "SysClock.h"
#include "gpio_driver.h"
#include "timer_driver.h"
#include "hcsr04.h"
#include "motor.h"

volatile float distance;

void HCSR04_Complete_Callback(float distan)
{
	distance = distan;
}

static uint32_t n = 0;
void TIM_IRQ_Callback(TIM_TypeDef *TIMx)
{
	if(TIMx == TIM1)
	{
		n++;
		if(n >= 1000)
		{
			n = 0;
			GPIO_Toggle_Pin(GPIOC, PIN_13);
		}
	}
}

Motor_TypeDef motor01;

int main(void)
{
	SysClockConfig();
	HCSR04_Init();
	
	GPIO_Config(GPIOC, PIN_13, OUT2, O_GP_PP);
	TIM_IT_Config(TIM1, 72, 999, 1);
	
	/*---------------- Motor Init ----------------*/
	motor_Init(&motor01, GPIOB, PIN_14, TIM1, CH1);
	TIM_PWM_Config(TIM1, CH1, 72, 1000);
	GPIO_Config(GPIOA, PIN_8, OUT50, O_AF_PP);
	GPIO_Config(GPIOB, PIN_14, OUT50, O_GP_PP);
	/*-------------------------------------------*/
	
	motor_Control(&motor01, MOTOR_CW, 50);
	
	uint32_t time_now = 0;
	
	while(1)
	{
		if((Get_Tick() - time_now) > 500)
		{
			HCSR04_Start();
			time_now = Get_Tick();
		}
		HCSR04_Handler();
	}
}
