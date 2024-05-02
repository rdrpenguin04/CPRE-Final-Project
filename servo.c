#include "button.h"
#include "lcd.h"
#include "servo.h"
#include "Timer.h"
#include <math.h>

void servo_init()
{
    SYSCTL_RCGCGPIO_R |= 0x02;
    while ((SYSCTL_PRGPIO_R & 0x02) == 0);
    SYSCTL_RCGCTIMER_R |= 0x02;

    TIMER1_CFG_R = 0x04;
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_PCTL_R &= ~0xF00000;
    GPIO_PORTB_PCTL_R |= 0x700000;
    TIMER1_TBMR_R |= 0x0A;
    TIMER1_CTL_R &= ~0x4000;

    // match val = 0x04E200
    TIMER1_TBPMR_R = 0x04;
    TIMER1_TBMATCHR_R = 0xE200;

    TIMER1_CTL_R |= 0x0100;
}

int pos0 = 336000; // ideal calibration for 0 degrees
int pos180 = 352000; // ideal calibration for 180 degrees

// pos must be a value from 0 to 180
void servo_set(float pos)
{
    int lastVal = pos0;
    int val = pos / 180 * (pos180 - pos0) + pos0;
    TIMER1_TBPR_R = val >> 16;
    TIMER1_TBILR_R = val & 0xFFFF;
    timer_waitMillis(abs(val - lastVal) / 200);
    lastVal = val;
}

void servo_calibrate()
{
    int duration = 200;
    while (button_getButton() != 4)
    {
        if (button_getButton() == 1)
        {
            pos0 -= duration;
            duration *= 1.1;
        }
        else if(button_getButton() == 2)
        {
            pos0 += duration;
            duration *= 1.1;
        }
        else
        {
            duration = 100;
        }
        timer_waitMillis(10);
        lcd_printf("%s%d", "B1/B2 = -/+\nB4 = next\n", pos0);
        timer_waitMillis(10);
        servo_set(0);
    }
    while (button_getButton() == 4) timer_waitMillis(100);
    lcd_printf("%s", "B1/B2 = -/+\nB4 = next");
    duration = 200;
    while (button_getButton() != 4)
    {
        if (button_getButton() == 1)
        {
            pos180 -= duration;
            duration *= 1.1;
        }
        else if(button_getButton() == 2)
        {
            pos180 += duration;
            duration *= 1.1;
        }
        else
        {
            duration = 100;
        }
        timer_waitMillis(10);
        lcd_printf("%s%d", "B1/B2 = -/+\nB4 = next\n", pos180);
        timer_waitMillis(10);
        servo_set(180);
    }
    lcd_printf("%d\n%d", pos0, pos180);
}
