#include "UART.h"
#include "stm32f030x8.h"
#include "string.h"



unsigned char receive_char;
const int ASCII_OFFSET = 48; 

void sendTexttoUSART1(char text[]) {

	unsigned int char_index = 0;
	while(char_index < strlen(text)){
	   while(!(USART1->ISR & 0x00000080)) { //While transmit data register is not empty, wait
		 
		 }
		 USART1->TDR = text[char_index]; 
		 ++char_index;
	}


}

unsigned short hextoascii (unsigned char hexnum) {
	unsigned short temphta1;
	unsigned short temphta2;

	temphta2 = 0;

	temphta1 = hexnum;
	temphta1 &=0x000F;
	if (temphta1 < 10) temphta1 += 0x0030;
	else temphta1 += 0x0037;
	temphta2 += temphta1;

	temphta1 = hexnum;
	temphta1 &=0x00F0;
	temphta1 >>= 4;
	if (temphta1 < 10) temphta1 += 0x0030;
	else temphta1 += 0x0037;
	temphta1 <<= 8;
	temphta2 += temphta1;
	return (temphta2);
}

unsigned char hex_to_ascii(unsigned char hex_number) {

	if(hex_number < 10) {
		return (hex_number |= 0x00000030); 
	}
	else {
	
		return hextoascii(hex_number);
	}

}

unsigned short get_number_of_digits(int number) {
	 if(number == 0) {
		 return 1;
	 }
	 int num_digits = 0;
	 while(number > 0) {
		 number /= 10;
		 ++num_digits;
	 }
	 
	 return num_digits; 
		
}


void short_to_chararray(unsigned short number, unsigned char* char_array) {
		int num_digits = get_number_of_digits(number);
		for(int i = 0; i < num_digits; ++i,number /= 10) {
				char_array[i] = (number % 10) + ASCII_OFFSET;
		}
		if(num_digits < 4) { //For erasing previous digits in input array - asm gyro & accel
				char_array[3] = 0;
				char_array[4] = 0;
		}	

}

void sendBuftoUSART1_backwards(unsigned char bufarray[], short int start, short int end) {
	int char_index = end;
	const short int BUF_LENGTH = start; 
	unsigned char* temp; 
	while(char_index >= BUF_LENGTH) {
		 while(!(USART1->ISR & 0x00000080)) { //While transmit data register is not empty, wait
		 
		 }
		 temp = &bufarray[char_index]; 
		 USART1->TDR = hex_to_ascii(*temp); 
		 //sendTexttoUSART1("  "); 
		 --char_index;
	
	}


}






void sendBuftoUSART1(unsigned char bufarray[], short int start, short int end) {
	int char_index = start;
	const short int BUF_LENGTH = end; 
	unsigned char* temp; 
	while(char_index <= BUF_LENGTH) {
		 while(!(USART1->ISR & 0x00000080)) { //While transmit data register is not empty, wait
		 
		 }
		 temp = &bufarray[char_index]; 
		 USART1->TDR = hex_to_ascii(*temp); 
		// sendTexttoUSART1("  "); 
		 ++char_index;
	
	}


}

void sendChartoUSART1(unsigned char chr) {
	while(!(USART1->ISR & 0x00000080)) {
	
	}
	USART1->TDR = chr; 
}

__irq void USART1_IRQHandler() {

	receive_char = USART1->RDR; 
	sendChartoUSART1(receive_char); 

}
