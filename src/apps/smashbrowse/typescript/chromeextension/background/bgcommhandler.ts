
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

    handle_nodejs_request(request: any) {
        console.log('handling request from nodejs: ' + JSON.stringify(request))
        // We intercept certain requests before it gets to the content script,
        // because content scripts can't use the chrome.* APIs except for parts of chrome.extension for message passing.
        switch (request.request) {
            case 'clear_all_cookies':
                function done_clear_all_cookies() {
                    let response = { response: true }
                    this.bg_comm.send_to_nodejs(response)
                }
                this.browser_wrap.clear_all_cookies(done_clear_all_cookies.bind(this))
                break
            case 'get_all_cookies':
                function done_get_all_cookies(cookies: chrome.cookies.Cookie[]) {
                    let response = { response: true, value: cookies }
                    this.bg_comm.send_to_nodejs(response)
                }
                this.browser_wrap.get_all_cookies(done_get_all_cookies.bind(this));
                break
            case 'set_all_cookies':
                let cookies = request.cookies
                let count = 0
                function done_set_all_cookies() {
                    count += 1
                    console.log('count is: ' + count)
                    if (count == cookies.length) {
                        let response = { response: true }
                        this.bg_comm.send_to_nodejs(response)
                    }
                }
                this.browser_wrap.set_all_cookies(cookies, done_set_all_cookies.bind(this))
            case 'get_zoom':
                function done_get_zoom(zoom: number) {
                    console.log('zoom is: ' + zoom)
                    let response = { response: true, value: zoom }
                    this.bg_comm.send_to_nodejs(response)
                }
                this.browser_wrap.get_zoom(this.bg_comm.get_tab_id(), done_get_zoom.bind(this));
            default:
                // Forward it to the content.
                this.bg_comm.send_to_content(request)
        }
    }
}