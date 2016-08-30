/// <reference path="D:\dev\windows\DefinitelyTyped\chrome\chrome.d.ts"/>

declare var g_nodejs_port: number

//Class which handles communication between:
// 1) this background script and nodejs
// 2) this background script and the content script
class BgComm {
    // Our dependencies.
    private handler: BgCommHandler

    // Our members.
    private nodejs_socket: WebSocket // socket to communicate with the nodejs app
    private connect_timer: number
    private content_tab_id: number
    private nodejs_receivers: {[request_type: string]: (message: any)=>void}

    constructor() {
        // Our dependencies.
        
        // Our data.
        this.nodejs_socket = null
        this.connect_timer = setInterval(this.connect_to_nodejs.bind(this), 1000) // continually try to connect to nodejs
        this.content_tab_id = null
        this.connect_to_content()
    }

    //------------------------------------------------------------------------------------------------
    // Communication between background and nodej.
    //------------------------------------------------------------------------------------------------

    //Setup communication channel to nodejs.
    connect_to_nodejs() : void {
        // If we're already connected, just return.
        if (this.nodejs_socket && (this.nodejs_socket.readyState == WebSocket.OPEN)) {
            return
        }
        // Otherwise try to connect.
        try {
            this.nodejs_socket = new WebSocket('wss://localhost:' + g_nodejs_port);
            this.nodejs_socket.onerror = function(error: ErrorEvent) {
                console.log("nodejs socket error: " + JSON.stringify(error))
            }
            this.nodejs_socket.onopen = function(event: Event) {
                this.nodejs_socket.send(JSON.stringify({ code: 'bg_comm is connected' }));
            };
            this.nodejs_socket.onmessage = this.receive_from_nodejs.bind(this)
        } catch (e) {
            this.nodejs_socket = null
        }
    }

    //Send message to nodejs.
    send_to_nodejs(socket_message: any): void {
        this.nodejs_socket.send(JSON.stringify(socket_message));
    }

    register_nodejs_request_handler(handler: BgCommHandler) {
        this.handler = handler
    }

    //Receive messages from nodejs. They will be forward to the content script.
    receive_from_nodejs(event: MessageEvent) {
        let request = JSON.parse(event.data);
        console.log("bg received message from nodejs: " + event.data + " parsed: " + request.request)
        bg_comm_handler.handle_nodejs_request(request)
    }

    //------------------------------------------------------------------------------------------------
    // Communication between background and content.
    //------------------------------------------------------------------------------------------------

    // Setup communication channel with chrome runtime.
    connect_to_content(): void {
        chrome.runtime.onMessage.addListener(this.receive_from_content.bind(this))
    }

    // Send a message to the content script.
    send_to_content(socket_message: any): void {
        console.log("bg sending message to content: " + JSON.stringify(socket_message))
        chrome.tabs.sendMessage(this.content_tab_id, socket_message)
    }

    // Receive a message from the content script. We simply forward the message to nodejs.
    receive_from_content(request: any, sender: chrome.runtime.MessageSender, send_response: (response: any) => void) {
        console.log("bg received message from content: " + JSON.stringify(request))
        // The first tab to send us a content message will be the tab that we pay attention to.
        if (!this.content_tab_id) {
            this.content_tab_id = sender.tab.id
        }
        // Skip messages from tabs that we're not interested in.
        if (this.content_tab_id != sender.tab.id) {
            return
        }
        // Pass the message to nodejs.
        this.send_to_nodejs(request);
    }

    get_tab_id() {
        return this.content_tab_id
    }
}



