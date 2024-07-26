#ifndef pfodDwgs_h
#define pfodDwgs_h
/*
 * (c)2014-2017 Forward Computing and Control Pty. Ltd.
 * NSW Australia, www.forward.com.au
 * This code is not warranted to be fit for any purpose. You may only use it at your own risk.
 * This code may be freely used for both private and commercial use
 * Provide this copyright is maintained.
 */
#include <stdint.h>
#include <Arduino.h>
#include "dwgs/pfodAutoCmd.h"
#include "dwgs/pfodAutoIdx.h"
#include "dwgs/pfodDwgsBase.h"
#include "dwgs/pfodCircle.h"
#include "dwgs/pfodRectangle.h"
#include "dwgs/pfodLabel.h"
#include "dwgs/pfodTouchZone.h"
#include "dwgs/pfodTouchAction.h"
#include "dwgs/pfodTouchActionInput.h"
#include "dwgs/pfodLine.h"
#include "dwgs/pfodArc.h"
#include "dwgs/pfodErase.h"
#include "dwgs/pfodHide.h"
#include "dwgs/pfodUnhide.h"
#include "dwgs/pfodIndex.h"
#include "dwgs/pfodInsertDwg.h"


class pfodDwgs : public pfodDwgsBase {
  public:
    pfodDwgs();
    // returns the first index of the reserved ones
    static int reserveIdx(int numToReserve);

    pfodDwgs(Print *out);
    void start(int cols, int rows, int backgroundColor = WHITE, uint8_t moreData = 0);
    void startUpdate(uint8_t moreData = 0);
    void end();
    void pushZero(double col, double row = 0.0, double scale = 1.0);
    void popZero(); // restore previous zero
    pfodRectangle& rectangle();
    pfodCircle& circle();
    pfodLabel& label();
    pfodTouchZone& touchZone();
    pfodTouchAction& touchAction();
    pfodTouchActionInput& touchActionInput();
    pfodInsertDwg& insertDwg();
    pfodLine& line();
    pfodArc& arc();
    pfodErase& erase();
    pfodHide& hide();
    pfodUnhide& unhide();
    pfodIndex& index();
    
    // utilies for finding the x,y location at the end of a radius of a given angle
    // the x position at the end of this radius at this angle
    float xRadius(float angle, float radius);
    // the y position at the end of this radius at this angle
    float yRadius(float angle, float radius);


  protected:
    struct pfodDwgVALUES values;
	
  private:
    pfodCircle c;
    pfodRectangle r;
    pfodLabel t;
    pfodTouchZone x;
    pfodTouchAction X;
    pfodTouchActionInput XI;
    pfodLine l;
    pfodArc a;
    pfodErase e;
    pfodHide h;
    pfodUnhide u;
    pfodIndex i;
    pfodInsertDwg d;
    static int _idx;

};
#endif // pfodDwgs_h
