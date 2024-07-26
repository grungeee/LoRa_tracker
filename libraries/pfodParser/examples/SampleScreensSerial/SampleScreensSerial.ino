/*
  Sample pfod Screens for Arduino Uno using Serial to connect to UART BT/BLE/Wifi shield at 9600
*/
/*
 * (c)2014-2017 Forward Computing and Control Pty. Ltd.
 * NSW Australia, www.forward.com.au
 * This code is not warranted to be fit for any purpose. You may only use it at your own risk.
 * This code may be freely used for both private and commercial use
 * Provide this copyright is maintained.
 */
// Using Serial and 9600 baud for send and receive
// Serial D0 (RX) and D1 (TX) on Arduino Uno, Micro, ProMicro, Due, Mega, Mini, Nano, Pro and Ethernet
// This code uses Serial so remove shield when programming the board

// download the libraries from http://www.forward.com.au/pfod/pfodParserLibraries/index.html
// pfodParser V3.8+, pfodDwgControls V1.2+
#include <pfodParser.h>
#include <pfodDwgControls.h>

pfodParser parser("SS_V38"); // create a parser to handle the pfod messages
pfodDwgs dwgs(&parser);  // drawing support

const int LED_pin = LED_BUILTIN;

Gauge gauge(&dwgs);
Slider slider('a', &dwgs);

const int maxTextChars = 11;
byte currentText[maxTextChars + 1]  = "Hello"; // allow max 11 chars + null == 12, note  msg {'x`11~Example Text Input screen.\n"  enforces max 11 bytes in return value

const int imagesize = 50;  // this set the image size
bool sendCleanImage = false; // this is set when next image load/update should send blank image

int currentSingleSelection = 1;

const int MAX_MULTI_SELECTIONS = 5; //max 5 possible selections out of all possible,
long multiSelections[MAX_MULTI_SELECTIONS]; // -1 means not selected

int redValue = 127;
int greenValue = 127;
int blueValue = 127;
int fanPosition = 0;

const unsigned long RAW_DATA_INTERVAL = 2000; // 2 sec
unsigned long rawDataTimer;
int counter = 0;

// the setup routine runs once when you press reset:
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  // allow a little time to connect the serialMonitor before running the rest of the setup.
  for (int i = 5; i > 0; i--) {
    delay(1000);
    // Serial.print(F(" "));
    // Serial.print(i);
  }
  //Serial.println();
  parser.connect(&Serial); // connect the parser to the i/o stream

  // setup multiselections
  // just clear for now
  for (int i = 0; i < MAX_MULTI_SELECTIONS; i++) {
    multiSelections[i] = -1; // empty
  }
  rawDataTimer = millis();

  gauge.setLabel(F("LED\n"));
  slider.setLabel(F("PWM "));
  slider.setValue(0); // 0 to 255
  gauge.setValue(slider.getValue());

  pinMode(LED_pin, OUTPUT);

}

// the loop routine runs over and over again forever:
void loop() {
  byte cmd = parser.parse(); // parse incoming data from connection
  // parser returns non-zero when a pfod command is fully parsed
  if (cmd != 0) {  // have parsed a complete msg { to }
    byte* pfodFirstArg = parser.getFirstArg(); // may point to \0 if no arguments in this msg.
    long pfodLongRtn; // used for parsing long return arguments, if any
    if ('.' == cmd) {
      // pfodApp has connected and sent {.} , it is asking for the main menu
      // send back the menu designed
      if (!parser.isRefresh()) {
        sendMainMenu();
      } else {
        sendMainMenuUpdate();
      }
    } else if ('p' == cmd) {
      sendPlottingScreen(); // plots the raw data being logged
    } else if ('l' == cmd) {
      if (!parser.isRefresh()) {
        sendListMenu();
      } else {
        sendListMenuUpdate();
      }
    } else if ('s' == cmd) {
      if (!parser.isRefresh()) {
        sendSliderMenu();
      } else {
        sendSliderMenuUpdate();
      }
    } else  if ('x' == cmd) {
      // return from text input,  pickup the first arg which is the number
      strncpy((char*)currentText, (const char*)pfodFirstArg, sizeof(currentText) - 1); // keep null at end
      parser.print(F("{}")); // nothing to update pfodApp will request previous menu
    } else  if ('y' == cmd) {
      sendSingleSelectionScreen();
    } else  if ('S' == cmd) {
      // return from single selection input, pickup the first arg which is the number
      parser.parseLong(pfodFirstArg, &pfodLongRtn); // stores result in a long
      currentSingleSelection = (int)pfodLongRtn;
      parser.print(F("{}"));
    } else  if ('F' == cmd) {
      if (!parser.isRefresh()) {
        sendFanControl();
      } else {
        sendFanControlUpdate();
      }
    } else if ('G' == cmd) {
      if (!parser.isRefresh()) {
        dwgMenu();
      } else {
        dwgMenuUpdate();
      }
    } else if ('A' == cmd) { // user pressed menu item that loaded drawing with load cmd 'z'
      byte dwgCmd = parser.parseDwgCmd();  // parse rest of dwgCmd, return first char of active cmd
      if (slider.getCmd() == dwgCmd) { // user touched LED control
        int col = parser.getTouchedCol();
        slider.setValue(col); // set new value
        gauge.setValue(slider.getValue()); // set gauge from slider
        analogWrite(LED_pin, slider.getValue()); // set LED PWM
      }
      sendDrawingUpdates_z(); // update the drawing
      // always send back a response or pfodApp will timeout

    } else if ('z' == cmd) { // pfodApp is asking to load dwg 'z'
      if (!parser.isRefresh()) { // not refresh send whole dwg
        sendDrawing_z();
      } else { // refresh just update drawing state
        sendDrawingUpdates_z();
      }

    } else  if ('o' == cmd) {
      // return from fan control, pickup the first arg which is the number
      parser.parseLong(pfodFirstArg, &pfodLongRtn);
      fanPosition = (int)pfodLongRtn;
      sendFanControlUpdate();
    } else  if ('r' == cmd) {
      // return from red colour selector, pickup the first arg which is the number
      parser.parseLong(pfodFirstArg, &pfodLongRtn);
      redValue = (int)pfodLongRtn;
      sendColourSelectorUpdate();
    } else  if ('g' == cmd) {
      // return from green colour selector, pickup the first arg which is the number
      parser.parseLong(pfodFirstArg, &pfodLongRtn);
      greenValue = (int)pfodLongRtn;
      sendColourSelectorUpdate();
    } else  if ('b' == cmd) {
      // return from blue colour selector, pickup the first arg which is the number
      parser.parseLong(pfodFirstArg, &pfodLongRtn);
      blueValue = (int)pfodLongRtn;
      sendColourSelectorUpdate();
    } else  if ('m' == cmd) {
      sendMultiSelectionScreen();
    } else  if ('M' == cmd) {
      // return from multi selection input
      byte* argIdx = pfodFirstArg; // pickup the first arg which is the number
      // will be null if no arges
      int i = 0;
      for (; (i < MAX_MULTI_SELECTIONS) && (*argIdx != 0); i++) {
        argIdx = parser.parseLong(argIdx, &multiSelections[i]);
        //argIdx will be null after last arg is parsed
      }
      if (i < MAX_MULTI_SELECTIONS) {
        // add a -1 to terminate the array of indices
        multiSelections[i] = -1;
      }
      parser.print(F("{}"));
    } else  if ('L' == cmd) {
      if (!parser.isRefresh()) {
        sendColourSelector();
      } else {
        sendColourSelectorUpdate();
      }
    } else  if ('i' == cmd) {
      sendTextInputScreen();
    } else  if ('u' == cmd) {
      sendRawDataScreen();
    } else if ('!' == cmd) {
      // CloseConnection command
      closeConnection(parser.getPfodAppStream());
    } else {
      // unknown command
      parser.print(F("{}")); // always send back a pfod msg otherwise pfodApp will disconnect.
    }
  }

  // this section will send raw data at set intervals, in CSV format
  if (millis() - rawDataTimer > RAW_DATA_INTERVAL) { // send raw data
    // note DO NOT call Serial print directly as it will interfer with the msg hash on secure connections
    parser.print(rawDataTimer / 1000); // the time in seconds, first field
    parser.print(','); // field separator
    parser.print(counter++); // second field
    parser.print(','); // field separator
    parser.print(analogRead(A0)); // third field
    parser.println(); // terminate data record
    rawDataTimer = millis(); // allow it to drift due to delays in sending data
  }

  // else keep looping
}

void closeConnection(Stream *io) {
  // add any special code here to force connection to be dropped
}

void sendMainMenu() {
  // put main msg in input array
  parser.print(F("{,~Serial Sample Screens"));
  parser.sendVersion(); // send pfod menu version
  parser.print(F("|^l~Lists|^s~Sliders and Dwgs|^i~Text Input|^u~Raw Data|^p~Plots}"));
}
void sendMainMenuUpdate() {
  parser.print(F("{;}")); // nothing to update here
}

void sendSliderMenu() {
  parser.print(F("{,~Slider and Dwg Examples"));
  parser.sendVersion(); // send pfod menu version
  parser.print(F("|L<bg w>~<+1><r>Colour</r> <g>Selector</g> <bl>Example</bl>"
                 "|F~<+1>Fan Control"
                 "|G~<+1>Drawings\nSlider/Gauge Example"
                 "}"));
}

void sendSliderMenuUpdate() {
  parser.print(F("{;}")); // nothing to update here
}

void sendListMenu() {
  parser.print(F("{,~Lists"));
  parser.sendVersion(); // send pfod menu version
  parser.print(F("|y~Single Selection|m~Multi-selection}"));
}
void sendListMenuUpdate() {
  parser.print(F("{;}")); // nothing to update here
}

void sendTextInputScreen() {
  parser.print(F("{'x`"));
  parser.print(maxTextChars);
  parser.print(F("~Text Input\n(Limited to 11 chars by Arduino code to match allocated storage)"
                 "|"));
  parser.print((char*)currentText);
  parser.print(F("}"));

}

void sendSingleSelectionScreen() {
  parser.print(F("{?S`")); // rest of msg handle in main loop
  parser.print(currentSingleSelection);
  parser.print(F("~Single Selection"
                 "|Enable|Disable}"));
}

void sendMultiSelectionScreen() {
  parser.print(F("{*M"));
  // add the current selections
  for (int i = 0; i < MAX_MULTI_SELECTIONS; i++) {
    long idx = multiSelections[i];
    if (idx < 0) {
      break;
    } // else
    parser.print(F("`"));
    parser.print(idx);
  }
  parser.print(F("~Multi-selection"
                 "|Fade on/off|3 Levels|Feature A|Feature B|Feature C}"));
}

void sendColourSelector() {
  parser.print(F("{,"));
  sendColourScreen();
  parser.print(F("}"));
}
void sendColourSelectorUpdate() {
  parser.print(F("{;"));
  parser.print(F("<bg "));  // sent the background colour in the prompt area to match the slider settings
  if (redValue < 16) {
    parser.print('0');
  }
  parser.print(redValue, HEX);
  if (greenValue < 16) {
    parser.print('0');
  }
  parser.print(greenValue, HEX);
  if (blueValue < 16) {
    parser.print('0');
  }
  parser.print(blueValue, HEX);
  parser.print(F(">"));
  parser.print(F("|r"));
  parser.print('`');
  parser.print(redValue);
  parser.print(F("|g"));
  parser.print('`');
  parser.print(greenValue);
  parser.print(F("|b"));
  parser.print('`');
  parser.print(blueValue);

  parser.print(F("}"));
}

void sendColourScreen() {
  parser.print(F("<bg "));  // sent the background colour in the prompt area to match the slider settings
  if (redValue < 16) {
    parser.print('0');
  }
  parser.print(redValue, HEX);
  if (greenValue < 16) {
    parser.print('0');
  }
  parser.print(greenValue, HEX);
  if (blueValue < 16) {
    parser.print('0');
  }
  parser.print(blueValue, HEX);
  parser.print(F(">~"));
  parser.sendVersion(); // send pfod menu version
  parser.print(F("|r<bg r>"));
  parser.print('`');
  parser.print(redValue);
  parser.print(F("~<w>Red <b>"));
  parser.print(F("`255`0~</b>")); //
  parser.print(F("|g<bg g>"));
  parser.print('`');
  parser.print(greenValue);
  parser.print(F("~<bk>Green <b>"));
  parser.print(F("`255`0~</b>")); //
  parser.print(F("|b<bg bl>"));
  parser.print('`');
  parser.print(blueValue);
  parser.print(F("~<w>Blue <b>"));
  parser.print(F("`255`0~</b>")); //
}

void sendFanControl() {
  parser.print(F("{,"));
  sendFanControlScreen();
  parser.print(F("}"));
}
void sendFanControlUpdate() {
  parser.print(F("{;"));
  parser.print('`');
  parser.print(fanPosition);
  parser.print(F("}"));
}

void sendFanControlScreen() {
  parser.print(F("~<+5>Fan Control"));
  parser.sendVersion(); // send pfod menu version
  parser.print(F("|o"));
  parser.print('`');
  parser.print(fanPosition);
  parser.print(F("~<+4><b>Fan is "));
  parser.print(F("~~Off\\Low\\High"));
}

void dwgMenu() {
  parser.print(F("{,"));  // start a Menu screen pfod message
  // send menu background, format, prompt, refresh and version
  parser.print(F("~`0"));
  parser.sendVersion(); // send the menu version
  // send menu items
  parser.print(F("|+A~z"));
  parser.print(F("}"));  // close pfod message
}

void dwgMenuUpdate() {
  parser.print(F("{;"));  // start an Update Menu pfod message
  // send menu items
  parser.print(F("|A"));
  parser.print(F("}"));  // close pfod message
}

void sendDrawing_z() {
  dwgs.start(50, 35, dwgs.WHITE); // background defaults to WHITE if omitted i.e. dwgs.start(50,35);
  parser.sendVersion(); // send the parser version to cache this image
  dwgs.pushZero(25, 12, 1.5); // move zero to centre of dwg to 25,12 and scale by 1.5 times
  gauge.draw(); // draw the control
  dwgs.popZero();
  dwgs.pushZero(9, 29); // move zero to centre of dwg to 9,30 and scale is 1 (default)
  slider.draw(); // draw the control
  dwgs.popZero();
  dwgs.end();
}

void sendDrawingUpdates_z() {
  dwgs.startUpdate();
  gauge.update(); // update with current state
  slider.update(); // update with current state
  dwgs.end();
}


void sendRawDataScreen() {
  // this illustrates how you can send much more then 255 bytes as raw data.
  // The {=Raw Data Screen} just tells the pfodApp to open the rawData screen and give it a title
  // all the rest of the text (outside the { } ) is just raw data text and can be a much as you like
  // Note the raw data includes { } using { is OK as long as the next char is not a pfodApp msg identifer
  // that is the following cannot appear in rawData {@ {. {: {^ {= {' {# {?  or {*
  parser.print(F("{=Raw Data Screen}"
                 "This is the Raw Data Screen\n\n"
                 "The pfodDevice can write more the 255 chars to this screen.\n"
                 "Any bytes sent outside the pfod message { } start/end characters are"
                 " displayed here.\n"
                 " The raw data used for plotting and data logging is also displayed here.\n"
                 "\nUse the Back Button on the mobile to go back to the previous menu."
                 "\n \n"));
}

void sendPlottingScreen() {
  parser.print(F("{=" // streaming raw data screen
                 "counter and ADC A0 versus time" // plot title
                 "|Time (sec)|counter|A0}")); // show data in a plot
  // these match the 3 fields in the raw data record
}


