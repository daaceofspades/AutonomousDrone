/*Include definitions for core and peripheral registers*/
#include "stm32f030x8.h"
#include "PWM.h"
#include "Timers.h"


void delay (int b) {
	for(int a = 0; a < b; a++){

  }
}


void blink_LED(int period) {

  
				unsigned static int LED_STATE = 0;
				unsigned static int curr_time = 0;
	      unsigned int elapsed_time = getmsTicks(); 
				int BSRR_VAL;
   
				if((elapsed_time - curr_time) >= 100) {
						LED_STATE ^= 1; 
						
					  if(LED_STATE) { //Is blue
								BSRR_VAL = (1<<8); 
								GPIOC->BSRR = BSRR_VAL;
							  GPIOC->BRR = (1<<9); //Disable green
						}
						else {
								BSRR_VAL = (1<<9); 
								GPIOC->BSRR = BSRR_VAL;
							  GPIOC->BRR = (1<<8); //Disable green
						}
						curr_time = elapsed_time; 
				}
//				BSRR_VAL = (1<<8);
//        /* Set PC8*/
//        GPIOC->BSRR = BSRR_VAL;
//			  /* Reset PC8*/
//			  GPIOC->BRR = BSRR_VAL; 
//				BSRR_VAL = (1<<9); 
//        /* Set PC9*/
//        GPIOC->BSRR = BSRR_VAL;
//			  /* Reset PC9*/
//			  GPIOC->BRR = BSRR_VAL; 
//    

}


void cyclic_PWM(int dutyCycle) {
	
	
		
		int16_t is_decreasing = 1; 
		
		 while(dutyCycle > 150 && is_decreasing) {
				dutyCycle = dutyCycle - 50;
				TIM14->CCR1 = dutyCycle;
				delay(5000); 
		 }
		 is_decreasing = !is_decreasing; 
		 while(dutyCycle < 46000 && !is_decreasing) {
				dutyCycle = dutyCycle + 50;
				TIM14->CCR1 = dutyCycle;
				delay(5000); 
		 }
		

}

void normal_pulse(int dutyCycle) {
	TIM14->CCR1 = (dutyCycle/100)*48000;  


}