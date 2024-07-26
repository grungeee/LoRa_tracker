#ifndef PFOD_AUTO_CMD_H
#define PFOD_AUTO_CMD_H

#include <Printable.h>

class pfodAutoCmd : public Printable {
  public: 
  	  pfodAutoCmd();
  	  const char cmd[5] = "";	// allow for cmds from c1 to c999  
      size_t printTo(Print& p) const;
  private:
  	  static size_t _cmdInt;
};
#endif