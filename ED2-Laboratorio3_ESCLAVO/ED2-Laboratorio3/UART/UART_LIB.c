/*
 * UART_LIB.c
 *
 * Created: 5/10/2025 12:50:30 PM
 *  Author: itzle
 */ 

#include "UART_LIB.h"
#include <avr/interrupt.h>
#include <stdlib.h>  

#define RX_BUFFER_SIZE	64

static volatile char rx_buffer[RX_BUFFER_SIZE]; //Buffer 
static volatile uint8_t rx_index = 0;           //Posición actual
static volatile uint8_t comando_listo = 0;      //Flag

ISR(USART_RX_vect){
	
	char dato = UDR0;
	
	if (comando_listo) return;

	if (dato == '\n' || dato == '\r') {
		rx_buffer[rx_index] = '\0'; //TERMINA EL STRING
		comando_listo = 1;          //SUBE L ABANDERA
		rx_index = 0;               //REINICIAMOS EL INDICE
	}
	else {
		// Guardamos si hay espacio
		if (rx_index < RX_BUFFER_SIZE - 1) {
			rx_buffer[rx_index] = dato;
			rx_index++;
		}
	}
	

}

void UART_CONF(){
	UBRR0H = 0;
	UBRR0L = 103;  //9600 bauds a 16MHz
	
	UCSR0B = (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);  //Habilitar interrupción RX
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);  //8 bits, sin paridad, 1 stop bit
}



void UART_TRANS(uint8_t DATOS){
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = DATOS;
}


void UART_PrintString(const char *str) {
	while (*str) {
		UART_TRANS(*str++);
	}
	UART_TRANS('\r');  //Retorno adicional
	UART_TRANS('\n');  //Salto de línea
}

void UART_PrintNumber(uint16_t num) {
	char buffer[6]; 
	itoa(num, buffer, 10);
	UART_PrintString(buffer);
}

//SE AGREGÓ EL NUEVO MODULO PARA IMPRIMIR TEXTOS MÁS LARGOS
void UART_PrintText(const char *str){
	
	while(*str) UART_TRANS(*str++);
}


//LLAMAR SI LA BANDERA ESTA EN 1
uint8_t COMANDO_NUEVO(void){
	return comando_listo;
	
}

void RECIBIR_COMANDO(char* buffer_destino){
	//COPIAMOS EL BUFFER PRIVADO
	uint8_t i = 0;
	while(rx_buffer[i] != '\0'){
		buffer_destino[i] = rx_buffer[i];
		i++;
	}
	buffer_destino[i] = '0';	//CERRAR STRING
	comando_listo = 0; //BANDERA = 0
	
	
	
}