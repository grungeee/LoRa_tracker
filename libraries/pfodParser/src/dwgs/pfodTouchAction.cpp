/*
 * (c)2014-2017 Forward Computing and Control Pty. Ltd.
 * NSW Australia, www.forward.com.au
 * This code is not warranted to be fit for any purpose. You may only use it at your own risk.
 * This code may be freely used for both private and commercial use
 * Provide this copyright is maintained.
 */
#include "pfodTouchAction.h"
 

pfodTouchAction::pfodTouchAction()  {
}

void pfodTouchAction::init(Print *_out, struct pfodDwgVALUES* _values) {
 // initValues(_values);
  out = _out;
 // don't do this here else action(pfodDwgsBase &_action)  will force send
 // valuesPtr->lastDwg = this; 
 valuesPtr = _values;
 // Only this class clears and uses these variables
 valuesPtr->actionPtr = NULL; // for linking touchAction to action  NOTE: not cleared by other classes init()
 valuesPtr->touchCmd = ' '; // for linking touchActions to touchZone  NOTE: not cleared by other classes init()
 valuesPtr->touchCmdStr = NULL; // for linking touchActions to touchZone NOTE: not cleared by other classes init()
}


pfodTouchAction &pfodTouchAction::cmd(const char _cmd) {
  valuesPtr->touchCmd = _cmd;
  valuesPtr->touchCmdStr = NULL;
  return *this;
}

pfodTouchAction &pfodTouchAction::cmd(const char* _cmdStr) {
  valuesPtr->touchCmd = ' ';
  valuesPtr->touchCmdStr = _cmdStr;
  return *this;
}

pfodTouchAction &pfodTouchAction::cmd(pfodAutoCmd &a_Cmd) {
  valuesPtr->touchCmd = ' ';
  valuesPtr->touchCmdStr = a_Cmd.cmd;
  return *this;
}

pfodTouchAction &pfodTouchAction::action(pfodDwgsBase &_action) {
  valuesPtr->actionPtr = &_action;
  return *this;
}

void pfodTouchAction::send(char _startChar) {
  out->print(_startChar);
  out->print('X');
  out->print('~');

  if ((valuesPtr->touchCmdStr) && (*valuesPtr->touchCmdStr)) {
    out->print(valuesPtr->touchCmdStr);
  } else {
    out->print(valuesPtr->touchCmd);
  }
  if (valuesPtr->actionPtr != NULL) {
  	  valuesPtr->actionPtr->send('~');
  }
 // valuesPtr->lastDwg = NULL; // sent now
}


