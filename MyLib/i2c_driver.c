#include "i2c_driver.h" 

void I2C_Config(I2C_TypeDef *I2Cx)
{
	/************** STEPS FOLLOWED ***************
	1.	Enable the I2C Clock and GPIO Clock
	2.	Configure the I2C PINs for Alternate Functions
		a) Select Alternater Funtion in Moder Register
		b) Select Open Drain Output
		c) Select High SPEED for the PINs
		d) Select Pull-up for both the Pins
		e) Configure the Alternate Function in AFR Register
	3.	Reset the I2C
	4.	Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
	5.	Configure the clock control registers (CCR)
	6.	Configure the rise time register
	7.	Program the I2C_CR1 register to enable the peripheral
	*********************************************/
	
	//Enable the I2C Clock and GPIO Clock
	if(I2Cx == I2C1)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	}
	else if(I2Cx == I2C2)
	{
		RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
	}
//	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	
	//Configure the I2C PINs for Alternate Funtions
//	GPIOB->CRL |= GPIO_CRL_MODE6 | GPIO_CRL_MODE7;			//PB6 - Alternate Funtion Open-Drain max speed 50MHz
//	GPIOB->CRL |= GPIO_CRL_CNF6 | GPIO_CRL_CNF7;				//PB7 - Alternate Funtion Open-Drain max speed 50MHz
	
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;									//Enable Alternate Function Clock
	
	//Reset the I2C
	I2Cx->CR1 |= I2C_CR1_SWRST;
	I2Cx->CR1 &= ~I2C_CR1_SWRST;
	
	//Program the peripheral input clock in I2C_CR2 Register in order to generate correct timings
	I2Cx->CR2 |= (36<<0);																//PCLK1 PREQUENCY in MHz
	
	//Configure the clock control registers
	I2Cx->CCR = 180<<0;																	//CCR = T(high)/T(pclk1)= [Tr(SCL) + Tw(SCLH)]/T(pclk1) = (1000 + 4000)[ns]/(1/36[MHz]*1000)[ns] = 180 (speed mode)
	
	//Configure the rise time register
	I2Cx->TRISE = 37;																		//TRISE = (Tr(SCL)/T(PCLK1))+1 = 1000 [ns]/(1/36*1000) [ns] + 1 = 37 (check PDF again)
	
	//Program the I2C_CR1 register to enable the peripheral
	I2Cx->CR1 |= I2C_CR1_PE;														//Enable I2C
}

void I2C_Start(I2C_TypeDef *I2Cx)
{
	/*********** STEPS FOLLOWED *************
	1.	Send the START condition
	2.	Wait for the SB (bit 0 in SR) to set. This indicates that the start condition is generate
	****************************************/
	
	I2Cx->CR1 |= I2C_CR1_START;													//Generate START
	while(!(I2Cx->SR1 & I2C_SR1_SB));										//Wait for SB bit to set
}

void I2C_Address(I2C_TypeDef *I2Cx, uint8_t address, uint8_t R_W)
{
	/*********** STEPS FOLLOWED *************
	1.	Send the Slave Address to the DR Register
	2.	Wait for the ADDR (bit 1 in SR1) to set. This indicates the end of address transmisstion
	3.	Clear the ADDR by reading the SR1 and SR2
	****************************************/
	int temp;
	I2Cx->DR = (uint8_t)((address << 1) | R_W);																	//Senf the address
	while(!(I2Cx->SR1 & I2C_SR1_ADDR));									//Wait for ADDR bit to set
	while(I2Cx->SR1 & I2C_SR1_ADDR)											//Read SR! and SR2 to clear the ADDR bit
	{
		temp = I2Cx->SR1;
		temp = I2Cx->SR2;
		if(!(I2Cx->SR1 & I2C_SR1_ADDR))
		{
			break;
		}
	}
//	temp = I2Cx->SR1 | I2Cx->SR2;								//Read SR! and SR2 to clear the ADDR bit
}

void I2C_Stop(I2C_TypeDef *I2Cx)
{
	I2Cx->CR1 |= I2C_CR1_STOP;													//Stop I2C
}

void I2C_WriteChr(I2C_TypeDef *I2Cx, uint8_t address, uint8_t data)
{
	/*********** STEPS FOLLOWED *************
	1.	Wait for the TXE (bit 7 in SR1) to set. This indicate that the DR is empty
	2.	Send the DATA to the DR Register
	3.	Wait for the BTF (bit 2 in SR1) to set. This indicate the end of LAST DATA transmission
	****************************************/
	I2C_Start(I2Cx);
	I2C_Address(I2Cx, address, W);
	
	while(!(I2Cx->SR1 & I2C_SR1_TXE));									//Wait for TXE bit to set
	I2Cx->DR = data;
	while(!(I2Cx->SR1 & I2C_SR1_BTF));									//Wait for BTF bit to set
	
	I2C_Stop(I2Cx);
}

void I2C_WriteStr(I2C_TypeDef *I2Cx, uint8_t address, uint8_t *data, uint8_t size)
{
	/*********** STEPS FOLLOWED *************
	1.	Wait for the TXE (bit 7 in SR1) to set. This indicate that the DR is empty
	2.	Keep Sending DATA to the DR Register after performing the check if the TXE bit is set
	3.	Once the DATA transfer is complete, wait for the BTF (bit 2 in SR1) to set. This indicates the end of LAST DATA transmission.
	****************************************/
	I2C_Start(I2Cx);
	I2C_Address(I2Cx, address, W);
	
	while(!(I2Cx->SR1 & I2C_SR1_TXE));									//Wait for TXE bit to set
	while(size)
	{
		while(!(I2Cx->SR1 & I2C_SR1_TXE));								//Wait for TXE bit to set
		I2Cx->DR = (volatile uint8_t)*data++;							//Send data
		size--;
	}
	while(!(I2Cx->SR1 & I2C_SR1_BTF));									//Wait for BTF bit to set
	
	I2C_Stop(I2Cx);
}
