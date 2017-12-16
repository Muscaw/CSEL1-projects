#ifndef button__h
#define button__h

typedef void (*callback_fnc_ptr) (int switch_gpio);

int button_open(int switch_gpio, callback_fnc_ptr fnc_ptr);

void button_run();

#endif