#ifndef DATA_H__
#define DATA_H__

#include "mqtt_topic.h"

#define BUF_SIZE_CLOUD_TO_UART         (1024)
#define BUF_SIZE_UART_TO_CLOUD         (1024)



extern char flag_receive_uart_data ;
extern char flag_receive_cloud_data;

extern char buf_uart_to_cloud_payload[BUF_SIZE_UART_TO_CLOUD];
extern char buf_cloud_to_uart_payload[BUF_SIZE_CLOUD_TO_UART];
extern char buf_cloud_to_uart_topic[MAX_TOPIC_LEN];                          //含有身份信息的topic;


#endif