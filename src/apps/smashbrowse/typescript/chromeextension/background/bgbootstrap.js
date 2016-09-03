
requirejs.config({
    baseUrl: chrome.extension.getURL("/")
});

requirejs(['background'],
function (background) {
});
