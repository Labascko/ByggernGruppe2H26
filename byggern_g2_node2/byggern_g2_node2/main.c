/*
 * byggern_g2_node2.c
 *
 * Created: 05.10.2026 13:17:52
 * Author : eskilod
 */ 

#include <stdio.h>
#include <unistd.h>
#include <time.h>

#include "sam.h"
#include "uart.h"
#include "time.h"
#include "can.h"

#define F_CPU 84000000


volatile uint8_t IR_flag = 0;
void PIOA_Handler(void)
{
	//Les status register
	uint32_t status = PIOA->PIO_ISR;
	//Sjekk om det er PA28 som utløste interrupten
	if (status & (1u << 28))
	{
		IR_flag = 1;
	}
}

volatile uint8_t adc_up_flag = 0;
volatile uint8_t adc_down_flag = 0;
void ADC_Handler(void) {
	uint32_t status = ADC->ADC_ISR;
	// Sjekk om det var comparison interrupt som utl�ste avbruddet
	if (status & ADC_ISR_COMPE) {
		// Hva er n�v�rende tilstand?
		if ((ADC->ADC_EMR & ADC_EMR_CMPMODE_Msk) == ADC_EMR_CMPMODE_HIGH) {
			// Vi passerte H�Y terskel (> 2200)
			// Endre hardware til � vente p� at signalet faller under LAV terskel (< 1800)
			ADC->ADC_EMR = (ADC->ADC_EMR & ~ADC_EMR_CMPMODE_Msk) | ADC_EMR_CMPMODE_LOW;
			adc_up_flag = 1;
		}
		else {
			// Vi passerte LAV terskel (< 1800) 
			// Endre hardware til � vente p� at signalet stiger over H�Y terskel (> 2200)
			ADC->ADC_EMR = (ADC->ADC_EMR & ~ADC_EMR_CMPMODE_Msk) | ADC_EMR_CMPMODE_HIGH;
			adc_down_flag = 1;
		}
	}
}

volatile int16_t error_integral = 0;
int16_t error = 0;
int16_t posref = 0;
int16_t k_p = 3;
int16_t k_i = 1;
// T = 100ms
volatile uint8_t solenoid_on = 0;
volatile uint8_t solenoid_loopcounter = 0;
void TC3_Handler(void) {
	//Ikke reguler dersom feilen i posisjon er under 150
	if(abs(error)>150){
		error_integral += error;
		//"Clamping" på integrator slik at denne ikke kan integreres opp til veldig høye verdier
		if (error_integral>2000){
			error_integral =2000;
		}else if(error_integral<-2000){
			error_integral = -2000;
		}
		//Beregning av pådrag
		int16_t input = k_p  * error + k_i * error_integral/5;
		if(input<0){
			PIOC->PIO_SODR = (1u << 23);  // HIGH motorretning
			pwm_set_duty(20000+input);
		} else{
			PIOC->PIO_CODR = (1u << 23);  // LOW motorretning
			pwm_set_duty(20000-input);
		}
	}else{
		//0 hastighet
		pwm_set_duty(20000);
	}
	
	//Solenoiden "tjuvlytter" på timeren for å bestemme hvor lenge pinnen dens skal være høy
	if(solenoid_on == 1){
		solenoid_loopcounter += 1;
	}
	if((solenoid_on == 1) && (solenoid_loopcounter > 5)){
		solenoid_on = 0;
		solenoid_loopcounter = 0;
		//Skru av solenoidepinne
		PIOB->PIO_CODR = (1u << 14);  
	}
	
	//Interrupt fjærnes ved å lese status registeret
	volatile uint32_t status = TC1->TC_CHANNEL[0].TC_SR;
}

int main(void)
{
	//Initialiser SAM systemet
    SystemInit();
	
	//Skru av Watchdog Timer
	WDT->WDT_MR = WDT_MR_WDDIS; 
	
	//Initialiers UART med 9600 baudrate
    uart_init(F_CPU, 9600);
    
	//Initialier ADC, inkludert hardware comparison interrupt
	adc_init();
	
	//Initialiser timer 3
	timer3_init();
	


	/*Koden under for både solenoide og motorretning skulle egentlig legges i funksjoner.
	Men grunnet at siste endringer gjøres borte fra lab-oppsettet nedprioriterte vi dette, da vi ikke ønsker å tukle med filstrukturen*/
	//Ordne pinne B14 (D53) som styringssignal til solenoide:
	//Skru på PCIO clock
	PMC->PMC_PCER0 = (1u << ID_PIOB);
	//Gi pinnen strøm
	PIOB->PIO_PER = (1u << 14);
	//Pinnen settes som utgang
	PIOB->PIO_OER = (1u << 14);
	
	//Styre solenoide
	//PIOB->PIO_SODR = (1u << 14);  // HIGH (solenoide p�)
	//PIOB->PIO_CODR = (1u << 14);  // LOW (solenoide av)

	
	//Konfigurer pinne PC23 som output for å styre retning på motor
	//Skru på PCIO clock 
    PMC->PMC_PCER0 = (1u << ID_PIOC);
	
    PIOC->PIO_PER = (1u << 23);
	//Pinnen settes som utgang
    PIOC->PIO_OER = (1u << 23);
	
	//PIOC->PIO_SODR = (1u << 23);  // HIGH
	PIOC->PIO_CODR = (1u << 23);  // LOW
	

	//Kode for å konfigurere PA28 som input interrupt. Mulig denne ikke ble brukt allikevel.
	//Skru på PCIO clock
	PMC->PMC_PCER0 = (1u << ID_PIOA);
	//Sett som vanlig IO
	PIOA->PIO_PER = (1u << 28);
	
	PIOA->PIO_ODR = (1u << 28);
	// Skru av interrupts under oppsett
	PIOA->PIO_IDR = (1u << 28);
	//Skru på "avansert" interrupt mode
	PIOA->PIO_AIMER = (1u << 28);
	//Velg flanke deteksjon
	PIOA->PIO_ESR = (1u << 28);
	// Enable Falling Edge detection
	PIOA->PIO_FELLSR = (1u << 28);
	// Clear any old pending interrupts
	(void)PIOA->PIO_ISR;
	// Skru på interrupt PA28 lytter etter falling edge
	PIOA->PIO_IER = (1u << 28);
	NVIC_ClearPendingIRQ(PIOA_IRQn);
	// Skru på interrupt på port A
	NVIC_EnableIRQ(PIOA_IRQn);

	

	//init av CAN
	can_init((CanInit){.brp = 21-1, .phase1 = 4-1, .phase2 = 4-1, .propag = 7-1, .sjw = 0}, 1);
	received_msg_flag = 0;
	
	servo_pwm_init();
	timer_encoder_init();
	
	pwm_init();
	servo_pwm_set_duty(1500);
	
	
	int16_t servopos = 1500;
	pwm_set_duty(15000);

	printf("Starting node 2:\n\r");
    while(1) 
    {
		int16_t position = TC2->TC_CHANNEL[0].TC_CV;
		error = posref - position;	
		
		if(received_msg_flag){
			//Kode som kj�res n�r CAN melding mottas, oppdatert meldingsverdi i received_msg
			if(received_msg.id == 0x0003){
				//Siste utgave som bruker en melding for å styre alle signalene
				
				//Servoposisjon settes basert på joystick x
				servopos = (int16_t)(servopos - 2*(int8_t)received_msg.byte[0]);
				if(servopos > 2100){
					servopos = 2100;
					}else if(servopos < 900){
					servopos = 900;
				}
				servo_pwm_set_duty(servopos);
				
				//Posisjonsreferanse settes basert på touchpad
				posref = (int16_t)(((int8_t)received_msg.byte[4])*(-1)+100)*10;
				
				//Joystick knapp trykket
				if(received_msg.byte[2]==1){
					//skru på solenoide
					PIOB->PIO_SODR = (1u << 14);
					solenoid_on = 1;
				}
			}	
			if(received_msg.id == 0x0005){
				//Joystick posisjon fra node 1
				servopos = (int16_t)(servopos - 2*(int8_t)received_msg.byte[0]);
				if(servopos > 2100){
					servopos = 2100;
				}else if(servopos < 900){
					servopos = 900;
				}
				servo_pwm_set_duty(servopos);
			}
			if(received_msg.id == 0x0006){
				//Touchpad posisjon fra node 1
				posref = (int16_t)(((int8_t)received_msg.byte[1])*(-1)+100)*10;
			}
			
			received_msg_flag = 0;
		}
		if(adc_down_flag){
			//Send CAN-melding om mål dersom ADC-en registrerer kryssing av nedre grense
			CanMsg goal = {.id = 4, .length = 1, .byte[0]=1};
			can_tx(goal);
			adc_down_flag = 0;
		}
		if(adc_up_flag){
			adc_up_flag = 0;
		}
			
    }
}

