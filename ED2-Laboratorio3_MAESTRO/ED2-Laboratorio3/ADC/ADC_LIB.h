/*
 * ADC_LIB.h
 *
 * Created: 1/27/2026 10:07:00 AM
 *  Author: itzle
 */ 


#ifndef ADC_LIB_H_
#define ADC_LIB_H_

#include <avr/io.h>
#include <stdint.h>

void ADC_CONF();
uint16_t ADC_READ(uint8_t canal);



#endif /* ADC_LIB_H_ */