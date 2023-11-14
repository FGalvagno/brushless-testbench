/*
 * HX711 library for LPC17XX
*/

#include "lpc17xx_hx711.h"
#define __USE_C99_MATH

uint8_t GAIN;
uint32_t OFFSET;
float SCALE;

uint8_t HX711_is_ready() {
    return (DOUT_READ == 0);
}

void HX711_set_gain(uint8_t gain) {
    switch (gain) {
        case 128:        // channel A, gain factor 128
            GAIN = 1;
            break;
        case 64:        // channel A, gain factor 64
            GAIN = 3;
            break;
        case 32:        // channel B, gain factor 32
            GAIN = 2;
            break;
    }

    PD_SCK_SET_LOW;
    HX711_read();
}



long HX711_read() {
    while (!HX711_is_ready());

    unsigned long value = 0;
    uint8_t data[3] = { 0 };
    uint8_t filler = 0x00;

    //obtenemos medidas del hx711 pulsando 24 veces el clock
    for(int8_t n = 2; n>=0; n--) {
        for(int8_t i=7; i>=0; i--) {
            PD_SCK_SET_HIGH;
            delay1us(); // clock delay
            data[n] |= DOUT_READ << i;
            PD_SCK_SET_LOW;
            delay1us(); // clock delay
        }
    }
    // preparamos el canal y la ganancia para la siguiente medida
    for (unsigned int i = 0; i < GAIN; i++) {
        PD_SCK_SET_HIGH;
        delay1us(); // clock delay
        PD_SCK_SET_LOW;
        delay1us();
    }

    // replicamos el MSB para crear una variable de 32bits signada
    if (data[2] & 0x80) {
        filler = 0xFF;
    } else {
        filler = 0x00;
    }

    // Construct a 32-bit signed integer
    value = ( (unsigned long)(filler) << 24
            | (unsigned long)(data[2]) << 16
            | (unsigned long)(data[1]) << 8
            | (unsigned long)(data[0]) );

    return (long)(value);
}

long HX711_read_average(uint8_t times) {
    long sum = 0;
    for (uint8_t i = 0; i < times; i++) {
        sum += HX711_read();
//         yield();
    }
    return sum / times;
}

double HX711_get_value() {
    return HX711_read_average(1) - OFFSET;
}

double HX711_get_mean_value(uint8_t times) {
    return HX711_read_average(times) - OFFSET;
}

float HX711_get_units() {
    return HX711_get_value() / SCALE;
}

float HX711_get_mean_units(uint8_t times) {
    return HX711_get_mean_value(times) / SCALE;
}

void HX711_tare(uint8_t times) {
    double sum = HX711_read_average(times);
    HX711_set_offset(sum);
}

void HX711_set_scale(float scale) {
    SCALE = scale;
}

float HX711_get_scale() {
    return SCALE;
}

void HX711_set_offset(long offset) {
    OFFSET = offset;
}

long HX711_get_offset() {
    return OFFSET;
}

void HX711_power_down() {
    PD_SCK_SET_LOW;
    PD_SCK_SET_HIGH;
}

void HX711_power_up() {
    PD_SCK_SET_LOW;
}
