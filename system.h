#ifndef _SYSTEM_
#define _SYSTEM_

class SystemData
{
  public:
    SystemData();
    void SystemUpdate();
    bool system_start_puls;
    bool system_100ms_puls;
    bool system_100ms_block;
    bool system_500ms_puls;
    bool system_500ms_block;
    bool system_1s_puls;
    bool system_1s_block;
    bool system_2s_puls;
    bool system_2s_block;
    bool system_5s_puls;
    bool system_5s_block;
    bool system_10s_puls;
    bool system_10s_block;
    bool system_30s_puls;
    bool system_30s_block;

  private:
    bool system_start_mem;
    long system_millis;
    long system_millis_100ms = 0;
    long system_millis_500ms = 0;
    long system_millis_1s = 0;
    long system_millis_2s = 0;
    long system_millis_5s = 0;
    long system_millis_10s = 0;
    long system_millis_30s = 0;
};

#endif
