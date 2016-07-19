

//Class which handles communication between:
//1) this content script and the background script.
var ContentCommunication = function () {
    this.connect_to_bg()
}

//Setup communication channel with chrome runtime.
ContentCommunication.prototype.connect_to_bg = function() {
    chrome.runtime.onMessage.addListener(this.receive_bg_message.bind(this))
}

//Send a message to the bg script.
ContentCommunication.prototype.send_bg_message = function(socket_message) {
    chrome.runtime.sendMessage(socket_message)
}

//Receive a message from the bg script.
ContentCommunication.prototype.receive_bg_message = function(request, sender, send_response) {
    switch(request.code) {
        case unblock:
            break
    }
    send_bg_message(ResponseMessage(true))
}


//------------------------------------------------------------------------------------------------
//Globals.
//------------------------------------------------------------------------------------------------

var g_content_comm = new ContentCommunication()















