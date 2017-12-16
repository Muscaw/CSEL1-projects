#include "pwm.h"
#include <linux/pwm.h>

#define PERIOD_NS 20972

static struct pwm_device* pwmHandle;

void pwm_init(unsigned int baseDutyPercentage){
    pwmHandle = pwm_request(0, "fan_pwm");
    pwm_change_duty(baseDutyPercentage);
}

void pwm_change_duty(unsigned int dp){
    if(dp >= 100){
        dp = 99;
    }
    pwm_config(pwmHandle, PERIOD_NS / 100.f * dp, PERIOD_NS);
}

void pwm_start(){
    pwm_enable(pwm);
}

void pwm_stop(){
    pwm_stop(pwm);
}

void pwm_relese(){
    pwm_free(pwm);
    pwm = NULL;
}

int pwm_is_released(){
    return pwm == NULL;
}