/*
 * UART_LIB.h
 *
 * Created: 5/10/2025 12:34:01 PM
 *  Author: itzle
 */ 


#ifndef UART_LIB_H_
#define UART_LIB_H_

#include <avr/io.h>

//FUNCIONES DE CONFFIGURACION Y ENVIO
void UART_CONF(void);
void UART_TRANS(uint8_t DATOS);
void UART_PrintString(const char *str);
void UART_PrintNumber(uint16_t num);
void UART_PrintText(const char *str);

//FUNCIONES ASÍNCRONAS
uint8_t COMANDO_NUEVO(void); //CONTROLAR LA BANDERA 1 o 0
void	RECIBIR_COMANDO(char* buffer_destino); //COPIA Y EL COMANDO Y LIMPIA EL BUFFER


#endif /* UART_LIB_H_ */