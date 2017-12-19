#include "button.h"
#include "gpio.h"

#include <fcntl.h>
#include <stdio.h>
#include <sys/epoll.h>


struct epoll_content {
    int switch_gpio,
    callback_fnc_ptr fnc_ptr
};

struct gpio* button_open(int switch_gpio, callback_fnc_ptr fnc_ptr){
    gpio_unexport(switch_gpio);

    gpio_export(switch_gpio);

    gpio_direction(switch_gpio, "in");

    char* v = "/value";
    char* my_path = malloc(strlen(GPIO_BASE) + strlen(switch_gpio) + strlen(v));
    strcpy(my_path, GPIO_BASE);
    strcat(my_path, switch_gpio);
    strcat(my_path, v);
    int f = open(my_path, O_RDWR | O_NONBLOCK);
    free(my_path);
    my_path = 0;

    char* d = "/edge";
    my_path = malloc(strlen(GPIO_BASE) + strlen(switch_gpio) + strlen(d));
    strcpy(my_path, GPIO_BASE);
    strcat(my_path, switch_gpio);
    strcat(my_path, d);
    int j = open(my_path, O_RDWR | O_NONBLOCK);
    char* edge = "both";
    write(j, edge, strlen(edge));
    close(j);
    free(my_path);
    
    //Clean up trigger event
    char[10] dontcare;
    pread(f, dontcare, 10, 0);

    struct epoll_content ec = malloc(sizeof(struct epoll_content));
    ec->switch_gpio = switch_gpio;
    ec->fnc_ptr = fnc_ptr;

    //EPOLL conf
    struct epoll_event eevent = {
        .events = EPOLLERR,
        .data.ptr = ec
    }

    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, f, &eevent);
    if(ret == -1){
        //Log error
        //printf("My bad... sorry. Error %d\n", errno);
        return NULL;
    }


    struct gpio* returnV = malloc(sizeof(struct gpio));
    returnV->value_fd = f;
    return returnV;
}

void button_run(){
    struct epoll_event returned_events[max_events];
    while(1){
        //printf("Will wait\n");
        int nr = epoll_wait(epfd, returned_events, max_events, -1);

        if (nr == -1){
            printf("BUTTON: Well, adios amigos\n");
            return -1;
        }

        for(int i = 0; i < nr; i++){
            if(returned_events[i].events & EPOLLERR){
                struct epoll_content* s = (struct epoll_content*)(returned_events[i].data.ptr);
                s->fnc_ptr(s->switch_gpio);
                continue;
            }
        }
    }
}