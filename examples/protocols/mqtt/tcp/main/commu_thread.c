#include "commu_thread.h" 

static const char *TAG = "COMMU_THREAD";

void* communication_deal_thread_handle = NULL;
extern  esp_mqtt_client_handle_t ucas_client;  //mqtt client for global;

 void communication_deal_thread(void *pv)
{
    while(1)
    {
            vTaskDelay(100);
            ESP_LOGI(TAG, "communication_deal_thread running ");
            if(flag_receive_uart_data)  //收到云端数据，则转发给云端。
            {
                if(-1 == ucas_mqtt_pub(ucas_client, ucas_base_pub_topic[0], buf_uart_to_cloud_payload ,0,MQTT_QUALITY_0))
                {
                   ESP_LOGE(TAG, "ucas_mqtt_pub %s error",ucas_base_pub_topic[0]);
                }
                else
                {
                   ESP_LOGI(TAG, "ucas_mqtt_pub %s success",ucas_base_pub_topic[0]);
                   vTaskDelay(100); 
                }

                //测试时注释，真实运行时，需要去掉注释；
                // flag_receive_uart_data = 0;         
                //memset(buf_uart_to_cloud_payload,0,sizeof(buf_uart_to_cloud_payload));
            }

            if(flag_receive_cloud_data)  //收到云端的数据；
            {
                ESP_LOGI(TAG, "receive data form cloud : %s :  %s",buf_cloud_to_uart_topic,buf_cloud_to_uart_payload);

                flag_receive_cloud_data = 0;
                memset(buf_cloud_to_uart_topic,0,sizeof(buf_cloud_to_uart_topic));
                memset(buf_cloud_to_uart_payload,0,sizeof(buf_cloud_to_uart_payload));
            }
           
    }
//     esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t) pv;
//     uint32_t last_retransmit = 0;
//     int32_t msg_tick = 0;
//     client->run = true;

//     //get transport by scheme
//     client->transport = esp_transport_list_get_transport(client->transport_list, client->config->scheme);

//     if (client->transport == NULL) {
//         ESP_LOGE(TAG, "There are no transports valid, stop mqtt client, config scheme = %s", client->config->scheme);
//         client->run = false;
//     }
//     //default port
//     if (client->config->port == 0) {
//         client->config->port = esp_transport_get_default_port(client->transport);
//     }

//     client->state = MQTT_STATE_INIT;
//     xEventGroupClearBits(client->status_bits, STOPPED_BIT);
//     while (client->run) {
//         MQTT_API_LOCK(client);
//         switch ((int)client->state) {
//         case MQTT_STATE_INIT:
//             xEventGroupClearBits(client->status_bits, RECONNECT_BIT);
//             client->event.event_id = MQTT_EVENT_BEFORE_CONNECT;
//             esp_mqtt_dispatch_event_with_msgid(client);

//             if (client->transport == NULL) {
//                 ESP_LOGE(TAG, "There is no transport");
//                 client->run = false;
//             }

//             if (esp_transport_connect(client->transport,
//                                       client->config->host,
//                                       client->config->port,
//                                       client->config->network_timeout_ms) < 0) {
//                 ESP_LOGE(TAG, "Error transport connect");
//                 client->event.event_id = MQTT_EVENT_ERROR;
//                 client->event.error_handle->error_type = MQTT_ERROR_TYPE_ESP_TLS;
//                 client->event.error_handle->connect_return_code = 0;
// #ifdef MQTT_SUPPORTED_FEATURE_TRANSPORT_ERR_REPORTING
//                 client->event.error_handle->esp_tls_last_esp_err = esp_tls_get_and_clear_last_error(esp_transport_get_error_handle(client->transport),
//                                                                                                     &client->event.error_handle->esp_tls_stack_err,
//                                                                                                     &client->event.error_handle->esp_tls_cert_verify_flags);
// #endif
//                 esp_mqtt_dispatch_event_with_msgid(client);
//                 esp_mqtt_abort_connection(client);
//                 break;
//             }
//             ESP_LOGD(TAG, "Transport connected to %s://%s:%d", client->config->scheme, client->config->host, client->config->port);
//             if (esp_mqtt_connect(client, client->config->network_timeout_ms) != ESP_OK) {
//                 ESP_LOGI(TAG, "Error MQTT Connected");
//                 esp_mqtt_abort_connection(client);
//                 break;
//             }
//             client->event.event_id = MQTT_EVENT_CONNECTED;
//             client->event.session_present = mqtt_get_connect_session_present(client->mqtt_state.in_buffer);
//             client->state = MQTT_STATE_CONNECTED;
//             esp_mqtt_dispatch_event_with_msgid(client);
//             client->refresh_connection_tick = platform_tick_get_ms();

//             break;
//         case MQTT_STATE_CONNECTED:
//             // receive and process data
//             if (mqtt_process_receive(client) == ESP_FAIL) {
//                 esp_mqtt_abort_connection(client);
//                 break;
//             }
            
//             // resend all non-transmitted messages first
//             outbox_item_handle_t item = outbox_dequeue(client->outbox, QUEUED, NULL);
//             if (item) {
//                 if (mqtt_resend_queued(client, item) == ESP_OK) {
//                     outbox_set_pending(client->outbox, client->mqtt_state.pending_msg_id, TRANSMITTED);
//                 }
//                 // resend other "transmitted" messages after 1s
//             } else if (platform_tick_get_ms() - last_retransmit > 1000) {
//                 last_retransmit = platform_tick_get_ms();
//                 item = outbox_dequeue(client->outbox, TRANSMITTED, &msg_tick);
//                 if (item && (last_retransmit - msg_tick > 1000))  {
//                     mqtt_resend_queued(client, item);
//                 }
//             }

//             if (platform_tick_get_ms() - client->keepalive_tick > client->connect_info.keepalive * 1000 / 2) {
//                 //No ping resp from last ping => Disconnected
//                 if (client->wait_for_ping_resp) {
//                     ESP_LOGE(TAG, "No PING_RESP, disconnected");
//                     esp_mqtt_abort_connection(client);
//                     client->wait_for_ping_resp = false;
//                     break;
//                 }
//                 if (esp_mqtt_client_ping(client) == ESP_FAIL) {
//                     ESP_LOGE(TAG, "Can't send ping, disconnected");
//                     esp_mqtt_abort_connection(client);
//                     break;
//                 } else {
//                     client->wait_for_ping_resp = true;
//                 }
//                 ESP_LOGD(TAG, "PING sent");
//             }

//             if (client->config->refresh_connection_after_ms &&
//                     platform_tick_get_ms() - client->refresh_connection_tick > client->config->refresh_connection_after_ms) {
//                 ESP_LOGD(TAG, "Refreshing the connection...");
//                 esp_mqtt_abort_connection(client);
//                 client->state = MQTT_STATE_INIT;
//             }

//             //Delete message after 30 seconds
//             int deleted = outbox_delete_expired(client->outbox, platform_tick_get_ms(), OUTBOX_EXPIRED_TIMEOUT_MS);
//             client->mqtt_state.pending_msg_count -= deleted;
//             if (client->mqtt_state.pending_msg_count < 0) {
//                 client->mqtt_state.pending_msg_count = 0;
//             }
//             //
//             outbox_cleanup(client->outbox, OUTBOX_MAX_SIZE);
//             break;
//         case MQTT_STATE_WAIT_TIMEOUT:

//             if (!client->config->auto_reconnect) {
//                 client->run = false;
//                 client->state = MQTT_STATE_UNKNOWN;
//                 break;
//             }
//             if (platform_tick_get_ms() - client->reconnect_tick > client->wait_timeout_ms) {
//                 client->state = MQTT_STATE_INIT;
//                 client->reconnect_tick = platform_tick_get_ms();
//                 ESP_LOGD(TAG, "Reconnecting...");
//                 break;
//             }
//             MQTT_API_UNLOCK(client);
//             xEventGroupWaitBits(client->status_bits, RECONNECT_BIT, false, true,
//                                 client->wait_timeout_ms / 2 / portTICK_RATE_MS);
//             // continue the while loop instead of break, as the mutex is unlocked
//             continue;
//         }
//         MQTT_API_UNLOCK(client);
//         if (MQTT_STATE_CONNECTED == client->state) {
//             if (esp_transport_poll_read(client->transport, MQTT_POLL_READ_TIMEOUT_MS) < 0) {
//                 ESP_LOGE(TAG, "Poll read error: %d, aborting connection", errno);
//                 esp_mqtt_abort_connection(client);
//             }
//         }

//     }
//     esp_transport_close(client->transport);
//     xEventGroupSetBits(client->status_bits, STOPPED_BIT);

    vTaskDelete(NULL);
}