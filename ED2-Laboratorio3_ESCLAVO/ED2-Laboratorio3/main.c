//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// ELECTRÓNICA DIGITAL 2 - SECCIÓN - 20 -
// LAB_SPI_SLAVE.c
// AUTOR1: ANTHONY ALEJANDRO BOTEO LÓPEZ
// AUTOR2: ALMA LISBETH MATA IXCAYAU
// PROYECTO: LABORATORIO SPI
// HARDWARE: ATMEGA328P
// CREADO: 02/02/2026
// ULTIMA MODIFICACION: 02/03/2025
//-----------------------------------------------

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "SPI_LIB/SPI_LIB.h"
#include "ADC/ADC_LIB.h"
#include "UART/UART_LIB.h"

volatile uint8_t POTE1 = 0;
volatile uint8_t POTE2 = 0;
volatile uint8_t DATO_RECIBIDO = 0;
volatile uint8_t MODO_LED = 0; 
volatile uint8_t flag_next_is_data = 0;

void setup(){
	DDRD = 0xFF;
	PORTD = 0x00;
	DDRB|= ((1 << PINB0)|(1<<PINB1));
	PORTB &= ~((1 << PINB0)|(1<<PINB1));
}

void ACTUALIZAR_LEDS(uint8_t valor) {
	PORTD = (PORTD & 0b00000011)|((valor<<2)& 0b11111100);
	PORTB = (PORTB & 0b11111100)|((valor >> 6) & 0b00000011);
}

int main(void)
{
	setup();
	ADC_CONF();
	SPI_INIT(SPI_SLAVE, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	sei();
	
	while (1)
	{
		POTE1 = ADC_READ(1)/4;
		POTE2 = ADC_READ(2)/4;
		
		switch(MODO_LED)
		{
			case 0:
			ACTUALIZAR_LEDS(POTE2);
			break;
			
			case 1:
			ACTUALIZAR_LEDS(DATO_RECIBIDO);
			break;
		}
	}
}

ISR(SPI_STC_vect){
	
	uint8_t received = SPDR;
	
	if (flag_next_is_data == 1) {
		DATO_RECIBIDO = received;
		flag_next_is_data = 0;
		SPDR = 0;
	}
	else {
		if(received == '1')
		{
			SPDR = POTE1;
			MODO_LED = 0; 
		}
		else if (received == '2')
		{
			SPDR = POTE2;
			MODO_LED = 0; 
		}
		else if (received == '3')
		{

			flag_next_is_data = 1;
			MODO_LED = 1;
			SPDR = 0;
		}
		else
		{
			SPDR = 0;
		}
	}
}