
// Class which handles communication between this content script and the background script.
class ContentComm {
    // Our dependencies.
    private handler: ContentCommHandler

    // Our data.

    // Constructor.
    constructor () {
        this.connect_to_bg()
    }

    // Setup communication channel with chrome runtime.
    connect_to_bg(): void {
        chrome.runtime.onMessage.addListener(this.receive_from_bg_bound)
    }

    // Send a message to the bg script.
    send_to_bg(msg: BaseMessage): void {
        chrome.runtime.sendMessage(msg)
    }

    register_bg_request_handler(handler: ContentCommHandler) {
        this.handler = handler
    }

    // Receive a message from the bg script.
    receive_from_bg(obj: any, sender: chrome.runtime.MessageSender, send_response: (response: any) => void){
        // The base message will get flattened out into a regular dict obj, during the transfer from the bgcomm.
        let msg = BaseMessage.create_from_obj(obj)

        // Some messages apply to all frames.
        if (msg.get_msg_type() == MessageType.kRequestMessage) {
            let req = <RequestMessage>(msg)
            // Update overlays on all frames.
            if (req.request == RequestType.kUpdateOveralys) {
                this.handler.handle_bg_request(req)
                return
            }
        } 

        // Ignore the message if it doesn't match our iframe.
        if (msg.iframe != PageWrap.get_iframe_index_path_as_string(window)) {
            return
        }

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

    receive_from_bg_bound = (obj: any, sender: chrome.runtime.MessageSender, send_response: (response: any) => void) => {
        this.receive_from_bg(obj, sender, send_response)
    }
}















