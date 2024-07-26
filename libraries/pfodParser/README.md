# pfodParser
The pfodParser library is handles commands sent from the Android pfodApp, pfodApp supports WiFi, BLE, Bluetooth and SMS connections, the library also includes support for sending dwg commands back to pfodApp to create interactive GUI's.

This library includes:-  
* **pfodParser**, parses pfodApp commands  
* **pfodSecurity**, alternative to pfodParser that adds 128bit sercuity   
* **pfodDwgs**, classes for sending dwg commands to create interactive GUI's  

# How-To
See [pfodParser Documentation](https://www.forward.com.au/pfod/pfodParserLibraries/index.html)  
See [pfodParser Examples](https://www.forward.com.au/pfod/index.html)  
See [pfodDwgs Examples](https://www.forward.com.au/pfod/pfodControls/index.html)  

# Software License
(c)2014-2022 Forward Computing and Control Pty. Ltd.  
NSW Australia, www.forward.com.au  
This code is not warranted to be fit for any purpose. You may only use it at your own risk.  
This code may be freely used for both private and commercial use  
Provide this copyright is maintained.  

# Revisions
Version 3.63.1 added multiple connections support  
Version 3.62 added default initializations  
Version 3.61 added linked list for processing dwg cmds, added pfodESPBufferedClient, fixed label/touchActionInput encoding  
Version 3.60 revised arguments to supports pfodSecurity  
Version 3.59 added minor edit  
Version 3.58 added pfodDrawing for pfodGUIdesigner
Version 3.57 added pfodAutoCmd, mods for pfodGUIdesigner
Version 3.56 added cmdEquals  
Version 3.55 added pfodAutoIdx  
Version 3.54 added xRadius, yRadius as an alterative to arc().start(..).angle(0)  
Version 3.53 minor edit to remove code warning  
Version 3.52 added support for multi-char dwg cmds, menu cmds already supported multi-char  
Version 3.51 fixed ESP32, ESP8266 request for version when none set  
Version 3.50 added hide/unhide/erase insertDwg  
Version 3.49 more fixes for mbed Stream  
Version 3.48 fixed more warnings  
Version 3.47 fixed warnings  
Version 3.46 for Arduino Nano 33 IoT  
Version 3.45 for Arduino Nano 33 BLE  
Version 3.44 pfodNonBlockingInput for empty lines  
Version 3.42-43 enhanced pfodNonBlockingInput  
Version 3.41 Gauge spelling corrections  
Version 3.40 added copyTo to pfodRingBuffer and parser.flush() now calls underlying stream flush()  
Version 3.39 fixed millisDelay.h include in pfodNonBlockingInput.h  
Version 3.38 fixed pfodNonBlockingInput.h  
Version 3.37 added alignment options .left() .right() to dwg label()  Needs pfodApp V3.0.371+   
Version 3.36 added NonBlockingInput and Bluetooth Classic Buffered Stream  
Version 3.35 renamed pfodDelay.isFinished() to pfodDelay.justFinished()  
Version 3.34 added setBLEBlockSendDelay and clear ble send buffer on connection  
Version 3.33 fixed Redbear NanoV2 Stream compile errors  
Version 3.32 made pfodBLEBufferedSerial blocking   
Version 3.31 added getStartTime() to pfodDelay  
Version 3.30 added pfodDelay  
Version 3.29 added blocking option to pfodBufferedStream  
Version 3.28 added Teensy to pfodEEPROM.h  
Version 3.27 added markWrite, resetWrite to pfodRingBuffer  
Version 3.26 fix availableForWrite and for different EEPROM classes begin(),commit(), added pfodRadio and pfodSecurityClient  
Version 3.25 added support for pfodSecurity without EEPROM and fixes some bugs in pfodSecurity powerCycling. Also includes fixes for non-8bit processors  
Version 3.24 added SIM_5320 examples  
Version 3.23 added ESP32 examples  
Version 3.22 fixed runtime error RedBear BLE boards  
Version 3.21 fixed compile error (NanoV2) and rolled back ESP32 support until complete  
Version 3.20 added support for ESP32  
Version 3.19 fixed float to label conversion in drawings  
Version 3.18 fixed pfodBLEBufferedSerial compile errors under Nano  
Version 3.17 added Feather52 example  
Version 3.16 ignore non-printable seqNo e.g. CR LF TAB SPACE etc  
Version 3.15 added BLE cmd sequence numbers support  
Version 3.14 added Adafruit nRF52 Feather support  
Version 3.13 added pfodBLEBufferedSerial and example  
Version 3.11 added BLE Nano V2 example screens example  
Version 3.10 added support for RedBear BLE NanoV2  
Version 3.9 added Redbear BLE Nano example file  
Version 3.8 revised examples to add dwgs and removed the un-reliable pfodESP8266_AT support files  
Version 3.7 added support for mbed builds, i.e. Redbear Nano etc  
Version 3.6 pfodEEPROM include fix  
Version 3.5 correction to keywords.txt  
Version 3.4 added insertDwg and fixed unsigned int sizes  
Version 3.3 added hide/unhide and index  
Version 3.2 renamed DISABLED to TOUCH_DISABLED to avoid conflict with ESP8266 define  
Version 3.1 added Drawing Primitive support  
For earlier revisions see the [pfodParser LibraryVersion.txt file](https://www.forward.com.au/pfod/pfodParserLibraries/LibraryVersion.txt)  
