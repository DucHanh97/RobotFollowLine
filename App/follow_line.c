#include "follow_line.h"
#include "gpio_driver.h"
#include "timer_driver.h"
#include "motor.h"

/*----------------- global arguments -----------------*/
extern Motor_TypeDef left_motor;
extern Motor_TypeDef right_motor;
extern uint8_t	miss_way;

static int8_t		P = 0;
static int16_t	I = 0;
static int8_t		D = 0;
static int8_t		Pre_error = 0;
/*----------------------------------------------------*/

/*------------ */
int8_t read_sensor_error(void)
{
	/* 5 sensors with following states
		1 1 1 1 1 = 31: error = 5
		0 0 0 0 1	= 1: error = 4
		0 0 0 1 1 = 3: error = 3
		0 0 0 1 0 = 2: error = 2
		0 0 1 1 0 = 6: error = 1			- right
		0 0 1 0 0 = 4: error = 0			- center
		0 1 1 0 0 = 12: error = -1		- left
		0 1 0 0 0 = 8: error = -2
		1 1 0 0 0 = 24: error = -3
		1 0 0 0 0 = 16: error = -4
		0 0 0 0 0 = 0: error = -5
	*/
	int8_t error = 0;
	uint8_t sensor[5];

	sensor[0] = GPIO_Read_Pin(GPIOA, PIN_2);
	sensor[1] = GPIO_Read_Pin(GPIOA, PIN_3);
	sensor[2] = GPIO_Read_Pin(GPIOA, PIN_4);
	sensor[3] = GPIO_Read_Pin(GPIOA, PIN_5);
	sensor[4] = GPIO_Read_Pin(GPIOA, PIN_6);
	
	if((sensor[0] == 1) & (sensor[1] == 1) & (sensor[2] == 1) & (sensor[3] == 1) & (sensor[4] == 1))
	{
		error = 5;
	}
	else if((sensor[0] == 0) & (sensor[1] == 0) & (sensor[2] == 0) & (sensor[3] == 0) & (sensor[4] == 1))
	{
		error = 4;
		miss_way = 1;
	}
	else if((sensor[0] == 0) & (sensor[1] == 0) & (sensor[2] == 0) & (sensor[3] == 1) & (sensor[4] == 1))
	{
		error = 3;
		miss_way = 1;
	}
	else if((sensor[0] == 0) & (sensor[1] == 0) & (sensor[2] == 0) & (sensor[3] == 1) & (sensor[4] == 0))
	{
		error = 2;
	}
	else if((sensor[0] == 0) & (sensor[1] == 0) & (sensor[2] == 1) & (sensor[3] == 1) & (sensor[4] == 0))
	{
		error = 1;
	}
	else if((sensor[0] == 0) & (sensor[1] == 0) & (sensor[2] == 1) & (sensor[3] == 0) & (sensor[4] == 0))
	{
		error = 0;
	}
	else if((sensor[0] == 0) & (sensor[1] == 1) & (sensor[2] == 1) & (sensor[3] == 0) & (sensor[4] == 0))
	{
		error = -1;
	}
	else if((sensor[0] == 0) & (sensor[1] == 1) & (sensor[2] == 0) & (sensor[3] == 0) & (sensor[4] == 0))
	{
		error = -2;
	}
	else if((sensor[0] == 1) & (sensor[1] == 1) & (sensor[2] == 0) & (sensor[3] == 0) & (sensor[4] == 0))
	{
		error = -3;
		miss_way = 2;
	}
	else if((sensor[0] == 1) & (sensor[1] == 0) & (sensor[2] == 0) & (sensor[3] == 0) & (sensor[4] == 0))
	{
		error = -4;
		miss_way = 2;
	}
	else if((sensor[0] == 0) & (sensor[1] == 0) & (sensor[2] == 0) & (sensor[3] == 0) & (sensor[4] == 0))
	{
		error = -5;
	}
	
	return error;
}
/*----------------------------------------------------------------------------------------*/

/*------------------------------------- PID Calculate ------------------------------------*/
int16_t PID_value_calculate(int8_t error)
{
	P = error;
	I += error;
	D = error - Pre_error;
	Pre_error = error;
	int16_t PID_value = (int16_t)(Kp * P + Ki * I + Kd * D);
	return PID_value;
}

/*----------------------------------------------------------------------------------------*/

/*---------------------------- Following line handle -------------------------------------*/
void car_following_line_handle(void)
{
	int8_t delta_pid_speed = (int8_t)(PID_value_calculate(read_sensor_error()) / 10);
	int8_t right_speed = BASE_SPEED - delta_pid_speed;
	int8_t left_speed = BASE_SPEED + delta_pid_speed;
	if(right_speed < 0)
	{
		right_speed = 0;
		left_speed = BASE_SPEED;
	}
	if(left_speed < 0)
	{
		left_speed = 0;
		right_speed = BASE_SPEED;
	}
	motor_Control(&left_motor, MOTOR_CW, (uint8_t)left_speed);
	motor_Control(&right_motor, MOTOR_CW, (uint8_t)right_speed);
}
/*----------------------------------------------------------------------------------------*/
