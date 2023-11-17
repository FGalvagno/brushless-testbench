#include "perip.h"



/****** PERIP CONFIG ************/
void config_perip(){
	config_PWM();
	config_ADC();
	config_TIMER0();
	config_TIMER2();
	config_UART();
	config_SysTick();
}

void config_PWM(){

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

void config_ADC(void){

	ADC_Init(LPC_ADC, 200000);                             //Alimenta y acomoda el clock del adc para que su fs sea 200KHz
	ADC_BurstCmd(LPC_ADC, ENABLE);                         //Configuración del modo burst
	ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);           //Configuración del inicio de conversión
	ADC_ChannelCmd(LPC_ADC, 0, ENABLE);                    //Selección de canal de conversión
	LPC_ADC->ADGDR &= LPC_ADC->ADGDR;                      //Lee el Global Data para limpiar flag de interrupción
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN5, ENABLE);          //Activación de interrupción de ADC para que haya DMA request
	//No activo el NVIC para que el core no me atienda la inter y solo haya una trasferencia DMA

}



void config_TIMER0(void){
	 LPC_SC->PCONP |= (1 << 1);
	 LPC_SC->PCLKSEL0 |= (1 << 2); // PCLK = cclk

	 LPC_TIM0->PR = 19;

	 LPC_TIM0->MR0 = 4;
	 LPC_TIM0->MCR = 2;         // Timer0 reset on Match0
}

void config_TIMER2(void){
    TIM_TIMERCFG_Type timCfg;
    timCfg.PrescaleOption = TIM_PRESCALE_TICKVAL;
    timCfg.PrescaleValue = 1;

	TIM_CAPTURECFG_Type capCfg;
	capCfg.CaptureChannel = 0;
	capCfg.FallingEdge = DISABLE;
	capCfg.IntOnCaption = ENABLE;
	capCfg.RisingEdge = ENABLE;

	TIM_Init(LPC_TIM2, TIM_TIMER_MODE, (void*) &timCfg);
	TIM_ConfigCapture(LPC_TIM2, &capCfg);
	TIM_Cmd(LPC_TIM2, ENABLE);

	NVIC_EnableIRQ(TIMER2_IRQn);
}

void config_UART(void){
	UART_CFG_Type UARTConfigStruct;
	UART_ConfigStructInit(&UARTConfigStruct);
	UART_Init(LPC_UART0, &UARTConfigStruct);

	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);

	UART_TxCmd(LPC_UART0, ENABLE);
    UART_IntConfig(LPC_UART0, UART_INTCFG_RBR, ENABLE);
    UART_IntConfig(LPC_UART0, UART_INTCFG_RLS, ENABLE);
	NVIC_EnableIRQ(UART0_IRQn);

}

void config_SysTick(void){
	SysTick_Config(SystemCoreClock/1000); //Configuramos SysTick para 1ms
}

/********* PERIP CONTROL ******************/

void update_PWM(int newValue){
	int var = newValue*(25000/4096)+25000;

	PWM_MatchUpdate(LPC_PWM1, 1, var, PWM_MATCH_UPDATE_NEXT_RST);
}


