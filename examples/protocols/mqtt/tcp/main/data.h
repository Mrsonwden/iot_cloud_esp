#ifndef DATA_H__
#define DATA_H__
#include "mqtt_topic.h"


#define BUF_SIZE_CLOUD_TO_UART         (1024)
#define BUF_SIZE_UART_TO_CLOUD         (1024)

#define MAX_BASE_TOPIC_LEN                  (30)
#define MAX_TOPIC_LEN                       (MAX_BASE_TOPIC_LEN + 30)  


typedef enum {
  NO_RECEIVED_UART_DATA,
  RECEIVED_UART_DATA,
  DEALWITH_UART_DATA,
  FINISHED_UART_DATA
} status_uart_data;

typedef enum {
  NO_RECEIVED_CLOUD_DATA,
  RECEIVED_CLOUD_DATA,
  DEALWITH_CLOUD_DATA,
  FINISHED_CLOUD_DATA
} status_cloud_data;





//extern char buf_cloud_to_uart_topic[];
extern char buf_cloud_to_uart_topic[MAX_TOPIC_LEN];   //这里还有一个问题，应该去掉上面的定义的 MAX_TOPIC_LEN
extern char buf_uart_to_cloud_payload[BUF_SIZE_UART_TO_CLOUD];
extern char buf_cloud_to_uart_payload[BUF_SIZE_CLOUD_TO_UART];

extern char ucas_status_uart_data ;
extern char ucas_status_cloud_data;



extern int  buf_cloud_to_uart_payload_len ;

#endif