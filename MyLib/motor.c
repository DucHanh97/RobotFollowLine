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
			GPIO_Write_Pin(motor->io_Port, motor->io_Pin, PIN_SET);
			TIM_PWM_SetDuty(motor->timer, motor->Tim_Channel, speed);
			break;
		case MOTOR_CCW:
			GPIO_Write_Pin(motor->io_Port, motor->io_Pin, PIN_SET);
			TIM_PWM_SetDuty(motor->timer, motor->Tim_Channel, 100 - speed);
			break;
	}
}

void motor_Init(Motor_TypeDef *Motor_x, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
								TIM_TypeDef *TIMx, uint16_t tim_channel)
{
	Motor_x->io_Port = GPIOx;
	Motor_x->io_Pin = GPIO_Pin;
	Motor_x->timer = TIMx;
	Motor_x->Tim_Channel = tim_channel;
	Motor_x->motor_state = MOTOR_STOP;
	Motor_x->Speed = 0;
}

