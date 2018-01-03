#include "stm32f030x8.h"i
#include "ADC.h"
#include "UART.h"
#include "Timers.h"

unsigned short Raw_ADC_Data = 0;
unsigned char print_raw_ADC_Data[4] = {0,0,0,0};

void calibrate_ADC() {
		ADC->CCR |= 0x00C00000; //Presc = 4, Fadc = 4MHz, tempsensor and vrefint enabled
	ADC1->CHSELR |= 0x00000001; //Select ADC_IN0 for PA0 analog input
	ADC1->ISR |= 0x00000004;			//End-of conversion interrupt flag clear
	ADC1->IER |= 0x00000004;			//End-of conversion of a channel interrupt enable
	NVIC->ISER[0] |= 0x00001000;     	//Enable ADC interrupt (ADC1_IRQn = 12)
	ADC1->SMPR = 0x00000007; //Sampling rate = every 239.5 clock cycles, one clock cycle is 1/14MHz
//	ADC1->CR &= 0xFFFFFFF0; //Disable ADC for setup config
//	while(ADC1->CR & 1) { //Ensure ADC is disabled
//	
//	}
//	ADC1->CR |= 0x80000000; //Enable ADCAL to calibrate ADC


//	
//	while(ADC1->CR & 0x80000000) { //Wait until ADCAL == 0
//	
//	}
  ADC1->CR |= 0x00000001; //Enable ADC1
	
//	while(!(ADC1->ISR & 0x00000001)) { //Wait until ADC is ready to start conversion
//	
//	}
//	
	ADC1->CR |= 0x00000004; //Start the ADC 
	
	
		
}

void reset_ADC_array() {
	for(int i = 0; i < 4; ++i) {
		print_raw_ADC_Data[i] = 0;
	}

}

void start_ADC() {
	unsigned int count_time = 0;
	unsigned int trigger_time = getmsTicks();
	if((trigger_time - count_time) >= 10) {
			count_time = trigger_time; 
		  ADC1->CR |= 0x00000004;
	}
}

void read_ADC1() {
	Raw_ADC_Data = ADC1->DR;
	if(ADC1->ISR & 0x00000008)  {
		ADC1->ISR |= 0x00000008; //Pointless?
	}
}

void print_ADC_value() {
	sendTexttoUSART1("ADC 12bit Register Reading:  "); 
	 short_to_chararray(Raw_ADC_Data, print_raw_ADC_Data ); 
  sendBuftoUSART1_backwards(print_raw_ADC_Data, 0, 3);	
	sendTexttoUSART1("    "); 


	sendTexttoUSART1("\r");

}
__irq void ADC1_IRQHandler() {
	ADC1->ISR |= 0x00000004; //Reset Interrupt Handler
	read_ADC1();
	//print_ADC_value(); 
	reset_ADC_array(); 

	
}