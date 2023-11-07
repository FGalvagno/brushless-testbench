#include "LPC17xx.h"

#ifndef HX711_h
#define HX711_h

#define SCK_PORT         LPC_GPIO0->FIOPIN
#define SCK_PIN          20
#define PD_SCK_SET_HIGH     SCK_PORT |= (1<<SCK_PIN)
#define PD_SCK_SET_LOW      SCK_PORT &= ~(1<<SCK_PIN)

#define DOUT_PORT           LPC_GPIO0->FIOPIN
#define DOUT_PIN            19
#define DOUT_READ           (DOUT_PORT & (1<<DOUT_PIN))

#define DOUT_SET_HIGH       DOUT_PORT |= (1<<DOUT_PIN)
#define DOUT_SET_LOW        DOUT_PORT &= ~(1<<DOUT_PIN)


uint8_t GAIN;
long OFFSET;
float SCALE;

void HX711_init(uint8_t gain);
int HX711_is_ready(void);
void HX711_set_gain(uint8_t gain);
uint32_t HX711_read(void);
uint32_t HX711_read_average(uint8_t times);
double HX711_get_value(uint8_t times);
float HX711_get_units(uint8_t times);
void HX711_tare(uint8_t times);
void HX711_set_scale(float scale);
float HX711_get_scale(void);
void HX711_set_offset(double offset);
double HX711_get_offset(void);
void HX711_power_down(void);

void HX711_power_up(void);


uint8_t shiftIn(void);

unsigned long HX711_Read(void);

#endif /* HX711_h */
