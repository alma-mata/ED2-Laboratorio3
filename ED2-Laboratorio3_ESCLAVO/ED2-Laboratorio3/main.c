//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// ELECTRÓNICA DIGITAL 2 - SECCIÓN - 20 -
// LAB_SPI_SLAVE.c
// AUTOR1: ANTHONY ALEJANDRO BOTEO LÓPEZ
// AUTOR2: ALMA LISBETH MATA IXCAYAU
// PROYECTO: LABORATORIO SPI
// HARDWARE: ATMEGA328P
// CREADO: 02/02/2026
// ULTIMA MODIFICACION: 09/02/2026
// DESCRIPCIÓN: Esclavo que envía sus ADCs y recibe un comando '3' para mostrar datos en sus LEDs.
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

// Bandera para máquina de estados en SPI ISR
// 0: Esperando comando ('1', '2', '3')
// 1: El siguiente byte es el DATO para los LEDs
volatile uint8_t proximo_byte_es_dato = 0;
volatile uint8_t valor_recibido_maestro = 0;

void setup(){
	DDRD = 0xFF; // LEDs
	PORTD = 0x00;
	DDRB |= ((1 << PINB0)|(1<<PINB1)); // LEDs parte alta
	PORTB &= ~((1 << PINB0)|(1<<PINB1));
}

// Función para actualizar LEDs (copiada la lógica de bits del original)
void ACTUALIZAR_LEDS(uint8_t valor) {
	PORTD = (PORTD & 0b00000011) | ((valor << 2) & 0b11111100);
	PORTB = (PORTB & 0b11111100) | ((valor >> 6) & 0b00000011);
}

int main(void)
{
	setup();
	ADC_CONF();
	SPI_INIT(SPI_SLAVE, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	sei();
	
	while (1)
	{
		// Leemos los sensores continuamente
		POTE1 = ADC_READ(1)/4;
		POTE2 = ADC_READ(2)/4;
		
		// --- MODIFICACION IMPORTANTE ---
		// Comenté la actualización de LEDs basada en POTE2.
		// Si dejaras esto descomentado, los LEDs parpadearían entre el valor
		// del Maestro y el POTE2 local muy rápido.
		// Ahora los LEDs solo obedecen al Maestro (comando '3').
		
		// ACTUALIZAR_LEDS(POTE2); <--- ANTES ESTABA ESTO
	}
}

ISR(SPI_STC_vect){
	
	uint8_t recibido = SPDR; // Leer lo que envió el maestro
	
	if (proximo_byte_es_dato == 1)
	{
		// Si la bandera está activa, lo que acabamos de recibir es el NÚMERO del maestro
		valor_recibido_maestro = recibido;
		ACTUALIZAR_LEDS(valor_recibido_maestro);
		
		proximo_byte_es_dato = 0; // Reiniciamos la bandera, volvemos a esperar comandos
		SPDR = 0x00; // Limpiamos buffer de salida
	}
	else
	{
		// MODO COMANDO
		if(recibido == '1')
		{
			SPDR = POTE1; // Cargamos Pote1 para el proximo ciclo de reloj
		}
		else if (recibido == '2')
		{
			SPDR = POTE2; // Cargamos Pote2 para el proximo ciclo de reloj
		}
		else if (recibido == '3')
		{
			// El maestro quiere escribir en mis LEDs
			proximo_byte_es_dato = 1; // Activamos bandera
			SPDR = 0x00; // No enviamos nada útil de vuelta
		}
		else
		{
			SPDR = 0x00;
		}
	}
}