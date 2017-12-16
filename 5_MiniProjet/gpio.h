#ifndef gpio__h
#define gpio__h

const char* GPIO_EXPORT = "/sys/class/gpio/export";
const char* GPIO_UNEXPORT = "/sys/class/gpio/unexport";
const char* GPIO_BASE = "/sys/class/gpio/gpio";

struct gpio{
    int value_fd,

};

#endif