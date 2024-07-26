
#include <Arduino.h> // for String
#include "pfodAutoCmd.h"

size_t pfodAutoCmd::_cmdInt = 1;

pfodAutoCmd::pfodAutoCmd() {
  String cmdString('c'); // cannot use _ as this interferes with security handshake
  cmdString += _cmdInt++;
  strncpy((char*)cmd,cmdString.c_str(),sizeof(cmd));
  ((char*)cmd)[(sizeof(cmd)-1)] = '\0'; // always terminate
}

size_t pfodAutoCmd::printTo(Print& p) const {
  return p.print(cmd);
}
