/*
 * adc.h
 *
 * Created: 14.09.2026 10:14:15
 *  Author: eskilod
 */ 


#ifndef ADC_H_
#define ADC_H_


void adc_init(void);
uint8_t adc_read(uint8_t channel);
void pos_calibrate(void);
//pos_t pos_read(void);

#endif /* ADC_H_ */