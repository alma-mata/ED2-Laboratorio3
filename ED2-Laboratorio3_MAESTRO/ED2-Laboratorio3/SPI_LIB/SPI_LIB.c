/*
 * SPI_LIB.c
 *
 * Created: 2/3/2026 10:34:56 AM
 *  Author: itzle
 */ 

#include "SPI_LIB.h"





void SPI_INIT(SPI_TYPE type, SPI_ORDER dataOrder, CLOCK_POLARITY clockPolarity, CLOCK_PHASE clockPhase){
	
	if(type &(1<<MSTR))
	{
	 SPI_DDR |= (1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS);
	 
	 SPI_DDR &= ~(1<<SPI_MISO);
	 
	 SPI_PORT |= (1<<SPI_SS);
	}
	else
	{
		SPI_DDR |= (1<<SPI_MISO);
		
		SPI_DDR &= ~((1<<SPI_MOSI)|(1<<SPI_SCK)|(1<<SPI_SS));
	}
	
	//CAMBIO EN LA LIBRERIA DEL "MASTER" (1<<SPIE) ES INTERRUPCIÓN Y SE DEBE CAMBIAR
	SPCR = (1<<SPE)|dataOrder|clockPolarity|clockPhase;
	
	if(type &(1<<MSTR))
	{
	SPCR |= (1<<MSTR);
	}
	else
	{
		SPCR &= ~(1<<MSTR);
	}
	
	uint8_t V_TEMP = type & 0b00000111;
	switch(V_TEMP) {
		// DIV2
		case 0: 
			SPCR &= ~((1<<SPR1)|(1<<SPR0)); 
			SPSR |= (1<<SPI2X);            
		break;
		// DIV4
		case 1: 
			SPCR &= ~((1<<SPR1)|(1<<SPR0));
			SPSR &= ~(1<<SPI2X);         
		break;
		// DIV8
		case 2: 
			SPCR |= (1<<SPR0);
			SPCR &= ~(1<<SPR1);
			SPSR |= (1<<SPI2X);            
		break;
		 // DIV16
		case 3:
			SPCR |= (1<<SPR0);
			SPCR &= ~(1<<SPR1);
			SPSR &= ~(1<<SPI2X);           
		break;
		// DIV32
		case 4: 
			SPCR &= ~(1<<SPR0);
			SPCR |= (1<<SPR1);
			SPSR |= (1<<SPI2X);             
		break;
		 // DIV64
		case 5:
			SPCR &= ~(1<<SPR0);
			SPCR |= (1<<SPR1);
			SPSR &= ~(1<<SPI2X);            
		break;
		// DIV128
		case 6: 
			SPCR |= (1<<SPR0)|(1<<SPR1);
			SPSR &= ~(1<<SPI2X);            
		break;
	}
	
}


void SPI_WRITE(uint8_t dato){
	SPDR = dato;
	while (!(SPSR &(1<<SPIF)));
}

uint8_t SPI_READ(void){
	
	 while (!(SPSR &(1<<SPIF)));
	 return SPDR;
	
}







