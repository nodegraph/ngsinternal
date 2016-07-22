//Class which handles communication between:
//1) this background script and nodejs
//2) this background script and the content script
var BackgroundCommunication = function () {
    this.nodejs_socket = null // socket to communicate with the nodejs app
    this.connect_timer = setInterval(this.connect_to_nodejs.bind(this), 1000) // continually try to connect to nodejs
    this.content_tab_id = null
    this.connect_to_content()
}

//------------------------------------------------------------------------------------------------
//Nodejs communication.
//------------------------------------------------------------------------------------------------

//Setup communication channel to nodejs.
BackgroundCommunication.prototype.connect_to_nodejs = function() {
    // If we're already connected, just return.
    if (this.nodejs_socket && (this.nodejs_socket.readyState == WebSocket.OPEN)) {
        return
    }
    // Otherwise try to connect.
    try {
        this.nodejs_socket = new WebSocket('wss://localhost:8083');
        this.nodejs_socket.onerror=function(error) {
            console.log("nodejs socket error: " + JSON.stringify(error))
        }
        this.nodejs_socket.onopen = function (event) {
            this.nodejs_socket.send(JSON.stringify({code: 'bg_comm is connected'})); 
        };
        this.nodejs_socket.onmessage = this.receive_message_from_nodejs.bind(this)
    } catch(e){
        this.nodejs_socket = null
    }
}

//Send message to nodejs.
BackgroundCommunication.prototype.send_message_to_nodejs = function(socket_message) {
    this.nodejs_socket.send(JSON.stringify(socket_message));
}

//Receive messages from nodejs. They will be forward to the content script.
BackgroundCommunication.prototype.receive_message_from_nodejs = function(event) {
    var request = JSON.parse(event.data);
    console.log("bg received message from nodejs: " + event.data + " parsed: " + request.request)
    this.send_message_to_content(request)
}

//------------------------------------------------------------------------------------------------
//Content script communication.
//------------------------------------------------------------------------------------------------

//Setup communication channel with chrome runtime.
BackgroundCommunication.prototype.connect_to_content = function() {
    chrome.runtime.onMessage.addListener(this.receive_message_from_content.bind(this))
}

//Send a message to the content script.
BackgroundCommunication.prototype.send_message_to_content = function(socket_message) {
    console.log("bg sending message to content: " + JSON.stringify(socket_message))
    chrome.tabs.sendMessage(this.content_tab_id, socket_message)
}

//Receive a message from the content script. We simply forward the message to nodejs.
BackgroundCommunication.prototype.receive_message_from_content = function(request, sender, send_response) {
    console.log("bg received message from content: " + JSON.stringify(request))
    //The first tab to send us a content message will be the tab that we pay attention to.
    if (!this.content_tab_id) {
        this.content_tab_id = sender.tab.id
    }
    //Skip messages from tabs that we're not interested in.
    if (this.content_tab_id != sender.tab.id) {
        return
    }
    //Pass the message to nodejs.
    this.send_message_to_nodejs(request);
}


//------------------------------------------------------------------------------------------------
//Globals.
//------------------------------------------------------------------------------------------------

var g_bg_comm = null

// We only create on the top window, not in other iframes.
if (window == window.top) {
    g_bg_comm = new BackgroundCommunication()
}




