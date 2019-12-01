
#include <stdio.h>
#include <string.h>

void (*func)(int para,int len);

void fun(int a, int b)
{
    printf("this is fun\n\r");
}

int main()
{
    func = fun;
    if(func == NULL)
    {
        printf("func == null");
    }
    else
    {
        printf("func != null");
        func(3,3);

    }

}

/*
                char* base_topic = NULL;
                char received_base_topic[30];
                char flag_base_sub_topic[10] = "sub";
                char buf_cloud_to_uart_topic[50] = "runner/ucas/ucas/sub/actor/get";
                if( NULL != (base_topic = strstr(buf_cloud_to_uart_topic,flag_base_sub_topic)) )
                {
                       printf(" base_topic : %s ",base_topic);
                       strcpy(received_base_topic,base_topic);
                }
                else
                {
                      printf(" received_base_topic : %s ",received_base_topic);
                }
                */
