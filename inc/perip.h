/*
 * perip.h
 *
 *  Created on: Nov 13, 2023
 *      Author: facundog
 */

#ifndef PERIP_H_
#define PERIP_H_

#include "lpc17xx_pwm.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_uart.h"


#define MAX_ARRAY 16

void config_perip(void);
void config_DMA(void);
void config_ADC(void);
void config_TIMER0(void);
void config_UART(void);
void config_SysTick(void);

void update_PWM(int newValue);

#endif /* PERIP_H_ */
