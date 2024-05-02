#include "Timer.h"
#include "lcd.h"
#include "open_interface.h"
#include "movement.h"
#include "cyBot_Scan.h"
#include "uart-interrupt.h"
#include "adc.h"
#include "ping.h"
#include "servo.h"
#include "button.h"

#include <math.h>

//void calibrate()
//{
////    cyBOT_SERVRO_cal_t calibration = cyBOT_SERVO_cal();
//    servo_calibrate();
//
//    char text[80] = "";
//    snprintf(text, 80, "Calibration: left = %d; right = %d;", calibration.left,
//             calibration.right);
//    uart_sendStr(text);
//    lcd_printf("%s", text);
//    left_calibration_value = calibration.left;
//    right_calibration_value = calibration.right;
//}

#define NUM_IR_POLL 2
#define NUM_PING_POLL 5
#define MAX_OBJ 8

float small_dist;
float small_angle;

void scan(int start, int end, int count)
{
    char line[80];
    float angle;
    int i, idx;
    int avg_int_tmp;
    float avg_float_tmp;
    uint32_t data;
    float dist;

    for (angle = start; angle <= end; angle += (((float) end - start) / count))
    {
        servo_set(angle);
        timer_waitMillis(100);
        data = poll_ir(1024);
        dist = 86400.0 / data - 15; // TODO: calibrate

        snprintf(line, 80, "IR %f = %f\r\n", angle, dist);
        uart_sendStr(line);

        CHECK_KILL;
    }
}

void pingScan(int angle)
{
    char line[80];
    int i, idx;
    int avg_int_tmp;
    float avg_float_tmp;
    uint32_t data;
    float dist;
    servo_set(angle);
    timer_waitMillis(50);
    for (i = 0; i < 5; i++)
    {
        dist += ping_getDistance();
        CHECK_KILL;
        timer_waitMillis(20);
    }
    dist /= 5;

    snprintf(line, 80, "PING %3d = %f\r\n", angle, dist);
    uart_sendStr(line);
}

int go(oi_t *sensor_data)
{
    if (small_angle < 90)
    {
        if (turn_right(sensor_data, (90 - small_angle) * 0.75) == 0.0)
            return 1;
        CHECK_KILL 0;
    }
    else if (small_angle > 90)
    {
        if(turn_left(sensor_data, (small_angle - 90) * 0.75) == 0.0)
        return 1;
        CHECK_KILL 0;
    }
    if(move_forward(sensor_data, small_dist * 9.5 - 80) == 0.0)
    return 1;
    CHECK_KILL 0;
    return 0;
}

char str[] = "Secret message has been displayed";
void lcd_rotatingBanner(void)
{
    char test[20 + strlen(str) + 19 + 1];
    int i, j, x;
    for (i = 0; i < 20; i++)
    {
        test[i] = ' ';
    }
    test[20] = '\0';
    strcat(test, str);
    for (i = 0, j = 20 + strlen(str); i < 19; i++, j++)
    {
        test[j] = ' ';
    }
    test[j] = '\0';

    for (x = 0; x < 20 + strlen(str); x++)
    {
        lcd_printf("%.20s", &test[x]);
        timer_waitMillis(300);
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
    adc_init();

    pos0 = 326524;     //left cal
    pos180 = 354667;    //right cal

    servo_init();
//    servo_calibrate();w

    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    while (1)
    {
        if (buffer_ready)
        {

            int i = 2;
            int result = 0;
            // Parse the buffer and run the command
            if (buffer[0] == 'w') // move forward
            {
                while (buffer[i] != ' ' && buffer[i] != '\0')
                {
                    result *= 10;
                    result += buffer[i] - '0';
                    i++;
                }
                move_forward(sensor_data, result);
            }
            else if (buffer[0] == 'a') // turn left
            {
                while (buffer[i] != ' ' && buffer[i] != '\0')
                {
                    result *= 10;
                    result += buffer[i] - '0';
                    i++;
                }
                turn_left(sensor_data, result);
            }
            else if (buffer[0] == 's') // move backward
            {
                while (buffer[i] != ' ' && buffer[i] != '\0')
                {
                    result *= 10;
                    result += buffer[i] - '0';
                    i++;
                }
                move_backward(sensor_data, result);
            }
            else if (buffer[0] == 'd') // turn right
            {
                while (buffer[i] != ' ' && buffer[i] != '\0')
                {
                    result *= 10;
                    result += buffer[i] - '0';
                    i++;
                }
                turn_right(sensor_data, result);
            }
            else if (buffer[0] == 'x') //the scans
            {
                i = 4;
                int j;
                int IRVals[3];
                if (buffer[2] == 'i') //IR sensor
                {
                    for (j = 0; j < 3; j++)
                    {
                        while (buffer[i] != ' ' && buffer[i] != '\0')
                        {
                            result *= 10;
                            result += buffer[i] - '0';
                            i++;
                        }
                        i++;
                        IRVals[j] = result;
                        result = 0;

                    }
                    scan(IRVals[0], IRVals[1], IRVals[2]);
                }
                else if (buffer[2] == 'p') //Ping sensor
                {
                    while (buffer[i] != ' ' && buffer[i] != '\0')
                    {
                        result *= 10;
                        result += buffer[i] - '0';
                        i++;
                    }
                    pingScan(result);
                }
            }
            else if (buffer[0] == 'm') //Send secret message
            {
                while (buffer[i] != '\0')
                {
                    result *= 10;
                    result += buffer[i] - '0';
                    i++;
                }
//                playSong(sensor_data);
                lcd_rotatingBanner();
            }
            buffer_ready = 0;
            buffer[0] = '\0';
            buffer_len = 0;
            uart_sendStr("Ready\r\n");
        }
        kill_action = 0;
    }

    unsigned char note_duration[] = {18, 18, 18, 18, 18, 18, 18, 18, 18, 37, 10, 10, 13, 42, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 56, 12, 12, 12, 12, 12, 12}; //31
    unsigned char song_notes[] =    {60, 5, 72, 75, 77, 78, 77, 75, 72, 5, 70, 74, 72, 5, 55, 60, 5, 72, 75, 77, 78, 77, 75, 78, 5, 78, 77, 75, 78, 77, 75}; //31

//    void playSong(oi_t *sensor_data) {
//        oi_loadSong(0, sizeof(song_notes), song_notes, note_duration);
//        oi_play_song(0);
//    }
}
