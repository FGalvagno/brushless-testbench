/*brushless-testbench
 *periféricos usados: GPDMA, ADC, TIMER0, GPIO
 *
 */

#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_pinsel.h"
#include "stdlib.h"
#include "stdio.h"

#include "lpc17xx_hx711.h"
#include "pins.h"
#include "perip.h"
unsigned int count = 1000;
 
unsigned int ActualTick = 0;
int ActualRPM = 0;
int ActualPWM = 12;
float ActualThrust = 500.3;

int Channel0_TC;
int Channel0_Err;


char ActualTickArray [MAX_ARRAY];
char ActualPWMArray  [MAX_ARRAY];
char ActualRPMArray  [MAX_ARRAY];
char ActualThrustArray [MAX_ARRAY];

volatile uint32_t rawThrust; //Valor ADC
volatile uint32_t rawRPM; //Valor RPM
void delay1us(void);
void send_bench_data(void);
void HX711_INICIO(void);

char command;
int state;

int main(void) {
	//Config pins and periperials.
	config_pins();
	config_perip();

	for(uint8_t i=0; i==100; i++){
		delay1us();
	}


	GPDMA_LLI_Type LLI1;
	LLI1.SrcAddr = (uint32_t) &LPC_ADC->ADDR0;
	LLI1.DstAddr = (uint32_t) &rawThrust;
	LLI1.NextLLI = (uint32_t) &LLI1;
	LLI1.Control = 32
				   | (2<<18)     //source width 32 bits
				   | (2<<21);     //dest width 32 bits

	GPDMA_Init();

	GPDMA_Channel_CFG_Type DMACFG;
	DMACFG.ChannelNum=0;
	DMACFG.SrcMemAddr=0;
	DMACFG.DstMemAddr=(uint32_t) &rawThrust;
	DMACFG.TransferSize=32;
	DMACFG.TransferType=GPDMA_TRANSFERTYPE_P2M;
	DMACFG.TransferWidth=0;
	DMACFG.SrcConn=GPDMA_CONN_ADC;
	DMACFG.DstConn=0;
	DMACFG.DMALLI=(uint32_t) &LLI1;
	GPDMA_Setup(&DMACFG);

	NVIC_EnableIRQ(DMA_IRQn);
	GPDMA_ChannelCmd(0, ENABLE);


	HX711_INICIO();


    while(1) {
    	switch (state){
    	case 1:
    	ActualPWM = ((rawThrust>>4)&0xFFF)/4.095;
    	update_PWM((rawThrust>>4)&0xFFF);
    	break;

    	case 2:
        ActualPWM = 500;
        PWM_MatchUpdate(LPC_PWM1, 1, 37500, PWM_MATCH_UPDATE_NEXT_RST);

        break;
    	}

    	ActualThrust=HX711_get_mean_units(5);       //mide aplicando offset y scale


    	send_bench_data();


    	delay1us();


    		}
//        GPDMA_Setup(&DMACFG);
    return 0 ;
}

void delay1us(void){

    LPC_TIM0->IR |= 0x3F;      // Clear IF
    LPC_TIM0->TCR = 3;         // Enable and Reset
    LPC_TIM0->TCR &= ~2;

    while(LPC_TIM0->IR & 1);

}


void send_bench_data(void){
	itoa(ActualTick, ActualTickArray, 10);
	sprintf(ActualRPMArray, "%d",ActualRPM);
	itoa(ActualPWM, ActualPWMArray, 10);
	sprintf(ActualThrustArray, "%.2f", ActualThrust);

	UART_Send(LPC_UART0, ActualTickArray, sizeof(ActualTickArray), BLOCKING);
	UART_Send(LPC_UART0, ",", 1, BLOCKING);


	UART_Send(LPC_UART0, ActualPWMArray, sizeof(ActualPWMArray), BLOCKING);
	UART_Send(LPC_UART0, ",", 1, BLOCKING);

	UART_Send(LPC_UART0, ActualRPMArray, sizeof(ActualRPMArray), BLOCKING);
	UART_Send(LPC_UART0, ",", 1, BLOCKING);

	UART_Send(LPC_UART0, ActualThrustArray, sizeof(ActualThrustArray), BLOCKING);
	UART_Send(LPC_UART0, "\n\r", sizeof("\n\r"), BLOCKING);

}


void HX711_INICIO(void){

	HX711_set_gain(128);

	HX711_tare(5);      //Hago una medición limpia para encontrar el offset.

	HX711_set_scale(441);                //441 Scale calculado por medición práctica

	NVIC_EnableIRQ(EINT3_IRQn);          //Pulsador para setear el offset

}

void EINT3_IRQHandler(void){

	HX711_tare(5);  //Hago una medición limpia para encontrar el offset.
	GPIO_ClearInt(PORT2,PIN10);
}

void TIMER2_IRQHandler(void){
    rawRPM++;

    TIM_ClearIntPending(LPC_TIM2, TIM_CR0_INT);
}


void DMA_IRQHandler (void)
{
	// check GPDMA interrupt on channel 0
	if (GPDMA_IntGetStatus(GPDMA_STAT_INT, 0)){ //check interrupt status on channel 0
		// Check counter terminal status
		if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0)){
			// Clear terminate counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, 0);
				Channel0_TC++;

		}
		if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, 0)){
			// Clear error counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, 0);
			Channel0_Err++;
		}
	}
}

void SysTick_Handler(void){
	ActualTick++; //1 tick = 1ms
	if(count-ActualTick == 0){
		count = ActualTick+1000;
		ActualRPM = rawRPM*60/2;
		rawRPM = 0;
	}
	SysTick->CTRL &= SysTick->CTRL; //Clear flag
}

void UART0_IRQHandler(void) {

    // Comprobar si la interrupción es por recepción de datos
    if (UART_GetIntId(LPC_UART0) & UART_IIR_INTID_RDA) {
        command = UART_ReceiveByte(LPC_UART0); // Leer el dato recibido

    	if(command == 'L')
    		state = 1;
		if (command == 'H')
    		state = 2;
 }
}

