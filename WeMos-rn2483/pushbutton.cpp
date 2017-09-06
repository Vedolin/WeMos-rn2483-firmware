// **********************************************************************************
// pushbutton source file for WeMos Lora RN2483 Shield
// **********************************************************************************
// Creative Commons Attrib Share-Alike License
// You are free to use/extend this library but please abide with the CC-BY-SA license:
// http://creativecommons.org/licenses/by-sa/4.0/
//
// Written by Charles-Henri Hallard http://ch2i.eu
//
// History : V1.20 2016-06-11 - Creation
//
// All text above must be included in any redistribution.
//
// **********************************************************************************
#include "pushbutton.h"

// These variables manages button feature
btn_state_e   _btn_State;    // Button management state machine
btn_action_e  _btn_Action;   // button action after press
bool          _btn_LongPress;// indicate a long press on button
unsigned long _btn_StartTime;// when push started


/* ======================================================================
Function: buttonManageState
Purpose : manage button states (longpress actions/simple press/debounce)
Input   : pin button state (LOW or HIGH) we read just before
Output  : current state machine
Comments: need to be called after push until state machine = BTN_WAIT_PUSH
          code inspired from one button library
====================================================================== */
btn_state_e buttonManageState(uint8_t buttonLevel)
{
  // get current time in msecs
  unsigned long now = millis(); 

  // Implementation of the state machine
  // waiting for menu pin being pressed.
  if (_btn_State == BTN_WAIT_PUSH) { 
    // Putton pushed ?
    if (buttonLevel == BTN_PRESSED) {
      // now wait for release
      _btn_State = BTN_WAIT_RELEASE; 
      // Set starting time
      _btn_StartTime = now; 
    } 

  // we're waiting for button released.
  } else if (_btn_State == BTN_WAIT_RELEASE) { 
    // effectivly released ?
    if (buttonLevel == BTN_RELEASED) {
      // next step is to check debounce 
      _btn_State = BTN_WAIT_DEBOUNCE; 
    // still pressed, is it a Long press that is now starting ?
    } else if ((buttonLevel == BTN_PRESSED) && (now > _btn_StartTime + BTN_LONG_PUSH_DELAY)) {
      // Set long press state
      _btn_LongPress = true;  

      // step to waiting long press release
      _btn_State = BTN_WAIT_LONG_RELEASE; 
    }

  // waiting for being pressed timeout.
  } else if (_btn_State == BTN_WAIT_DEBOUNCE) { 
    // do we got debounce time reached when released ?
    if (now > _btn_StartTime + BTN_DEBOUNCE_DELAY) 
      _btn_Action = BTN_QUICK_PRESS;
    else
      _btn_Action = BTN_BAD_PRESS;

    // restart state machine
    _btn_State = BTN_WAIT_PUSH; 

  // waiting for menu pin being release after long press.
  } else if (_btn_State == BTN_WAIT_LONG_RELEASE) { 
    // are we released the long press ?
    if (buttonLevel == BTN_RELEASED) {
      // we're not anymore in a long press
      _btn_LongPress = false;  

      // be sure to light off the blinking RGB led
      LedRGBOFF();

      // We done, restart state machine
      _btn_State = BTN_WAIT_PUSH; 
    } else {
      uint8_t sec_press;
      uint16_t rgb_led_color;

      // for how long we have been pressed (in s)
      sec_press = ((now - _btn_StartTime)/1000L);

      // we're still in a long press
      _btn_LongPress = true;

      // We pressed button more than 7 sec
      if (sec_press >= 7 ) {
        // Led will be off 
        _btn_Action = BTN_TIMEOUT;
      // We pressed button between 6 and 7 sec
      } else if (sec_press >= 6  ) {
        _btn_Action = BTN_PRESSED_67;
        // Prepare LED color
        rgb_led_color = COLOR_RED;
      // We pressed button between 5 and 6 sec
      } else if (sec_press >= 5  ) {
        _btn_Action = BTN_PRESSED_56;
        rgb_led_color = COLOR_BLUE;
      // We pressed button between 4 and 5 sec
      } else if (sec_press >= 4  ) {
        _btn_Action = BTN_PRESSED_45;
        rgb_led_color = COLOR_CYAN;
      // We pressed button between 3 and 4 sec
      } else if (sec_press >= 3 ) {
        _btn_Action = BTN_PRESSED_34;
        rgb_led_color = COLOR_YELLOW;
      // We pressed button between 2 and 3 sec
      } else if (sec_press >= 2 ) {
        _btn_Action = BTN_PRESSED_23;
        rgb_led_color = COLOR_GREEN;
      // We pressed button between 1 and 2 sec
      } else if (sec_press >= 1 ) {
        _btn_Action = BTN_PRESSED_12;
        rgb_led_color = COLOR_MAGENTA;
      }

      // manage the fast blinking 
      // 20ms ON / 80ms OFF
      if (millis() % 100 < 50 ) {
        // Set Color
        rgb_led.SetPixelColor(0, HslColor( rgb_led_color / 360.0f, 1.0f, 0.005f * rgb_luminosity));
        rgb_led.Show();    
      } else {
        rgb_led.SetPixelColor(0, RgbColor(0,0,0));
        rgb_led.Show();    
      }
    }   
  }

  // return the state machine we're in
  return (_btn_State);
} 


