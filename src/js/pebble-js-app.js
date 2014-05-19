var initialized = false;

Pebble.addEventListener("ready", function() {
  console.log("ready called!");
  initialized = true;
});


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
	}
);

// display configuration screen
//
Pebble.addEventListener("ready",
  function(e) {
    console.log("JavaScript app ready and running!");
		var settings = localStorage.getItem("settings");
    if (!settings) {
      //do nothing
    }
    else {
      //send initial settings to the phone
      console.log("settings found, sending to phone");
			console.log("Settings: " + settings);
      settings = JSON.parse(settings);
      Pebble.sendAppMessage(settings, appMessageAck, appMessageNack);
    }
  }
);

Pebble.addEventListener("showConfiguration",
	function() {
		var settings = localStorage.getItem("settings");
    var config = "'http://braindancedesigns.com/pebble/primetime-config2.html";
    var url = config + "?settings=" + encodeURIComponent(JSON.stringify(settings));
		console.log("Opening Config: " + url);
    console.log("Settings: " + localStorage.getItem("settings"));
    Pebble.openURL('http://braindancedesigns.com/pebble/primetime-config2.html?settings=' + encodeURIComponent(JSON.stringify(settings)));
	}
);

// close configuration screen
//
Pebble.addEventListener("webviewclosed",
	function(e) {
		var settings;
		try {
			settings = JSON.parse(decodeURIComponent(e.response));
			localStorage.clear();
			localStorage.setItem("settings", JSON.stringify(settings));
			console.log("Settings: " + localStorage.getItem("settings"));
			Pebble.sendAppMessage(settings, appMessageAck, appMessageNack);
		} catch(err) {
			settings = false;
			console.log("No JSON response or received Cancel event");
		}
	}
);
