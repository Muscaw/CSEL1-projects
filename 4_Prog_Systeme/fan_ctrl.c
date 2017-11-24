#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <stdio.h>


/*
* GPIOs
*/

char* GPIO_EXPORT = "/sys/class/gpio/export";
char* GPIO_UNEXPORT = "/sys/class/gpio/unexport";
char* GPIO_BASE = "/sys/class/gpio/gpio";

// All GPIOS
char* GPIO_PWM = "203"; // Fan controller
const char* GPIO_SW1 = "29";
const char* GPIO_SW2 = "30";
const char* GPIO_SW3 = "22";

const char* GPIO_SWITCHES[] = {
    "29",
    "30",
    "22"
};

//Duty for the PWM
int duty = 50;

// Fan fd
int ffd = 0;

//Clock fd
int cfd = 0;

// Continuous press clock fd
int cpfd = 0;

// Timer specification
struct itimerspec tspec;

struct itimerspec lpspec;

static void gpio_unexport(const char* gpio){
    int f = open(GPIO_UNEXPORT, O_WRONLY);
    write(f, gpio, strlen(gpio));
    close(f);
}

static void gpio_export(const char* gpio) {
    int f = open(GPIO_EXPORT, O_WRONLY);
    write(f, gpio, strlen(gpio));
    close(f);
}

static void gpio_direction(const char* gpio, const char* direction){
    char* d = "/direction";
    char* my_path = malloc(strlen(GPIO_BASE) + strlen(gpio) + strlen(d) + 1);
    strcpy(my_path, GPIO_BASE);
    strcat(my_path, gpio);
    strcat(my_path, d);
    int f = open(my_path, O_WRONLY);
    free(my_path);
    my_path = 0;
    write(f, direction, strlen(direction));
    close(f);
}

//Opens the GPIO fan and returns the FD associated to the value end point
static int open_fan_gpio(){
    // Unexport to make sure it does not exist yet
    gpio_unexport(GPIO_PWM);

    // Export GPIO
    gpio_export(GPIO_PWM);

    // Set GPIO direction
    gpio_direction(GPIO_PWM, "out");

    //Opens the file associated to the value of the GPIO

    char* v = "/value";
    char* my_path = malloc(strlen(GPIO_BASE) + strlen(GPIO_PWM) + strlen(v));
    strcpy(my_path, GPIO_BASE);
    strcat(my_path, GPIO_PWM);
    strcat(my_path, v);
    int f = open(my_path, O_WRONLY);
    free(my_path);
    return f;
}

static int open_switch_gpio(int switch_number){
    gpio_unexport(GPIO_SWITCHES[switch_number]);

    gpio_export(GPIO_SWITCHES[switch_number]);

    gpio_direction(GPIO_SWITCHES[switch_number], "in");

    char* v = "/value";
    char* my_path = malloc(strlen(GPIO_BASE) + strlen(GPIO_SWITCHES[switch_number]) + strlen(v));
    strcpy(my_path, GPIO_BASE);
    strcat(my_path, GPIO_SWITCHES[switch_number]);
    strcat(my_path, v);
    int f = open(my_path, O_RDWR | O_NONBLOCK);
    free(my_path);
    my_path = 0;

    char* d = "/edge";
    my_path = malloc(strlen(GPIO_BASE) + strlen(GPIO_SWITCHES[switch_number]) + strlen(d));
    strcpy(my_path, GPIO_BASE);
    strcat(my_path, GPIO_SWITCHES[switch_number]);
    strcat(my_path, d);
    int j = open(my_path, O_RDWR | O_NONBLOCK);
    char* edge = "both";
    write(j, edge, strlen(edge));
    close(j);
    free(my_path);
    return f;
}


void read_dummy(int fd){
    char dontcare[10];
    pread(fd, dontcare, 10, 0);
}


void change_clock(int cDuty){
    tspec.it_interval.tv_sec = 0;
    tspec.it_interval.tv_nsec = 50000000 / 100 * cDuty;
    tspec.it_value.tv_sec = 0;
    tspec.it_value.tv_nsec = 50000000 / 100 * cDuty;
    timerfd_settime(cfd, 0, &tspec, NULL);
}


//Clock manual PWM
int IsUp = 0;
void clock_event(int fd){
    char* v;
    int cDuty = 0;
    if(IsUp){
        v = "1";
        cDuty = duty;
    } else {
        v = "0";
        cDuty = 100-duty;
    }
    IsUp = !IsUp;
    write(ffd, v, 1);
    change_clock(cDuty);
    read_dummy(fd);
}

//Continuous long press clock handler

int increment = 0;

void setup_long_press(int inc){
    lpspec.it_interval.tv_sec = 0;
    lpspec.it_interval.tv_nsec = 0;
    lpspec.it_value.tv_sec = 1;
    lpspec.it_value.tv_nsec = 0;
    increment = inc;
    timerfd_settime(cpfd, 0, &lpspec, NULL);
    printf("Clock setup\n");
}

void disable_long_press(){
    lpspec.it_interval.tv_sec = 0;
    lpspec.it_interval.tv_nsec = 0;
    lpspec.it_value.tv_sec = 0;
    lpspec.it_value.tv_nsec = 0;
    timerfd_settime(cpfd, 0, &lpspec, NULL);
    printf("Clock disabled\n");
}

void long_press_clock(int fd){
    if((duty < 99 && increment > 0) || (duty > 0 && increment < 0)){
        printf("Current duty: %d\n", duty);
        duty += increment;
    }
    uint64_t s;
    read(fd, &s, sizeof(uint64_t));
//    read_dummy(fd);
}

// SW1
int IsDownDown = 1;
void button_down(int fd){
    if(IsDownDown){
        setup_long_press(-1);
        /*if(duty > 0){
            duty -= 1;
        }*/
    } else {
        disable_long_press();
    }
    IsDownDown = !IsDownDown;
    read_dummy(fd);
}
//SW2
int IsMidDown = 1;
void button_mid(int fd){
    if(IsMidDown){
        duty = 50;
        printf("Current duty: %d\n", duty);
        //change_clock();
        
    }
    IsMidDown = !IsMidDown;
    read_dummy(fd);
}
//SW3
int IsUpDown = 1;
void button_up(int fd){
    if(IsUpDown){
        /*if(duty < 99){
            duty += 1;
        }*/
        //change_clock();
        setup_long_press(1);
        printf("Current duty: %d\n", duty);
    } else {
        disable_long_press();
    }
    IsUpDown = !IsUpDown;
    read_dummy(fd);
}

struct mepe {
    void (*callback)(int);
    int fd;
};


int main(void){
    //EPOLL context creation
    int epfd = epoll_create1(0);
    if (epfd == -1){
        //Log to syslog that error
        printf("Could not create EPOLL. Error %d", errno);
        return -1;
    }

    //FD for fan and switches
    cfd = timerfd_create(CLOCK_MONOTONIC, 0);
    int sw1fd = open_switch_gpio(0);
    char dontcare[10];
    pread(sw1fd, dontcare, 10, 0);
    int sw2fd = open_switch_gpio(1);
    pread(sw2fd, dontcare, 10, 0);
    int sw3fd = open_switch_gpio(2);
    pread(sw3fd, dontcare, 10, 0);
    ffd = open_fan_gpio();

    //FD for long press
    cpfd = timerfd_create(CLOCK_MONOTONIC, 0);

    
    struct mepe cepe = {
        .callback = &clock_event,
        .fd = cfd
    };

    struct mepe s1epe = {
        .callback = &button_down,
        .fd = sw1fd
    };

    struct mepe s2epe = {
        .callback = &button_mid,
        .fd = sw2fd
    };

    struct mepe s3epe = {
        .callback = &button_up,
        .fd = sw3fd
    };

    struct mepe lpepe = {
        .callback = &long_press_clock,
        .fd = cpfd
    };

    // Events for EPOLL
    struct epoll_event events[] = {
         {
            .events = EPOLLIN,
            .data.ptr = &cepe
         },
         {
             .events = EPOLLERR,
             .data.ptr = &s2epe,
         },
         {
            .events = EPOLLERR,
            .data.ptr = &s1epe,
        },
        {
            .events = EPOLLERR,
            .data.ptr = &s3epe,
        },
        {
            .events = EPOLLIN,
            .data.ptr = &lpepe
        }
    };

    // Setup EPOLL
    for(unsigned int i = 0; i < sizeof(events) / sizeof(events[0]); i++){
        int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, ((struct mepe*)(events[i].data.ptr))->fd, &events[i]);
        if(ret == -1){
            //Log error
            printf("My bad... sorry. Error %d\n", errno);
            return -1;
        }
    }

    tspec.it_interval.tv_sec = 0;
    tspec.it_interval.tv_nsec = 50000000 / 100 * duty;
    tspec.it_value.tv_sec = 0;
    tspec.it_value.tv_nsec = 1;
    timerfd_settime(cfd, 0, &tspec, NULL);

    int max_events = sizeof(events) / sizeof(events[0]);
    struct epoll_event returned_events[max_events];
    while(1){
        //printf("Will wait\n");
        int nr = epoll_wait(epfd, returned_events, max_events, -1);

        if (nr == -1){
            printf("Well, adios amigos\n");
            return -1;
        }

        for(int i = 0; i < nr; i++){
            if(returned_events[i].events & EPOLLERR){
                struct mepe* s = (struct mepe*)(returned_events[i].data.ptr);
                s->callback(s->fd);
                continue;
            }
            if (returned_events[i].events & EPOLLHUP){
                continue;
            } else if (returned_events[i].events & EPOLLIN){
                struct mepe* s = (struct mepe*)(returned_events[i].data.ptr);
                s->callback(s->fd);
            } else {
                continue;
            }
        }
    }

    return 0;
}
