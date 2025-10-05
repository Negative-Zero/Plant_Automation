#ifndef SENSOR_H
#define SENSOR_H

class Sensor
{
  private:
    int _pin;
    int _value;
    int _status;
    int _max_value;
    int _min_value;
    int _threshold;
  public:
    Sensor(int adc_pin, int min_val, int max_val = 0, int threshold = 1000);
    
    void update(void);
    
    int pin(void){return _pin;};
    int value(void){return _value;};
    void set_status(int status);
    int status(void){return _status;};
    int max(void){return _max_value;};
    int min(void){return _min_value;};
};

#endif