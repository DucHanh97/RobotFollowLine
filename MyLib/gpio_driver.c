#include "gpio_driver.h"

void GPIO_Config(GPIO_TypeDef *GPIOx, uint16_t PIN, uint16_t IN_OUT, uint16_t IO_Type)
{
	uint16_t tPin = PIN;
	
	if(GPIOx == GPIOA)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	}
	else if(GPIOx == GPIOB)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	}
	else if(GPIOx == GPIOC)
	{
		RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	}
	
	if(PIN < 8)
	{
		GPIOx->CRL &= (uint32_t)~(0xF << (PIN * 4));
		GPIOx->CRL |= (uint32_t)((IN_OUT << (PIN * 4)) | (IO_Type << (PIN*4 + 2)));
	}
	else
	{
		tPin -= 8;
		GPIOx->CRH &= (uint32_t)~(0xF << (tPin * 4));
		GPIOx->CRH |= (uint32_t)((IN_OUT << (tPin * 4)) | (IO_Type << (tPin*4 + 2)));
	}
}

void GPIO_Write_Pin(GPIO_TypeDef *GPIOx, uint16_t PIN, uint8_t PIN_State)
{
	if(PIN_State == PIN_SET)
	{
		GPIOx->ODR |= (1<<PIN);
	}
	else if(PIN_State == PIN_RESET)
	{
		GPIOx->BRR |= (1<<PIN);
	}
}

uint8_t GPIO_Read_Pin(GPIO_TypeDef *GPIOx, uint16_t PIN)
{
	uint8_t PIN_State;
	PIN_State = (uint8_t)(GPIOx->IDR >> PIN) & 1;
	return PIN_State;
}

void GPIO_Toggle_Pin(GPIO_TypeDef *GPIOx, uint16_t PIN)
{
	GPIOx->ODR ^= (1<<PIN);
}
