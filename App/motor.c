#include "motor.h"
#include "timer_driver.h"
#include "gpio_driver.h"

/* SPEED = 0 - 100 */

void motor_Control(Motor_TypeDef *motor, MotorState state_motor, uint8_t speed)
{
	switch(state_motor)
	{
		case MOTOR_STOP:
			GPIO_Write_Pin(motor->io_Port, motor->io_Pin, PIN_RESET);
			TIM_PWM_SetDuty(motor->timer, motor->Tim_Channel, 0);
			break;
		case MOTOR_CW:
			GPIO_Write_Pin(motor->io_Port, motor->io_Pin, PIN_RESET);
			TIM_PWM_SetDuty(motor->timer, motor->Tim_Channel, speed);
			break;
		case MOTOR_CCW:
			GPIO_Write_Pin(motor->io_Port, motor->io_Pin, PIN_SET);
			TIM_PWM_SetDuty(motor->timer, motor->Tim_Channel, 100 - speed);
			break;
	}
}

void motor_Init(Motor_TypeDef *motor, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
								TIM_TypeDef *TIMx, uint16_t tim_channel)
{
	motor->io_Port = GPIOx;
	motor->io_Pin = GPIO_Pin;
	motor->timer = TIMx;
	motor->Tim_Channel = tim_channel;
	motor->motor_state = MOTOR_STOP;
	motor->Speed = 0;
}

