#ifndef pfodParser_h
#define pfodParser_h
/**
  pfodParser for Arduino
  Parses commands of the form { cmd | arg1 ` arg2 ... }
  Arguments are separated by `
  The | and the args are optional
  This is a complete paser for ALL commands a pfodApp will send to a pfodDevice
  see www.pfod.com.au  for more details.

  pfodParser adds about 482 bytes to the program and uses about 260 bytes RAM

  The pfodParser parses messages of the form
  { cmd | arg1 ` arg2 ` ... }
  The message is parsed into the args array by replacing '|', '`' and '}' with '/0' (null)
  When the the end of message } is seen
  parse() returns the first byte of the cmd
  getCmd() returns a pointer to the null terminated cmd
  skipCmd() returns a pointer to the first arg (null terminated)
      or a pointer to null if there are no args
  getArgsCount() returns the number of args found.
  These calls are valid until the start of the next msg { is parsed.
  At which time they are reset to empty command and no args.
*/
/*
   (c)2014-2017 Forward Computing and Control Pty. Ltd.
   NSW Australia, www.forward.com.au
   This code is not warranted to be fit for any purpose. You may only use it at your own risk.
   This code may be freely used for both private and commercial use
   Provide this copyright is maintained.
*/

#include <Arduino.h>
#include "pfodStream.h"
#include "pfodParserUtils.h"
#include "pfodDelay.h"
#include "pfodDwgs.h"
#include "pfodControl.h"
#include "pfodDrawing.h" 
#include "pfodLinkedList.h"
//#include "pfodSecurity.h"

class pfodDrawing;

// used to suppress warning
#define pfod_MAYBE_UNUSED(x) (void)(x)

class pfodParser: public Print {
  public:
    pfodParser();
    pfodParser(const char* version);
    // methods required for Print
	virtual size_t write(uint8_t c);
    virtual size_t write(const uint8_t *buffer, size_t size);
    virtual void flush();  // may do nothing calls io->flush()

    // stream methods only useful to read the raw cmd
    virtual int read();
    virtual int peek();
    virtual int available();
    // you can reduce this value if you are sending shorter commands.  Most pfod commands are very short <20 bytes, but depends on the pfod menu items you serve
    // but never increase it.
    static const byte pfodMaxMsgLen = 0xff; // == 255, if no closing } by now ignore msg

    virtual void connect(Stream* ioPtr);
    virtual void closeConnection();
    virtual byte parse(); // call this in loop() every loop, it will read bytes, if any, from the pfodAppStream and parse them
    // returns 0 if message not complete, else returns the first char of a completed and verified message
    virtual bool isRefresh(); // starts with {version: and the version matches this parser's version
    virtual const char *getVersionRequested(); // the version asked for in the command i.e. {versionRequested:...}
    virtual const char* getVersion();
    virtual void setVersion(const char* version); // no usually needed
    virtual void sendVersion(); // send ~ version to parser.print
    virtual void sendRefreshAndVersion(unsigned long refresh_mS); // send `refresh_mS ~ version to parser.print
    virtual byte* getCmd();
    virtual byte* getFirstArg();
    virtual byte* getNextArg(byte *start);
    virtual byte getArgsCount();
    virtual byte* parseLong(byte* idxPtr, long *result);
    virtual bool cmdEquals(const char* cmdStr); // returns true if parser cmd, as returned by getCmd() == cmdStr 
    virtual bool cmdEquals(const char cmdChar); // returns true if parser cmd as returned by getCmd() is just once char and == cmdChar
    virtual bool cmdEquals(pfodAutoCmd &a_Cmd); // for load dwg cmds
    virtual void addDwg(pfodDrawing *dwgPtr); // add a pfodDrawing to the list of drawings to be automatically processed by parse()


    /**
       pfodWaitingForStart if outside msg
       pfodMsgStarted if just seen opening {
       pfodInMsg in msg after {
       pfodMsgEnd if just seen closing }
    */
    virtual byte getParserState();
    virtual void setCmd(byte cmd);
    static const byte pfodWaitingForStart = 0xff;
    static const byte pfodMsgStarted = '{';
    static const byte pfodRefresh = ':';
    static const byte pfodInMsg = 0;
    static const byte pfodMsgEnd = '}';
    virtual void setDebugStream(Print* debugOut); // does nothing
    virtual void setDebugOut(Print* out) { 	setDebugStream(out);}
    virtual void setDebug(Print* out) {	setDebugStream(out);}

    virtual void setIdleTimeout(unsigned long timeout); // does nothing in parser
    virtual Stream* getPfodAppStream(); // get the command response stream we are writing to
    // for pfodParser this is also the rawData stream

    // this is returned if pfodDevice should drop the connection
    // only returned by pfodParser in read() returns -1
    void init();  // for now do NOT make this virtual!!
    virtual byte parse(byte in); // for now
    virtual void ignoreSeqNum(); // for pfodSecurity so hash does not accidently drop a command.
    virtual byte parseDwgCmd();  // returns the first byte of the dwgCmd str, often only one char long
    virtual const byte* getDwgCmd(); // valid only after parseDwgCmd() called on image cmd
    // returns true if dwgCmd string == cmdStr, uses strcmp( ) internally
    virtual bool dwgCmdEquals(const char* dwgCmdStr); // valid only after parseDwgCmd() called on image cmd
    virtual bool dwgCmdEquals(pfodAutoCmd &a_Cmd); // valid only after parseDwgCmd() called on image cmd
    virtual bool dwgCmdEquals(const char dwgCmd); // valid only after parseDwgCmd() called on image cmd
    virtual bool isTouch(); // default TOUCH even if not parsed
    virtual bool isClick();
    virtual bool isDown();
    virtual bool isDrag();
    virtual bool isUp();
    virtual bool isPress();
    //    bool isEntry();
    //    bool isExit();
    virtual const byte* getEditedText(); // [0] = '\0' if no editedText returned


    virtual uint8_t getTouchType();
    virtual int getTouchedCol(); // default 0
    virtual int getTouchedRow(); // default 0
    virtual int getTouchedY(); // default 0
    virtual int getTouchedX(); // default 0
    const static int TOUCH = 0;
    const static int DOWN = 1;
    const static int DRAG = 2;
    const static int UP = 4;
    const static int CLICK = 8;
    const static int PRESS = 16;
    //    const static int ENTRY = 32;
    //    const static int EXIT = 64;
    const static int DOWN_UP = 256; // only for touchZone filter send, never recieved by parser
    const static int TOUCH_DISABLED = 512; // only for touchZone filter send, never recieved by parser
    static pfodLinkedList<pfodDrawing> listOfDrawings;

  private:
     void constructInit();
    // findStr length MUST be >= replacePtr length!!
    // input is const byte* BUT recast as (char*) and is modified by this method
    // const byte* is to match getEditedText() return
    void replace(const char* findStr, const char *replacePtr, char* buffer);
    //static const byte DisconnectNow = '!';
    Stream* io;
    char emptyVersion[1];
    byte emptyBytes[1];
    byte missingEditedText[1];
    byte argsCount;  // no of arguments found in msg
    byte argsIdx;
    byte parserState;
    byte args[pfodMaxMsgLen + 1]; // allow for trailing null
    byte *versionStart;
    const byte *activeCmdStart;
    byte *editedText;
    byte encodingProcessed;
    uint8_t seqNum; // 0 if not set else last char before leading {
    uint8_t lastSeqNum; // 0 if not set else last char before leading {
    byte ignoreCmdSeqNum; // != 0 if should ignore them
    uint8_t touchType;
    int col;
    int row;
    byte *cmdStart;
    bool refresh;
    const char *version;
    static const byte pfodBar = (byte)'|';
    static const byte pfodTilda = (byte)'~';
    static const byte pfodAccent = (byte)'`';
    static const byte pfodArgStarted = 0xfe;
};

#endif // pfodParser_h

