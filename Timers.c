#include "InitSettings.h"
#include "stm32f030x8.h"
#include "I2C.h"
#include "UART.h"

unsigned int msTicks = 0;							// counts 1ms timeTicks 


void SysTick_Handler() { //SysTick_Handler ISR is called every 1 ms, counter is updated every 1ms
	 ++msTicks; 
}

unsigned int getmsTicks() {

	return msTicks;
}



__irq void TIM3_IRQHandler() {

	TIM3->SR &= 0xFFFFFFFE; //Disable interrupt bit
	gyro_read(); //Send read to gyro

	
}

void Delay_Milliseconds (unsigned short milliseconds)
{	
	unsigned int referenceTime;
	unsigned int comparisonTime;
	
	referenceTime = getmsTicks(); //Time to compare with
	comparisonTime = referenceTime; //Initialize difference to 0
	while ((comparisonTime - referenceTime) < milliseconds) //Once difference surpasses, input milliseconds, function has effectively delayed set milliseconds
	{
			//IWDG->KR = 0x0000AAAA;							//Watchdog refresh
			comparisonTime = getmsTicks(); //Update comparison time
	}
}		
