#ifndef pfodTouchActionInput_h
#define pfodTouchActionInput_h
/*
 * (c)2014-2017 Forward Computing and Control Pty. Ltd.
 * NSW Australia, www.forward.com.au
 * This code is not warranted to be fit for any purpose. You may only use it at your own risk.
 * This code may be freely used for both private and commercial use
 * Provide this copyright is maintained.
 */
#include <stdint.h>
#include <Arduino.h>
#include "pfodDwgsBase.h"

class pfodTouchActionInput : public pfodDwgsBase {
  public:
    pfodTouchActionInput(); // always left() aligned
    pfodTouchActionInput &cmd(const char _cmd); // default ' ' not set
    pfodTouchActionInput &cmd(const char* _cmdStr); // for multi char cmds
    pfodTouchActionInput &cmd(pfodAutoCmd &a_Cmd);
    pfodTouchActionInput &encode(); // replace restricted chars in prompt the default
    pfodTouchActionInput &doNotEncode(); // disable replacement of restricted chars in prompt
    pfodTouchActionInput &prompt(const char* txt);
    pfodTouchActionInput &prompt(const __FlashStringHelper *txtF);
    pfodTouchActionInput &textIdx(uint16_t _idx); 
    pfodTouchActionInput &textIdx(pfodAutoIdx &a_idx); 
    pfodTouchActionInput &color(int _color); // default WHITE
    pfodTouchActionInput &backgroundColor(int _color); // default BLACK
    pfodTouchActionInput &fontSize(int _font); // default 0 = <+0>
    pfodTouchActionInput &bold(); 
    pfodTouchActionInput &italic(); 
    pfodTouchActionInput &underline();
    void init(Print *out, struct pfodDwgVALUES* _values);
    void send(char _startChar = '|');
  private:
};
#endif // pfodTouchActionInput_h
