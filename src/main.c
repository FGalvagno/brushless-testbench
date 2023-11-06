/*
 * Copyright 2022 NXP
 * NXP confidential.
 * This software is owned or controlled by NXP and may only be used strictly
 * in accordance with the applicable license terms.  By expressly accepting
 * such terms or by downloading, installing, activating and/or otherwise using
 * the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to
 * be bound by the applicable license terms, then you may not retain, install,
 * activate or otherwise use the software.
 */


#include "LPC17xx.h"
#include "lpc17xx_pwm.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_adc.h"

void configPinPWM();
void configPWM();
void configADC();
void configDMA();

uint32_t value; //Valor ADC

int main(void) {


	configPinPWM();
	configPWM();
	configADC();
	configDMA();

    while(1) {

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
