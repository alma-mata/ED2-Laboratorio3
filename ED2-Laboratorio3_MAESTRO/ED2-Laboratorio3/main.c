//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// ELECTRÓNICA DIGITAL 2 - SECCIÓN - 20 -
// LAB_SPI_MASTER.c
// AUTOR1: ANTHONY ALEJANDRO BOTEO LÓPEZ
// AUTOR2: ALMA LISBETH MATA IXCAYAU
// PROYECTO: LABORATORIO SPI
// HARDWARE: ATMEGA328P
// CREADO: 02/02/2026
// ULTIMA MODIFICACION: 02/03/2025
// DESCRIPCIÓN: el siguiente código tiene como objetivo el utillizar un arduino nano como receptor maestro de datos y presentar los valores en leds y en
//la terminal UART de los valores del esclavo.
//-----------------------------------------------


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "SPI_LIB/SPI_LIB.h"
#include "ADC/ADC_LIB.h"
#include "UART/UART_LIB.h"

#define SS_LOW() PORTB &= ~(1<<DDB2);
#define SS_HIGH() PORTB |= (1<<DDB2);


//BUFFERS
char buffer_UART[40];

//VARIABLES
uint8_t VALOR_POTE1 = 0;
uint8_t VALOR_POTE2 = 0;


void INSTRUCCIONES_UART(uint8_t P1, uint8_t P2);


void SETUP(){
	DDRD = 0xFF;
	PORTD = 0x00;
	
	DDRB |= ((1<<PINB0)|(1<<PINB1));
	PORTB &= ~((1<<PINB0)|(1<<PINB1));
	SS_HIGH();
	
}

void CONVERSION_VOLTAJE(uint8_t VALOR_ADC){
	
	uint32_t LECTURA_ADC = ((uint32_t)VALOR_ADC * 5000) / 1023;
	
	uint8_t PARTE_ENTERA = LECTURA_ADC / 1000;
	uint8_t PARTE_DECIMAL = (LECTURA_ADC % 1000) / 10;
	
	UART_PrintNumber(PARTE_ENTERA);
	UART_PrintText(".");
	if(PARTE_DECIMAL < 10)
	{
		UART_PrintText("0");
	}
	UART_PrintNumber(PARTE_DECIMAL);
	UART_PrintString("V");
	
}


int main(void)
{
	SETUP();
	UART_CONF();
	SPI_INIT(SPI_MASTER_DIV16, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	sei();

	
	/* Replace with your application code */
	while (1)
	{
		//RECIBIR POTE 1
		SS_LOW();
		SPI_WRITE('1');
		_delay_us(50);
		SPI_WRITE(0x00);
		VALOR_POTE1 = SPDR;
		SS_HIGH();
		
		_delay_ms(10);
		
		//RECIBIR POTE 2
		SS_LOW();
		SPI_WRITE('2');
		_delay_us(50);
		SPI_WRITE(0x00);
		VALOR_POTE2 = SPDR;
		SS_HIGH();
		
		//ACTUALIZAR LEDS DEL MAESTRO EN BASE A POTE 1
		PORTD = (VALOR_POTE1>>2)<<2;
		PORTB = (PORTB&0b11111100)|((VALOR_POTE1>>6) & 0b00000011);
		
		//MANDAR DATOS AL UART
		INSTRUCCIONES_UART(VALOR_POTE1, VALOR_POTE2);
		
		_delay_ms(100);
		
		
	}
}

void INSTRUCCIONES_UART(uint8_t P1, uint8_t P2){
	
	if(COMANDO_NUEVO())
	{
		RECIBIR_COMANDO(buffer_UART);
		if(buffer_UART[0] < 32)
		return;
		
		if(buffer_UART[0] == '1')
		{
			UART_PrintText("Pote 1: ");
			CONVERSION_VOLTAJE(P1);
		}
		else if (buffer_UART[0] == '2')
		{
			UART_PrintText("Pote 2: ");
			CONVERSION_VOLTAJE(P2);
		}
	}
}

