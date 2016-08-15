//This class represents the browser.
var BrowserWrap = function () {
}

BrowserWrap.prototype.clear_browser_cache = function(callback) {
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

//Callback receives no arguments.
BrowserWrap.prototype.clear_all_cookies = function(callback) {
    chrome.browsingData.removeCookies({
        "since": 1
    }, callback);
}

//Callback receives an array of cookies.
BrowserWrap.prototype.get_all_cookies = function(callback) {
    chrome.cookies.getAll({}, callback);
}

//Callback receives zero arguments and is called after each
//individual cookies is set.
BrowserWrap.prototype.set_all_cookies = function (cookies, callback) {
    for (var i=0; i<cookies.length; i++) {
        var cookie = cookies[i]
        var obj = {}
        var url = ""
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
        obj.sameSite = cookie.sameSite
        obj.expirationDate = cookie.expirationDate
        obj.storeId = cookie.storeId
        console.log('setting cookie: ' + JSON.stringify(obj))
        chrome.cookies.set(obj, callback)
    }
}

BrowserWrap.prototype.close_other_tabs = function(tab_id) {
    chrome.tabs.getAllInWindow(null, function(tabs) {
        for (var i = 0; i < tabs.length; i++) {
            if (tabs[i].id != tab_id) {
                chrome.tabs.remove(tabs[i].id)
            }
        }
    });
}

var g_browser_wrap = new BrowserWrap()
