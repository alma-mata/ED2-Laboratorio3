//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// LAB_SPI_MASTER.c
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

char buffer_UART[40];

// VARIABLES
uint8_t VALOR_POTE1 = 0;
uint8_t VALOR_POTE2 = 0;
uint8_t VALOR_LEDS = 0;
uint8_t MODO_MAESTRO = 0;

void SETUP(){
	DDRD = 0xFF;
	PORTD = 0x00;
	DDRB |= ((1<<PINB0)|(1<<PINB1));
	PORTB &= ~((1<<PINB0)|(1<<PINB1));
	SS_HIGH();
}

void CONVERSION_VOLTAJE(uint8_t VALOR_ADC){
	uint32_t LECTURA_ADC = ((uint32_t)VALOR_ADC * 5000) / 255;
	uint8_t PARTE_ENTERA = LECTURA_ADC / 1000;
	uint8_t PARTE_DECIMAL = (LECTURA_ADC % 1000) / 10;
	
	UART_PrintNumber(PARTE_ENTERA);
	UART_PrintText(".");
	if(PARTE_DECIMAL < 10) UART_PrintText("0");
	UART_PrintNumber(PARTE_DECIMAL);
	UART_PrintString("V\n");
}

void ACTUALIZAR_LEDS(uint8_t valor) {
	PORTD = (PORTD & 0b00000011)|((valor<<2)& 0b11111100);
	PORTB = (PORTB & 0b11111100)|((valor>>6) & 0b00000011);
}


uint8_t SPI_TRX(uint8_t dato_envio) {
	SS_LOW();
	SPI_WRITE(dato_envio);
	_delay_us(30);
	SPI_WRITE(0x00);
	uint8_t dato_recibido = SPDR;
	SS_HIGH();
	return dato_recibido;
}

int main(void)
{
	SETUP();
	UART_CONF();
	SPI_INIT(SPI_MASTER_DIV16, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	sei();

	UART_PrintString("Sistema Iniciado\n");
	UART_PrintString("Use 'm' para cambiar modo\n");

	while (1)
	{

		VALOR_POTE1 = SPI_TRX('1');
		_delay_us(50);
		VALOR_POTE2 = SPI_TRX('2');
		_delay_us(50);

		if (MODO_MAESTRO == 0) {
		
			SS_LOW(); SPI_WRITE('4'); SS_HIGH();
			ACTUALIZAR_LEDS(VALOR_POTE1); 
		}
		else {
		
			SS_LOW();
			SPI_WRITE('3');
			_delay_us(30);
			SPI_WRITE(VALOR_LEDS);
			SS_HIGH();
			ACTUALIZAR_LEDS(VALOR_LEDS); 
		}


		if(COMANDO_NUEVO())
		{
			RECIBIR_COMANDO(buffer_UART);

			if(buffer_UART[0] >= 32)
			{
				if(buffer_UART[0] == 'm') {
					MODO_MAESTRO = !MODO_MAESTRO;
					if(MODO_MAESTRO) UART_PrintString("MODO: Escribir LEDs\n");
					else UART_PrintString("MODO: Ver Sensores\n");
				}
				else if (MODO_MAESTRO == 0) {
		
					if(buffer_UART[0] == '1') {
						UART_PrintText("Pote 1: ");
						CONVERSION_VOLTAJE(VALOR_POTE1);
					}
					else if(buffer_UART[0] == '2') {
						UART_PrintText("Pote 2: ");
						CONVERSION_VOLTAJE(VALOR_POTE2);
					}
				}
				else {

					VALOR_LEDS = atoi(buffer_UART);
				}
			}
		}
		
		_delay_ms(50);
	}
}