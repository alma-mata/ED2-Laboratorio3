/*
 * Universidad del Valle de Guatemala
 * IE3054: Electrónica Digital 2
 * UART.h
 * Autor: Alma Lisbeth Mata Ixcayau
 * Proyecto: Laboratorio 2 - Digital 2
 * Descripcion: Librería de configuración y uso de UART
 * Creado: 01/02/2026
*/

#ifndef UART_H_
#define UART_H_

#define F_CPU 16000000UL
#include <avr/io.h>

void UART_init(void);
void send_CHAR(unsigned char caracter);
void send_STRING(char* texto);

#endif /* UART_H_ */