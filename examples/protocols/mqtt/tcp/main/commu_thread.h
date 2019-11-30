#ifndef COMMU_THREAD_H__
#define COMMU_THREAD_H__

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event_loop.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_task.h"

#include "mqtt_topic.h"
#include "data.h"


#define COMMU_THREAD_STACK_SIZE                          (4096)
#define COMMU_THREAD_TASK_PRIO                           (ESP_TASK_PRIO_MAX - 5)

extern void* communication_deal_thread_handle;

void communication_deal_thread(void *pv);


#endif


