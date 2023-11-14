/*brushless-testbench
 *periféricos usados: GPDMA, ADC, TIMER0, GPIO
 *
 */

#include "LPC17xx.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_gpdma.h"
#include "stdlib.h"
#include "stdio.h"

#include "lpc17xx_hx711.h"
#include "pins.h"
#include "perip.h"


int ActualTick = 85213;
int ActualRPM = 16000;
int ActualPWM = 12;
float ActualThrust = 500.3;

char ActualTickArray [MAX_ARRAY];
char ActualPWMArray  [MAX_ARRAY];
char ActualRPMArray  [MAX_ARRAY];
char ActualThrustArray [MAX_ARRAY];

uint32_t rawThrust = 0; //Valor ADC

void delay1us(void);
void send_bench_data(void);
void map_pwm(void);

int main(void) {
	//Config pins and periperials.
	config_pins();
	config_perip();
	config_DMA();
	//Init HX711
	//HX711_tare(5);
	//HX711_set_gain(128);

    while(1) {
         //map_pwm();
    	int mask = 0xFFF0;
         rawThrust = ((LPC_ADC->ADDR5&mask));;
    send_bench_data();
    }
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
	itoa(ActualRPM, ActualPWMArray, 10);
	itoa(ActualPWM, ActualRPMArray, 10);
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



void map_pwm(void){
	int PWM = ((rawThrust&0xFFF0)>>4)/4096;
	ActualPWM = PWM;
	//MAPEO
}

void config_DMA(void){

	GPDMA_LLI_Type LLI1;
	LLI1.SrcAddr = (uint32_t) &LPC_ADC->ADDR5;
	LLI1.DstAddr = (uint32_t) &rawThrust;
	LLI1.NextLLI = (uint32_t) &LLI1;
	LLI1.Control = 1
				   | (2<<18)     //source width 32 bits
				   | (2<<21);     //dest width 32 bits

	GPDMA_Init();

	GPDMA_Channel_CFG_Type DMACFG;
	DMACFG.ChannelNum=0;
	DMACFG.SrcMemAddr=0;
	DMACFG.DstMemAddr=(uint32_t) &rawThrust;
	DMACFG.TransferSize=1;
	DMACFG.TransferType=GPDMA_TRANSFERTYPE_P2M;
	DMACFG.TransferWidth=0;
	DMACFG.SrcConn=GPDMA_CONN_ADC;
	DMACFG.DstConn=0;
	DMACFG.DMALLI=0;
	GPDMA_Setup(&DMACFG);

	GPDMA_ChannelCmd(0, ENABLE);

}
