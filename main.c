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
#include "follow_line.h"

void EXTI0_Config(void);
void EXTI1_Config(void);
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);

/*----------- GLOBAL ARGUMENTS -----------*/
static LCD_TypeDef lcd;
Motor_TypeDef left_motor;
Motor_TypeDef right_motor;
static int8_t error;
uint8_t	miss_way = 0;

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
	EXTI->RTSR |= 1;																		//Enable Rising Edge Trigger for PA0
	EXTI->FTSR |= 1;																		//Enable Falling Edge Trigger for PA0
	NVIC_SetPriority(EXTI0_IRQn, 1);										//Set Priority
	NVIC_EnableIRQ(EXTI0_IRQn);													//Enable Interrupt
	__enable_irq();
}

void EXTI1_Config(void)
{
	//PA0 - EXTI0
	__disable_irq();
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	AFIO->EXTICR[0] &= (uint16_t)~AFIO_EXTICR1_EXTI1;		//Clear EXTI1 bits
	AFIO->EXTICR[0] |= AFIO_EXTICR1_EXTI1_PB;						//Set EXTI0 - PA0
	EXTI->IMR |= 1<<1;																	//Disable the Mask on EXTI0
//	EXTI->RTSR |= 1<<1;																	//Enable Rising Edge Trigger for PB1
	EXTI->FTSR |= 1<<1;																	//Enable Falling Edge Trigger for PB1
	NVIC_SetPriority(EXTI1_IRQn, 2);										//Set Priority
	NVIC_EnableIRQ(EXTI1_IRQn);													//Enable Interrupt
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
uint8_t flag = 0;
void EXTI1_IRQHandler(void)
{
	if(EXTI->PR & (1 << 1))
	{
		flag = 1;
	}
	EXTI->PR |= (1 << 1);
}

void HCSR04_Complete_Callback(HCSR04_TypeDef *hcsr04_x)
{
//	distance = hcsr04_x->distan;
	if(&hcsr04 == hcsr04_x)
	{
		if(hcsr04.distan < 20)
		{
			car_state = CAR_STOP;
			lcd_clear_display(&lcd);
			Delay_ms(1);
			lcd_printf(&lcd, "There's an obstacle");
		}
		else
		{
			car_state = CAR_RUN;
			lcd_clear_display(&lcd);
			Delay_ms(1);
			lcd_printf(&lcd, "Following line");
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
	
	EXTI1_Config();
	GPIO_Config(GPIOB, PIN_1, INPUT, IN_PP);
	
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
	motor_Init(&right_motor, GPIOB, PIN_14, TIM1, CH1);
	motor_Init(&left_motor, GPIOB, PIN_15, TIM1, CH2);
	TIM_PWM_Config(right_motor.timer, right_motor.Tim_Channel, 72, 1000);
	TIM_PWM_Config(left_motor.timer, left_motor.Tim_Channel, 72, 1000);
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
			error = read_sensor_error();
			if(error == 5)
			{
				car_state = CAR_STOP;
				lcd_clear_display(&lcd);
				Delay_ms(1);
				lcd_printf(&lcd, "AT THE STATION");
			}
			else if(error == -5)
			{
				if(miss_way == 0)
				{
					motor_Control(&left_motor, MOTOR_STOP, 0);
					motor_Control(&right_motor, MOTOR_STOP, 0);
				}
				if(miss_way == 1)
				{
					motor_Control(&left_motor, MOTOR_CW, BASE_SPEED);
					motor_Control(&right_motor, MOTOR_CW, 0);
				}
				if(miss_way == 2)
				{
					motor_Control(&left_motor, MOTOR_CW, 0);
					motor_Control(&right_motor, MOTOR_CW, BASE_SPEED);
				}
			}
			else
			{
			car_following_line_handle();
			}
		}
		if(car_state == CAR_STOP)
		{
			motor_Control(&right_motor, MOTOR_STOP, 0);
			motor_Control(&left_motor, MOTOR_STOP, 0);
		}
		if(flag == 1)
		{
			flag = 0;
			motor_Control(&left_motor, MOTOR_CW, BASE_SPEED);
			motor_Control(&right_motor, MOTOR_CW, BASE_SPEED);
			Delay_ms(100);
			car_state = CAR_RUN;
		}
	}
}
