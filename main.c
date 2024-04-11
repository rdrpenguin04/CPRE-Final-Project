#include "Timer.h"
#include "lcd.h"
#include "open_interface.h"
#include "movement.h"
#include "ping.h"
#include "servo.h"
#include "uart-interrupt.h"

#include <math.h>

void scan()
{
    float dist;
    char temp[80];
    while (1)
    {
//        dist = ping_getDistance();
//        sprintf(temp, "DIST: %.2f cm (%lo cycles)\r\n", dist, START_TIME - END_TIME);
//        uart_sendStr(temp);
//        lcd_printf("%s", temp);
        servo_set(90);
        timer_waitMicros(1000000 / 20);
        CHECK_KILL;
    }
}

/**
 * main.c
 */
int main(void)
{
    timer_init();
    lcd_init();
    button_init();
    uart_interrupt_init();
    ping_init();
    servo_init();

//    oi_t *sensor_data = oi_alloc();
//    oi_init(sensor_data);

    while (1)
    {
        if (run_scan)
        {
//            scan();

            servo_set(90);
            timer_waitMillis(1000);
            servo_set(30);
            timer_waitMillis(1000);
            servo_set(150);
            timer_waitMillis(1000);
            servo_set(90);
            run_scan = 0;
        } else if(run_calibrate)
        {
            servo_calibrate();
            run_calibrate = 0;
        }
        int val = byte_received;
        if (val >= '0' && val <= '9')
        {
            servo_set((val - '0') * 20);
        }
        byte_received = 0;
        kill_action = 0;
        timer_waitMillis(100);
    }
}
