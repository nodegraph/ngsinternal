/// <reference path="D:\dev\windows\DefinitelyTyped\chrome\chrome.d.ts"/>

//Class which handles communication between:
//1) this content script and the background script.
class ContentComm {
    // Our dependencies.
    private handler: ContentCommHandler

    // Our members.
    private bg_receivers: {[request_type: string]: (message: any)=>void}
    
    // Constructor.
    constructor () {
        this.connect_to_bg()
    }

    //Setup communication channel with chrome runtime.
    connect_to_bg(): void {
        chrome.runtime.onMessage.addListener(this.receive_from_bg.bind(this))
    }

    //Send a message to the bg script.
    send_to_bg(socket_message: any): void {
        chrome.runtime.sendMessage(socket_message)
    }

    register_bg_request_handler(handler: ContentCommHandler) {
        this.handler = handler
    }

    //Receive a message from the bg script.
    receive_from_bg(request: any, sender: chrome.runtime.MessageSender, send_response: (response: any) => void) {
        this.handler.handle_bg_request(request)
    }
}















