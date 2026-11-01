#include "configLED.h"
#include "stm32g431xx.h"
#include <stdint.h>
#include <stdio.h>

volatile uint16_t valor_ftr = 0;
volatile uint16_t valor_pot = 0;

void habilitar_clock(){
    RCC -> AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_ADC12EN;
    RCC -> APB2ENR |= RCC_APB2ENR_TIM1EN;
}

// El potenciometor y el fotoresistor son entradas analogicas, van en 11

void config_POT(){
    GPIOA -> MODER |= (3 << (2 * PIN_POT));
}

void config_FTR(){
    GPIOA -> MODER |= (3 << (2 * PIN_FTR));
}

void config_LEDs(){
    // Configurar los pines de salida, primero se limpia y despues se pone en 01
    GPIOA -> MODER &= ~(3 << (2 * PIN_LD1));
    GPIOA -> MODER |= (1 << (2 * PIN_LD1));

    GPIOA -> MODER &= ~(3 << (2 * PIN_LD2));
    GPIOA -> MODER |= (1 << (2 * PIN_LD2));

    GPIOA -> MODER &= ~(3 << (2 * PIN_LD3));
    GPIOA -> MODER |= (1 << (2 * PIN_LD3));

    GPIOA -> MODER &= ~(3 << (2 * PIN_LD4));
    GPIOA -> MODER |= (1 << (2 * PIN_LD4));

    // PWM, MODER en 10
    GPIOA -> MODER &= ~(3 << (2 * PIN_PWM));
    GPIOA -> MODER |= (2 << (2 * PIN_PWM));

    // AF6 = TIM1_CH1
    GPIOA -> AFR[1] &= ~(0xF << (4 * (PIN_PWM - 8)));
    GPIOA -> AFR[1] |= (6 << (4 * (PIN_PWM - 8)));
}

void config_ADC() {
    // Reloj de ADC
    ADC12_COMMON -> CCR &= ~(3 << 16);
    ADC12_COMMON -> CCR |= (3 << 16);
    // Salir de deep-power-down y encender regulador
    ADC1 -> CR &= ~ADC_CR_DEEPPWD;
    ADC1 -> CR |= ADC_CR_ADVREGEN;
    for (volatile int i = 0; i < 10000; i++);

    // Calibracion single ended
    ADC1 -> CR &= ~ADC_CR_ADCALDIF;
    ADC1 -> CR |= ADC_CR_ADCAL;
    while (ADC1 -> CR & ADC_CR_ADCAL);

    ADC1 -> CFGR |= ADC_CFGR_CONT;
    // Secuencia: PA0 y PA1
    ADC1 -> SQR1 = (1 << 0) | (1 << 6) | (2 << 12);
    // Tiempo de muestreo largo
    ADC1 -> SMPR1 |= (7 << 3) | (7 << 6);
    // Interrupcion al terminar cada conversion
    ADC1 -> IER |= ADC_IER_EOCIE;
    NVIC_EnableIRQ(ADC1_2_IRQn);

    // Habilitar y iniciar
    ADC1 -> ISR |= ADC_ISR_ADRDY;
    ADC1 -> CR |= ADC_CR_ADEN;
    while (!(ADC1 -> ISR & ADC_ISR_ADRDY));
    ADC1 -> CR |= ADC_CR_ADSTART;
}

// Rutina de interrupcion
void ADC1_2_IRQHandler(void){
    if(ADC1 -> ISR & ADC_ISR_EOC){
        uint16_t dato = ADC1 -> DR;
        if(ADC1 -> ISR & ADC_ISR_EOS){
            valor_pot = dato;
            ADC1 -> ISR = ADC_ISR_EOS;
        } else {
            valor_ftr = dato;
        }
    }

    if (ADC1 -> ISR & ADC_ISR_OVR){
        ADC1 -> ISR = ADC_ISR_OVR;
    }
}

void config_PWM(){
    // Periodo
    TIM1 -> PSC = 0;
    TIM1 -> ARR = PWM_ARR;
    TIM1 -> CCR1 = 0;

    // Canal 1 en PWM modo 1 con preload
    TIM1 -> CCMR1 &= ~TIM_CCMR1_OC1M;
    TIM1 -> CCMR1 |= (6 << TIM_CCMR1_OC1M_Pos);
    TIM1 -> CCMR1 |= TIM_CCMR1_OC1PE;
    // Preload de ARR
    TIM1 -> CR1 |= TIM_CR1_ARPE;
    // Habilitar salidas
    TIM1 -> CCER |= TIM_CCER_CC1E;
    TIM1 -> BDTR |= TIM_BDTR_MOE;
    // Cargar preload y iniciar
    TIM1 -> EGR |= TIM_EGR_UG;
    TIM1 -> CR1 |= TIM_CR1_CEN;
}

void set_duty(uint16_t valor){
    if (valor > PWM_ARR){
        valor = PWM_ARR;
    }
    TIM1 -> CCR1 = valor;
}
// Encender y apagar los LEDs

void encender(int pin){
    GPIOA -> ODR |= (MASK(pin));
}

void apagar_led(int pin){
    GPIOA -> ODR &= ~(MASK(pin));
}

void apagar(){
    apagar_led(PIN_LD1);
    apagar_led(PIN_LD2);
    apagar_led(PIN_LD3);
    apagar_led(PIN_LD4);
}

// Nivel 0 a 4, se enciende ese numero de LEDs
void nivel(int nivel){
    uint32_t patron = (1 << nivel) - 1; // 0->0000, 1->0001, 2->0011, 3->0111, 4->1111

    GPIOA -> ODR &= ~(0xF << PIN_LD4); // Apagar los 4 LEDs
    GPIOA -> ODR |= (patron << PIN_LD4); // Encender los LEDs correspondientes
}