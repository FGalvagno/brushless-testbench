#include "pins.h"
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

void config_pins(void){
	//PWM
	PINSEL_CFG_Type pinCfg;
	pinCfg.Funcnum = 2;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Portnum = PWM_PORT;
	pinCfg.Pinnum = PWM_PIN;
	PINSEL_ConfigPin(&pinCfg);

	//SERIAL
	pinCfg.Funcnum = 1;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Pinnum = SERIAL_TX_PIN;
	pinCfg.Portnum = SERIAL_TX_PORT;
	PINSEL_ConfigPin(&pinCfg);

	//ADC
	pinCfg.Funcnum = 3;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 2;
	pinCfg.Portnum = ADC_PORT;
	pinCfg.Pinnum = ADC_PIN;
	PINSEL_ConfigPin(&pinCfg);

	//HX_711
	pinCfg.Funcnum = 0;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Portnum = HX_711_DATA_PORT;
	pinCfg.Pinnum = HX_711_CLK_PIN;
	PINSEL_ConfigPin(&pinCfg);

	pinCfg.Funcnum = 0;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Portnum = HX_711_DATA_PORT;
	pinCfg.Pinnum = HX_711_DATA_PIN;
	PINSEL_ConfigPin(&pinCfg);

	GPIO_SetDir(0, 1<<20, 1); //20clk

	GPIO_SetDir(0, 1<<19, 0); //19data

}
