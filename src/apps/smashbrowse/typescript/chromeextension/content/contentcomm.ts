/// <reference path="D:\dev\windows\DefinitelyTyped\chrome\chrome.d.ts"/>

//import {BaseMessage, RequestMessage, ResponseMessage, InfoMessage, MessageType} from "../../controller/socketmessage" 
//import {ContentCommHandler} from "./contentcommhandler"

//Class which handles communication between:
//1) this content script and the background script.
class ContentComm {
    // Our dependencies.
    private handler: ContentCommHandler

    // Constructor.
    constructor () {
        this.connect_to_bg()
    }

    //Setup communication channel with chrome runtime.
    connect_to_bg(): void {
        chrome.runtime.onMessage.addListener(this.receive_from_bg.bind(this))
    }

    //Send a message to the bg script.
    send_to_bg(msg: BaseMessage): void {
        chrome.runtime.sendMessage(msg)
    }

    register_bg_request_handler(handler: ContentCommHandler) {
        this.handler = handler
    }

    //Receive a message from the bg script.
    receive_from_bg(obj: any, sender: chrome.runtime.MessageSender, send_response: (response: any) => void) {
        // The base message will get flattened out into a regular dict obj, during the transfer from the bgcomm.
        let msg = BaseMessage.create_from_obj(obj)
        switch (msg.get_msg_type()) {
            case MessageType.kRequestMessage:
                this.handler.handle_bg_request(<RequestMessage>msg)
                break
            case MessageType.kResponseMessage:
                this.handler.handle_bg_response(<ResponseMessage>msg)
                break
            case MessageType.kInfoMessage:
                this.handler.handle_bg_info(<InfoMessage>msg)
                break
        }
    }
}















