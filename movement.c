/*
 * movement.c
 *
 *  Created on: Feb 1, 2024
 *      Author: ngadelha
 */

//sensor_data->cliffLeftSignal >= 2700  || sensor_data->cliffLeftSignal <= 5 || sensor_data->cliffFrontLeftSignal >= 2700 || sensor_data->cliffFrontLeftSignal <= 5 ||
//                sensor_data->cliffRightSignal >= 2700 || sensor_data->cliffRightSignal <= 5 || sensor_data->cliffFrontRightSignal >= 2700 || sensor_data->cliffFrontRightSignal <= 5
//
//                sensor_data->cliffLeftSignal > 2700 || sensor_data->cliffFrontLeftSignal > 2700 ||
//                                sensor_data->cliffRightSignal > 2700 || sensor_data->cliffFrontRightSignal > 2700
#include "movement.h"
#include "open_interface.h"
#include "lcd.h"
#include "uart-interrupt.h"

double move_forward(oi_t *sensor_data, double distance_mm)
{
    double sum = 0;
    char line[80];
    oi_setWheels(50, 50);
    while (sum < distance_mm)
    {
        oi_update(sensor_data);
        sum += sensor_data->distance;
        lcd_printf("distance: %.2f", sum);
        if (sensor_data->bumpRight)
        {
            uart_sendStr("Small object detected: right.\r\n");
            move_backward(sensor_data, 15);
            oi_setWheels(0, 0);
//                    oi_init(sensor_data);
//                    snprintf(line, 80, "forward = %f\r\n", sum);
//                    uart_sendStr(line);
            return 0;
        }
        else if (sensor_data->bumpLeft)
        {
            uart_sendStr("Small object detected: left.\r\n");
            move_backward(sensor_data, 15);
            oi_setWheels(0, 0);
//                    oi_init(sensor_data);
//                    snprintf(line, 80, "forward = %f\r\n", sum);
//                    uart_sendStr(line);
            return 0;
        }
        //Determines if hole is nearby
        if (sensor_data->cliffLeftSignal >= 2700
                || sensor_data->cliffLeftSignal <= 5
                || sensor_data->cliffFrontLeftSignal >= 2700
                || sensor_data->cliffFrontLeftSignal <= 5
                || sensor_data->cliffRightSignal >= 2700
                || sensor_data->cliffRightSignal <= 5
                || sensor_data->cliffFrontRightSignal >= 2700
                || sensor_data->cliffFrontRightSignal <= 5)
        {
            uart_sendStr("Hole/Boundary nearby. \r\n");
            move_backward(sensor_data, 50);
            oi_setWheels(0, 0);
            oi_init(sensor_data);
            snprintf(line, 80, "forward = %f\r\n", sum);
            uart_sendStr(line);
            return 0;
        }
        snprintf(line, 80, "forward = %f\r\n", sum);
        uart_sendStr(line);
        CHECK_KILL sum;
    }
    oi_setWheels(0, 0); // Stop and don't crash into literally everything :D

    return sum;
}

double move_backward(oi_t *sensor_data, double distance_mm)
{
    double sum = 0;
    char line[80];
    while (sum > -distance_mm)
    {
        oi_setWheels(-200, -200);
        oi_update(sensor_data);
        sum += sensor_data->distance;
        lcd_printf("distance: %.2f", sum);
        if (sensor_data->bumpRight)
        {
            uart_sendStr("Small object detected: right.\r\n");
            move_forward(sensor_data, 15);
            oi_setWheels(0, 0);
            oi_init(sensor_data);
//                    snprintf(line, 80, "backward = %f\r\n", sum);
//                    uart_sendStr(line);
        }
        else if (sensor_data->bumpLeft)
        {
            uart_sendStr("Small object detected: left.\r\n");
            move_forward(sensor_data, 15);
            oi_setWheels(0, 0);
            oi_init(sensor_data);
//                    snprintf(line, 80, "backward = %f\r\n", sum);
//                    uart_sendStr(line);
        }
        //Determines if hole is nearby
//        if(sensor_data->cliffLeftSignal > 2700 || sensor_data->cliffFrontLeftSignal > 2700 ||
//                sensor_data->cliffRightSignal > 2700 || sensor_data->cliffFrontRightSignal > 2700){
//            uart_sendStr("Hole/Boundary nearby.");
////            move_backward(sensor_data, 15);
//            oi_setWheels(0,0);
//            oi_init(sensor_data);
//            snprintf(line, 80, "backward = %f\r\n", sum);
//            uart_sendStr(line);
//            break;
//        }
        snprintf(line, 80, "backward = %f\r\n", sum);
        uart_sendStr(line);
        CHECK_KILL sum;
    }
    oi_setWheels(0, 0); // Stop and don't crash into literally everything :D

    return sum;
}

double turn_right(oi_t *sensor_data, double degrees)
{
    double sum = 0;
    char line[80];
    while (sum > -degrees)
    {
        oi_setWheels(-200, 200);
        oi_update(sensor_data);
        sum += sensor_data->angle * 1.07;
        lcd_printf("angle: %.2f", sum);
//        if(sensor_data->bumpRight){
//                    uart_sendStr("Small object detected: right.\r\n");
//                    move_backward(sensor_data, 15);
//                    oi_setWheels(0,0);
//                    oi_init(sensor_data);
//                    snprintf(line, 80, "right = %f\r\n", sum);
//                    uart_sendStr(line);
//                }
//                else if(sensor_data->bumpLeft){
//                    uart_sendStr("Small object detected: left.\r\n");
//                     move_backward(sensor_data, 15);
//                     oi_setWheels(0,0);
//                    oi_init(sensor_data);
//                    snprintf(line, 80, "right = %f\r\n", sum);
//                    uart_sendStr(line);
//                }
        //Determines if hole is nearby
//        if(sensor_data->cliffLeftSignal > 2700 || sensor_data->cliffFrontLeftSignal > 2700 ||
//                sensor_data->cliffRightSignal > 2700 || sensor_data->cliffFrontRightSignal > 2700){
//            uart_sendStr("Hole/Boundary nearby.");
//            move_backward(sensor_data, 50);
//            oi_setWheels(0,0);
//            oi_init(sensor_data);
//            snprintf(line, 80, "right = %f\r\n", sum);
//            uart_sendStr(line);
//            break;
//        }
        snprintf(line, 80, "right = %f\r\n", sum);
        uart_sendStr(line);
        CHECK_KILL sum;
    }
    oi_setWheels(0, 0); // Stop and don't crash into literally everything :D

    return sum;
}

double turn_left(oi_t *sensor_data, double degrees)
{
    double sum = 0;
    char line[80];
    while (sum < degrees)
    {
        oi_setWheels(200, -200);
        oi_update(sensor_data);
        sum += sensor_data->angle * 1.07;
        lcd_printf("angle: %.2f", sum);
//        if(sensor_data->bumpRight){
//                    uart_sendStr("Small object detected: right.\r\n");
//                    move_backward(sensor_data, 15);
//                    oi_setWheels(0,0);
//                    oi_init(sensor_data);
//                    snprintf(line, 80, "left = %f\r\n", sum);
//                    uart_sendStr(line);
//                }
//        else if(sensor_data->bumpLeft){
//            uart_sendStr("Small object detected: left.\r\n");
//            move_backward(sensor_data, 15);
//            oi_setWheels(0,0);
//            oi_init(sensor_data);
//            snprintf(line, 80, "left = %f\r\n", sum);
//            uart_sendStr(line);
//         }
        //Determines if hole is nearby
//        if(sensor_data->cliffLeftSignal > 2700 || sensor_data->cliffFrontLeftSignal > 2700 ||
//                sensor_data->cliffRightSignal > 2700 || sensor_data->cliffFrontRightSignal > 2700){
//            uart_sendStr("Hole/Boundary nearby.");
//            move_backward(sensor_data, 50);
//            oi_setWheels(0,0);
//            oi_init(sensor_data);
//            snprintf(line, 80, "left = %f\r\n", sum);
//            uart_sendStr(line);
//            break;
//        }
        snprintf(line, 80, "left = %f\r\n", sum);
        uart_sendStr(line);
        CHECK_KILL sum;
    }
    oi_setWheels(0, 0); // Stop and don't crash into literally everything :D

    return sum;
}

