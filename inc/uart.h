#define SERVER_CODE 0x01
#define REQUEST_CODE 0x23
#define WRITE_CODE 0x16
#define REQUEST_INTERNAL_TEMPERATURE 0xC1
#define REQUEST_REFERENCE_TEMPERATURE 0xC2
#define REQUEST_USER_COMMANDS 0xC3
#define INTERNAL 1
#define REFERENCE_POTENTIOMETER 2

#ifndef UART_H_
#define UART_H_

int start_uart();
void close_uart();
float process_uart_return(int uart0);
void write_uart(int uart0, int code);
float internal_temperature(float current_temperature);
float reference_temperature(float current_temperature);

#endif