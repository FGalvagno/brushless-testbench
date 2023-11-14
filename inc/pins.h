
#ifndef PINS_H
#define PINS_H

#define PWM_PORT    1
#define PWM_PIN     18

#define ADC_PORT    1
#define ADC_PIN     31

#define SERIAL_TX_PORT    0
#define SERIAL_TX_PIN     2

#define HX_711_DATA_PORT    0
#define HX_711_CLK_PORT     0
#define HX_711_DATA_PIN    19
#define HX_711_CLK_PIN     20

void config_pins(void);


#endif /* PINS_H */
