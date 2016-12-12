
interface IframeInfoInterface {
    iframe: string
    left: number
    right: number
    top: number
    bottom: number
    weight?: number
}

class BgCommHandler {
    // Our Dependencies.
    bg_comm: BgComm
    browser_wrap: BrowserWrap

    // Our Members.
    
    // Used when searching for iframes.
    private iframes_found: IframeInfoInterface[]

    // Constructor.
    constructor(bc: BgComm, bw: BrowserWrap) {
        this.bg_comm = bc
        this.browser_wrap = bw
        this.iframes_found = []
    }

    found_iframe(iframe_info: IframeInfoInterface) {
        this.iframes_found.push(iframe_info)
    }

    handle_nodejs_request(req: RequestMessage) {
        //console.log('handling request from nodejs: ' + JSON.stringify(req))
        
        // We intercept certain requests before it gets to the content script,
        // because content scripts can't use the chrome.* APIs except for parts of chrome.extension for message passing.

        // The req.iframe should be "-1" most of the time.
        // However for kGetCrosshairInfo we allow the iframe to be specified.
        if (req.iframe != '-1'  && req.request != RequestType.kGetCrosshairInfo) {
            console.error('Error: bgcomm was expecting msg from nodejs with -1 iframe.')
        }

        switch (req.request) {
            case RequestType.kClearAllCookies: {
                let done_clear_all_cookies = () => {
                    // Send response to nodejs.
                    let response = new ResponseMessage(req.id, "-1", true)
                    this.bg_comm.send_to_nodejs(response)
                }
                BrowserWrap.clear_all_cookies(done_clear_all_cookies)
            } break
            case RequestType.kGetAllCookies: {
                let done_get_all_cookies = (cookies: chrome.cookies.Cookie[]) => {
                    // Send response to nodejs.
                    let response = new ResponseMessage(req.id, "-1", true, cookies)
                    this.bg_comm.send_to_nodejs(response)
                }
                BrowserWrap.get_all_cookies(done_get_all_cookies);
            } break
            case RequestType.kSetAllCookies: {
                let cookies = req.args.cookies
                let count = 0
                let done_set_all_cookies = () => {
                    count += 1
                    if (count == cookies.length) {
                        // Send response to nodejs.
                        let response = new ResponseMessage(req.id, "-1", true)
                        this.bg_comm.send_to_nodejs(response)
                    }
                }
                BrowserWrap.set_all_cookies(cookies, done_set_all_cookies)
            } break
            case RequestType.kGetZoom: {
                let done_get_zoom = (zoom: number) => {
                    // Send response to nodejs.
                    let response = new ResponseMessage(req.id, "-1", true, {'zoom': zoom})
                    this.bg_comm.send_to_nodejs(response)
                }
                BrowserWrap.get_zoom(this.bg_comm.get_tab_id(), done_get_zoom);
            } break
            case RequestType.kGetCrosshairInfo: {
                // If the iframe is specified we allow it go to the specified iframe.
                // However if it's not specified we pick up the iframe from this bg_comm as usual.
                if (req.iframe == '-1') {
                    req.iframe = this.bg_comm.get_iframe()
                }
                this.bg_comm.send_to_content(req)
            } break
            case RequestType.kFindIFrame: {
                // Clear our previous matched iframes.
                this.iframes_found.length = 0
                this.bg_comm.send_to_content(req, (first_return_value: any) =>{
                    // This gets called when all the content scripts in the iframes have finished processing,
                    // because we made sure that none of the listeners return true, and now of them call this as the send_response functor.
                    if (this.iframes_found.length == 1) {
                        let iframe = this.iframes_found[0].iframe
                        let response = new ResponseMessage(req.id, "-1", true, {iframe: iframe})
                        this.bg_comm.send_to_nodejs(response)
                    } else if (this.iframes_found.length > 1) {
                        // Add weights to the iframe info.
                        const max_page_width = 10000
                        const max_iframe_width = 10000
                        for (let i=0; i<this.iframes_found.length; i++) {
                            let info = this.iframes_found[i]
                            let whole_weight = (info.top * max_page_width) + info.left
                            let fraction_weight = (((info.bottom-info.top) * max_iframe_width) + info.right - info.left) / max_iframe_width / max_iframe_width
                            let weight = whole_weight + fraction_weight
                            info.weight = weight
                        }
                        // Sort the weighted frames.
                        this.iframes_found.sort(function(a, b) {
                            return a.weight - b.weight;
                        });
                        // We take the first one this should be the topmost and leftmost iframe.
                        let iframe = this.iframes_found[0].iframe
                        let response = new ResponseMessage(req.id, "-1", true, {iframe: iframe})
                        this.bg_comm.send_to_nodejs(response)
                    } else {
                        let response = new ResponseMessage(req.id, "-1", false, "No matching iframes could be found. Please try using a different criteria.")
                        this.bg_comm.send_to_nodejs(response)
                    }
                })
            } break
            case RequestType.kSwitchIFrame: {
                this.bg_comm.set_iframe(req.args.iframe)
                let response = new ResponseMessage(req.id, "-1", true)
                this.bg_comm.send_to_nodejs(response)
            } break
            case RequestType.kUpdateOveralys: {
                // These cases apply to all iframes. 
                // Each iframe should not send a response because there would be too many responses and this always succeeds.
                req.iframe = this.bg_comm.get_iframe()
                this.bg_comm.send_to_content(req)
                // Instead we send a single reponse back for all iframes.
                let response = new ResponseMessage(req.id, "-1", true)
                this.bg_comm.send_to_nodejs(response)
            } break
            default: {
                // Forward it to the right iframe in the content.
                req.iframe = this.bg_comm.get_iframe()
                this.bg_comm.send_to_content(req)
            } break
        }

    }
}