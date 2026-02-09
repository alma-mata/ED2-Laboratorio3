//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// ELECTRÓNICA DIGITAL 2 - SECCIÓN - 20 -
// LAB_SPI_MASTER.c
// AUTOR1: ANTHONY ALEJANDRO BOTEO LÓPEZ
// AUTOR2:ALMA LISBETH MATA IXCAYAU
// PROYECTO: LABORATORIO SPI
// HARDWARE: ATMEGA328P
// CREADO: 02/02/2026
// ULTIMA MODIFICACION: 02/03/2025
// DESCRIPCIÓN: el siguiente código tiene como objetivo el utillizar un arduino nano como esclavo para enviar datos de dos potenciometroes utilizando el ADC de los pines PC0 y PC1
//y visualizar los valores de PC1 en los leds de PD2 - PD5 del esclavo y de PC0 en los valores de los leds de PD2 - PD5 del maestro.
//la terminal UART de los valores del esclavo.
//-----------------------------------------------


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "SPI_LIB/SPI_LIB.h"
#include "ADC/ADC_LIB.h"
#include "UART/UART_LIB.h"


volatile uint8_t POTE1 = 0;
uint8_t POTE2 = 0;

void setup(){
	DDRD = 0xFF;
	PORTD = 0x00;
	DDRB|= ((1 << PINB0)|(1<<PINB1));
	PORTB &= ~((1 << PINB0)|(1<<PINB1));
	
}


int main(void)
{
	setup();
	ADC_CONF();
	SPI_INIT(SPI_SLAVE, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	sei();
	
	/* Replace with your application code */
	while (1)
	{
		POTE1 = ADC_READ(1)/4;
		POTE2 = ADC_READ(2)/4;
		
		PORTD = (PORTD & 0b00000011)|((POTE2<<2)& 0b11111100);
		PORTB = (PORTB & 0b11111100)|((POTE2 >> 6) & 0b00000011);
	}
}

ISR(SPI_STC_vect){
	
	uint8_t ORDEN = SPDR;
	
	if(ORDEN == '1')
	{
		SPDR = POTE1;
	}
	else if (ORDEN == '2')
	{
		SPDR = POTE2;
	}
	else
	{
		SPDR = 0;
	}
	
}