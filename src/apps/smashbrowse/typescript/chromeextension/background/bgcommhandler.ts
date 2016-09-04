//import {BgComm} from "./bgcomm"
//import {BaseMessage, RequestMessage, ResponseMessage, RequestType} from "../../controller/socketmessage"
//import {BrowserWrap} from "./browserwrap"

class BgCommHandler {
    // Our Dependencies.
    bg_comm: BgComm
    browser_wrap: BrowserWrap

    // Our Members.

    // Constructor.
    constructor(bc: BgComm, bw: BrowserWrap) {
        this.bg_comm = bc
        this.browser_wrap = bw
    }

    handle_nodejs_request(request: RequestMessage) {
        console.log('handling request from nodejs: ' + JSON.stringify(request))
        // We intercept certain requests before it gets to the content script,
        // because content scripts can't use the chrome.* APIs except for parts of chrome.extension for message passing.
        switch (request.request) {
            case RequestType.kClearAllCookies:
                function done_clear_all_cookies() {
                    let response = new ResponseMessage(true)
                    this.bg_comm.send_to_nodejs(response)
                }
                this.browser_wrap.clear_all_cookies(done_clear_all_cookies.bind(this))
                break
            case RequestType.kGetAllCookies:
                function done_get_all_cookies(cookies: chrome.cookies.Cookie[]) {
                    let response = new ResponseMessage(true, cookies)
                    this.bg_comm.send_to_nodejs(response)
                }
                this.browser_wrap.get_all_cookies(done_get_all_cookies.bind(this));
                break
            case RequestType.kSetAllCookies:
                let cookies = request.args.cookies
                let count = 0
                function done_set_all_cookies() {
                    count += 1
                    console.log('count is: ' + count)
                    if (count == cookies.length) {
                        let response = new ResponseMessage(true)
                        this.bg_comm.send_to_nodejs(response)
                    }
                }
                this.browser_wrap.set_all_cookies(cookies, done_set_all_cookies.bind(this))
            case RequestType.kGetZoom:
                function done_get_zoom(zoom: number) {
                    console.log('zoom is: ' + zoom)
                    let response = new ResponseMessage(true, zoom)
                    this.bg_comm.send_to_nodejs(response)
                }
                this.browser_wrap.get_zoom(this.bg_comm.get_tab_id(), done_get_zoom.bind(this));
            default:
                // Forward it to the content.
                this.bg_comm.send_to_content(request)
        }
    }
}