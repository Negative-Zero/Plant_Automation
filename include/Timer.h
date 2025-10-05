#ifndef TIMER_H
#define TIMER_H

class Timer
{
  private:
    int _time;
    int _unit;
    long _start_time;
    long _current_time;
    bool _enabled;
  public:
    Timer(int time, int unit = SECOND);

    void restart(void);
    bool is_complete(void);

    void end(void){_enabled = false;};
};

#endif