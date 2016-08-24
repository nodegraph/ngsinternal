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
        this.nodejs_socket = new WebSocket('wss://localhost:' + g_nodejs_port);
        this.nodejs_socket.onerror=function(error) {
            console.log("nodejs socket error: " + JSON.stringify(error))
        }
        this.nodejs_socket.onopen = function (event) {
            g_bg_comm.nodejs_socket.send(JSON.stringify({code: 'bg_comm is connected'})); 
        };
        this.nodejs_socket.onmessage = this.receive_from_nodejs.bind(this)
    } catch(e){
        this.nodejs_socket = null
    }
}

//Send message to nodejs.
BackgroundCommunication.prototype.send_to_nodejs = function(socket_message) {
    this.nodejs_socket.send(JSON.stringify(socket_message));
}

//Receive messages from nodejs. They will be forward to the content script.
BackgroundCommunication.prototype.receive_from_nodejs = function(event) {
    var request = JSON.parse(event.data);
    console.log("bg received message from nodejs: " + event.data + " parsed: " + request.request)
    
    // We intercept certain requests before it gets to the content script,
    // because content scripts can't use the chrome.* APIs except for parts of chrome.extension for message passing.
    switch(request.request) {
        case 'clear_all_cookies':
            function done_clear_all_cookies() {
                var response = {response: true}
                g_bg_comm.send_to_nodejs(response)
            }
            g_browser_wrap.clear_all_cookies(done_clear_all_cookies)
            break
        case 'get_all_cookies':
            function done_get_all_cookies(cookies) {
                var response = {response: true, value: cookies}
                g_bg_comm.send_to_nodejs(response)
            }
            chrome.cookies.getAll({}, done_get_all_cookies);
            break
        case 'set_all_cookies':
            var cookies = request.cookies
            var count=0
            function done_set_all_cookies() {
                count+=1
                console.log('count is: ' + count)
                if (count == cookies.length) {
                    var response = {response: true}
                    g_bg_comm.send_to_nodejs(response)
                }
            }
            g_browser_wrap.set_all_cookies(cookies, done_set_all_cookies)
            break
        case 'get_zoom':
            function done_get_zoom(zoom) {
        		console.log('zoom is: '+ zoom)
	            var response = {response: true, value: zoom}
	            g_bg_comm.send_to_nodejs(response)
	        }
            chrome.tabs.getZoom(g_bg_comm.content_tab_id, done_get_zoom);
        	break
        default:
            this.send_to_content(request)
            break
    }
    
}

//------------------------------------------------------------------------------------------------
//Content script communication.
//------------------------------------------------------------------------------------------------

//Setup communication channel with chrome runtime.
BackgroundCommunication.prototype.connect_to_content = function() {
    chrome.runtime.onMessage.addListener(this.receive_from_content.bind(this))
}

//Send a message to the content script.
BackgroundCommunication.prototype.send_to_content = function(socket_message) {
    console.log("bg sending message to content: " + JSON.stringify(socket_message))
    chrome.tabs.sendMessage(this.content_tab_id, socket_message)
}

//Receive a message from the content script. We simply forward the message to nodejs.
BackgroundCommunication.prototype.receive_from_content = function(request, sender, send_response) {
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
    this.send_to_nodejs(request);
}


//------------------------------------------------------------------------------------------------
//Globals.
//------------------------------------------------------------------------------------------------

var g_bg_comm = null

// We only create on the top window, not in other iframes.
if (window == window.top) {
    g_bg_comm = new BackgroundCommunication()
}




