#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H
#include "stm32f10x.h"

#define PIN_SET			1
#define PIN_RESET		0

#define INPUT				0
#define OUT10				1
#define OUT2				2
#define OUT50				3

#define IN_AN				0
#define IN_FT				1
#define IN_PP				2

#define O_GP_PP			0
#define O_GP_OD			1
#define O_AF_PP			2
#define O_AF_OD			3

#define PIN_0				0
#define PIN_1				1
#define PIN_2				2
#define PIN_3				3
#define PIN_4				4
#define PIN_5				5
#define PIN_6				6
#define PIN_7				7
#define PIN_8				8
#define PIN_9				9
#define PIN_10			10
#define PIN_11			11
#define PIN_12			12
#define PIN_13			13
#define PIN_14			14
#define PIN_15			15

void GPIO_Config(GPIO_TypeDef *GPIOx, uint16_t PIN, uint16_t IN_OUT, uint16_t IO_Type);
void GPIO_Write_Pin(GPIO_TypeDef *GPIOx, uint16_t PIN, uint8_t PIN_State);
uint8_t GPIO_Read_Pin(GPIO_TypeDef *GPIOx, uint16_t PIN);
void GPIO_Toggle_Pin(GPIO_TypeDef *GPIOx, uint16_t PIN);

#endif
