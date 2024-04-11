/**
 * Driver for ping sensor
 * @file ping.c
 * @author
 */

#include "ping.h"
#include "Timer.h"
#include "uart-interrupt.h"

volatile unsigned long START_TIME = 0;
volatile unsigned long END_TIME = 0;
volatile enum{LOW, HIGH, DONE} STATE = LOW; // State of ping echo pulse

void ping_init (void) {
    SYSCTL_RCGCGPIO_R |= 0x02;
    while ((SYSCTL_PRGPIO_R & 0x02) == 0);
    SYSCTL_RCGCTIMER_R |= 0x08;
//    while ((SYSCTL_PRTIMER_R & 0x08) == 0);

    GPIO_PORTB_DEN_R |= 0x08;
    GPIO_PORTB_AFSEL_R |= 0x08;
    GPIO_PORTB_PCTL_R |= 0x7000;

    TIMER3_CTL_R  &=  ~0x100;
    TIMER3_CFG_R  |=    0x04;
    TIMER3_TBMR_R =   0x0007;
    TIMER3_TBMR_R &= ~0x0010;
    TIMER3_CTL_R  |=  0x0D00;

    TIMER3_TBILR_R = 0xFFFF;
    TIMER3_TBPR_R = 0xFF;

    NVIC_EN1_R |= 0x10;
    NVIC_PRI9_R |= 0x20;

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();
}

void ping_trigger(void) {
    STATE = LOW;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0X0100;
    TIMER3_IMR_R &= ~0x0400;
    // Disable alternate function (disconnect timer from port pin)
    GPIO_PORTB_AFSEL_R &= ~0x08;

    GPIO_PORTB_DIR_R |= 0x0008;
    GPIO_PORTB_DATA_R &= ~0x0008;
    GPIO_PORTB_DATA_R |= 0x0008;
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= ~0x0008;
//    GPIO_PORTB_DIR_R &= ~0x0008;

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0x0400;
    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0x08;
    TIMER3_IMR_R |= 0x0400;
    TIMER3_CTL_R |= 0x0100;
}

void TIMER3B_Handler(void) {
    if(!(TIMER3_MIS_R & 0x400)) return;
    TIMER3_ICR_R |= 0x0400;
    switch (STATE)
    {
    case LOW:
        START_TIME = TIMER3_TBR_R;
        STATE = HIGH;
        break;
    case HIGH:
        END_TIME = TIMER3_TBR_R;
        STATE = DONE;
        break;
    default:
        // Well, uh. You shouldn't be here :)
        break;
    }
}

float ping_getDistance(void) {
    ping_trigger();
    while (STATE != DONE) CHECK_KILL -1;
    if (END_TIME > START_TIME) { uart_sendStr("---OVERFLOW DETECTED---\r\n"); }
    return (START_TIME - END_TIME) / 16000000.0 * 34300.0 / 2;
}
