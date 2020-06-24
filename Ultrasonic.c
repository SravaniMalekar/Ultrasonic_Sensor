/*
 * Ultrosonic.c
 *
 * Created: 24-06-2020 16:55:10
 * Author : Sravani Malekar
 */ 
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART.h"

volatile uint16_t time1;
volatile uint16_t time2;
volatile uint8_t Flag=0;

#define TRIGGER_PIN 4

void inittimer1(void){
	TCCR1B |= (1 << ICES1);//setting initial input capture edge as rising edge 
	TCCR1B |= (1 << CS11);// setting prescaler as 8
	TIMSK1 |= (1 << ICIE1);// enable input capture interrupt enable
	sei();// enable global interrupt
	
}
ISR(TIMER1_CAPT_vect){
	int distance;
	uint16_t distancedouble;
	uint16_t distancecm;
	if(Flag==0){//LOW TO HIGH (rising edge)
		time1 = ICR1; 
		TCCR1B &= ~(1 << ICES1);//TRIGGER CHANGE to falling edge
        Flag =1;
		
	}
	else if(Flag==1){//HIGH TO LOW (falling edge)
		serialString("Distance:");
		time2 = ICR1;
		distancedouble = (time2 - time1);//total traveltime
		distance= distancedouble/2;
		distancecm = distance/58;//distance in cm
		print_byte(distancecm);
		serialString(" cm\r\n");
		TCCR1B |=(1 << ICES1);//TRIGGER CHANGE to rising edge
		Flag =0;
		
}
}
int main(void)
{
	USART_Init();
	inittimer1();
    DDRB |= (1 << TRIGGER_PIN);//setting trigger pin as output

    while (1) 
    {
		PORTB &= ~(1 << 4);
		_delay_us(10);
		PORTB |= (1 << 4);// sending trigger pulse
		_delay_us(15);
		PORTB &= ~(1 << 4);
		TCNT1=0;// counter to 0 
		
		_delay_ms(2000);
		
    }
}

