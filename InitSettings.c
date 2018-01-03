/*Include definitions for core and peripheral registers*/
#include "stm32f030x8.h"
#include "InitSettings.h"
#include "I2C.h"
#include "Timers.h"
#include "ADC.h"


extern char gyro_readnwrite_state; 
extern char gyro_write_state; 
extern char gyro_start_address; 

void initial_gyro_write() {
	 
	gyro_readnwrite_state = 0;				//Write mode
	gyro_start_address = 0x07;
	gyro_write();
	Delay_Milliseconds (20);
	gyro_start_address = 0x09;
	gyro_write();
	Delay_Milliseconds (20);
	gyro_start_address = 0x10;
  gyro_write();
	Delay_Milliseconds (20);
	
	gyro_readnwrite_state = 1; 			 //Switch to readMode
  gyro_start_address = 0x07; 
}

void enable_PLL() {
	 //***Enable PLL***//
	 RCC->CR |= 0x00000001; //Turn HSI on
	
	 while((RCC->CR & 0x00000002) != 0x00000002) { //Wait until HSIRDY is ON, thus HSI is turned ON
	 
	 }
	
	 while((RCC->CR & 0x02000000) == 0x02000000) { //Wait until PLLRDY goes low, this ensures PLL is completely turned OFF
	 
	 }
	 RCC->CFGR |= 0x00280002;		//PLLMUL=12, HSI/2 is source of PLL, SYS_CLK=AHB=APB=48MHz
	 RCC->CR |= 0x01000000; //Turn PLL on
	 while((RCC->CR & 0x02000000) != 0x02000000) { //Wait until PLLRDY goes high, this ensures PLL is completely turned ON
	 
	 }

}

void init_settings() {
	

		enable_PLL();
	
		
		//***TIM14 Settings***//
		RCC->APB1ENR |= 0x00000100;             /*TIM14 clock enable*/    
	  TIM14->CCR1 = 0;  	  /*Selecting High time for Duty Cycle*/
    TIM14->PSC = 1; 	  /*TIM14 frequency control enable through prescaler/CNT register*/
	  TIM14->ARR = 48000; 	  /*Selecting Period Time*/
  	TIM14->CCMR1 |= 0x00000068; 	  /*Selecting PWM mode 1 on OC1 register and enabling preload register on OC1*/
	  TIM14->CCER |= 0x00000001; /*Selecting active high polarity on OC1 and enabling output on OC1*/
	  TIM14->EGR |= 0x00000001;  /*Force update generation so that frequency and duty cycle may be appropriately updated*/
		TIM14->CR1 |= 0x00000001; /*Enable counter, select edge aligned mode with up counter*/

		 //***TIM3 Settings***//
		RCC->APB1ENR |= 0x00000002;     // enable clock for TIM3, input clock = PCLK = 48MHz
		TIM3->PSC = 500;                  // set prescaler to 500 to divide 1KHz  by 500, achieving 2Hz
		TIM3->ARR = 48000;               // set auto-reload to make 0.5 second interrupt
		TIM3->CR1 = 0x0014;             // Interrupt on overflow/underflow, preload enabled
		TIM3->DIER = 0x0001;            // Update interrupt enabled
		NVIC->ISER[0] |= 0x00010000;    // enable TIMER3 interrupt (TIM3_IRQn = 16)
		TIM3->CR1 |= 0x00000001;                 // enable timer	

		 //***GPIOA Settings***//
		RCC->AHBENR |= 0x00020000; //Enable clock to GPIOA
		GPIOA->OTYPER &= 0x00000000; //Enable push-pull on all Pins
		GPIOA->MODER = 0x00288003; //Set PA7, PA9 and PA10 in alternate mode, set PA0 in analog input mode
		GPIOA->OSPEEDR |= 0x0000C000; //Ensure maximum speed on PA7
	  GPIOA->AFR[0] |= 0x40000000; //Set PA7 as Af4
    GPIOA->AFR[1] |= 0x00000110;//		PA9 for TX, and PA10 for RX, both in Af1
		
		
		//***GPIOB Settings***//
    RCC->AHBENR |= 0x00040000; //Enable clock to GPIOB
		GPIOB->MODER = 0x00A0000B; 	//Set PB1 in alternate mode and PB0 in analog input mode, PB10 and PB11 in alternate function mode
		GPIOB->OTYPER = 0x00000C00; /*Enable push-pull on all B pins and open drain on PB10 and PB11*/
	  GPIOB->OSPEEDR |= 0x0000000C; /*Ensure maximum speed on PB1*/
    GPIOB->AFR[0] |= 0x00000000; //Set PB1 as alternate function register, PB10 as I2C2_SCL and PB11 as I2C2_SDA
		GPIOB->AFR[1] |= 0x00001100; //Set PB10 and PB11 on AF1
		
				
		//***GPIOC Settings***//
		RCC->AHBENR |= 0x00080000; //Enable clock to GPIOC
		GPIOC->MODER |= 0x00050000; //Enable PC8 and PC9 in output mode
    GPIOC->PUPDR &= 0xFFF0FFFF; //Disable pulls on PC8 and PC9
		
		//***USART1 Settings***//
		RCC->APB2ENR = 0x00004000; //Enable clock to USART1
    USART1->CR1 = 0x0000002C; //Enable RX and TX, enable receiving interrupt
		USART1->CR2 = 0x00000000; // 1 stop bit
		USART1->BRR = 0x000001A1; //Baud rate of ~115200 for fPclk = 48MHz
		NVIC->ISER[0] = 0x08000000; //Enable interrupt on 27 -> USART1 global receiver interrupt
		USART1->CR1 |= 0x00000001; // Enable USART1
		
		//***System Clock Settings***//
		SysTick->LOAD = 6000; //48MHz/8 = 6MHz, interrupts every 1ms
		SysTick->VAL = 6000; //Clear current value
		SysTick->CTRL |= 0x00000003; //Enable counter and enable systick exception request
		
		//***Independent Watch Dog Timer***//
		IWDG->KR = 0x00005555; //Key write, accessing prescale and reload registers
		IWDG->PR = 0x00000006; //Set prescale divider to 256 
		IWDG->RLR = 0x00000FFF; // Watchdog counts down from 4077ms,32MHz CK_INT clcok
		//IWDG->KR = 0x0000CCCC; //Start watchdog timer
		
		
		
		//***I2C2 Settings***//
		RCC->APB1ENR |= 0x00400000; //Enable clock to I2C2
		I2C2->CR1 = 0x00000006; //Enable RX and TX interrupts for I2C
		I2C2->TIMINGR = 0xF0000E0E; //tPresc = 0.33us,  tSCLDEL = tPresc, tSDADEL = tPresc, tSCLH = tSCLL= 5us. (10us period causes 100kHz)
 		NVIC->ISER[0] |= 0x01000000; //Enable global interrupt for I2C2 on line 24
		I2C2->CR1 |= 0x00000001; //Enable I2C2	

		
		//***ADC1 Settings***//
		RCC->APB2ENR |= 0x00000200; //Enable clock to ADC, input clock = HSI = 14MHz, seperate HSI
		calibrate_ADC(); 
		

		
	
		
		
		
		
		initial_gyro_write(); 
	
	
}