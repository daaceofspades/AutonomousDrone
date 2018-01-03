/*Include definitions for core and peripheral registers*/
#include "stm32f030x8.h"
#include "InitSettings.h"
#include "PWM.h"
#include "UART.h"
#include <stdio.h>
#include "ADC.h"
#include "I2C.h"









int main(void) {
 
	  init_settings(); 
		sendTexttoUSART1("Start\n\r");
    

		while(1) {
		 blink_LED(2400000); //Used to indicate whether firmware is running
		 normal_pulse(50); 
		 start_ADC(); //Activates ADC Read every 10ms
			display_measures(); //Streams values of gyro/accel data
	   //cyclicPWM(24000); //Changes brightness dynamically
			
		}
	

    return 0;
}
