/*brushless-testbench
 *periféricos usados: GPDMA, ADC, TIMER0, GPIO
 *
 *
 *
 */


#include "LPC17xx.h"
#include "lpc17xx_pwm.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"

void configPinPWM();
void configPWM();
void configADC();
void configDMA();

long value; //Valor ADC
long reading;
double reading_avg;
double reading_value;
uint8_t times = 5;

int main(void) {
	HX711_init();
	initTimer0();
	configPinPWM();
	configPWM();
	configADC();
	configDMA();

	HX711_tare(5);
    while(1) {

    reading =	HX711_read(1);
    //reading_avg = HX711_read_average();
    //reading_value = HX711_get_value();
    delay1us();
    }
    return 0 ;
}

void configPinPWM()
{
	PINSEL_CFG_Type pinCfg;
	pinCfg.Funcnum = 2;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Portnum = 1;
	pinCfg.Pinnum = 18;
	PINSEL_ConfigPin(&pinCfg);
}

void configPWM(){

	PWM_TIMERCFG_Type PWMconfig;
		PWMconfig.PrescaleOption=PWM_TIMER_PRESCALE_TICKVAL;
		PWMconfig.PrescaleValue=1;

		PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &PWMconfig);

		PWM_MATCHCFG_Type match0;
		match0.IntOnMatch=DISABLE;
		match0.MatchChannel=0;
		match0.ResetOnMatch=ENABLE;
		match0.StopOnMatch=DISABLE;

		PWM_ConfigMatch(LPC_PWM1, &match0);

		PWM_MATCHCFG_Type match1;
		match1.IntOnMatch=DISABLE;
		match1.MatchChannel=1;
		match1.ResetOnMatch=DISABLE;
		match1.StopOnMatch=DISABLE;

		PWM_ConfigMatch(LPC_PWM1, &match1);

		PWM_MatchUpdate(LPC_PWM1, 0, 500000, PWM_MATCH_UPDATE_NEXT_RST); //VER update next rst

		PWM_MatchUpdate(LPC_PWM1, 1, 25000, PWM_MATCH_UPDATE_NEXT_RST);

		PWM_ChannelCmd(LPC_PWM1, 1, ENABLE);

		PWM_ResetCounter(LPC_PWM1);

		PWM_CounterCmd(LPC_PWM1, ENABLE);

		PWM_Cmd(LPC_PWM1, ENABLE);


}

void configADC(void){

	PINSEL_CFG_Type pinCfg;
	pinCfg.Funcnum = 3;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 2;
	pinCfg.Portnum = 1;
	pinCfg.Pinnum = 31;
	PINSEL_ConfigPin(&pinCfg);

	ADC_Init(LPC_ADC, 200000);                             //Alimenta y acomoda el clock del adc para que su fs sea 200KHz
	ADC_BurstCmd(LPC_ADC, ENABLE);                         //Configuración del modo burst
	ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);           //Configuración del inicio de conversión
	ADC_ChannelCmd(LPC_ADC, 5, ENABLE);                    //Selección de canal de conversión
	LPC_ADC->ADGDR &= LPC_ADC->ADGDR;                      //Lee el Global Data para limpiar flag de interrupción
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN5, ENABLE);          //Activación de interrupción de ADC para que haya DMA request
	//No activo el NVIC para que el core no me atienda la inter y solo haya una trasferencia DMA

}

void configDMA(void){

	GPDMA_LLI_Type LLI1;
	LLI1.SrcAddr = (uint32_t) &LPC_ADC->ADDR5;
	LLI1.DstAddr = (uint32_t) &value;
	LLI1.NextLLI = (uint32_t) &LLI1;
	LLI1.Control = 1
				   | (2<<18)     //source width 32 bits
				   | (2<<21);     //dest width 32 bits

	GPDMA_Init();

	GPDMA_Channel_CFG_Type DMACFG;
	DMACFG.ChannelNum=0;
	DMACFG.SrcMemAddr=0;
	DMACFG.DstMemAddr=(uint32_t) &value;
	DMACFG.TransferSize=1;
	DMACFG.TransferType=GPDMA_TRANSFERTYPE_P2M;
	DMACFG.TransferWidth=0;
	DMACFG.SrcConn=GPDMA_CONN_ADC;
	DMACFG.DstConn=0;
	DMACFG.DMALLI=0;
	GPDMA_Setup(&DMACFG);

	GPDMA_ChannelCmd(0, ENABLE);

}

void initTimer0(void){
	 LPC_SC->PCONP |= (1 << 1);
	 LPC_SC->PCLKSEL0 |= (1 << 2); // PCLK = cclk

	 LPC_TIM0->PR = 19;

	 LPC_TIM0->MR0 = 4;
	 LPC_TIM0->MCR = 2;         // Timer0 reset on Match0
}

void delay1us(void)
{


    LPC_TIM0->IR |= 0x3F;      // Clear all interrupt flag
    LPC_TIM0->TCR = 3;         // Enable and Reset
    LPC_TIM0->TCR &= ~2;

    while(LPC_TIM0->IR & 1);


}

void HX711_init(uint8_t gain)
{
	PINSEL_CFG_Type pinCfg;
	pinCfg.Funcnum = 0;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Portnum = 0;
	pinCfg.Pinnum = 20;
	PINSEL_ConfigPin(&pinCfg);

	pinCfg.Funcnum = 0;
	pinCfg.OpenDrain = 0;
	pinCfg.Pinmode = 0;
	pinCfg.Portnum = 0;
	pinCfg.Pinnum = 19;
	PINSEL_ConfigPin(&pinCfg);

	GPIO_SetDir(0, 1<<20, 1); //20clk

	GPIO_SetDir(0, 1<<19, 0); //19data
	HX711_set_gain(128);
}


