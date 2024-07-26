/*
   (c)2014-2017 Forward Computing and Control Pty. Ltd.
   NSW Australia, www.forward.com.au
   This code is not warranted to be fit for any purpose. You may only use it at your own risk.
   This code may be freely used for both private and commercial use
   Provide this copyright is maintained.
*/
#include "pfodDwgsBase.h"
#include "pfodDwgs.h"

pfodDwgsBase::pfodDwgsBase() {
    out = NULL;
    valuesPtr = NULL;
}

void pfodDwgsBase::send(char _startChar) {
  (void)(_startChar);
}

void pfodDwgsBase::init(Print *_out, struct pfodDwgVALUES* _values) {
  initValues(_values);
  valuesPtr = _values;
  out = _out;
}

void pfodDwgsBase::setOut(Print *_printPtr) {
    out = _printPtr;
}

struct pfodDwgVALUES *pfodDwgsBase::getValuesPtr() {
	return valuesPtr;
}
void pfodDwgsBase::setValuesPtr(struct pfodDwgVALUES *_valuesPtr) {
  valuesPtr = _valuesPtr;
}

void pfodDwgsBase::initValues(struct pfodDwgVALUES* _valuesPtr) {
  struct pfodDwgVALUES* valuesPtr = _valuesPtr;
  // valuesPtr->action = NULL;
  valuesPtr->startAngle = 0;
  valuesPtr->arcAngle = 180;
  valuesPtr->text = NULL;  // also used for prompt for touchActionInput
  valuesPtr->textF = NULL; ;
  valuesPtr->units = NULL;
  valuesPtr->unitsF = NULL;
  valuesPtr->reading = 0.0f;
  valuesPtr->haveReading = 0;
  valuesPtr->value = 0;
  valuesPtr->haveValue = 0;
  valuesPtr->displayMax = 1;
  valuesPtr->displayMin = 0;
  valuesPtr->max = 1;
  valuesPtr->min = 0;
  valuesPtr->decPlaces = 2;
  valuesPtr->fontSize = 0;
  valuesPtr->bold = 0;
  valuesPtr->italic = 0;
  valuesPtr->underline = 0;
  valuesPtr->color = BLACK_WHITE; // black or white which ever has best contrast to dwg background.
  valuesPtr->backgroundColor = BLACK;
  valuesPtr->width = 1;
  valuesPtr->height = 1;
  valuesPtr->colOffset = 0;
  valuesPtr->rowOffset = 0;
  valuesPtr->idx = 0; // also used for idxOfTextItem for touchActionInput
  valuesPtr->radius = 1;
  valuesPtr->filled = 0;
  valuesPtr->rounded = 0;
  valuesPtr->encodeOutput = 1;
  valuesPtr->cmd = ' ';
  valuesPtr->cmdStr = NULL;
  valuesPtr->loadCmd = ' '; // for loading and erase of insertDwgs
  valuesPtr->loadCmdStr = NULL; // for loading and erase of insertDwgs
//  valuesPtr->touchCmd = ' '; // do NOT initialize these here as it breaks the link
//  valuesPtr->touchCmdStr = NULL; // do NOT initialize these here  as it breaks the link
  valuesPtr->filter = 0;
  valuesPtr->centered = 0;
  valuesPtr->align = 'C'; // nothing defaults to centered
}

void pfodDwgsBase::startText() {
  printIdx();
  printColor();
  out->print('~');
  printTextFormats();
}

int pfodDwgsBase::getAutoIdx(uint16_t &idx) {
	if (idx >0) {
		return idx;
	}
	// else
    idx = pfodDwgs::reserveIdx(1);
	return idx;
}

void pfodDwgsBase::printTextFormatsWithBkgndColor() {
  out->print('~');
  int _backgroundColor = valuesPtr->backgroundColor;
  if (_backgroundColor > 0) {
  	out->print("<bg ");
    out->print(_backgroundColor);
    out->print('>');
  }
  
  int _colorValue = valuesPtr->color;
  if (_colorValue >= 0) {
  	out->print('<');
    out->print(_colorValue);
    out->print('>');
  } else { //bw
  	out->print("<bw>");   ////// <<<<<< SHOULD FIX THIS IN APP!!
  }  	  
  printTextFormats();
  
}

void pfodDwgsBase::printTextFormats() {
  struct pfodDwgVALUES* valPtr = valuesPtr;
  if (valPtr->fontSize != 0) {
    out->print('<');
    if (valPtr->fontSize >= 0) {
      out->print('+');
    }
    out->print(valPtr->fontSize);
    out->print('>');
  }
  if (valPtr->italic != 0) {
    out->print('<'); out->print('i'); out->print('>');
  }
  if (valPtr->bold != 0) {
    out->print('<'); out->print('b'); out->print('>');
  }
  if (valPtr->underline != 0) {
    out->print('<'); out->print('u'); out->print('>');
  }
}

// replace resticted char if encode non-zero
// i.e. reverse these replacements
//  replace("&#123;","{", (char*)editedText);
//  replace("&#125;","}", (char*)editedText);
//  replace("&#124;","|", (char*)editedText);
//  replace("&#126;","~", (char*)editedText);
//  replace("&#96;","`", (char*)editedText);
//  else just remove (don't send) { } ` ~ |
// these char < &  \ are not automatically replaced when encoded text is enabled (the default)
// you need to do them manually if needed.
//  replace("&lt;","<", (char*)editedText);
//  replace("&#92;","\\", (char*)editedText);
//  replace("&amp;","&", (char*)editedText);

void pfodDwgsBase::encodeText(Print* out, uint8_t encodeOutput, const __FlashStringHelper *ifsh) {
  PGM_P p = reinterpret_cast<PGM_P>(ifsh);
  if ((!p) || (!out)) {
    return;
  }
  while (char c = pgm_read_byte(p++)) { // not '\0'
    encodeChar(out, encodeOutput, c);
  }
}

void pfodDwgsBase::encodeText(Print* out, uint8_t encodeOutput, const char *cPtr) {
  if ((!cPtr) || (!out)) {
    return;
  }
  while (char c = *cPtr++) { // not '\0'
    encodeChar(out, encodeOutput, c);
  }
}

void pfodDwgsBase::encodeChar(Print* out, uint8_t encodeOutput, char c) {
  if (!out) {
    return;
  }
  if (!encodeOutput) { // just skip outputting ` ~ { }
    if ((c == '`') || (c == '~') || (c == '{') || (c == '}') || (c == '|') ) {
      return; // skip this restricted char
    } // else
    out->print(c);
    return;
  }

  if (c == '{') {
    out->print("&#123;");
  } else if (c == '}') {
    out->print("&#125;");
  } else if (c == '|') {
    out->print("&#124;");
  } else if (c == '~') {
    out->print("&#126;");
  } else if (c == '`') {
    out->print("&#96;");
  } else {
    out->print(c);
  }
  return;
}

// prints ~ + number
// drops trailing zeros prints at most 3 decimal places
void pfodDwgsBase::printFloat(float f) {
  out->print('~');
  printFloatNumber(f);
}

// prints just number rounded to decPlaces, -ve decPlaces round to left of decimal point
void pfodDwgsBase::printFloatDecimals(Print* out, float f, int decPlaces) {
  if (!out) {
    return;
  }
  if (f < 0) {
    f = -f;
    out->print('-');
  }
  if (decPlaces <= 0) {
    unsigned long iValue = (unsigned long ) f;
    if ((f - iValue) != 0) {
      // round
      iValue = (unsigned long) (f + 0.5);
    }
    if (decPlaces == 0) {
      out->print(iValue);
    } else {
      // < 0
      // limit divider to be < number so always get something
      unsigned long divider = 1;
      for (int i = 0; i < (-decPlaces) && (divider < iValue); i++) {
        divider = divider * 10;
      }
      if (divider > iValue) {
      	  divider = divider/10;
      }
      int idValue = (iValue/ divider) * divider;
      if ((idValue - iValue) != 0) {
        // need to round
        iValue = iValue + (divider / 2);
        iValue = iValue / divider;
        idValue = iValue * divider;
      }
      out->print(idValue);
    }
  } else {
    // (decPlaces > 0) {
    out->print(f, decPlaces);
  }
}

// prints just number
// drops trailing zeros prints at most 1 decimal places if f >=1
// prints at most 2 decimals if f <1 and >0.1
// prints at most 3 decimals if f <=0.1

void pfodDwgsBase::printFloatNumber(float f) {
  if (f < 0) {
    f = -f;
    out->print('-');
  }
  if (f < 1) { // i.e 0.9xx
    // only print at most 3 decimals
    // round
    f = f + 0.0005;
    unsigned long intPart = (unsigned long)(f * 1000);
    // 1.234 => 1234
    int fraction = intPart % 1000; // 1234 -> 234
    // no decimals
    out->print((unsigned long)f);
    if (fraction != 0) {
      out->print('.');
      int tenths = (fraction / 100); // 234 -> 2
      out->print(tenths);
      fraction =  fraction - tenths * 100; // -> 34
      int hundreths = (fraction / 10); // 34 -> 3
      if (hundreths != 0) {
        int thousandths =  fraction - hundreths * 10; // -> 4
        out->print(hundreths);
        if (thousandths != 0) {
          out->print(thousandths);
        }
      }
    }
  } else if (f < 10.0) { // i.e. 9.xx
    f = f + 0.005; // round to 2 decimal
    unsigned long intPart = (unsigned long)(f * 100);
    // 1.234 => 123.4
    int fraction = intPart % 100; // 123.4 -> 23
    // no decimals
    out->print((unsigned long)f);
    if (fraction != 0) {
      out->print('.');
      int tenths = (fraction / 10); // 23.4 -> 2
      out->print(tenths);
      fraction =  fraction - tenths * 10; // -> 3
      int hundreths = (fraction); // 3 -> 3
      if (hundreths != 0) {
        out->print(hundreths);
      }
    }

  } else { // i.e. 10.x and higher
    f = f + 0.05; // round to 1dec place
    unsigned long intPart = (unsigned long)(f * 10.0);
    // 1.2 => 12
    int fraction = intPart % 10; // 12 -> 2
    // no decimals
    out->print((unsigned long)f);
    if (fraction != 0) {
      out->print('.');
      int tenths = (fraction); // 2 -> 2
      out->print(tenths);
    }
  }
}

// pfodApp defaults these to 0 if missing
void pfodDwgsBase::sendColRowOffset() {
  float colOffset = valuesPtr->colOffset;
  float rowOffset = valuesPtr->rowOffset;
  if (colOffset == 0) {
    out->print('~');
  } else {
    sendColRowVars(colOffset);
  }
  if (rowOffset == 0) {
    out->print('~');
  } else {
    sendColRowVars(rowOffset);
  }
}

void pfodDwgsBase::sendWidthHeight() {
  sendColRowVars(valuesPtr->width);
  sendColRowVars(valuesPtr->height);
}

void pfodDwgsBase::sendColRowVars(float val) {
  out->print('~');
  long longVar = (long)val;
  if (longVar == TOUCHED_COL) {
    out->print('c');
  } else if (longVar == TOUCHED_ROW) {
    out->print('r');
  } else {
    printFloatNumber(val);
  }
}

void pfodDwgsBase::printColor() {
  int _colorValue = valuesPtr->color;
  out->print('~');
  if (_colorValue >= 0) {
    out->print(_colorValue);
  }
}

// only outputs idx if > 0
void pfodDwgsBase::printIdx() {
  unsigned int idx = valuesPtr->idx;
  if (idx > 0) {
    out->print('`');
    out->print(idx);
  }
}

