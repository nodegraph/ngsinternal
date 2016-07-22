var PopupDialog = function() {
    // Our dom elements.
    this.popup_hider = null // This is the topmost element.
    this.popup = null
    this.label = null
    this.input = null
    this.ok = null
    this.cancel = null
    
    // Wait mode dom elements.
    this.wait_popup = null
    
    // Our interaction state.
    this.cancelled = false // Is true if the last user interaction cancelled the popup.
    this.callback = null // A callback which is called with the users input text when the "ok" button is pressed.
}

PopupDialog.prototype.initialize = function() {
    // The top div.
    this.popup_hider = document.createElement('div')
    this.popup_hider.id = 'smash_browse_popup_hider'
    document.body.appendChild(this.popup_hider)
    
    // Fill the popup_hider with the popup contents.
    this.popup_hider.innerHTML = '\
    <div id="smash_browse_popup">\
    <center>\
    <p id="smash_browse_popup_label" class="smash_browse_text">Enter URL</p>\
    <input id="smash_browse_popup_input" class="smash_browse_text_input smash_browse_text" type="text" value="">\
    <p>\
        <input type="button" id="smash_browse_popup_ok" class="smash_browse_button" value="ok">\
        <input type="button" id="smash_browse_popup_cancel" class="smash_browse_button" value="cancel">\
    </p>\
    </center>\
    </div>'
    
    // Cache references to some elements.
    this.popup = document.getElementById("smash_browse_popup");
    this.label = document.getElementById("smash_browse_popup_label");
    this.input = document.getElementById("smash_browse_popup_input");
    this.ok = document.getElementById("smash_browse_popup_ok");
    this.cancel = document.getElementById("smash_browse_popup_cancel");
    
//    var style = 'font: normal 18px arial'
//    this.label.style = style
//    this.input.style = style
//    this.ok.style = style
//    this.cancel.style = style
    
    // Hook up listeners.
    this.ok.addEventListener("click", this.on_ok.bind(this), false)
    this.cancel.addEventListener("click", this.on_cancel.bind(this), false)
    
    // Our wait mode popup.
    var hourglass_url = chrome.extension.getURL("images/hourglass.gif"); 
    this.wait_popup = document.createElement('div')
    this.wait_popup.id="smash_browse_popup_hide"
    this.wait_popup.innerHTML = '<center>\
        <p class="smash_browse_label_text">Please wait for page to finish loading.</p>\
        <img src="' + hourglass_url + '" alt="www.smashbrowse.com" style="width:120px;height:120px;" />\
        </center>'
}

PopupDialog.prototype.open_wait_mode = function() {
    this.popup_hider.removeChild(this.popup)
    this.popup_hider.appendChild(this.wait_popup)
    this.wait_popup.id="smash_browse_popup"
    this.popup.id="" // remove id
    this.open(null)
}

PopupDialog.prototype.close_wait_mode = function() {
    this.popup_hider.removeChild(this.wait_popup)
    this.popup_hider.appendChild(this.popup)
    this.popup.id="smash_browse_popup"
    this.wait_popup.id="" // remove id
    this.close()
}

PopupDialog.prototype.contains_element = function(element) {
    if (!this.popup_hider) {
        return false
    }
    if (this.popup_hider.contains(element)) {
        return true
    }
    return false
}

PopupDialog.prototype.set_label_text = function(text) {
    this.label.innerHTML = text
}

//This is used to set the default text value before opening the popup.
PopupDialog.prototype.set_value = function(value) {
    this.input.value = value
}

//Clear the value to empty text.
PopupDialog.prototype.clear_value = function(value) {
    this.input.value = ""
}

//Retrieves the last text value in the popup.
PopupDialog.prototype.get_value = function(text) {
    return this.input.value
}

//Returns true if the last popup interaction was cancelled.
PopupDialog.prototype.was_cancelled = function() {
    return this.cancelled
}

//Opens the popup.
PopupDialog.prototype.open = function(callback) {
    // Cache the callback
    this.callback = callback
    
    // Clear our current value.
    this.clear_value()
    
    // Change style to dim the background.
    this.popup_hider.className = "smash_browse_page_overlay";
    
    if (this.popup.id == 'smash_browse_popup') {
        this.position_popup(this.popup)
    } else {
        this.position_popup(this.wait_popup)
    }
};

PopupDialog.prototype.position_popup = function(popup) {
    // Determine our height.
    var overflow = popup.offsetHeight - g_page_wrap.get_height();
    if(overflow > 0) {
        popup.style.maxHeight = (parseInt(window.getComputedStyle(popup).height) - overflow) + "px";
    }
    // Determine our margins.
    popup.style.marginTop = (-popup.offsetHeight)/2 + "px";
    popup.style.marginLeft = (-popup.offsetWidth)/2 + "px";
}

PopupDialog.prototype.close = function() {
    this.popup_hider.className = "";
};

PopupDialog.prototype.on_ok = function() {
    this.cancelled = false
    this.close()
    // Call the callback.
    if (this.callback) {
        this.callback(this.get_value())
    }
};

PopupDialog.prototype.on_cancel = function() {
    this.clear_value()
    this.cancelled = true
    this.close()
};
    
var g_popup_dialog = new PopupDialog()
        