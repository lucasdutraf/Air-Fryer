#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <ncurses.h>

#include "uart.h"
#include "menu_display.h"
#include "bme280temperature.h"
#include "pid.h"
#include "gpio.h"
#include "csv.h"
#include "ui.h"

#ifndef MAIN_H_
#define MAIN_H_

void finish();
void set_reference_input(int use_potenciometer, float new_reference);

#endif
