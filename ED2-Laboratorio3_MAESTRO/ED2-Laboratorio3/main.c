//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// ELECTRÓNICA DIGITAL 2 - SECCIÓN - 20 -
// LAB_SPI_MASTER.c
// AUTOR1: ANTHONY ALEJANDRO BOTEO LÓPEZ
// AUTOR2: ALMA LISBETH MATA IXCAYAU
// PROYECTO: LABORATORIO SPI
// HARDWARE: ATMEGA328P
// CREADO: 02/02/2026
// ULTIMA MODIFICACION: 09/02/2026
// DESCRIPCIÓN: Maestro que lee potes del esclavo y ADEMÁS envía un contador
// para visualizarlo en los LEDs del esclavo (Nuevo Modo).
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
uint8_t CONTADOR_MAESTRO = 0; // Variable para el nuevo modo

void INSTRUCCIONES_UART(uint8_t P1, uint8_t P2);

void SETUP(){
	DDRD = 0xFF; // Puerto D como salida (LEDs)
	PORTD = 0x00;
	
	// Configurar pines de control SPI (SS manual si es necesario, aunque la libreria lo maneje)
	DDRB |= ((1<<PINB0)|(1<<PINB1));
	PORTB &= ~((1<<PINB0)|(1<<PINB1)); // LEDs en PORTB
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

// Función auxiliar para mapear bits a los LEDs (Hardware específico)
void ACTUALIZAR_LEDS_LOCALES(uint8_t valor) {
	// LEDs en PD2-PD7 (6 bits) y PB0-PB1 (2 bits) asumo por la máscara original
	// Mapeo original: PORTD bits 2-7, PORTB bits 0-1
	PORTD = (PORTD & 0b00000011) | ((valor << 2) & 0b11111100);
	PORTB = (PORTB & 0b11111100) | ((valor >> 6) & 0b00000011);
}

int main(void)
{
	SETUP();
	UART_CONF();
	SPI_INIT(SPI_MASTER_DIV16, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	sei();

	UART_PrintString("Bienvenido\n");
	UART_PrintString("Sistema Maestro Iniciado\n");
	
	while (1)
	{
		// 1. RECIBIR POTE 1 DEL ESCLAVO
		SS_LOW();
		SPI_WRITE('1'); // Pido Pote 1
		_delay_us(50);
		SPI_WRITE(0x00); // Dummy para leer respuesta
		VALOR_POTE1 = SPDR;
		SS_HIGH();
		
		_delay_ms(10);
		
		// 2. RECIBIR POTE 2 DEL ESCLAVO
		SS_LOW();
		SPI_WRITE('2'); // Pido Pote 2
		_delay_us(50);
		SPI_WRITE(0x00);
		VALOR_POTE2 = SPDR;
		SS_HIGH();

		// 3. NUEVO MODO: MANDAR NUMERO AL ESCLAVO (LEDS)
		// Incrementamos un contador para simular un valor cambiante
		CONTADOR_MAESTRO++;
		
		SS_LOW();
		SPI_WRITE('3');      // COMANDO NUEVO: '3' significa "Te voy a enviar un dato para tus LEDs"
		_delay_us(50);       // Pequeña espera para que el ISR del esclavo procese
		SPI_WRITE(CONTADOR_MAESTRO); // Enviamos el número
		SS_HIGH();

		// 4. ACTUALIZAR LEDS DEL MAESTRO (Para ver lo mismo que el esclavo)
		// Nota: En tu código original mostrabas POTE1.
		// Aquí muestro CONTADOR_MAESTRO para cumplir "se tiene que ver en los leds tanto del esclavo como del maestro"
		ACTUALIZAR_LEDS_LOCALES(CONTADOR_MAESTRO);
		
		// MANDAR DATOS AL UART
		INSTRUCCIONES_UART(VALOR_POTE1, VALOR_POTE2);
		
		_delay_ms(100); // Velocidad de actualización visual
	}
}

void INSTRUCCIONES_UART(uint8_t P1, uint8_t P2){
	if(COMANDO_NUEVO())
	{
		RECIBIR_COMANDO(buffer_UART);
		if(buffer_UART[0] < 32) return;
		
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