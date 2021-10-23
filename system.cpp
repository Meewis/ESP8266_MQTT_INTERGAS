#include "system.h"
#include <Arduino.h>

SystemData::SystemData()
{
    system_start_puls = false;
    system_start_mem = false;
    system_millis = millis();
    system_millis_100ms = system_millis + 100;
    system_100ms_puls = false;
    system_100ms_block = false;
    system_millis_500ms = system_millis + 500;
    system_500ms_puls = false;
    system_500ms_block = false;
    system_millis_1s = system_millis + 1000;
    system_1s_puls = false;
    system_1s_block = false;
    system_millis_2s = system_millis + 2000;
    system_2s_puls = false;
    system_2s_block = false;
    system_millis_5s = system_millis + 5000;
    system_5s_puls = false;
    system_5s_block = false;
    system_millis_10s = system_millis + 10000;
    system_10s_puls = false;
    system_10s_block = false;
    system_millis_30s = system_millis + 30000;
    system_30s_puls = false;
    system_30s_block = false;
}

void SystemData::SystemUpdate()
{
  if (!system_start_mem)
  {
    system_start_puls = true;
    system_start_mem = true;
  }
  else
  {
    system_start_puls = false;
  }

  system_millis = millis();

  if (system_millis > system_millis_100ms)
  {
    system_millis_100ms = system_millis_100ms + 100;
    system_100ms_puls = true;
    system_100ms_block = !system_100ms_block;
  }
  else
  {
    system_100ms_puls = false;
  }

  if (system_millis > system_millis_500ms)
  {
    system_millis_500ms = system_millis_500ms + 500;
    system_500ms_puls = true;
    system_500ms_block = !system_500ms_block;
  }
  else
  {
    system_500ms_puls = false;
  }

  if (system_millis > system_millis_1s)
  {
    system_millis_1s = system_millis_1s + 1000;
    system_1s_puls = true;
    system_1s_block = !system_1s_block;
  }
  else
  {
    system_1s_puls = false;
  }

  if (system_millis > system_millis_2s)
  {
    system_millis_2s = system_millis_2s + 2000;
    system_2s_puls = true;
    system_2s_block = !system_2s_block;
  }
  else
  {
    system_2s_puls = false;
  }

  if (system_millis > system_millis_5s)
  {
    system_millis_5s = system_millis_5s + 5000;
    system_5s_puls = true;
    system_5s_block = !system_5s_block;
  }
  else
  {
    system_5s_puls = false;
  }

  if (system_millis > system_millis_10s)
  {
    system_millis_10s = system_millis_10s + 10000;
    system_10s_puls = true;
    system_10s_block = !system_10s_block;
  }
  else
  {
    system_10s_puls = false;
  }

  if (system_millis > system_millis_30s)
  {
    system_millis_30s = system_millis_30s + 30000;
    system_30s_puls = true;
    system_30s_block = !system_30s_block;
  }
  else
  {
    system_30s_puls = false;
  }
}
