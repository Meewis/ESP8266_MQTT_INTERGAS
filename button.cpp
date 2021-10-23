#include "button.h"
#include <Arduino.h>

ButtonData::ButtonData()
{
  button_pin = 0;
  button_init_done = false;
  button_data = false;
  button_data_mem = false;
  button_push = false;
  button_push_mem = false;
  button_puls = false;
  button_millis_change = millis();     
}

void ButtonData::ButtonInit(int io_pin)
{
  if (io_pin >= 0)
  {
    button_pin = io_pin;
    pinMode(button_pin, INPUT); 
    //digitalWrite(button_pin, true);
    button_init_done = true;
  }
}

void ButtonData::ButtonUpdate()
{
   button_millis = millis();    
   button_millis_delta = button_millis - button_millis_change;

  if (button_init_done)
  {
    button_data = !digitalRead(button_pin);
  }

  if (button_data != button_data_mem)
  {
     button_millis_change = millis();     
  }
  
  if (button_data)
  {
    if (button_millis_delta > 100)
    {
      button_push = true;
    }
  }
  else
  {
    if (button_millis_delta > 100)
    {
      button_push = false;
    }
  }

  if (button_push && !button_push_mem)
  {
    button_puls = true;
  }
  else
  {
    button_puls = false;
  }
  
  button_data_mem = button_data;
  button_push_mem = button_push;
}
