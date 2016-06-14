//------------------------------------------------------------------------------------------------
//Our communication state with the controller (node.js).
//------------------------------------------------------------------------------------------------
var controller_socket = null

var connect_to_controller = function() {
	if (!controller_socket || (controller_socket.readyState != WebSocket.OPEN)) {
		try {
			controller_socket = new WebSocket('wss://localhost:8083');
			controller_socket.onerror=function(error){
			    log_error("connecting to browser controller: " + JSON.stringify(error))
			}
			controller_socket.onopen = function (event) {
				controller_socket.send(JSON.stringify({code: 'background_connected'})); 
			};
			controller_socket.onmessage = on_message_from_controller
		} catch(e){
		    controller_socket = null
		}
	} else {
		log_info("background connected")
	}
}

//We continuously try connect to the native app allowing the
//user to have the flexibility to run or not run the native app.
var timer = setInterval(function() {
 connect_to_controller()
}, 1000)

var start_recording_called = false

//Listen for messages from our nodejs controller.
var on_message_from_controller = function(event) {
    var msg = JSON.parse(event.data);
    console.log("received message: " + event.data + " parsed: " + msg.code)

    switch (msg.code) {
        case 'start_recording':
            start_recording_called = true
            
            // Initialize the recording state.
            // The recording tab id will be retrieved from the first tab to message us.
            recording = true;
            recording_tab_id = null;
            
            // Clear the cookies so that the websites don't
            // present us with customized pages.
            clear_browser_cookies(function() {})
            break
        case 'stop_recording':
            console.log("start_recording was called: " + start_recording_called)
            
            // Tell the recording tab to stop recording.
            chrome.tabs.sendMessage(recording_tab_id, {
                code : "stop_recording"
            });
            
            // Update our state.
            recording = false
            recording_tab_id = null
            
//            // Build up the script message.
//            commands.push("send_result()\n")
//            var stringified = JSON.stringify(commands)
//            var msg = {
//                code: "recording_finished",
//                commands: stringified
//            }
//            // Send the final script back to the controller.
//            controller_socket.send(JSON.stringify(msg))
//            log_info("sending script: " + stringified)
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
    
    console.log("got content message: " + request.code + " from tabid: " + sender.tab.id)
    
    //When content scripts tell us they are embedded we respond with
    //whether they should be recording.
    if (request.code == "recorder_embedded") {
        log_info("background got recorder_embedded message: " + recording)
        if (recording && (recording_tab_id == null)) {
            recording_tab_id = sender.tab.id
            sendResponse(true)
        } else if (sender.tab.id == recording_tab_id) {
            sendResponse(true)
        } else {
            sendResponse(false)
        }
        return
    }
    
    // If we get a message from a non recording tab we tell it to stop.
    if (sender.tab.id != recording_tab_id) {
        console.log("recording tab id is: " + recording_tab_id)
        console.log("sender tab id is: " + sender.tab.id)
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
        // request.command is a string
        console.log("got command: " + request.command)
        controller_socket.send(JSON.stringify({code: 'command', command: request.command}))
    } else if (request.code == 'jitter_browser_size') {
        var msg = {
                code: "jitter_browser_size",
        }
        controller_socket.send(JSON.stringify({code: 'jitter_browser_size'}))
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

//We grab the recording id from the first tab to message us after recording is set to true.
var recording = false
var recording_tab_id = null 




