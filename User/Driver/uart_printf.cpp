//
// Created by chengfeng on 2025/10/30.
//

#include "uart_printf.h"
#include <stdarg.h>
#include <stdio.h>
#include "usart.h"

#define UART_TX_HANDLE huart6

void uart_printf(const char *format, ...)
{
    char buffer[128];
    va_list args;
    va_start(args, format);
    int len = vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    HAL_UART_Transmit(&UART_TX_HANDLE, (uint8_t *) buffer, len, HAL_MAX_DELAY);
}
