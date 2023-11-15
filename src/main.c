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

#define PIN10  ((uint32_t)  (1<<10))
#define PORT2  ((uint8_t)   (2))
#define INPUT  ((uint8_t)   (0))

unsigned int ActualTick = 0;
int ActualRPM = 16000;
int ActualPWM = 12;
float ActualThrust = 500.3;

int Channel0_TC;
int Channel0_Err;


char ActualTickArray [MAX_ARRAY];
char ActualPWMArray  [MAX_ARRAY];
char ActualRPMArray  [MAX_ARRAY];
char ActualThrustArray [MAX_ARRAY];

volatile uint32_t rawThrust; //Valor ADC

void delay1us(void);
void send_bench_data(void);
void HX711_INICIO(void);
int map_pwm(void);

long reading=0;

int main(void) {
	//Config pins and periperials.
	config_pins();
	config_perip();


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
    	ActualPWM = ((rawThrust>>4)&0xFFF)/4.095;
    	update_PWM((rawThrust>>4)&0xFFF);



    	send_bench_data();


    	delay1us();
        GPDMA_Setup(&DMACFG);
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


void HX711_INICIO(void){

	HX711_set_gain(128);

	HX711_set_offset(HX711_read());      //Hago una medición limpia para encontrar el offset.

	HX711_set_scale(441);                //441 Scale calculado por medición práctica

	NVIC_EnableIRQ(EINT3_IRQn);          //Pulsador para setear el offset

}

void EINT3_IRQHandler(void){

	HX711_set_offset(HX711_read());  //Hago una medición limpia para encontrar el offset.
	GPIO_ClearInt(PORT2,PIN10);
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
	ActualTick++;

	SysTick->CTRL &= SysTick->CTRL; //Clear flag
}


