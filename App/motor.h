#ifndef MOTOR_H
#define MOTOR_H
#include "stm32f10x.h"

typedef enum
{
	MOTOR_STOP,
	MOTOR_CW,						// Same clockwise
	MOTOR_CCW						// Counter-clockwise
}MotorState;

typedef struct
{
	GPIO_TypeDef	*io_Port;
	uint16_t			io_Pin;
	TIM_TypeDef		*timer;
	uint16_t			Tim_Channel;
	MotorState		motor_state;
	uint8_t				Speed;
}Motor_TypeDef;

void motor_Control(Motor_TypeDef *motor, MotorState state_motor, uint8_t speed);
void motor_Init(Motor_TypeDef *motor, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin,
								TIM_TypeDef *TIMx, uint16_t tim_channel);

#endif
