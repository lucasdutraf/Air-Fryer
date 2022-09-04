#ifndef GPIO_H_
#define GPIO_H_

#define PWM_MAX_RANGE 100
#define PWM_MIN_RANGE 0
#define RESISTOR_PIN 4
#define FAN_PIN 5

void manage_gpio_devices(int intensity);
void deactivate_fan_and_resistor();
void deactivate_fan();
void activate_fan(int intensity);
void deactivate_resistor();
void activate_resistor(int intensity);
void gpio_setup();

#endif
