#ifndef _BUTTON_
#define _BUTTON_

class ButtonData
{
  public:
    ButtonData();
    void ButtonInit(int io_pin);
    void ButtonUpdate();

    bool button_push;
    bool button_puls;

  private:
    int button_pin;
    bool button_init_done;
    bool button_data;
    bool button_data_mem;
    bool button_push_mem;
    long button_millis;
    long button_millis_change;
    long button_millis_delta;
};

#endif
