
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

    handle_nodejs_request(req: RequestMessage) {
        console.log('handling request from nodejs: ' + JSON.stringify(req))
        // We intercept certain requests before it gets to the content script,
        // because content scripts can't use the chrome.* APIs except for parts of chrome.extension for message passing.

        // The req.iframe should be "-1" here.
        if (req.iframe != '-1') {
            console.error('bgcomm was expecting msg from with -1 iframe.')
        }

        switch (req.request) {
            case RequestType.kClearAllCookies: {
                function done_clear_all_cookies() {
                    // Send response to nodejs.
                    let response = new ResponseMessage("-1", true)
                    this.bg_comm.send_to_nodejs(response)
                }
                this.browser_wrap.clear_all_cookies(done_clear_all_cookies.bind(this))
            } break
            case RequestType.kGetAllCookies: {
                function done_get_all_cookies(cookies: chrome.cookies.Cookie[]) {
                    // Send response to nodejs.
                    let response = new ResponseMessage("-1", true, cookies)
                    this.bg_comm.send_to_nodejs(response)
                }
                this.browser_wrap.get_all_cookies(done_get_all_cookies.bind(this));
            } break
            case RequestType.kSetAllCookies: {
                let cookies = req.args.cookies
                let count = 0
                function done_set_all_cookies() {
                    count += 1
                    console.log('count is: ' + count)
                    if (count == cookies.length) {
                        // Send response to nodejs.
                        let response = new ResponseMessage("-1", true)
                        this.bg_comm.send_to_nodejs(response)
                    }
                }
                this.browser_wrap.set_all_cookies(cookies, done_set_all_cookies.bind(this))
            } break
            case RequestType.kGetZoom: {
                function done_get_zoom(zoom: number) {
                    // Send response to nodejs.
                    console.log('zoom is: ' + zoom)
                    let response = new ResponseMessage("-1", true, zoom)
                    this.bg_comm.send_to_nodejs(response)
                }
                this.browser_wrap.get_zoom(this.bg_comm.get_tab_id(), done_get_zoom.bind(this));
            } break
            case RequestType.kSwitchIFrame: {
                console.log('bgcomm setting iframe to ' + req.args.iframe)
                this.bg_comm.set_iframe(req.args.iframe)
                // Send response to nodejs.
                let response = new ResponseMessage("-1", true)
                this.bg_comm.send_to_nodejs(response)
            } break
            default:
                // Forward it to the right iframe in the content.
                req.iframe = this.bg_comm.get_iframe()
                this.bg_comm.send_to_content(req)
        }

    }
}