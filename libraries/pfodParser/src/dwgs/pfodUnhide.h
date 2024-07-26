#ifndef pfodUnhide_h
#define pfodUnhide_h
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

class pfodUnhide : public pfodDwgsBase {
  public:
    pfodUnhide();
    pfodUnhide &idx(uint16_t _idx); // default 0 i.e. not set
    pfodUnhide &idx(pfodAutoIdx &_idx); // reserve if idx 0 else just use idx if not 0
    pfodUnhide &cmd(const char _cmd); // default ' ' i.e. not set
    pfodUnhide &cmd(const char* _cmdStr); 
    pfodUnhide &cmd(pfodAutoCmd &a_Cmd);
    pfodUnhide &loadCmd(const char _loadCmd); // for insertDwgs default ' ' i.e. not set
    pfodUnhide &loadCmd(const char* _loadCmdStr); 
    pfodUnhide &loadCmd(pfodAutoCmd &a_Cmd);
    void init(Print *out, struct pfodDwgVALUES* _values);
    void send(char _startChar = '|');
};
#endif // pfodUnhide_h
