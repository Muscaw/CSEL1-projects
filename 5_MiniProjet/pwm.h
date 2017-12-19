
#ifndef pwm__h
#define pwm__h
/*
* Inits the first PWM and configures it with baseDutyPercentage.
*/
void pwm_init(unsigned int baseDutyPercentage);

/*
* Changes duty percentage of the PWM.
*/
void pwm_change_duty(unsigned int dutyPercentage);

/*
* Starts the PWM.
*/
void pwm_start();
/*
* Stops the PWM.
*/
void pwm_stop();
/*
* Releases the handle to the PWM.
*/
void pwm_release();

/*
* Returns true if the PWM is released (therefore, you need to use pwm_init)
*/
int pwm_is_released();

#endif