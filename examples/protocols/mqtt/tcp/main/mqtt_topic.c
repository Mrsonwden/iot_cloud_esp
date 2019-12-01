#include "mqtt_topic.h"


static const char *TAG = "MQTT_TOPIC";
// char device_type[2][10] = 
// {
//     "actor",
//     "sensor"
// };

char flag_base_sub_topic[MAX_FLAG_BASE_TOPIC_LEN]  = "sub";
char flag_base_pub_topic[MAX_FLAG_BASE_TOPIC_LEN]  = "pub";



char ucas_base_sub_topic[MAX_SUB_TOPIC][MAX_BASE_TOPIC_LEN]=
{
    // "sub/actor/set",
    // "sub/actor/get",
    // "sub/sensor/get"
} ;

char ucas_base_pub_topic[MAX_PUB_TOPIC][MAX_BASE_TOPIC_LEN]=
{
    "pub/actor/status",
    "pub/sensor/status"
} ;

KV_sub_topic_func   kv_ucas_sub_topic_func[MAX_SUB_TOPIC] = {

    // {"sub/actor/set",&sub_actor_set},
    // {"sub/actor/get",&sub_actor_get},
    // {"sub/sensor/get",&sub_sensor_get}
};

char my_device_type[MAX_TYPE_ID_PSWD_LEN] = "runner";
char my_device_id[MAX_TYPE_ID_PSWD_LEN]   = "ucas";
char my_device_pswd[MAX_TYPE_ID_PSWD_LEN] = "ucas";

char register_topic_func(char* topic_str ,void (*func)(char* para,int para_len))
{
    if(func == NULL)
    {
        ESP_LOGE(TAG, "register_topic_func func == NULL");
        return -1;
    }
    if((strlen(topic_str) > MAX_BASE_TOPIC_LEN) || ((strlen(topic_str)  < 1) ))
    {
        ESP_LOGE(TAG, "register_topic_func strlen(topic_str) is error %d ",strlen(topic_str));
        return -1;
    }
    int i = 0;
    for(i = 0; i <  MAX_SUB_TOPIC; i++)
    {
          if(strlen(ucas_base_sub_topic[i]) == 0)
          {
              break;
          }
    }
    if(i == MAX_SUB_TOPIC)
    {
        ESP_LOGE(TAG, "can't add : topic num have reach the MAX %d ",MAX_SUB_TOPIC);
        return -1;
    }

    strcpy(ucas_base_sub_topic[i],topic_str);                           //拷贝给基础的subtopic；
    strcpy(kv_ucas_sub_topic_func[i].sub_topic,topic_str);              //拷贝给键值对；
    kv_ucas_sub_topic_func[i].func = func;

    // for(i = 0; i <  MAX_SUB_TOPIC; i++)
    // {
    //       if(strlen(ucas_base_sub_topic[i]) != 0)
    //       {
    //           ESP_LOGI(TAG, "have register topic  %s ",ucas_base_sub_topic[i]);
    //       }
    // }
    return 0;
}


//topic 基本格式： client_type/client_id/client_password/base_topic

char ucas_mqtt_sub(esp_mqtt_client_handle_t client, const char base_sub_topic[][MAX_BASE_TOPIC_LEN], int topic_num ,int qos)
{
     int i = 0;
     char topic[MAX_TOPIC_LEN];
     memset(topic,0,sizeof(topic));
     strcat(topic,my_device_type);
     strcat(topic,"/"); 
     strcat(topic,my_device_id);
     strcat(topic,"/"); 
     strcat(topic,my_device_pswd);
     strcat(topic,"/"); 
     char temp_topic[MAX_TOPIC_LEN];
     char temp[MAX_BASE_TOPIC_LEN];
     ESP_LOGI(TAG, "topic_num  %d",topic_num);
     for(i = 0;i < topic_num; i++)
     {

         memset(temp,0,sizeof(temp));
         strncpy(temp,base_sub_topic[i],MAX_BASE_TOPIC_LEN);
         ESP_LOGI(TAG, "temp %s  ",temp);
         if(0 == strlen(temp))
         {
             ESP_LOGI(TAG, "sub topic  finished i: %d",i);
             break;
         }

         memset(temp_topic,0,sizeof(temp_topic));
         strcpy(temp_topic,topic);
         strcat(temp_topic,temp);
         ESP_LOGI(TAG, "sub topic: %s",temp_topic);

         if(-1 == esp_mqtt_client_subscribe(client, temp_topic, qos))
         {
            ESP_LOGI(TAG, "sent subscribe %s failed",temp_topic);
         }
         else
         {
             ESP_LOGI(TAG, "sent subscribe  %s successed",temp_topic);
         };
         
     } 
     return 0;        
}

// /**
//  * @brief Client to send a publish message to the broker
//  *
//  * Notes:
//  * - Client doesn't have to be connected to send publish message
//  *   (although it would drop all qos=0 messages, qos>1 messages would be enqueued)
//  * - It is thread safe, please refer to `esp_mqtt_client_subscribe` for details
//  *
//  * @param client    mqtt client handle
//  * @param topic     topic string
//  * @param data      payload string (set to NULL, sending empty payload message)
//  * @param len       data length, if set to 0, length is calculated from payload string
//  * @param qos       qos of publish message
//  * @param retain    retain flag
//  *
//  * @return message_id of the publish message (for QoS 0 message_id will always be zero) on success.
//  *         -1 on failure.
//  */
// int esp_mqtt_client_publish(esp_mqtt_client_handle_t client, const char *topic, const char *data, int len, int qos, int retain);


char ucas_mqtt_pub(esp_mqtt_client_handle_t client, const char base_pub_topic[], char* payload ,int payload_len,int qos)
{
     char topic[MAX_TOPIC_LEN];
     memset(topic,0,sizeof(topic));
     strcat(topic,my_device_type);
     strcat(topic,"/"); 
     strcat(topic,my_device_id);
     strcat(topic,"/"); 
     strcat(topic,my_device_pswd);
     strcat(topic,"/"); 
     strcat(topic,base_pub_topic);
     ESP_LOGI(TAG, "pub topic  %s",topic);


    if(-1 == esp_mqtt_client_publish(client,topic, payload, payload_len, qos, 0))
    {
       ESP_LOGI(TAG, "sent  pub  %s failed",topic);
       return -1;
    }
    else
    {
        ESP_LOGI(TAG, "sent pub  %s successed",topic);
    }
         
     return 0;        
}


void* sub_actor_set(char* para,int para_len)
{
      ESP_LOGI(TAG, "sub_actor_set para %s ",para);

      return NULL;
}

void* sub_actor_get(char* para,int para_len)
{
      ESP_LOGI(TAG, "sub_actor_get para %s ",para);

      return NULL;
}

void* sub_sensor_get(char* para,int para_len)
{
      ESP_LOGI(TAG, "sub_sensor_get para %s ",para);

      return NULL;
}