/*
 * PrimeTime Watchface v1.3
 * 
 * preferences.c
 *
 * Copyright (c) 2014 Brain Dance Designs LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <pebble.h>

#include "preferences.h"
  
const int persistPrefsKey = 1012; //version 1.2
const int persistPrefsKeyOld = 1000; //version 1.0

typedef struct __attribute__((__packed__)) {
     char *appversion;
     uint8_t battIndOn;
     uint8_t btIndOn;
     uint8_t vibOnDisconnect;
     uint8_t invScreen;
 } persistPrefs;

typedef struct __attribute__((__packed__)) {
     uint8_t battIndOn;
     uint8_t btIndOn;
     uint8_t vibOnDisconnect;
     uint8_t invScreen;
 } persistPrefsOld;

persistPrefs pprefs;

int battInd = 0;
int btInd = 0;
int vibrate = 0;
int screen = 0;

int getBattInd(){
  return battInd;
}

int getBtInd(){
  return btInd;
}

int getVibrate(){
  return vibrate;
}

int getScreen(){
  return screen;
}
void setBattInd(int battIndIn){
  battInd = battIndIn;
}

void setBtInd(int btIndIn){
  btInd = btIndIn;
}

void setVibrate(int vibrateIn){
  vibrate = vibrateIn;
}

void setScreen(int screenIn){
  screen = screenIn;
}

void init_preferences () {
//  retrieve settings
  if (persist_exists(persistPrefsKey)) {
    APP_LOG(APP_LOG_LEVEL_INFO, "found new version! Version key = %d",persistPrefsKey);
    persist_read_data(persistPrefsKey, &pprefs, sizeof(persistPrefs));
    battInd = pprefs.battIndOn;
    btInd = pprefs.btIndOn;
    vibrate = pprefs.vibOnDisconnect;
    screen = pprefs.invScreen;
  }
  else {
    if (persist_exists(persistPrefsKeyOld)) {
      //prior version - need to upgrade settings
      APP_LOG(APP_LOG_LEVEL_INFO, "found old version! Version key = %d",persistPrefsKeyOld);
      persistPrefsOld pprefsold;
      persist_read_data(persistPrefsKeyOld, &pprefsold, sizeof(persistPrefsOld));
      
      //read settings
      battInd = pprefsold.battIndOn;
      btInd = pprefsold.btIndOn;
      vibrate = pprefsold.vibOnDisconnect;
      screen = pprefsold.invScreen;  
      
      //delete old version
      persist_delete(persistPrefsKeyOld);
      
      //store new version
      store_preferences();
      
    }
  }
}

void store_preferences () {
  // store settings
  pprefs.appversion = "1.2";
  pprefs.battIndOn = battInd;
  pprefs.btIndOn = btInd;
  pprefs.vibOnDisconnect = vibrate;
  pprefs.invScreen = screen;
  
  persist_write_data(persistPrefsKey, &pprefs, sizeof(persistPrefs));
}


