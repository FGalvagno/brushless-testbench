#include "pins.h"
#include "LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

#define PIN10  ((uint32_t) (1<<10))
#define PORT2  ((uint8_t)  (2))
#define INPUT  ((uint8_t)  (0))

void config_pins(void){

	//PWM
	PINSEL_CFG_Type pinCfg;
	pinCfg.Funcnum = 2;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Portnum = PWM_PORT;
	pinCfg.Pinnum = PWM_PIN;
	PINSEL_ConfigPin(&pinCfg);

	//CAP2.0
	pinCfg.Funcnum = 3;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Portnum = RPM_COUNTER_PORT;
	pinCfg.Pinnum = RPM_COUNTER_PIN;
	PINSEL_ConfigPin(&pinCfg);

	//SERIAL
	pinCfg.Funcnum = 1;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Pinnum = SERIAL_TX_PIN;
	pinCfg.Portnum = SERIAL_TX_PORT;
	PINSEL_ConfigPin(&pinCfg);

	//ADC
	pinCfg.Funcnum = 1;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Portnum = ADC_PORT;
	pinCfg.Pinnum = ADC_PIN;
	PINSEL_ConfigPin(&pinCfg);
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

	//Switch integrado para seteat offset
	pinCfg.Portnum = PINSEL_PORT_2;
	pinCfg.Pinnum = PINSEL_PIN_10;                  //integrated switch para offset
	pinCfg.Funcnum = PINSEL_FUNC_0;                 //P2.10 as GPIO
	pinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PINSEL_ConfigPin(&pinCfg);
    GPIO_SetDir(PORT2, PIN10, INPUT);
    GPIO_IntCmd(PORT2,PIN10, 1);                    //interrupcion por flanco descendente
    GPIO_ClearInt(PORT2,PIN10);                     //Limpio flags por las dudas


}
