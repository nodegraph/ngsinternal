
requirejs.config({
	baseUrl: chrome.extension.getURL("/")
});

requirejs(['content'],
function (contentmain) {
});