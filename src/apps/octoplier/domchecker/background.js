

function getword(info,tab) {
    console.log("Word " + info.selectionText + " was clicked.");
    chrome.tabs.create({  
      url: "http://www.google.com/search?q=" + info.selectionText,
    });           
  }

var options = {
        title: "Test1", 
        contexts: ['all'],
        onclick: getword,
}

chrome.contextMenus.removeAll();

var context_menu_item_id = chrome.contextMenus.create(options);