#ifndef UCAS_UART_H__
#define UCAS_UART_H__

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_task.h"
#include "data.h"
#include "freertos/portmacro.h"


#define EX_UART_NUM UART_NUM_0
#define PATTERN_CHR_NUM    (3)       

#define BUF_SIZE (1024)
#define RD_BUF_SIZE (BUF_SIZE)

void ucas_uart_init(long ucas_baud_rate);



#endif