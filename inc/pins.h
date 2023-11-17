
#ifndef PINS_H
#define PINS_H

#define PWM_PORT    1
#define PWM_PIN     18

#define ADC_PORT    0
#define ADC_PIN     23

#define SERIAL_TX_PORT    0
#define SERIAL_TX_PIN     2

#define SERIAL_RX_PORT    0
#define SERIAL_RX_PIN     3

#define HX_711_DATA_PORT    0
#define HX_711_CLK_PORT     0
#define HX_711_DATA_PIN    19
#define HX_711_CLK_PIN     20

#define RPM_COUNTER_PORT    0
#define RPM_COUNTER_PIN	    4

#define PIN10  ((uint32_t) (1<<10))
#define PORT2  ((uint8_t)  (2))
#define INPUT  ((uint8_t)  (0))

void config_pins(void);


#endif /* PINS_H */
