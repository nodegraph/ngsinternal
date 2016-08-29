/// <reference path="D:\dev\windows\DefinitelyTyped\chrome\chrome.d.ts"/>

// This class represents the browser.
class BrowserWrap {
    // Dependencies.
    bg_comm: BgComm

    constructor(bg_comm: BgComm) {
        this.bg_comm = bg_comm

        // We handle certain messages from nodejs.
        this.bg_comm.register_nodejs_message_receiver('clear_all_cookies',this.on_clear_all_cookies.bind(this))
        this.bg_comm.register_nodejs_message_receiver('get_all_cookies',this.on_get_all_cookies.bind(this))
        this.bg_comm.register_nodejs_message_receiver('set_all_cookies',this.on_set_all_cookies.bind(this))
        this.bg_comm.register_nodejs_message_receiver('get_zoom',this.on_get_zoom.bind(this))
    }

    on_clear_all_cookies(request: any) {
        function done_clear_all_cookies() {
            let response = { response: true }
            this.bg_comm.send_to_nodejs(response)
        }
        this.clear_all_cookies(done_clear_all_cookies)
    }

    on_get_all_cookies(request: any) {
        function done_get_all_cookies(cookies: chrome.cookies.Cookie[]) {
            let response = { response: true, value: cookies }
            this.bg_comm.send_to_nodejs(response)
        }
        this.get_all_cookies(done_get_all_cookies);
    }

    on_set_all_cookies(request: any) {
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
        this.set_all_cookies(cookies, done_set_all_cookies)
    }

    on_get_zoom(request: any) {
        function done_get_zoom(zoom: number) {
            console.log('zoom is: ' + zoom)
            let response = { response: true, value: zoom }
            this.send_to_nodejs(response)
        }
        this.get_zoom(this.bg_comm.get_tab_id(), done_get_zoom);
    }

    // --------------------------------------------------------------------------------------------------------
    // Lower Level Methods.
    // --------------------------------------------------------------------------------------------------------

    clear_browser_cache(callback: () => void) {
        chrome.browsingData.remove({
            "since": 1
        }, {
                "appcache": true,
                "cache": true,
                "cookies": true,
                "downloads": true,
                "fileSystems": true,
                "formData": true,
                "history": true,
                "indexedDB": true,
                "localStorage": true,
                "pluginData": true,
                "passwords": true,
                "webSQL": true
            }, callback);
    }

    // Callback receives no arguments.
    clear_all_cookies(callback: () => void) {
        chrome.browsingData.removeCookies({
            "since": 1
        }, callback);
    }

    // Callback receives an array of cookies.
    get_all_cookies(callback: (cookies: chrome.cookies.Cookie[]) => void) {
        chrome.cookies.getAll({}, callback);
    }

    // Callback receives the zoom as a number.
    get_zoom(tab_id: number, callback: (zoom: number)=>void) {
        chrome.tabs.getZoom(tab_id, callback);
    }

    // Callback receives zero arguments and is called after each
    // individual cookies is set.
    set_all_cookies(cookies: chrome.cookies.Cookie[], callback: (cookie?: chrome.cookies.Cookie) => void) {
        for (let i = 0; i < cookies.length; i++) {
            let cookie = cookies[i]
            let obj: chrome.cookies.SetDetails
            let url: string = ""
            if (cookie.secure) {
                url = "https://"
            } else {
                url = "http://"
            }
            if (cookie.domain.charAt(0) == '.') {
                url += "www"
            }
            url += cookie.domain
            obj.url = url
            obj.name = cookie.name
            obj.value = cookie.value
            obj.domain = cookie.domain
            obj.path = cookie.path
            obj.secure = cookie.secure
            obj.httpOnly = cookie.httpOnly
            //obj.sameSite = cookie.sameSite
            obj.expirationDate = cookie.expirationDate
            obj.storeId = cookie.storeId
            console.log('setting cookie: ' + JSON.stringify(obj))
            chrome.cookies.set(obj, callback)
        }
    }

    close_other_tabs(tab_id: number) {
        chrome.tabs.query({}, function(tabs: chrome.tabs.Tab[]) {
            for (let i = 0; i < tabs.length; i++) {
                if (tabs[i].id != tab_id) {
                    chrome.tabs.remove(tabs[i].id)
                }
            }
        })
    }
}
