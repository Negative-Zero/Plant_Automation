#include "Timer.h"

void Timer::Timer(int time, int unit = 1000) //Default unit is seconds
{
    _time = time;
    _unit = unit;
    _start_time = 0;
    _current_time = 999999;
    _enabled = true;
};

void Timer::restart(void)
{
    _start_time = millis();
    _enabled = true;
};

bool Timer::is_complete(void)
{
    _current_time = millis();
    if(!_enabled)
    return true;
    
    if(_current_time - _start_time >= _time * _unit)
    return true;
    else
    return false;
};