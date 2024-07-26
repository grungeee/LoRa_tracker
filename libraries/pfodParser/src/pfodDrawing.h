#ifndef PFOD_DRAWING_H
#define  PFOD_DRAWING_H
/*
   pfodDrawing.h
   (c)2022 Forward Computing and Control Pty. Ltd.
   NSW Australia, www.forward.com.au
   This code is not warranted to be fit for any purpose. You may only use it at your own risk.
   This code may be freely used for both private and commercial use
   Provide this copyright is maintained.
*/
#include "pfodParser.h"
#include "pfodDwgs.h"

class pfodParser;
class pfodDwgs;

class pfodDrawing : public pfodAutoCmd {
  public:
    pfodDrawing(pfodParser *parserPtr, pfodDwgs* dwgsPtr);
    pfodDrawing();
    void setParserDwgs(pfodParser *_parserPtr , pfodDwgs* _dwgsPtr);
    void setParser(pfodParser* _parserPtr); // overrides the output stream
    virtual bool sendDwg(); // returns is dwg sent else false i.e. not this dwg's loadCmd
    virtual bool processDwgCmds(); // return true if handled else false
 protected:
 	pfodParser *parserPtr;
    pfodDwgs *dwgsPtr;
 };
 
#endif