#ifndef PWM_H
#define PWM_H

void blink_LED(int period); 
void delay (int b); 
void cyclic_PWM(int dutyCycle);
void normal_pulse(int dutyCycle); 

#endif