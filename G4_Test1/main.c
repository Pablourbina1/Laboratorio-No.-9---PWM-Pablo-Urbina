#include "RTE_Components.h"
#include CMSIS_device_header
#include "stm32g431xx.h"

#include "configLED.h"

int main() {
    habilitar_clock();
    config_POT();
    config_FTR();
    config_LEDs();
    config_PWM();
    config_ADC();

    while (1){
        // Copia local de los valores
        uint16_t pot = valor_pot;
        uint16_t ftr = valor_ftr;

        nivel((pot * 5) >> 12);
        // LED PWM
        if (ftr > OSCURIDAD){
            set_duty(pot);
        } else {
            set_duty(0);
        }
    }
}
