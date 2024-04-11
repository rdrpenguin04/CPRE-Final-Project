/*
 * movement.c
 *
 *  Created on: Feb 1, 2024
 *      Author: ngadelha
 */

#include "movement.h"
#include "open_interface.h"
#include "lcd.h"
#include "uart-interrupt.h"

double move_forward(oi_t *sensor_data, double distance_mm)
{
    double sum = 0;
    while (sum < distance_mm)
    {
        oi_setWheels(500, 500);
        oi_update(sensor_data);
        sum += sensor_data->distance;
        lcd_printf("distance: %.2f", sum);
        if (sensor_data->bumpLeft)
        {
            move_backward(sensor_data, 25);
            turn_right(sensor_data, 90 * 0.9);
            move_forward(sensor_data, 80);
            turn_left(sensor_data, 90 * 0.9);
            move_forward(sensor_data, 80);
            turn_left(sensor_data, 90 * 0.9);
            move_forward(sensor_data, 80);
            turn_right(sensor_data, 90 * 0.9);
            sum += 80;
        }
        else if (sensor_data->bumpRight)
        {
            move_backward(sensor_data, 25);
            turn_left(sensor_data, 90 * 0.9);
            move_forward(sensor_data, 80);
            turn_right(sensor_data, 90 * 0.9);
            move_forward(sensor_data, 80);
            turn_right(sensor_data, 90 * 0.9);
            move_forward(sensor_data, 80);
            turn_left(sensor_data, 90 * 0.9);
            sum += 80;
        }
        CHECK_KILL sum;
    }
    oi_setWheels(0, 0); // Stop and don't crash into literally everything :D

    return sum;
}

double move_backward(oi_t *sensor_data, double distance_mm)
{
    double sum = 0;
    while (sum > -distance_mm)
    {
        oi_setWheels(-200, -200);
        oi_update(sensor_data);
        sum += sensor_data->distance;
        lcd_printf("distance: %.2f", sum);
        CHECK_KILL sum;
    }
    oi_setWheels(0, 0); // Stop and don't crash into literally everything :D

    return sum;
}

double turn_right(oi_t *sensor_data, double degrees)
{
    double sum = 0;
    while (sum > -degrees)
    {
        oi_setWheels(-200, 200);
        oi_update(sensor_data);
        sum += sensor_data->angle * 1.07;
        lcd_printf("angle: %.2f", sum);
        CHECK_KILL sum;
    }
    oi_setWheels(0, 0); // Stop and don't crash into literally everything :D

    return sum;
}

double turn_left(oi_t *sensor_data, double degrees)
{
    double sum = 0;
    while (sum < degrees)
    {
        oi_setWheels(200, -200);
        oi_update(sensor_data);
        sum += sensor_data->angle * 1.07;
        lcd_printf("angle: %.2f", sum);
        CHECK_KILL sum;
    }
    oi_setWheels(0, 0); // Stop and don't crash into literally everything :D

    return sum;
}

