#ifndef SYSCLOCK_H
#define SYSCLOCK_H
#include <stdint.h>
void SysClockConfig(void);
void SysTick_Handler(void);
uint32_t Get_Tick(void);
void Delay_ms(uint32_t ms);
#endif
