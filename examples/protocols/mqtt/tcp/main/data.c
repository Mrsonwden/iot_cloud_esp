

#include "data.h"

//传感器/执行器数据格式（v1)： string；
char flag_receive_uart_data = 1;
char flag_receive_cloud_data = 0;

char buf_uart_to_cloud_payload[BUF_SIZE_UART_TO_CLOUD] = "hello , iot cloud ,this message come from runner";
char buf_cloud_to_uart_payload[BUF_SIZE_CLOUD_TO_UART];
char buf_cloud_to_uart_topic[MAX_TOPIC_LEN];                          //含有身份信息的topic;