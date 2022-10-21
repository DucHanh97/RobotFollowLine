/*
	*	I2C1: PB6 - SCL
					PB7 - SDA
	* I2C2: PB10 - SCL
					PB11 - SDA
	In I2C mode, configure GPIO PINs to Output Alternate Function Open-Drain max speed 50 MHz
*/


#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H
#include "stm32f10x.h"

#define R	1
#define W	0

void I2C_Config(I2C_TypeDef *I2Cx);
void I2C_Start(I2C_TypeDef *I2Cx);
void I2C_Address(I2C_TypeDef *I2Cx, uint8_t address, uint8_t R_W);
void I2C_Stop(I2C_TypeDef *I2Cx);
void I2C_WriteChr(I2C_TypeDef *I2Cx, uint8_t address, uint8_t data);
void I2C_WriteStr(I2C_TypeDef *I2Cx, uint8_t address, uint8_t *data, uint8_t size);

#endif
