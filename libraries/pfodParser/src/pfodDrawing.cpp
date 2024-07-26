/*
   pfodDrawing.cpp
   (c)2022 Forward Computing and Control Pty. Ltd.
   NSW Australia, www.forward.com.au
   This code is not warranted to be fit for any purpose. You may only use it at your own risk.
   This code may be freely used for both private and commercial use
   Provide this copyright is maintained.
*/

#include <pfodDrawing.h>

pfodDrawing::pfodDrawing() {
  parserPtr = NULL;
  dwgsPtr = NULL;
}

pfodDrawing::pfodDrawing(pfodParser *_parserPtr, pfodDwgs* _dwgsPtr) {
   setParserDwgs(_parserPtr,_dwgsPtr);
}

// overrides the output stream
void pfodDrawing::setParser(pfodParser *_parserPtr) {
  parserPtr = _parserPtr;
  dwgsPtr->setOut(_parserPtr); 
}        

void pfodDrawing::setParserDwgs(pfodParser *_parserPtr , pfodDwgs* _dwgsPtr) {
  parserPtr = _parserPtr;
  dwgsPtr = _dwgsPtr;
  parserPtr->addDwg(this);
}

bool pfodDrawing::sendDwg() { return false; } // returns is dwg sent else false i.e. not this dwg's loadCmd
bool pfodDrawing::processDwgCmds() {return false;} // return true if handled else false
