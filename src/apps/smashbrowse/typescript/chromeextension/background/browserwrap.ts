/// <reference path="D:\dev\windows\DefinitelyTyped\chrome\chrome.d.ts"/>

// This class holds browser related utility methods.
class BrowserWrap {
    // Our Dependencies.
    
    // Our Methods.

    constructor() {
    }

    static clear_browser_cache(callback: () => void) {
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
    static clear_all_cookies(callback: () => void) {
        chrome.browsingData.removeCookies({
            "since": 1
        }, callback);
    }

    // Callback receives an array of cookies.
    static get_all_cookies(callback: (cookies: chrome.cookies.Cookie[]) => void) {
        chrome.cookies.getAll({}, callback);
    }

    // Callback receives the zoom as a number.
    static get_zoom(tab_id: number, callback: (zoom: number)=>void) {
        chrome.tabs.getZoom(tab_id, callback);
    }

    // Callback receives zero arguments and is called after each
    // individual cookies is set.
    static set_all_cookies(cookies: chrome.cookies.Cookie[], callback: (cookie?: chrome.cookies.Cookie) => void) {
        for (let i = 0; i < cookies.length; i++) {
            let cookie = cookies[i]
            let obj: chrome.cookies.SetDetails = {url: ""}
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

    static close_other_tabs(tab_id: number) {
        chrome.tabs.query({}, function(tabs: chrome.tabs.Tab[]) {
            for (let i = 0; i < tabs.length; i++) {
                if (tabs[i].id != tab_id) {
                    chrome.tabs.remove(tabs[i].id)
                }
            }
        })
    }
}
