#include "Timer.h"
#include "lcd.h"
#include "open_interface.h"
#include "movement.h"
#include "cyBot_Scan.h"
#include "uart-interrupt.h"
#include "adc.h"
#include "ping.h"
#include "servo.h"

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
    // Psuedocode:
    // 1. Scan using IR, making multiple readings at each angle (average)
    //   a. Detect object edges, store in array
    // 2. Pick middle of each object, scan multiple times with PING (average)
    // 3. Output results
    char line[80];
//    cyBOT_Scan_t scan;
    int angle;
    int i, idx;
    int avg_int_tmp;
    float avg_float_tmp;
    uint32_t data;
    float dist;

//    int obj_num = 0;
//    int obj_start[MAX_OBJ];
//    int obj_end[MAX_OBJ];
//    float obj_dist[MAX_OBJ];
//    int is_scanning_obj = 0;

    for (angle = start; angle <= end; angle += ((end - start) / count))
    {
//        avg_int_tmp = 0;
//        for (i = 0; i < NUM_IR_POLL; i++)
//        {

//            cyBOT_Scan(angle, &scan);

//            avg_int_tmp += scan.IR_raw_val;
            data = poll_ir(1024);
            dist = 43400.0 / data - 15;

            snprintf(line, 80, "Value at %3d degrees: %f\r\n", angle, dist);
            uart_sendStr(line);

            CHECK_KILL;
//        }
//        avg_int_tmp /= NUM_IR_POLL;

//        snprintf(line, 80, "Value at %3d degrees: %d\r\n", angle, avg_int_tmp);
//        uart_sendStr(line);

        // While not presently scanning an object
        // if ir > 1000
        // start scanning, set start
        // ...
        // whle scanning object
        // if ir < 1000
        // stop scanning, set end

//        if (!is_scanning_obj)
//        {
//            if (avg_int_tmp > 700)
//            {
//                if (obj_num >= MAX_OBJ)
//                {
//                    uart_sendStr(
//                            "Too many objects! Change MAX_OBJ before running again.\r\n");
//                    return;
//                }
//                is_scanning_obj = 1;
//                obj_start[obj_num] = angle;
//            }
//        }
//        else if (is_scanning_obj)
//        {
//            if (avg_int_tmp < 700)
//            {
//                is_scanning_obj = 0;
//                if (angle - obj_start[obj_num] < 3)
//                    continue;
//                obj_end[obj_num] = angle;
//                obj_num++;
//            }
//        }

        CHECK_KILL;
//        uart_sendStr
    }
}

void pingScan(int angle) {
    char line[80];
//    cyBOT_Scan_t scan;
    int i, idx;
    int avg_int_tmp;
    float avg_float_tmp;
    uint32_t data;
    float dist;
    for (i = 0; i < 5; i ++) {
//        ping_getDisatnce();
//        cyBOT_Scan(angle, &scan);
        dist += ping_getDistance();
        CHECK_KILL;
    }
    dist /= 5;

    snprintf(line, 80, "Value at %3d degrees: %f\r\n", angle, dist);
    uart_sendStr(line);


//    for (idx = 0; idx < obj_num; idx++)
//    {
//        avg_float_tmp = 0;
//        for (i = 0; i < NUM_PING_POLL; i++)
//        {
//            cyBOT_Scan((obj_end[idx] + obj_start[idx]) / 2, &scan);
//            avg_float_tmp += scan.sound_dist;
//            CHECK_KILL;
//        }
//        avg_float_tmp /= NUM_PING_POLL;
//
//        obj_dist[idx] = avg_float_tmp;
//        CHECK_KILL;
//    }
//
//    for (idx = 0; idx < obj_num; idx++)
//    {
//        snprintf(line, 80, "Object #%d: angle %d to %d, %.2f cm away\r\n", idx,
//                 obj_start[idx], obj_end[idx], obj_dist[idx]);
//        uart_sendStr(line);
//        CHECK_KILL;
//    }
//
//    float small_width = 100000000;
//
//    for (idx = 0; idx < obj_num; idx++)
//    {
//        float a = obj_end[idx] - obj_start[idx];
//        float d = obj_dist[idx];
//        float width = 2 * M_PI * a / 360 * d;
//        snprintf(line, 80, "Object #%d: approx %f cm wide\r\n", idx, width);
//        uart_sendStr(line);
//
//        if (width < small_width)
//        {
//            small_angle = (obj_end[idx] + obj_start[idx]) / 2;
//            small_dist = d;
//            small_width = width;
//        }
//        CHECK_KILL;
//    }
}

int go(oi_t *sensor_data)
{
    if (small_angle < 90)
    {
        if(turn_right(sensor_data, (90 - small_angle) * 0.75) == 0.0)
            return 1;
        CHECK_KILL 0;
    } else if (small_angle > 90)
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
    for(i = 0; i < 20; i++){
        test[i] = ' ';
    }
    test[20] = '\0';
    strcat(test, str);
    for(i = 0, j = 20 + strlen(str); i < 19; i++, j++){
        test[j] = ' ';
    }
    test[j] = '\0';

    for(x = 0; x < 20 + strlen(str); x++){
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
    uart_interrupt_init();
//    cyBOT_init_Scan(0b0111);
    ping_init();
    servo_init();
//    calibrate();

//    left_calibration_value;     //TODO
//    right_calibration_value;    //TODO


    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    while (1)
    {
//        if (run_scan)
//        {
//            scan();
//            run_scan = 0;
//        }
//        else if (run_calibrate)
//        {
//            calibrate();
//            run_calibrate = 0;
//        }
//        else if (run_go)
//        {
//            while(go(sensor_data))
//            {
//                scan();
//            }
//            run_go = 0;
//        }


        if (buffer_ready)
        {

            //Impliment somewhere - sends info to GUI
            //Should be sending info at every function call
//            char line[80];
//            snprintf(line, 80, "Distance(s): ", sensor_data, result);
//            uart_sendStr(line);

            int i = 2;
            int result = 0;
            // Parse the buffer and run the command
            if (buffer[0] == 'w') // move forward
            {
                while (buffer[i] != ' ' && buffer[i] != '\0') {
                    result *= 10;
                    result += buffer[i] - '0';
                    i++;
                }
                move_forward(sensor_data, result);
            }
            else if (buffer[0] == 'a') // turn left
            {
                while (buffer[i] != ' ' && buffer[i] != '\0') {
                    result *= 10;
                    result += buffer[i] - '0';
                    i++;
                }
                turn_left(sensor_data, result);
            }
            else if (buffer[0] == 's') // move backward
            {
                while (buffer[i] != ' ' && buffer[i] != '\0') {
                    result *= 10;
                    result += buffer[i] - '0';
                    i++;
                }
                move_backward(sensor_data, result);
            }
            else if (buffer[0] == 'd') // turn right
            {
                while (buffer[i] != ' ' && buffer[i] != '\0') {
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
                    for (j = 0; j < 3; j++) {
                        while (buffer[i] != ' ' &&  buffer[i] != '\0') {
                            result *= 10;
                            result += buffer[i] - '0';
                            i++;
                        }
                        i += 2;
                        IRVals[j] = result;
                        result = 0;
                    }
                    scan(IRVals[0], IRVals[1], IRVals[2]);
                }
                else if (buffer[2] == 'p') //Ping sensor
                {
                    while (buffer[i] != ' ' &&  buffer[i] != '\0') {
                        result *= 10;
                        result += buffer[i] - '0';
                        i++;
                    }
                    pingScan(result);
                }
            }
            else if (buffer[0] == 'm')
                       {
                           while (buffer[i] != '\0') {
                               result *= 10;
                               result += buffer[i] - '0';
                               i++;
                           }

                           lcd_rotatingBanner();
                       }

        }
        kill_action = 0;
        timer_waitMillis(100);
    }
}
//test
