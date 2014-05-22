/*
 * PrimeTime Watchface v1.3
 * 
 * pebble-js-app.js
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

// app message sent successfully
//
function appMessageAck(e) {
	console.log("App message sent successfully");
}

// app message failed to send
//
function appMessageNack(e) {
	console.log("App message failed to send: " + e.error.message);
}

// app ready event
//
Pebble.addEventListener("ready",
	function(e) {
		console.log("connect! [" + e.ready + "] (" + e.type + ")");
    console.log("JavaScript app ready and running!");
		var settings = localStorage.getItem("settings");
    if (!settings) {
      //initialize settings to all zeros
      settings = {"battIndOn":"0","btIndOn":"0","vibOnDisconnect":"0","invScreen":"0"};
    }
    else {
      //send stored settings to watch
      console.log("settings found");
			console.log("Settings: " + settings);
      settings = JSON.parse(settings);
    }
    Pebble.sendAppMessage(settings, appMessageAck, appMessageNack);
  }
);

// display configuration screen
Pebble.addEventListener("showConfiguration",
	function() {
		var settings = localStorage.getItem("settings");
    var config = "'http://braindancedesigns.com/pebble/primetime-config2.html";
    var url = config + "?settings=" + encodeURIComponent(JSON.stringify(settings));
		console.log("Opening Config: " + url);
    console.log("Settings: " + localStorage.getItem("settings"));
    if (!settings) {
      Pebble.openURL('http://braindancedesigns.com/pebble/primetime-config2.html');      
    }
    else {
      Pebble.openURL('http://braindancedesigns.com/pebble/primetime-config2.html?settings=' + encodeURIComponent(JSON.stringify(settings)));
    }
  }
);

// close configuration screen
Pebble.addEventListener("webviewclosed",
	function(e) {
		var settings;
		try {
			settings = JSON.parse(decodeURIComponent(e.response));
			localStorage.clear();
      if (!settings) {
        console.log("settings reset");
      //initialize settings to all zeros
      settings = {"battIndOn":"0","btIndOn":"0","vibOnDisconnect":"0","invScreen":"0"};
      console.log("Settings: " + settings);
      }
      else {
        localStorage.setItem("settings", JSON.stringify(settings));
      }
			console.log("Settings: " + localStorage.getItem("settings"));
			Pebble.sendAppMessage(settings, appMessageAck, appMessageNack);
		} catch(err) {
			settings = false;
			console.log("No JSON response or received Cancel event");
		}
	}
);
