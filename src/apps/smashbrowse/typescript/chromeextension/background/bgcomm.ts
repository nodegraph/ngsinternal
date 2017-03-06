
//Class which handles communication between:
// 1) this background script and our app
// 2) this background script and all the content scripts running in their respective frames
class BgComm {
    // Our dependencies.
    private handler: BgCommHandler

    // Our members.
    private app_socket: WebSocket // socket to communicate with our app
    private app_port: number

    // Our internal state.
    // This is an array of tab_ids in order from the oldest to the newest created tab.
    // The current tab id is always assumed to be the last entry.
    private tab_ids: number[] = [] 
    private created_tab_ids: number[] = []

    constructor() {
        // Our dependencies.
        
        // Our app socket.
        this.app_socket = null
        this.app_port = -1

        // Connect to the content scripts.
        this.connect_to_content()

        chrome.tabs.onUpdated.addListener((tabId: number, changeInfo: chrome.tabs.TabChangeInfo, tab: chrome.tabs.Tab) => {
            this.on_tab_updated(tabId, changeInfo, tab)
        })
        chrome.tabs.onCreated.addListener((tab: chrome.tabs.Tab) => {
            this.on_tab_created(tab)
        })
        chrome.tabs.onRemoved.addListener((tab_id: number, remove_info: any) => {
           this.on_tab_removed(tab_id, remove_info)
        })
    }

    get_current_tab_id() {
        return this.tab_ids[this.tab_ids.length-1]
    }

    // Joins the novel tab ids with the existing and makes the last tab_id be the current tab id.
    update_current_tab_id() {
        this.tab_ids = this.tab_ids.concat(this.created_tab_ids)
        this.created_tab_ids.length = 0
        chrome.tabs.update(this.get_current_tab_id(), {active: true, pinned: true});
        console.log('adding created_tab_ids: ' + JSON.stringify(this.created_tab_ids) + ' to yield tab_ids: ' + JSON.stringify(this.tab_ids))
    }

    on_tab_created(tab: chrome.tabs.Tab) {
        console.log('new tab created: ' + tab.id + ' with url: ' + tab.url)
        // Skip tabs or windows which hold chrome debugging tools.
        // These usually have urls that start with: chrome:// or chrome-devtools://
        // In these case the tab.url will empty.
        if (!tab.url) {
            return
        }
        this.created_tab_ids.push(tab.id)
        chrome.tabs.update(tab.id, {pinned: true});
        this.send_to_app(new InfoMessage(-1, InfoType.kTabCreated))
    }

    on_tab_removed(tab_id: number, remove_info: any) {
        if (tab_id != this.get_current_tab_id()) {
            console.error('Warning: A tab was not the latest tab was manually destroyed. The nodes must be used to destoy tabs.')
            // If the tab_id is in tab_ids, remove it.
            for (let i = 0; i < this.tab_ids.length; i++) {
                if (this.tab_ids[i] == tab_id) {
                    this.tab_ids.splice(i,1)
                    i -= 1
                }
            }
            // If the tab_id is in created_tab_ids, remove it.
            for (let i = 0; i < this.created_tab_ids.length; i++) {
                if (this.created_tab_ids[i] == tab_id) {
                    this.created_tab_ids.splice(i, 1)
                    i -= 1
                }
            }
        } else {
            this.tab_ids.pop()
            chrome.tabs.update(this.get_current_tab_id(), {active: true, pinned: true});
        }

        console.log('tab removed: ' + tab_id + ' to yield: ' + JSON.stringify(this.tab_ids))
        this.send_to_app(new InfoMessage(-1, InfoType.kTabDestroyed))
    }

    on_tab_updated(tabId: number, change_info: chrome.tabs.TabChangeInfo, tab: chrome.tabs.Tab) {

        // We don't allow the user to unpin the tabs, because when the tabs are unpinned they can be
        // accidentally destroyed by the user. We want to keep the destroying under our control.
        chrome.tabs.update(tabId, {pinned: true});

        // Hack to retrieve the app's socket port.
        // The very first opened tab's url will have the port number embedded into the url.
        if (this.app_port != -1) {
            return
        }
        if (!tab.url) {
            return
        }
        
        if (tab.url.search("smashbrowse.html?") < 0) {
            return
        }

        this.app_port = BgComm.extract_port_from_url(tab.url)
        if (this.app_port != -1) {
        	console.log("found app port")
            this.connect_to_app()
            //chrome.tabs.onUpdated.removeListener(this.on_tab_updated_bound)
            this.tab_ids = [tab.id]
            BrowserWrap.close_other_tabs(this.get_current_tab_id())
        }
    }

    static extract_port_from_url(url: string): number {
        let index = url.indexOf('?')
        if (index != -1) {
            return Number(url.substring(index + 1))
        } else {
            //console.error("Error: no port number present")
            return -1
        }
    }

    //------------------------------------------------------------------------------------------------
    // Communication between background and nodej.
    //------------------------------------------------------------------------------------------------

    // Setup communication channel to app.
    connect_to_app(): void {
        // Return if we haven't received our app port number yet.
        if (this.app_port == -1) {
            return
        }
        // If we're already connected, just return.
        if (this.app_socket && (this.app_socket.readyState == WebSocket.OPEN)) {
            return
        }
        console.log('trying to connect to app')
        // Otherwise try to connect.
        try {
            this.app_socket = new WebSocket('wss://localhost:' + this.app_port)
            this.app_socket.onerror = (error: ErrorEvent) => {
                console.error("Error: app socket error: " + JSON.stringify(error))
            }
            this.app_socket.onopen = (event: Event) => {
                console.log('app sockected connected ... testing sending info over it')
                // BgComm is now connected.
                this.send_to_app(new InfoMessage(-1, InfoType.kBgIsConnected))
                console.log('app sockected connected ... done sending over it')
            }
            this.app_socket.onclose = (event: Event) => {
            	console.log('app sockect is now closed')
            }
            this.app_socket.onmessage = (event: MessageEvent) => { this.receive_from_app(event) }
        } catch (e) {
            console.error("Error: trying to connect to port error: " + JSON.stringify(e))
            this.app_socket = null
        }
    }

    // Send message to app.
    send_to_app(msg: BaseMessage): void {
        // If we're not connected to the app yet, then just return.
        if (!this.app_socket || (this.app_socket.readyState != WebSocket.OPEN)) {
            console.log('app socket is not connected, unable to send: ' + msg.to_string())
            return
        }
        this.app_socket.send(JSON.stringify(msg));
    }

    register_app_request_handler(handler: BgCommHandler) {
        this.handler = handler
    }

    // Receive messages from the app. They will be forward to the content script.
    receive_from_app(event: MessageEvent) {
        let msg = BaseMessage.create_from_string(event.data);
        //console.log("bg received message from app: " + event.data)
        if (msg.get_msg_type() != MessageType.kRequestMessage) {
            console.error('bgcomm was expecting a request message')
            return
        }
        this.handler.handle_app_request(<RequestMessage>msg)
    }

    //------------------------------------------------------------------------------------------------
    // Communication between background and content.
    //------------------------------------------------------------------------------------------------

    // Notes from chrome api web page.
    // Note: If multiple pages are listening for onMessage events, only the first to call sendResponse() for a particular event will succeed in sending the response. All other responses to that event will be ignored.
    // Note: The sendResponse callback is only valid if used synchronously, or if the event handler returns true to indicate that it will respond asynchronously. 
    //       The sendMessage function's callback will be invoked automatically if no handlers return true or if the sendResponse callback is garbage-collected.
    // 
    // Note we currently do message passing synchronously. So the sendResponse callback will be called by the first listener who calls send_response.
    // However none of our lisetners call send_response, so the sendResponse callback will be called after all listeners finish processing the message.

    // Setup communication channel with chrome runtime.
    connect_to_content(): void {
        chrome.runtime.onMessage.addListener(
            (msg: BaseMessage, sender: chrome.runtime.MessageSender, send_response: (response: any) => void) =>
            { this.receive_from_content(msg, sender, send_response) })
    }

    // Send a message to all frames.
    send_to_content(msg: BaseMessage, on_response: (response: any) => void = function(){}): void {
        //console.log("bg sending message to content: " + JSON.stringify(msg))
        chrome.tabs.sendMessage(this.get_current_tab_id(), msg, on_response)
    }

    // Receive a message from on of the frames.
    receive_from_content(msg: BaseMessage, sender: chrome.runtime.MessageSender, send_response: (response: any) => void) {
        console.log("bg received from frameid: " + sender.frameId + " of tab: " + sender.tab.id + " with msg: " + JSON.stringify(msg))

        // // The first tab to send us a content message will be the tab that we pay attention to.
        // if (!this.tab_id) {
        //     this.tab_id = sender.tab.id
        // }

        // Skip messages from tabs that we're not interested in.
        if (this.get_current_tab_id() != sender.tab.id) {
            return
        }

        // Pass the message to the app.
        this.send_to_app(msg);
    }

}



