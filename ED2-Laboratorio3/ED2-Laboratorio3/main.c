/*
 * Universidad del Valle de Guatemala
 * IE3054: Electrónica Digital 2
 * ED2-Laboratorio2.c
 * Autor: Alma Lisbeth Mata Ixcayau
 * Proyecto: Laboratorio 2 - Digital 2
 * Descripcion: Uso de SPI
 * Creado: 03/02/2026
*/
/****************************************/
// Encabezado
#define F_CPU 16000000UL	//Frecuencia es 16Mhz
#include <avr/io.h>
#include <avr/interrupt.h>	// Librería de interrupciones
#include <stdint.h>
#include <stdio.h>
#include "ADC/ADC_libreria.h"
#include "SPI/SPI_libreria.h"
#include "UART/UART.h"

/****************************************/
// Prototipos de función


// Variables globales
volatile uint16_t POT1 = 0;			// Entrada Potenciometro 1 ADC
volatile uint16_t POT2 = 0;			// Entrada potenciometro 2 ADC
volatile uint8_t canal_ADC = 0;		// canal para configuración ADC
volatile uint8_t received_RX = 0;	// Dato recibido del UART
volatile uint8_t dato_ENVIADO = 0;	// Bandera de dato enviado UART
volatile uint8_t contador_UART = 0; // Contador UART


/****************************************/
// Función principal

int main(void)
{
	setup();		// Se llama a la configuración general
	
    while (1) 
    {	
		_delay_ms(100);
    }
}

/****************************************/
// Subrutinas sin Interrupcion
void setup(void)
{
	cli();
	// Configuracion de puertos
	DDRD = 0xFF;	// Todo el PORTD es salida
	PORTD = 0x00;	// Se apagan los pines del PORTD
	
	DDRB = 0x03;	// Primeros 2 bits son salidas
	PORTB = 0x00;	// Se apagan los pines del PORTB
	
	//Inicializacion de SPI
	
	
	// Inicializacion de ADC
	ADC_Init(canal_ADC);
	// Inicializacion de UART
	UART_init();
	sei();
}

/****************************************/
// Subrutinas de Interrupcion
ISR(ADC_vect){				// Leer canal y asignarlo al POT correspondiente
	switch (canal_ADC){
		case 0: POT1 = ADC; break;
		case 1: POT2 = ADC; break;
	}
	canal_ADC = (canal_ADC + 1) % 2;
	ADMUX = (ADMUX & 0xF0) | (canal_ADC & 0x03);
	ADCSRA |= (1 << ADSC); // Volver a iniciar conversion
}

ISR(USART_RX_vect){			// Interrupción UART
	received_RX = UDR0;
	dato_ENVIADO = 1;
}