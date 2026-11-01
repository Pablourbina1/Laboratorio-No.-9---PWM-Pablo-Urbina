#ifndef configLED_H_
#define configLED_H_
#include "RTE_Components.h"
#include CMSIS_device_header

// Mascara
#define MASK(x) (1 << (x))

// Pines de entrada (Potenciometro y Fotoresistor)
#define PIN_FTR 0 // PA0 -> ADC1_IN1
#define PIN_POT 1 // PA1 -> ADC1_IN2

// Pines de salida (LEDs)
#define PIN_LD1 7
#define PIN_LD2 6
#define PIN_LD3 5
#define PIN_LD4 4
#define PIN_PWM 8 // PWM

#define PWM_ARR 4095 // Periodo del PWM

#define OSCURIDAD 2500   // Fotoresistor por encima de lo oscuro

extern volatile uint16_t valor_ftr;   // PA0, fotoresistor (0 a 4095)
extern volatile uint16_t valor_pot;   // PA1, potenciometro (0 a 4095)

void habilitar_clock(); // Habilitar la señal de reloj de los GPIO A B

void config_POT(); // Configurar el potenciometro

void config_FTR(); // Configurar el fotoresistor

void config_LEDs(); // Configurar los pines LEDs como salida (MODER = 01)

void config_ADC(); // Configurar ADC

void config_PWM(); // Configurar PWM
void set_duty(uint16_t valor); // En 0 esta apagado, en PWM_ARR esta en brillo maximo

// Control de LEDs
void encender(int pin);
void apagar_led(int pin);

void apagar();

void nivel(int nivel);


#endif /* configLED_H_ */