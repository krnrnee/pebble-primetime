/*
 * PrimeTime Watchface v1.02
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
  
const int persistPrefsKey = 1000;

typedef struct __attribute__((__packed__)) {
     uint8_t battIndOn;
     uint8_t btIndOn;
     uint8_t vibOnDisconnect;
     uint8_t invScreen;
 } persistPrefs;

persistPrefs pprefs;
    
void init_preferences () {
  //  retrieve settings
  if (persist_exists(persistPrefsKey)) {
    persist_read_data(persistPrefsKey, &pprefs, sizeof(persistPrefs));
    battInd = pprefs.battIndOn;
    btInd = pprefs.btIndOn;
    vibrate = pprefs.vibOnDisconnect;
    screen = pprefs.invScreen;
  }  
}

void store_preferences () {
  // store settings
  pprefs.battIndOn = battInd;
  pprefs.btIndOn = btInd;
  pprefs.vibOnDisconnect = vibrate;
  pprefs.invScreen = screen;
  
  persist_write_data(persistPrefsKey, &pprefs, sizeof(persistPrefs));
}


