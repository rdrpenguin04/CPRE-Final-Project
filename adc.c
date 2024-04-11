#include <inc/tm4c123gh6pm.h>
#include "adc.h"

void adc_init(void)
{
    //enable clock to GPIO port B
    SYSCTL_RCGCGPIO_R |= 0x02;

    //enable clock to ADC0
    SYSCTL_RCGCADC_R |= 0x01;

    //wait for GPIOB and ADC0 peripherals to be ready
    while ((SYSCTL_PRGPIO_R & 0x02) == 0)
    {
    }
    while ((SYSCTL_PRADC_R & 0x01) == 0)
    {
    }

    //disable digital functionality on PB4
    GPIO_PORTB_DEN_R &= ~0x10;

    //enable alternate functions on port B pins
    GPIO_PORTB_AFSEL_R |= 0x10;

    //enable analog functionality on PB4
    GPIO_PORTB_AMSEL_R |= 0x10;

    //configure ADC0 for 125K samples/sec
    ADC0_PC_R &= ~0xF;
    ADC0_PC_R |= 0x1;

    //set sequencer 3 at highest priority
    ADC0_SSPRI_R &= ~0x3333; // why do the reserved bits have to be interspersed aghh
    ADC0_SSPRI_R |= 0x0123;

    //disable sequencer 3
    ADC0_ACTSS_R &= ~0x0008;

    //set sequencer 3 to trigger by software
    ADC0_EMUX_R &= ~0xF000;

    //set sequencer 3 channel
    ADC0_SSMUX3_R &= ~0x000F;
    ADC0_SSMUX3_R |= 10;

    //set: no temperature sensor, yes interrupt, 1st sample is last sample, no differential sampling
    ADC0_SSCTL3_R &= ~0x000F;
    ADC0_SSCTL3_R |= 0x0006;

    //disable interrupts, again
    ADC0_IM_R &= ~0x0008;

    //enable sequencer 3
    ADC0_ACTSS_R |= 0x0008;
}

uint32_t poll_ir_raw(void)
{
    uint32_t result;
    ADC0_PSSI_R |= 0x08;
    while((ADC0_RIS_R & 0x08) == 0){}
    result = ADC0_SSFIFO3_R & 0xFFF;
    ADC0_ISC_R |= 0x08;
    return result;
}

uint32_t poll_ir(int samples)
{
    uint32_t result = 0;
    int i;
    for (i = 0; i < samples; i++)
    {
        result += poll_ir_raw();
    }
    return result / samples;
}
