#include "Sensor.h"
#include "Arduino.h"

Sensor::Sensor(int adc_pin, int min_val, int max_val = 0, int threshold = 100)
    {
      _pin = adc_pin;
      _value = 0;
      _status = 0;
      _max_value = max_val;
      _min_value = min_val;
      _threshold = threshold;
    };

void Sensor::update(void)
    {
      _value = analogRead(_pin);
      if (_max_value < _value)
        _max_value = _value;
      else if (_min_value > _value)
        _min_value = _value;
    };