
#include <wiringPi.h> /* include wiringPi library */
#include <stdio.h>
#include <softPwm.h> /* include header file for software PWM */
#include "gpio.h"

void gpio_setup()
{
    wiringPiSetup();
    pinMode(RESISTOR_PIN, OUTPUT);
    pinMode(FAN_PIN, OUTPUT);
}

void activate_resistor(int intensity)
{

    softPwmCreate(RESISTOR_PIN, PWM_MIN_RANGE, PWM_MAX_RANGE);
    softPwmWrite(RESISTOR_PIN, intensity);
}

void deactivate_resistor()
{
    softPwmCreate(RESISTOR_PIN, PWM_MIN_RANGE, PWM_MAX_RANGE);
    softPwmWrite(RESISTOR_PIN, PWM_MIN_RANGE);
}

void activate_fan(int intensity)
{
    softPwmCreate(FAN_PIN, PWM_MIN_RANGE, PWM_MAX_RANGE);
    softPwmWrite(FAN_PIN, intensity);
}

void deactivate_fan()
{
    softPwmCreate(FAN_PIN, PWM_MIN_RANGE, PWM_MAX_RANGE);
    softPwmWrite(FAN_PIN, PWM_MIN_RANGE);
}

void deactivate_fan_and_resistor()
{
    deactivate_resistor();
    deactivate_fan();
}

void manage_gpio_devices(int intensity)
{
    if (intensity > 0)
    {
        activate_resistor(intensity);
        deactivate_fan();
    }
    else if (intensity < -40)
    {
        activate_fan((int)intensity * -1);
        deactivate_resistor();
    }
    else
    {
        deactivate_fan_and_resistor();
    }
}