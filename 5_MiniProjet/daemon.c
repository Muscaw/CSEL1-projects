#include "pwm.h"


//Button GPIO addresses
const char* GPIO_SW1 = "29";
const char* GPIO_SW2 = "30";
const char* GPIO_SW3 = "22";

int main(void){
    pwm_init(50);

}