/*
 * (c)2014-2017 Forward Computing and Control Pty. Ltd.
 * NSW Australia, www.forward.com.au
 * This code is not warranted to be fit for any purpose. You may only use it at your own risk.
 * This code may be freely used for both private and commercial use
 * Provide this copyright is maintained.
 */
#include "pfodTouchActionInput.h"
 

pfodTouchActionInput::pfodTouchActionInput()  {
}

void pfodTouchActionInput::init(Print *_out, struct pfodDwgVALUES* _values) {
 // initValues(_values);
  out = _out;
 // don't do this here else action(pfodDwgsBase &_action)  will force send
 // valuesPtr->lastDwg = this; 
 valuesPtr = _values;
 valuesPtr->actionPtr = NULL; // for linking touchAction to action  NOTE: not cleared by other classes init()
 valuesPtr->touchCmd = ' '; // for linking touchActions to touchZone  NOTE: not cleared by other classes init()
 valuesPtr->touchCmdStr = NULL; // for linking touchActions to touchZone NOTE: not cleared by other classes init()
 valuesPtr->align = 'L'; // always
}

pfodTouchActionInput &pfodTouchActionInput::cmd(const char _cmd) {
  valuesPtr->touchCmd = _cmd;
  valuesPtr->touchCmdStr = NULL;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::cmd(const char* _cmdStr) {
  valuesPtr->touchCmd = ' ';
  valuesPtr->touchCmdStr = _cmdStr;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::cmd(pfodAutoCmd &a_Cmd) {
  valuesPtr->touchCmd = ' ';
  valuesPtr->touchCmdStr = a_Cmd.cmd;
  return *this;
}


pfodTouchActionInput &pfodTouchActionInput::textIdx(uint16_t _idx) {
  valuesPtr->idx = _idx;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::textIdx(pfodAutoIdx &a_idx) {
	return textIdx(getAutoIdx(a_idx.idx));
}

pfodTouchActionInput &pfodTouchActionInput::color(int _color) {
  valuesPtr->color = _color;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::backgroundColor(int _color) {
  valuesPtr->backgroundColor = _color;
  return *this;	
}

pfodTouchActionInput &pfodTouchActionInput::fontSize(int _font) {
  valuesPtr->fontSize = _font;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::bold() {
  valuesPtr->bold = 1;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::italic() {
  valuesPtr->italic = 1;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::underline() {
  valuesPtr->underline = 1;
  return *this;
}

 // replace restricted chars in text and units, the default
pfodTouchActionInput &pfodTouchActionInput::encode() {
	valuesPtr->encodeOutput = 1;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::doNotEncode() {
	valuesPtr->encodeOutput = 0;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::prompt(const char *txt) {
  valuesPtr->text = txt;
  valuesPtr->textF = NULL;
  return *this;
}

pfodTouchActionInput &pfodTouchActionInput::prompt(const __FlashStringHelper *txtF) {
  valuesPtr->textF = txtF;
  valuesPtr->text = NULL;
  return *this;
}

void pfodTouchActionInput::send(char _startChar) {
  out->print(_startChar);
  out->print("XI");
  out->print('~');
  if ((valuesPtr->touchCmdStr) && (*valuesPtr->touchCmdStr)) {
    out->print(valuesPtr->touchCmdStr);
  } else {
    out->print(valuesPtr->touchCmd);
  }
  printTextFormatsWithBkgndColor(); // prints leading ~ for text
  if (valuesPtr->textF != NULL) {
  	encodeText(out,valuesPtr->encodeOutput,valuesPtr->textF);
  } else if (valuesPtr->text != NULL) {
   	encodeText(out,valuesPtr->encodeOutput,valuesPtr->text);
  }
  printIdx(); // this could go before prompt but then cmd would look like an indexed cmd!!
 // valuesPtr->lastDwg = NULL; // sent now
}


