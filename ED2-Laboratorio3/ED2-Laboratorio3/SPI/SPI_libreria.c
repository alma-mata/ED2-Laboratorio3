/*
 * SPI_library.c
 *
 * Created: 3/02/2026 12:41:27
 *  Author: ialma
 */ 

#include "SPI_libreria.h"

// Pines de comunicación
// PB3: MOSI, PB4: MISO, PB5: SCK

// Pin de control para esclavo
// PB2: Esclavo

void SPI_INIT(Spi_Type sType, Spi_Data_Order sDataOrder, Spi_Clock_Polarity SClockPolarity, Spi_Clock_Phase sClockPhase){
	//Seleccionar el tipo de spi a utilizar
	if (sType & (1<<MSTR))//Si es maestro
	{
		DDRB |= (1<<DDB3)|(1<<DDB5)|(1<<DDB2);	//MOSI, SCK, NEGADO_SS
		DDRB &= ~(1<<DDB4); //MISO
		SPCR |= (1<<MSTR);	//MASTER
		
		uint8_t temp= sType & 0b00000111;
		switch(temp){
			case 0:	//DIV2
			SPCR &= ~((1<<SPR1)|(1<<SPR0));
			SPSR |= (1<<SPI2X);
			break;
			
			case 1: //DIV4
			SPCR &= ~((1<<SPR1)|(1<<SPR0));
			SPSR &= ~(1<<SPI2X);
			break;
			
			case 3:
			SPCR |= (1<<SPR0);
			SPCR &= ~(1<<SPR1);
			SPSR &= ~(1<<SPI2X);
			break;
			
			case 4:
			SPCR &= ~(1<<SPR0);
			SPCR |= (1<<SPR1);
			SPSR |= (1<<SPI2X);
			break;
			
			case 5:	//DIV64
			SPCR &= ~(1<<SPR0);
			SPCR |= (1<<SPR1);
			SPSR &= ~(1<<SPI2X);
			break;
			
			case 6:	//DIV128
			SPCR |= (1<<SPR0);
			SPCR |= (1<<SPR1);
			SPSR &= ~(1<<SPI2X);
			break;
		}
	}
	else//SI ES MODO SCLAVO
	{
		DDRB |= (1<<DDB4); //MISO
		DDRB &=~ (1<<DDB3)|(1<<DDB5)|(1<<DDB2);	//MOSI, SCK, NEGADO_SS
		SPCR &=~(1<<MSTR);	//ESCLAVO
	}
	//Enable SPI, Data Order, Clock Polarity, Clock Phase
	SPCR |= (1<<SPE)|sDataOrder|SClockPolarity|sClockPhase;
	
}


void SPI_WRITE(uint8_t dat){
	SPDR=dat;
}

unsigned spiDataReady(){
	if (SPSR & (1<<SPIF))
	return 1;
	else
	return 0;
}

uint8_t SPI_READ(void){
	while (!(SPSR & (1<<SPIF)));	//Espera de datos completo
	return(SPDR);	//lee el valor recibido
}