/// <reference path="D:\dev\windows\DefinitelyTyped\chrome\chrome.d.ts"/>
/// <reference path="..\..\message\message.d.ts"/>

//Class which handles communication between:
// 1) this background script and nodejs
// 2) this background script and the content script
class BgComm {
    // Our dependencies.
    private handler: BgCommHandler

    // Our members.
    private nodejs_socket: WebSocket // socket to communicate with the nodejs app
    private nodejs_port: number

    // Timer to attempt connections.
    private connect_timer: number

    // Our internal state.
    private content_tab_id: number // This is set by the first message received from any tab.
    private iframe: string // This is the current iframe, that the webdriver is acting upon.

    // Listeners to extract the the nodejs's chrome extension server port number.
    bound_on_tab_updated: any

    constructor() {
        // Our dependencies.
        
        // Our nodejs data.
        this.nodejs_socket = null
        this.nodejs_port = -1

        // Connect to the content scripts.
        this.connect_to_content()

        // Our state.
        this.content_tab_id = null
        this.iframe = ""

        // Hack to retrieve the nodejs port.
        // The very first opened tab's url will have the port number embedded into the url.
        this.bound_on_tab_updated = this.on_tab_updated.bind(this)
        chrome.tabs.onUpdated.addListener(this.bound_on_tab_updated)

        // Seems like the nodejs connect is lost frequently so we poll it.
        this.connect_timer = setInterval(this.connect_to_nodejs.bind(this), 500)

        // The background script is persistent once the browser is up.
        // So we shouldn't be closing the nodejs_socket until the browser closes down.

        // window.onbeforeunload = function() {
        //     this.nodejs_socket.close()
        // }
    }

    set_iframe(iframe: string): void {
        this.iframe = iframe
    }

    get_iframe(): string {
        return this.iframe
    }

    on_tab_updated(tabId: number, changeInfo: chrome.tabs.TabChangeInfo, tab: chrome.tabs.Tab) {
        this.initialize_from_tab(tab)
    }

    initialize_from_tab(tab: chrome.tabs.Tab) {
        if (!tab.url) {
            return
        }
        this.content_tab_id = tab.id
        this.nodejs_port = BgComm.extract_port_from_url(tab.url)
        this.connect_to_nodejs()
        chrome.tabs.onUpdated.removeListener(this.bound_on_tab_updated)
    }

    static extract_port_from_url(url: string): number {
        let index = url.indexOf('?')
        if (index != -1) {
            return Number(url.substring(index + 1))
        } else {
            console.error("Error: no port number present")
            return -1
        }
    }

    //------------------------------------------------------------------------------------------------
    // Communication between background and nodej.
    //------------------------------------------------------------------------------------------------

    // Setup communication channel to nodejs.
    connect_to_nodejs(): void {
        // Return if we haven't received our nodejs port number yet.
        if (this.nodejs_port == -1) {
            return
        }
        // If we're already connected, just return.
        if (this.nodejs_socket && (this.nodejs_socket.readyState == WebSocket.OPEN)) {
            return
        }
        // Otherwise try to connect.
        try {
            this.nodejs_socket = new WebSocket('wss://localhost:' + this.nodejs_port)
            this.nodejs_socket.onerror = function(error: ErrorEvent) {
                console.error("nodejs socket error: " + JSON.stringify(error))
            }.bind(this)
            this.nodejs_socket.onopen = function(event: Event) {
                // BgComm is now connected.
                this.send_to_nodejs(new InfoMessage(-1, "-1", InfoType.kBgIsConnected))
            }.bind(this);
            this.nodejs_socket.onmessage = this.receive_from_nodejs.bind(this)
        } catch (e) {
            console.error("Error: trying to connect to port error: " + JSON.stringify(e))
            this.nodejs_socket = null
        }
    }

    // Send message to nodejs.
    send_to_nodejs(msg: BaseMessage): void {
        // If we're not connected to nodejs yet, then just return.
        if (!this.nodejs_socket || (this.nodejs_socket.readyState != WebSocket.OPEN)) {
            console.log('nodejs socket is not connected')
            console.log('unable to send: ' + msg.to_string())
            return
        }
        this.nodejs_socket.send(JSON.stringify(msg));
    }

    register_nodejs_request_handler(handler: BgCommHandler) {
        this.handler = handler
    }

    // Receive messages from nodejs. They will be forward to the content script.
    receive_from_nodejs(event: MessageEvent) {
        let msg = BaseMessage.create_from_string(event.data);
        //let request = JSON.parse(event.data);
        //console.log("bg received message from nodejs: " + event.data)
        if (msg.get_msg_type() != MessageType.kRequestMessage) {
            console.error('bgcomm was expecting a request message')
            return
        }
        this.handler.handle_nodejs_request(<RequestMessage>msg)
    }

    //------------------------------------------------------------------------------------------------
    // Communication between background and content.
    //------------------------------------------------------------------------------------------------

    // Setup communication channel with chrome runtime.
    connect_to_content(): void {
        chrome.runtime.onMessage.addListener(this.receive_from_content.bind(this))
    }

    // Send a message to the content script.
    send_to_content(msg: BaseMessage): void {
        //console.log("bg sending message to content: " + JSON.stringify(msg))
        chrome.tabs.sendMessage(this.content_tab_id, msg)
    }

    // Receive a message from the content script. We simply forward the message to nodejs.
    receive_from_content(msg: BaseMessage, sender: chrome.runtime.MessageSender, send_response: (response: any) => void) {
        console.log("bg received from frameid: " + sender.frameId + " : " + JSON.stringify(msg))
        // The first tab to send us a content message will be the tab that we pay attention to.
        if (!this.content_tab_id) {
            this.content_tab_id = sender.tab.id
        }
        // Skip messages from tabs that we're not interested in.
        if (this.content_tab_id != sender.tab.id) {
            return
        }

        // If the current iframe doesn't match we note this in the 
        if (msg.msg_type == MessageType.kInfoMessage) {
            let req = <InfoMessage>msg
            if (req.info == InfoType.kShowWebActionMenu && req.iframe != this.iframe) {
                console.log('message is from a different iframe')
                req.value.prev_iframe = this.iframe
            }

        }

        // Pass the message to nodejs.
        console.log("sending to nodejs: " + JSON.stringify(msg))
        this.send_to_nodejs(msg);
    }

    get_tab_id() {
        return this.content_tab_id
    }
}



