#ifndef UART_H
#define UART_H

void sendTexttoUSART1(char text[]);
void sendBuftoUSART1(unsigned char bufarray[], short int start, short int end);
void sendBuftoUSART1_backwards(unsigned char bufarray[], short int start, short int end);
void sendChartoUSART1(unsigned char chr); 
unsigned short hextoascii (unsigned char hexnum);
unsigned char hex_to_ascii (unsigned char hexnum);
void short_to_chararray(unsigned short number, unsigned char* char_array); 
unsigned short get_number_of_digits(int number); 

#endif