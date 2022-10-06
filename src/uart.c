#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include "crc16.h"
#include "uart.h"

static int uart_filestream = -1;

int start_uart()
{
    uart_filestream = open("/dev/serial0", O_RDWR | O_NOCTTY | O_NDELAY);      //Open in non blocking read/write mode
    if (uart_filestream == -1)
    {
        printf("Failed to init UART\n");
        return -1;
    }
    else {
        printf("UART initialized\n");
    }

    struct termios options;
    tcgetattr(uart_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart_filestream, TCIFLUSH);
    tcsetattr(uart_filestream, TCSANOW, &options);

    return uart_filestream;
}

void close_uart() {
    close(uart_filestream);
}


float process_uart_return(int uart_filestream) {
    float result = 0;
    if (uart_filestream != -1) {
        unsigned char rx_buffer[256];
        int rx_length = read(uart_filestream, (void *)rx_buffer, 255);
        if (rx_length < 0)
        {
            printf("Error on read UART.\n");
            return -1;
        }
        else if (rx_length == 0)
        {
            printf("Nothing on UART\n");
            return -1;
        }
        else
        {
            rx_buffer[rx_length] = '\0';
        }

        if (rx_length != 9)
        {
            printf("Wrong UART data!\n");
            return -1;
        }

        short crc_received;
        memcpy(&crc_received, &rx_buffer[rx_length - 2], 2);

        unsigned char response[7];
        memcpy(&response, &rx_buffer[0], rx_length - 2);
        if (crc_received != calcula_CRC(response, 7))
        {
            printf("Wrong CRC!\n");
            return -1;
        }

        if (result == 0)
        {
            float f;
            memcpy(&f, &rx_buffer[3], 4);
            return f;
        }
    }
    else {
        return -1;
    }
}

void write_uart(int uart_filestream, int code)
{
    unsigned char tx_buffer[20];
    unsigned char *p_tx_buffer;
    p_tx_buffer = &tx_buffer[0];

    *p_tx_buffer++ = SERVER_CODE;
    *p_tx_buffer++ = REQUEST_CODE;

    if (code == INTERNAL)
    {
        *p_tx_buffer++ = REQUEST_INTERNAL_TEMPERATURE;
    }
    else if (code == REFERENCE_POTENTIOMETER)
    {
        *p_tx_buffer++ = REQUEST_REFERENCE_TEMPERATURE;
    }

    *p_tx_buffer++ = 0;
    *p_tx_buffer++ = 9;
    *p_tx_buffer++ = 3;
    *p_tx_buffer++ = 9;

    short crc = calcula_CRC(tx_buffer, (p_tx_buffer - &tx_buffer[0]));
    memcpy(&tx_buffer[(p_tx_buffer - &tx_buffer[0])], &crc, 2);
    p_tx_buffer += 2; // CRC

    if (uart_filestream != -1)
    {
        int count = write(uart_filestream, &tx_buffer[0], (p_tx_buffer - &tx_buffer[0]));
        if (count < 0)
        {
            printf("UART error on write\n");
            exit(1);
        }
    }
    usleep(100000);
}

float internal_temperature(float current_temperature)
{
    write_uart(uart_filestream, 1);
    float temp = process_uart_return(uart_filestream);
    if (temp < 0)
    {
        temp = current_temperature;
    }
    return temp;
}

float reference_temperature(float current_temperature)
{
    write_uart(uart_filestream, 2);
    float temp = process_uart_return(uart_filestream);
    if (temp < 0 )
    {
        temp = current_temperature;
    }
    return temp;
}
