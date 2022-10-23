/*****************PINs have used:********************
	*	HC-SR04:	PA0 - echo, PA1 - trigger	(TIM2)
	* LCD16x2:	PB6 - SCL,	PB7 - SDA
	*	MOTOR1:		PA8 - PWM,	PB14 - DIR		(TIM1 - CH1)
	*	MOTOR2:		PA9 - PWM,	PB15 - DIR		(TIM1 - CH2)
	*	SENSORS:	PA2-7
	
	
****************************************************/

#include "stm32f10x.h"
#include "SysClock.h"
#include "gpio_driver.h"
#include "timer_driver.h"
#include "i2c_driver.h"

#include "hcsr04.h"
#include "lcd_i2c.h"
#include "motor.h"
void EXTI0_Config(void);
void EXTI0_IRQHandler(void);

/*----------- GLOBAL ARGUMENTS -----------*/
static LCD_TypeDef lcd;
static Motor_TypeDef motor01;
static Motor_TypeDef motor02;

typedef enum
{
	CAR_STOP,
	CAR_RUN
}CAR_State;

static CAR_State car_state = CAR_STOP;
/*---------------------- HCSR04 ------------------------*/
static HCSR04_TypeDef hcsr04;

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

void EXTI0_IRQHandler(void)
{
	if(EXTI->PR & (1 << 0))
	{
		EXTI_HCSR04_Callback(&hcsr04);
	}
	EXTI->PR |= (1 << 0);
}

//volatile float distance;

void HCSR04_Complete_Callback(HCSR04_TypeDef *hcsr04_x)
{
//	distance = hcsr04_x->distan;
	if(&hcsr04 == hcsr04_x)
	{
		lcd_clear_display(&lcd);
		Delay_ms(1);
		lcd_printf(&lcd, "%.2f", (double)hcsr04.distan);
		if(hcsr04.distan < 20)
		{
			car_state = CAR_STOP;
		}
		else
		{
			car_state = CAR_RUN;
		}
	}
}
/*------------------^^^^^ HCSR04 ^^^^^--------------------*/

static uint32_t n = 0;
void TIM_IRQ_Callback(TIM_TypeDef *TIMx)
{
	if(TIMx == TIM3)
	{
		n++;
		if(n >= 1000)
		{
			n = 0;
			GPIO_Toggle_Pin(GPIOC, PIN_13);
		}
	}
}
/*--------------------------------------------------------*/


int main(void)
{
	SysClockConfig();
	
	/*---------------- HCSR04 Init ---------------*/
	HCSR04_Init(&hcsr04, TIM2, GPIOA, PIN_1, PIN_0);
	TIM_Config(TIM2, 72, MAX_ARR_VALUE);
	EXTI0_Config();
	GPIO_Config(GPIOA, PIN_0, INPUT, IN_PP);
	GPIO_Config(GPIOA, PIN_1, OUT50, O_GP_PP);
	/*--------------------------------------------*/
	
	/*------------------ LCD Init ----------------*/
	GPIO_Config(GPIOB, PIN_6, OUT50, O_AF_OD);
	GPIO_Config(GPIOB, PIN_7, OUT50, O_AF_OD);
	I2C_Config(I2C1);
	lcd_init(&lcd, I2C1, LCD_ADDR_DEFAULT);
	/*--------------------------------------------*/	
	
	/*---------------- Motor Init ----------------*/
	motor_Init(&motor01, GPIOB, PIN_14, TIM1, CH1);
	motor_Init(&motor02, GPIOB, PIN_15, TIM1, CH2);
	TIM_PWM_Config(motor01.timer, motor01.Tim_Channel, 72, 1000);
	TIM_PWM_Config(motor02.timer, motor02.Tim_Channel, 72, 1000);
	GPIO_Config(GPIOA, PIN_8, OUT50, O_AF_PP);
	GPIO_Config(GPIOB, PIN_14, OUT50, O_GP_PP);
	GPIO_Config(GPIOA, PIN_9, OUT50, O_AF_PP);
	GPIO_Config(GPIOB, PIN_15, OUT50, O_GP_PP);
	/*--------------------------------------------*/
	
	/*-------------- TIMER_IT Init ---------------*/
	GPIO_Config(GPIOC, PIN_13, OUT2, O_GP_PP);
	TIM_IT_Config(TIM3, 72, 999, 1);
	/*--------------------------------------------*/
	
	
	uint32_t time_now = 0;
	
	
	while(1)
	{
		if((Get_Tick() - time_now) > 200)
		{
			HCSR04_Start(&hcsr04);
			time_now = Get_Tick();
		}
		HCSR04_Handler(&hcsr04);
		if(car_state == CAR_RUN)
		{
			motor_Control(&motor01, MOTOR_CW, 25);
			motor_Control(&motor02, MOTOR_CW, 25);
		}
		if(car_state == CAR_STOP)
		{
			motor_Control(&motor01, MOTOR_STOP, 25);
			motor_Control(&motor02, MOTOR_STOP, 25);
		}
	}
}
