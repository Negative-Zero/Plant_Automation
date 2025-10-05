#include <Arduino.h>
#include <heltec.h>
#include <ezButton.h>
#include "images.h"


const int SOIL_PIN = 2;
const int WINDOW_PIN = 27;
const int PUMP_RELAY_PIN = 14;
const int BUTTON_A_PIN = 12;
const int BUTTON_B_PIN = 13;
const int LED_PIN = 25;

const long DAY = 86400000;
const long HOUR = 3600000;
const long MINUTE = 60000;
const long SECOND =  1000;

void display_runtime(int, int);
String update_soil_status();
String update_window_status();
void menu_stats1(bool);
void menu_stats2();
void menu_blank();
bool menu_auto(bool);


const int _MOVING_AVERAGE_SAMPLE_SIZE = 40;
class Sensor
{
  private:
    int _pin;
    int _value;
    int _status;
    int _max_value;
    int _min_value;
    int _threshold;
    int _average_value; // Calculated from moving average
    int _moving_average[_MOVING_AVERAGE_SAMPLE_SIZE - 1];
    int _average_index = 0;
    int _sample_index = 0;
    int _SAMPLE_SPACING = 1000; // Number of readings to skip before taking sample
  public:
    Sensor(int adc_pin, int min_val, int max_val = 0, int threshold = 100)
    {
      _pin = adc_pin;
      _value = 0;
      _status = 0;
      _max_value = max_val;
      _min_value = min_val;
      _threshold = threshold;
    };

    void update(void)
    {
      _value = analogRead(_pin);
      if (_max_value < _value)
        _max_value = _value;
      else if (_min_value > _value)
        _min_value = _value;
    };

    int update_average(int value)
    {;
      if(_sample_index >= _SAMPLE_SPACING)
      {
        _moving_average[_average_index] = value;
        if (_average_index < _MOVING_AVERAGE_SAMPLE_SIZE - 1)
          _average_index++;
        else
          _average_index = 0;

        for(int i = 0; i < _MOVING_AVERAGE_SAMPLE_SIZE - 1; i++)
        {
          _average_value += _moving_average[i];
        }
        _average_value /= _MOVING_AVERAGE_SAMPLE_SIZE;


        _sample_index = 0;
      }
      else
      {
        _sample_index++;
      }
      return _average_value;
    };

    int average(void)
    {return _average_value;};

    int pin(void)
    {return _pin;};

    int value(void)
    {return _value;};

    void set_status(int status)
    {_status = status;};

    int status(void)
    {return _status;};
    
    int max(void)
    {return _max_value;};

    int min(void)
    {return _min_value;};

};


class Timer
{
  private:
    int _time;
    int _unit;
    long _start_time;
    long _current_time;
    bool _enabled;
  public:
    Timer(int time, int unit = 1)
    {
      _time = time;
      _unit = unit;
      _start_time = 0;
      _current_time = 999999;
      _enabled = true;
    };

    void restart(void)
    {
      _start_time = millis();
      _enabled = true;
    };

    void end(void)
    {
      _enabled = false;
    }

    bool is_complete(void)
    {
      _current_time = millis();
      if(!_enabled)
        return true;
        
      if(_current_time - _start_time >= _time * _unit)
        return true;
      else
        return false;
    };
};


Sensor soil(SOIL_PIN, 3000);
Sensor window(WINDOW_PIN, 5000);

Timer display_refresh(1,SECOND);
Timer display_timeout(30, SECOND);
Timer water_pump(8, SECOND);

ezButton button_a(BUTTON_A_PIN, INPUT_PULLUP);
ezButton button_b(BUTTON_B_PIN, INPUT_PULLUP);


void setup()
{
  Serial.begin(115200);
  display_refresh.restart();

  Heltec.begin(true /*DisplayEnable Enable*/, false /*LoRa Disable*/, true /*Serial Enable*/);
  Heltec.display->setTextAlignment(TEXT_ALIGN_LEFT);
  Heltec.display->setFont(ArialMT_Plain_10);
  Heltec.display->clear();

  pinMode(soil.pin(), INPUT);
  pinMode(window.pin(), INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT); // Onboard LED to display auto-enabled

  digitalWrite(PUMP_RELAY_PIN, LOW); // Set water pump to OFF
  water_pump.end();

  button_a.setDebounceTime(50);
  button_b.setDebounceTime(50);
  delay(1);
}


void loop()
{
  static int menu_page = 0;
  static bool auto_pump_enabled = false;
  static int dry_value = 1700;
  static int moist_value = 1550;

  button_a.loop();
  button_b.loop();

  soil.update();
  window.update();

  if(button_a.isPressed())
  {
    Heltec.display->clear();
    display_refresh.end();
    display_timeout.restart();
    if(menu_page < 2) // Number of menu pages
      menu_page++;
    else
      menu_page = 0;
  }

  if(menu_page == 0)
    menu_stats1(auto_pump_enabled);
  else if(menu_page == 1)
    menu_stats2();
  else if(menu_page == 99)
    menu_blank();
  else
    auto_pump_enabled = menu_auto(auto_pump_enabled);
  
  if(display_refresh.is_complete())
    display_refresh.restart();
  
  if(display_timeout.is_complete())
  {
    display_timeout.end();
    menu_page = 99;
  }

// Water when soil is dry and auto enabled
  if(auto_pump_enabled)
  {
    digitalWrite(LED_PIN, HIGH);
    if(water_pump.is_complete() && soil.status() == 2)
      water_pump.restart();
  }
  else
    digitalWrite(LED_PIN, LOW);

// Run the pump
  if(!water_pump.is_complete())
    digitalWrite(PUMP_RELAY_PIN, HIGH);
  else
    digitalWrite(PUMP_RELAY_PIN, LOW);

  Heltec.display->display();
}


void display_runtime(int oled_x, int oled_y)
{
  long time_now = millis();
  
  int days = time_now / DAY ;
  int hours = (time_now % DAY) / HOUR;
  int minutes = ((time_now % DAY) % HOUR) / MINUTE ;
  int seconds = (((time_now % DAY) % HOUR) % MINUTE) / SECOND;
  Heltec.display->drawString(oled_x, oled_y, "Runtime: " + String(days) + "d " + String(hours) + "h " + String(minutes) + "m " + String(seconds) + "s");
}


String update_soil_status()
{
  int average_soil_value = soil.update_average(soil.value());
  String soil_status;

  switch(soil.status())
  {
    case 0:
      if(average_soil_value > 1550)
        soil.set_status(1);
      break;
    case 1:
      if(average_soil_value > 1700)
        soil.set_status(2);
      else if (average_soil_value < 1500)
        soil.set_status(0);
      break;
    case 2:
      if(average_soil_value < 1650)
        soil.set_status(1);
      break;
  }
    
  switch(soil.status())
  {
    case 0:
      soil_status = "Wet";
      break;
    case 1:
      soil_status = "Moist";
      break;
    case 2:
      soil_status = "Dry";
      break;
  }

  return soil_status;
}


String update_window_status()
{
  String window_status;

  switch(window.status())
  {
    case 0:
      if(window.value() > 500)
        window.set_status(1);
      break;
    case 1:
      if(window.value() > 900)
        window.set_status(2);
      else if(window.value() < 450)
        window.set_status(0);
      break;
    case 2:
      if(window.value() > 2000)
        window.set_status(3);
      else if(window.value() < 850)
        window.set_status(1);
      break;
    case 3:
      if(window.value() < 1950)
        window.set_status(2);
      break;
  }

  switch(window.status())
  {
    case 0:
      window_status = "Night";
      break;
    case 1:
      window_status = "Cloudy";
      break;
    case 2:
      window_status = "Shady";
      break;
    case 3:
      window_status = "Sunny";
      break;
  }

  return window_status;
}


void menu_stats1(bool auto_enabled)
{
  String soil_status = update_soil_status();
  String window_status = update_window_status();

  float voltage_value = float(map(soil.value(), 929, 2783, 889, 2396))/1000; // Voltage ranges from 0.889V dry to 2.396V wet
  if(display_refresh.is_complete())
  {
    Heltec.display->clear();
    Heltec.display->drawString(0, 0, "Soil: " + String(soil.value()));
    Heltec.display->drawString(70, 0, soil_status);
    Heltec.display->drawString(0, 12, "Window: " + String(window.value()));
    Heltec.display->drawString(70, 12, window_status);
    Heltec.display->drawString(0, 30, "SL: " + String(soil.min()));
    Heltec.display->drawString(70, 30, "SH: " + String(soil.max()));
    //Heltec.display->drawString(0, 42, "WL: " + String(window.min()));
    //Heltec.display->drawString(70, 42, "WH: " + String(window.max()));
    Heltec.display->drawString(0, 42, "S Avg: " + String(soil.average()));
    Heltec.display->drawString(70, 42, "Auto: ");
    if(auto_enabled)
      Heltec.display->drawString(100, 42, "E");
    else
      Heltec.display->drawString(100, 42, "D");
    display_runtime(0, 54);
  }
}

void menu_stats2()
{
  update_soil_status();
  update_window_status();

  if(display_refresh.is_complete())
  {
    Heltec.display->clear();
    int image_index = soil.status() * 10 + window.status();
    //Serial.println(image_index);
    switch(image_index)
    {
      case 03:
        Heltec.display->drawXbm(0, 0, image1_width, image1_height, image1_bits);
        break;
      case 13:
        Heltec.display->drawXbm(0, 0, image2_width, image2_height, image2_bits);
        break;
      case 23:
        Heltec.display->drawXbm(0, 0, image3_width, image3_height, image3_bits);
        break;
      case 02:
        Heltec.display->drawXbm(0, 0, image4_width, image4_height, image4_bits);
        break;
      case 12:
        Heltec.display->drawXbm(0, 0, image5_width, image5_height, image5_bits);
        break;
      case 22:
        Heltec.display->drawXbm(0, 0, image6_width, image6_height, image6_bits);
        break;
      case 01:
        Heltec.display->drawXbm(0, 0, image7_width, image7_height, image7_bits);
        break;
      case 11:
        Heltec.display->drawXbm(0, 0, image8_width, image8_height, image8_bits);
        break;
      case 21:
        Heltec.display->drawXbm(0, 0, image9_width, image9_height, image9_bits);
        break;
      case 00:
        Heltec.display->drawXbm(0, 0, image10_width, image10_height, image10_bits);
        break;
      case 10:
        Heltec.display->drawXbm(0, 0, image11_width, image11_height, image11_bits);
        break;
      case 20:
        Heltec.display->drawXbm(0, 0, image12_width, image12_height, image12_bits);
        break;
    }
  }
  
  // Water manually
  if(soil.status() == 2)
  {
    Heltec.display->drawString(0,0, "Press B");
    Heltec.display->drawString(0,12, "to add water.");
  }
  
  if(button_b.getState() == 0 && water_pump.is_complete())
  {
    water_pump.restart();
  //  Serial.println("B"); // debugging **************
  }
}

void menu_blank()
{
  update_soil_status(); // need to update even while display is off
  update_window_status();
  Heltec.display->clear();
}

bool menu_auto(bool auto_enabled)
{
  static bool action_button_flag = false;

  update_soil_status();
  update_window_status();

  //Serial.println(button_b.getState()); // debugging **************

  //reset button flag
  if(button_b.getState() && !action_button_flag)
    action_button_flag = true;

  if(!button_b.getState() && action_button_flag)
  {
    display_refresh.restart();
    Heltec.display->clear();
    if(auto_enabled)
      auto_enabled = false;
    else
      auto_enabled = true;
    action_button_flag = false;
  }
  Heltec.display->drawString(0,0, "Automated watering:");
  Heltec.display->drawString(0,24, "(press B to change option)");
  if(auto_enabled)
    Heltec.display->drawString(0,12, "enabled");
  else
    Heltec.display->drawString(0,12, "disabled");
  
  return auto_enabled;
}
