//-------------------------------------------------------------------------------------
//Base Popup.
//-------------------------------------------------------------------------------------

var BasePopup = function() {
    // Our dom elements.
    this.popup_hider = null // This is the topmost element.
    this.popup = null
}

BasePopup.prototype.initialize = function() {
    // The top div.
    this.popup_hider = document.createElement('div')
    this.popup_hider.classList.add('smash_browse_popup_hider')
    document.body.appendChild(this.popup_hider)
    
    this.popup = document.createElement('div')
    this.popup.classList.add('smash_browse_popup')
    this.popup_hider.appendChild(this.popup)
}

BasePopup.prototype.open = function() {    
    // Change style to dim the background.
    this.popup_hider.classList.add("smash_browse_page_overlay");
    
    // Position the popup.
    this.position_popup(this.popup)
};

BasePopup.prototype.position_popup = function(popup) {
//    // Determine our height.
//    var overflow = popup.offsetHeight - g_page_wrap.get_height();
//    if(overflow > 0) {
//        popup.style.maxHeight = (parseInt(window.getComputedStyle(popup).height) - overflow) + "px";
//    }
    // Determine our margins.
    popup.style.marginTop = (-popup.offsetHeight)/2 + "px";
    popup.style.marginLeft = (-popup.offsetWidth)/2 + "px";
}

BasePopup.prototype.close = function() {
    this.popup_hider.classList.remove("smash_browse_page_overlay");
};

BasePopup.prototype.contains_element = function(element) {
    if (!this.popup_hider) {
        return false
    }
    if (this.popup_hider.contains(element)) {
        return true
    }
    return false
}

//-------------------------------------------------------------------------------------
//Wait Popup.
//-------------------------------------------------------------------------------------

var WaitPopup = function() {
    BasePopup.call(this)
}

//Setup the prototype.
WaitPopup.prototype = Object.create(BasePopup.prototype);
WaitPopup.prototype.constructor = WaitPopup;


WaitPopup.prototype.initialize = function() {
    BasePopup.prototype.initialize.call(this)
    
    // Fill the popup's contents with a static wait message with a spinning hourglass.
    var hourglass_url = chrome.extension.getURL("images/hourglass.gif"); 
    this.popup.innerHTML = '\
        <center>\
        <p class="smash_browse_label_text">Please wait for page to finish loading.</p>\
        <img src="' + hourglass_url + '" alt="www.smashbrowse.com" style="width:120px;height:120px;" />\
        </center>'
}




//-------------------------------------------------------------------------------------
//Input Popup.
//-------------------------------------------------------------------------------------

var InputPopup = function () {
    // Call base constructor.
    BasePopup.call(this)
    
    // Our dom elements.
    this.label = null
    this.ok = null
    this.cancel = null
    
    // Callback to call with the input value, when ok is pressed.
    this.callback = null
}

//Setup the prototype.
InputPopup.prototype = Object.create(BasePopup.prototype);

InputPopup.prototype.initialize = function() {
    BasePopup.prototype.initialize.call(this)
    
    // Centering element.
    this.center = document.createElement('center')
    this.popup.appendChild(this.center)
    
    // Label element.
    this.label = document.createElement('p')
    this.label.classList.add("smash_browse_label_text")
    this.label.text = "input something"
    this.center.appendChild(this.label)
        
    // Input div element.
    this.input_div = document.createElement("div")
    this.center.appendChild(this.input_div)
    
    // Button group element.
    this.button_group = document.createElement('p')
    this.center.appendChild(this.button_group)
    
    // Ok button.
    this.ok = document.createElement('input')
    this.ok.classList.add('smash_browse_button')
    this.ok.type = 'button'
    this.ok.value = 'ok'
    this.button_group.appendChild(this.ok)
    
    // Cancel button.
    this.cancel = document.createElement('input')
    this.cancel.classList.add('smash_browse_button')
    this.cancel.type = 'button'
    this.cancel.value = 'cancel'
    this.button_group.appendChild(this.cancel)
    
    // This will be created be derived classes.
    this.input = null

    // Hook up listeners.
    this.ok.addEventListener("click", this.on_ok.bind(this), false)
    this.cancel.addEventListener("click", this.on_cancel.bind(this), false)
}

// Note in the inner_html the input element must have an id of "smash_browse_popup_input".
InputPopup.prototype.set_input_element = function(element) {
    this.input = element
    this.input_div.appendChild(this.input)
}

InputPopup.prototype.set_label_text = function(text) {
    this.label.innerHTML = text
}

InputPopup.prototype.clear_value = function() {
    this.input.value = ""
}

InputPopup.prototype.get_value = function() {
    return this.input.value
}

InputPopup.prototype.open = function(callback) {
    // Cache the callback
    this.callback = callback
    
    // Clear our current value.
    this.clear_value()
    
    // Call our base implementation.
    BasePopup.prototype.open.call(this)
}

InputPopup.prototype.on_ok = function() {
    this.close()
    // Call the callback.
    if (this.callback) {
        this.callback(this.get_value())
    }
};

InputPopup.prototype.on_cancel = function() {
    this.clear_value()
    this.close()
};

//-------------------------------------------------------------------------------------
//Select Input Popup.
//-------------------------------------------------------------------------------------

var SelectInputPopup = function() {
    // Call base constructor.
    InputPopup.call(this)
}

//Setup the prototype.
SelectInputPopup.prototype = Object.create(InputPopup.prototype);
SelectInputPopup.prototype.constructor = SelectInputPopup;

SelectInputPopup.prototype.initialize = function() {
    InputPopup.prototype.initialize.call(this)
    
    // Setup the input element.
    var select = document.createElement('select')
    this.set_input_element(select)
}

SelectInputPopup.prototype.set_options = function(option_values, option_texts) {
    // Remove any existing select options.
    for(var i=this.input.options.length-1; i>=0; i--)
    {
        this.input.remove(this.input.options[i]);
    }
    // Add the new select options.
    for (var i=0; i<option_values.length; i++) {
        var option = document.createElement('option');
        option.value = option_values[i];
        option.innerHTML = option_texts[i];
        this.input.appendChild(option);
    }
}

SelectInputPopup.prototype.get_value = function() {
    return this.input.options[this.input.selectedIndex].text
}

//-------------------------------------------------------------------------------------
//Text Input Popup.
//-------------------------------------------------------------------------------------

var TextInputPopup = function() {
    // Call base constructor.
    InputPopup.call(this)
    
    // Our dom elements.
    this.input = null
}

// Setup the prototype.
TextInputPopup.prototype = Object.create(InputPopup.prototype); // See note below
TextInputPopup.prototype.constructor = TextInputPopup;

TextInputPopup.prototype.initialize = function() {
    InputPopup.prototype.initialize.call(this)
    
    // Setup the input element.
    var input = document.createElement('input')
    input.classList.add('smash_browse_text_input')
    input.classList.add('smash_browse_text')
    input.setAttribute('type','text')
    input.setAttribute('value','')
    this.set_input_element(input)
    //this.set_input_html('<input id="smash_browse_popup_input" class="smash_browse_text_input smash_browse_text" type="text" value="">')
}

//-------------------------------------------------------------------------------------
//Globals.
//-------------------------------------------------------------------------------------
    
var g_wait_popup = new WaitPopup()
var g_text_input_popup = new TextInputPopup()
var g_select_input_popup = new SelectInputPopup()


        