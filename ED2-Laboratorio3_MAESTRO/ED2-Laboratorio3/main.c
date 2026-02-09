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
uint8_t VALOR_LEDS = 0; // Variable que guarda el número recibido por UART (0-255)
uint8_t MODO_MAESTRO = 0; // 0 = Modo Original (Sensores), 1 = Nuevo Modo (LEDs UART)

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

void ACTUALIZAR_LEDS(uint8_t valor) {
	// Mapeo para mostrar el byte en PORTD[7:2] y PORTB[1:0]
	PORTD = (PORTD & 0b00000011)|((valor<<2)& 0b11111100);
	PORTB = (PORTB & 0b11111100)|((valor>>6) & 0b00000011);
}

int main(void)
{
	SETUP();
	UART_CONF();
	SPI_INIT(SPI_MASTER_DIV16, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	sei();

	UART_PrintString("Bienvenido\n");
	UART_PrintString("m: Cambiar Modo\n");
	UART_PrintString("1: Ver Pote 1\n");
	UART_PrintString("2: Ver Pote 2\n");
	UART_PrintString("Escriba un numero (0-255) para los LEDs\n");
	
	while (1)
	{
		// Revisar UART para leer comandos o el numero para los LEDs
		INSTRUCCIONES_UART(VALOR_POTE1, VALOR_POTE2);
		
		switch(MODO_MAESTRO)
		{
			case 0: // --- MODO ORIGINAL (LEER POTES) ---
			// Pedir Pote 1
			SS_LOW();
			SPI_WRITE('1');
			_delay_us(50);
			SPI_WRITE(0x00);
			VALOR_POTE1 = SPDR;
			SS_HIGH();
			
			_delay_ms(10);
			
			// Pedir Pote 2
			SS_LOW();
			SPI_WRITE('2');
			_delay_us(50);
			SPI_WRITE(0x00);
			VALOR_POTE2 = SPDR;
			SS_HIGH();

			// En modo original, mostramos Pote 1 en LEDs del maestro
			ACTUALIZAR_LEDS(VALOR_POTE1);
			break;
			
			case 1: // --- NUEVO MODO (UART A LEDS) ---
			
			// Enviar el valor ingresado por UART al esclavo
			SS_LOW();
			SPI_WRITE('3'); // Comando '3': Preparar esclavo para recibir dato LED
			_delay_us(50);
			SPI_WRITE(VALOR_LEDS); // Enviamos el valor (0-255)
			SS_HIGH();
			
			// Mostramos el mismo valor en los LEDs del maestro
			ACTUALIZAR_LEDS(VALOR_LEDS);
			break;
		}

		_delay_ms(100);
	}
}

void INSTRUCCIONES_UART(uint8_t P1, uint8_t P2){
	if(COMANDO_NUEVO())
	{
		RECIBIR_COMANDO(buffer_UART);
		
		// 1. Convertir siempre el buffer a numero
		// Si el usuario escribe "255", VALOR_LEDS será 255.
		// Si escribe "1", VALOR_LEDS será 1 (leds casi apagados).
		VALOR_LEDS = atoi(buffer_UART);

		// 2. Revisar comandos especificos
		if(buffer_UART[0] == 'm') {
			MODO_MAESTRO = !MODO_MAESTRO;
			if(MODO_MAESTRO == 1) UART_PrintString("\nMODO: Escribir Valor LEDs\n");
			else UART_PrintString("\nMODO: Leer Sensores\n");
		}
		
		else if(buffer_UART[0] == '1' && buffer_UART[1] < 32)
		{
			UART_PrintText("Pote 1: ");
			CONVERSION_VOLTAJE(P1);
		}
		else if (buffer_UART[0] == '2' && buffer_UART[1] < 32)
		{
			UART_PrintText("Pote 2: ");
			CONVERSION_VOLTAJE(P2);
		}
		else
		{
			
			// UART_PrintString("Valor actualizado.");
		}
	}
}