#include "I2C.h"
#include "stm32f030x8.h"
#include "UART.h"
#include <string.h>
#include "Timers.h"

unsigned char gyro_readnwrite_state;
unsigned char gyro_read_state; 
unsigned char gyro_write_state;
unsigned char gyro_write_state;
unsigned char gyro_start_address; 
unsigned char gyro_read_inprogress = 0;
unsigned char gyro_write_inprogress = 0; 
unsigned long gyro_display[3] = {0,0,0}; 
unsigned long accel_display[3] = {0,0,0}; 

unsigned char raw_IMU_Data[5] = {0,0,0,0,0};





unsigned char Gyro_buffer[120] = {0xFF,0x01,0x11,0x31,0x00,0x00,0x00,0x00,0x00,\
0x10,0x12,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};




__irq void I2C2_IRQHandler() {

	if(gyro_readnwrite_state == 1) {
		gyro_read();
		 
	}
	else {
		gyro_write();
	}

}


void print_buffer() {
	sendBuftoUSART1(Gyro_buffer, 27, 38); 
  sendTexttoUSART1("\r");
}



void gyro_read() {
	static unsigned char array_count = 0; //Array used to send bytes over I2C2
	
	if(!gyro_read_inprogress) {
		gyro_read_inprogress = 1;
		gyro_read_state = 0; //Used to jump to start condition
		array_count = 0; //Reset array index
		
		I2C2->CR1 |= 0x00000040; //Transfer complete interrupt enabled TCIE
	  I2C2->CR2 &= 0x00000000; //Clear all bits on transfer register
		I2C2->CR2 |= 0x000100D6; //AUTOEND = 0, 1 Byte being sent, this is the slave address 0xD6
		
	}
	switch(gyro_read_state) {
		
		case 0: //Send start bit
			I2C2->CR2 |= 0x00002000; //Turn Start bit on
		  gyro_read_state = 1;
			break;
		
		case 1: //Send start address to read from
			I2C2->TXDR = gyro_start_address; 
		  gyro_read_state = 2;
			break;
		
		case 2: //Configure reading from sensor
			I2C2->CR1 &= 0xFFFFFFBF; //Disable transfer complete interrupt
		  I2C2->CR2 &= 0x00000000; //Clear CR2
		  I2C2->CR2 |= (120<<16); //120 Bytes of Data to be Read
		  I2C2->CR2 |= 0x020004D6;	//AUTOEND=1, slave address is 0xD6			
		  I2C2->CR2 |= 0x00002000;	//Re-start condition, turn start bit on again
		  array_count = 0;
		  gyro_read_state = 3;
			break;
		
		case 3: //Read Data from sensor into gyro_buf array
			Gyro_buffer[array_count] = I2C2->RXDR;
		  ++array_count;
		  if(array_count >= 120) {
				gyro_read_state = 0;
				gyro_read_inprogress = 0;
			}
			break; 
			
		default:
			break; 
	
	
	}


}

void gyro_write() {
  static unsigned char array_count = 0; //Array used to send bytes over I2C2
	if(!gyro_write_inprogress) {
		gyro_write_inprogress = 1;
		gyro_write_state = 0;
	  array_count = 0;
		I2C2->CR1 |= 0x00000040;
		I2C2->CR2 &= 0x00000000;
		I2C2->CR2 |= 0x020300D6; //AUTOEND = 1, same slave address, writing 3 bytes of data
	
	}
	switch(gyro_write_state) {
		case 0:
			I2C2->CR2 |= 0x00002000; //Start I2C
			gyro_write_state = 1;
			break;
		
		case 1:
			I2C2->TXDR = gyro_start_address; 
		  gyro_write_state = 2;
			break;
		
		case 2:
			I2C2->TXDR = Gyro_buffer[gyro_start_address-7];
		  gyro_write_state = 3;
			break;
		
		case 3:
			I2C2->TXDR = Gyro_buffer[gyro_start_address - 6];
		  gyro_write_state = 0;
	    gyro_write_inprogress = 0;
			break;
		
		default:
			break;
	
	
	}

}

void display_measures() {
	unsigned short gyro_temp = 0;
	unsigned short accel_temp = 0;
	unsigned static int time_elapsed = 0;
	unsigned int refresh_time = getmsTicks();
	if((refresh_time - time_elapsed) > 200) {
		time_elapsed = refresh_time; 
		gyro_temp = (unsigned short)Gyro_buffer[0x1C];
		gyro_temp <<= 8;
		gyro_temp += (unsigned short)Gyro_buffer[0x1B]; 

		if(gyro_temp >= 0x8000) {
			gyro_temp *= -1;
			sendTexttoUSART1("-"); 
		}
		else {
			sendTexttoUSART1(" "); 
		}
		gyro_display[0] = (unsigned long)gyro_temp;
		gyro_display[0] = (gyro_display[0]*12500) >> 15;
short_to_chararray((unsigned short)gyro_display[0], raw_IMU_Data);
			sendBuftoUSART1_backwards(raw_IMU_Data, 0, 4); 		
	//	display5digitswpoint2 (conv6hexbcd (gyro_display[0]));
		
		sendTexttoUSART1("d/s  ");
		  gyro_temp = (unsigned short)Gyro_buffer[0x1E];
			gyro_temp <<= 8;
			gyro_temp += (unsigned short)Gyro_buffer[0x1D];
			
			if (gyro_temp >= 0x8000)			{
				gyro_temp *= -1;
				sendTexttoUSART1("-");
			}	
			else { 
				sendTexttoUSART1(" ");	
			}
			gyro_display[1] = (unsigned long)gyro_temp;
			gyro_display[1] = (gyro_display[1]*12500) >> 15;
			
			short_to_chararray((unsigned short)gyro_display[1], raw_IMU_Data);
			sendBuftoUSART1_backwards(raw_IMU_Data, 0, 4); 
		
			//display5digitswpoint2 (conv6hexbcd (gyro_display[1]));
			//u1printmagenta ();	
			sendTexttoUSART1("d/s  ");
				
			
			gyro_temp = (unsigned short)Gyro_buffer[0x20];
			gyro_temp <<= 8;
			gyro_temp += (unsigned short)Gyro_buffer[0x1F];
			
			if (gyro_temp >= 0x8000)			{
				gyro_temp *= -1;
				sendTexttoUSART1("-");
			}	
			else {	
				sendTexttoUSART1(" ");	
			}
			gyro_display[2] = (unsigned long)gyro_temp;
			gyro_display[2] = (gyro_display[2]*12500) >> 15;	
			
			short_to_chararray((unsigned short)gyro_display[2], raw_IMU_Data);
			sendBuftoUSART1_backwards(raw_IMU_Data, 0, 4); 
	//		display5digitswpoint2 (conv6hexbcd (gyro_display[2]));
	//		u1printmagenta ();	
			sendTexttoUSART1("d/s    ");
		//	u1printdefault ();		
			
//****Accelerometer calculation****			
			accel_temp = (unsigned short)Gyro_buffer[0x22];
			accel_temp <<= 8;
			accel_temp += (unsigned short)Gyro_buffer[0x21];		
			if (accel_temp >= 0x8000)			{
				accel_temp *= -1; 
				sendTexttoUSART1("-");
			}	
			else {
				sendTexttoUSART1(" ");	
			}
			accel_display[0] = (unsigned long)accel_temp;
			accel_display[0] = (accel_display[0]*10000) >> 14;	
			
			short_to_chararray((unsigned short)accel_display[0], raw_IMU_Data);
			sendBuftoUSART1_backwards(raw_IMU_Data, 0, 4); 
//			u1printmagenta ();	
			sendTexttoUSART1("g   ");
//			u1printdefault ();	
			
			accel_temp = (unsigned short)Gyro_buffer[0x24];
		  accel_temp <<= 8;
			accel_temp += (unsigned short)Gyro_buffer[0x23];
			
			if (accel_temp >= 0x8000)			{
				gyro_temp *= -1;
				sendTexttoUSART1("-");
			}	
			else {	
				sendTexttoUSART1(" ");	
			}
			accel_display[1] = (unsigned long)accel_temp;
			accel_display[1] =  (accel_display[1]*10000) >> 14; 
    short_to_chararray((unsigned short)accel_display[1], raw_IMU_Data);			
				sendBuftoUSART1_backwards(raw_IMU_Data, 0, 4); 	
		//	display5digitswpoint (conv6hexbcd (accel_display[1]));				
//			u1printmagenta ();	
			sendTexttoUSART1("g   ");
//			u1printdefault ();			
			
			accel_temp = (unsigned short)Gyro_buffer[0x26];
			accel_temp <<= 8;
			accel_temp += (unsigned short)Gyro_buffer[0x25];
			
			
			if (accel_temp >= 0x8000)			{
				accel_temp *= -1;
				sendTexttoUSART1("-");
			}	
			else	{ 
				sendTexttoUSART1(" ");	
			}
			accel_display[2] = (unsigned long)accel_temp;
			accel_display[2] = (accel_display[2]*10000) >> 14; 
				short_to_chararray((unsigned short)accel_display[2], raw_IMU_Data);
			sendBuftoUSART1_backwards(raw_IMU_Data, 0, 4); 		
		//	display5digitswpoint (conv6hexbcd (accel_display[2]));				
//			u1printmagenta ();	
			sendTexttoUSART1("g   ");
//			u1printdefault ();		
		
		  sendTexttoUSART1("\r"); 
	}



}