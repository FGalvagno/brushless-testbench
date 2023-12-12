# brushless-testbench
Testbench para motores brushless y hélices, implementado con LPC1769

El objetivo de este trabajo es el desarrollo de un banco de pruebas para motores BLDC de aeromodelismo utilizando como interfaz digital para tomar medidas un LPC1769.

Las medidas que se necesitan tomar del conjunto hélice-motor son:
- Empuje
- RPM
- % del PWM
- Además, se requiere acompañar las mediciones con una marca de tiempo, para posteriormente facilitar el cálculo del modelo matemático.

# Hardware
El hardware necesario para ensamblar el proyecto es el siguiente:
- Celda de carga 5Kg
- MCU: LPC1769
- Botones
- Conversor UART/USB
- IC: HX711.
- 1 x Potenciómetro
- Motor brushless con hélice
- ESC adecuado para el motor
- Sensor IR

# Esquema de conexión

![1](https://github.com/FGalvagno/brushless-testbench/blob/main/img/1.png)
