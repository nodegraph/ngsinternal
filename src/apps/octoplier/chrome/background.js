//------------------------------------------------------------------------------------------------
//Debugging State.
//------------------------------------------------------------------------------------------------
var debugging = true
var log_info = function(msg) {
    if (debugging) {
        console.log("Info: " + msg)
    }
}
var log_error = function(msg) {
    if (debugging) {
        console.log("Error: " + msg)
    }
}
var log_exception = function(e) {
    if (debugging) {
        console.log("Exception: " + e.message + " stack: " + e.stack)
    }
}

//------------------------------------------------------------------------------------------------
//Our communication state with the controller (node.js).
//------------------------------------------------------------------------------------------------
var controller_socket = null

var connect_to_app = function() {
	if (!controller_socket || (controller_socket.readyState != WebSocket.OPEN)) {
		try {
			controller_socket = new WebSocket('wss://localhost:8083');
			controller_socket.onerror=function(error){
			    log_error("connecting to browser controller: " + JSON.stringify(error))
			}
			controller_socket.onopen = function (event) {
				controller_socket.send(JSON.stringify({code: 'background_connected'})); 
			};
			controller_socket.onmessage = onmessage
		} catch(e){}
	} else {
		log_info("background connected")
	}
}

//We continuously try to the native app allowing the
//user to have the flexibility to run or not run the native app.
var timer = setInterval(function() {
 connect_to_app()
}, 1000)

//Listen for messages from our nodejs browser controller.
var onmessage = function(event) {
    var msg = JSON.parse(event.data);
    log_info("received message: " + event.data + " parsed: " + msg.code)

    switch (msg.code) {
        case 'start_recording':
            // Clear the cookies so that the websites don't
            // present us with customized pages.
            clear_browser_cookies(function() {
                get_active_tab(start_recording)
            })
            break
        case 'stop_recording':
            // Tell the recording tab to stop recording.
            chrome.tabs.sendMessage(recording_tab_id, {
                code : "stop_recording"
            });
            // Build up the script message.
            commands.push("send_result()\n")
            var stringified = JSON.stringify(commands)
            var msg = {
                code: "script",
                commands: stringified
            }
            // Send the final script back to the controller.
            controller_socket.send(JSON.stringify(msg))
            log_info("sending script: " + stringified)
            recording_tab_id = null
            commands.length = 0
            break
        case 'clear_browser_cookies':
            // This gets called from the controller to create
            // a pristine environment in which to replay scripts.
            clear_browser_cookies(function(){})
            break
    }
}

//------------------------------------------------------------------------------------------------
//Our communication state with the content scripts.
//------------------------------------------------------------------------------------------------

//Listen for messages from our content scripts.
chrome.runtime.onMessage.addListener(function(request, sender, sendResponse) {
    
    //When content scripts tell us they are embedded we respond with
    //whether they should be recording.
    if (request.code == "recorder_embedded") {
        log_info("background got recorder_embedded message")
        if (sender.tab.id == recording_tab_id) {
            sendResponse(true)
        } else {
            sendResponse(false)
        }
        return
    }
    
    // If we get a message from a non recording tab we tell it to stop.
    if (sender.tab.id != recording_tab_id) {
        chrome.tabs.sendMessage(sender.tab.id, {code: "stop_recording"})
        return
    }
    
    // Otherwise we process the message.
    if (request.code == "browser_info") {
        log_info(request.error)
    } else if (request.code == "browser_error") {
        log_error(request.error)
    } else if (request.code == "browser_exception") {
        log_error(request.error)
    } else if (request.code == "page_is_ready") {
        // Notify the controller that the page is ready to be automated.
        controller_socket.send(JSON.stringify({code: 'page_is_ready'})); 
    } else if (request.code == "command") {
        commands.push(request.command)
    }
    sendResponse({code: 'ok'})
    return true
});

//------------------------------------------------------------------------------------------------
//Utilities.
//------------------------------------------------------------------------------------------------

var clear_browser_cache = function(callback) {
    chrome.browsingData.remove({
        "since": 1
      }, {
        "appcache": true,
        "cache": true,
        "cookies": true,
        "downloads": true,
        "fileSystems": true,
        "formData": true,
        "history": true,
        "indexedDB": true,
        "localStorage": true,
        "pluginData": true,
        "passwords": true,
        "webSQL": true
      }, callback);
}

var clear_browser_cookies = function(callback) {
    chrome.browsingData.removeCookies({
        "since": 1
    }, callback);
}

var close_other_tabs = function(tab_id) {
    chrome.tabs.getAllInWindow(null, function(tabs) {
        for (var i = 0; i < tabs.length; i++) {
            if (tabs[i].id != tab_id) {
                chrome.tabs.remove(tabs[i].id)
            }
        }
    });
}

//------------------------------------------------------------------------------------------------
//Recording State.
//------------------------------------------------------------------------------------------------
var recording_tab_id = null // This will only be non-null during recording.
var commands = []

//Retrieves the active tab and calls the callback with the tabs array
//which should only have one element in it
var get_active_tab = function(callback) {
    var queryInfo = {
            active : true,
            currentWindow : true
    };
    chrome.tabs.query(queryInfo, callback)
}

//Start recording.
//tabs[0] should contain the active tab
var start_recording = function(tabs) {
    if (!tabs[0]) {
        console.log("There is not active tab to record from!")
        return
    }
    recording_tab_id = tabs[0].id;

    // Initialize the script.
    commands.length = 0
    commands.push("goto_url('" + tabs[0].url + "')\n")

    // Reload the active tab to clear prefilled values and to start recording.
    chrome.tabs.reload(recording_tab_id, {}, function(){});
}




