#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

/*
* GPIOs
*/

char* GPIO_EXPORT = "/sys/class/gpio/export";
char* GPIO_UNEXPORT = "/sys/class/gpio/unexport";
char* GPIO_BASE = "/sys/class/gpio/gpio";

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

