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

BrowserWrap.prototype.clear_browser_cookies = function(callback) {
    chrome.browsingData.removeCookies({
        "since": 1
    }, callback);
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