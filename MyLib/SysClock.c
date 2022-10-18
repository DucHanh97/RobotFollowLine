#include "SysClock.h"
#include "stm32f10x.h"

void SysClockConfig(void)
{
	/*1. Enable HSE and wait for the HSE to become ready*/
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY));
	
	/*2. Set the POWER ENABLE CLOCK*/
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;

	/*3. Configure the FLASH PREFETCH and the LATENCY Related Settings*/
	FLASH->ACR |= FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY_2;
	while(!(FLASH->ACR & FLASH_ACR_PRFTBS));

	/*4. Configure the PRESCALARS HCLK, PCLK1, PCLK2*/
	RCC->CFGR |= RCC_CFGR_PLLMULL9 | RCC_CFGR_HPRE_DIV1 | RCC_CFGR_PPRE1_DIV2 | RCC_CFGR_PPRE2_DIV1;

	/*5. Configure the MAIN PLL*/
	RCC->CFGR |= RCC_CFGR_PLLXTPRE_HSE | RCC_CFGR_PLLSRC;

	/*6. Enable the PLL and wait for it to become ready*/
	RCC->CR |= RCC_CR_PLLON;
	while(!(RCC->CR & RCC_CR_PLLRDY));

	/*7. Select the Clock Source and wait for it to be set*/
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while(!(RCC->CFGR &RCC_CFGR_SWS_PLL));

	/*8. Configure the SysTick period interrupt 1ms*/
	SystemInit();
	SystemCoreClockUpdate();
	SysTick_Config(SystemCoreClock / 1000UL);
}

volatile uint32_t Tick;

void SysTick_Handler(void)
{
	Tick++;
}

uint32_t Get_Tick(void)
{
	return Tick;
}

void Delay_ms(uint32_t ms)
{
	uint32_t Current_Tick = Get_Tick();
	while(Get_Tick() - Current_Tick < ms);
}
