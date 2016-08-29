//-------------------------------------------------------------------------------------
//Base Popup.
//-------------------------------------------------------------------------------------

class BasePopup {
    // Our Dependencies

    // Our Members.
    popup_hider: HTMLDivElement
    popup: HTMLDivElement
    
    
    constructor() {
        // Our Dependencies.
        
        // Our dom elements.
        this.popup_hider = null // This is the topmost element.
        this.popup = null
    }

    initialize(): void {
        // The top div.
        this.popup_hider = document.createElement('div')
        this.popup_hider.classList.add('smash_browse_popup_hider')
        document.body.appendChild(this.popup_hider)

        this.popup = document.createElement('div')
        this.popup.classList.add('smash_browse_popup')
        this.popup_hider.appendChild(this.popup)
    }

    open(): void {
        // Change style to dim the background.
        this.popup_hider.classList.add("smash_browse_page_overlay");

        // Position the popup.
        this.position_popup(this.popup)
    };

    position_popup(popup: HTMLDivElement): void {
        // Determine our margins.
        popup.style.marginTop = (-popup.offsetHeight) / 2 + "px";
        popup.style.marginLeft = (-popup.offsetWidth) / 2 + "px";
    }

    close(): void {
        this.popup_hider.classList.remove("smash_browse_page_overlay");
    };

    contains_element(element: Node): boolean {
        if (!this.popup_hider) {
            return false
        }
        if (this.popup_hider.contains(element)) {
            return true
        }
        return false
    }
}




//-------------------------------------------------------------------------------------
//Wait Popup.
//-------------------------------------------------------------------------------------

class WaitPopup extends BasePopup {
    constructor() {
        super()
    }

    initialize(): void {
        super.initialize()

        // Fill the popup's contents with a static wait message with a spinning hourglass.
        let hourglass_url = chrome.extension.getURL("images/hourglass.gif");
        this.popup.innerHTML = '\
        <center>\
        <p class="smash_browse_label_text">Please wait for page to finish loading.</p>\
        <img src="' + hourglass_url + '" alt="www.smashbrowse.com" style="width:120px;height:120px;" />\
        </center>'
    }
}

//-------------------------------------------------------------------------------------
//Input Popup.
//--------------------------------------------------------------------------

class InputPopup extends BasePopup {
    // Layout Elements.
    center: HTMLBlockElement
    label: HTMLParagraphElement
    input_div: HTMLDivElement
    button_group: HTMLParagraphElement
    // Button Elements.
    ok: HTMLInputElement 
    cancel: HTMLInputElement 
    // Result Callback.
    callback: (result: string) => void
    
    constructor() {
        // Call base constructor.
        super()

        // Our dom elements.
        this.label = null
        this.ok = null
        this.cancel = null

        // Callback to call with the input value, when ok is pressed.
        this.callback = null
    }

    initialize(): void {
        super.initialize()

        // Centering element.
        this.center = document.createElement('center')
        this.popup.appendChild(this.center)

        // Label element.
        this.label = document.createElement('p')
        this.label.classList.add("smash_browse_label_text")
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

        // Hook up listeners.
        this.ok.addEventListener("click", this.on_ok.bind(this), false)
        this.cancel.addEventListener("click", this.on_cancel.bind(this), false)
    }

    // Note in the inner_html the input element must have an id of "smash_browse_popup_input".
    set_input_element(element: HTMLElement): void {
        this.input_div.appendChild(element)
    }

    set_label_text(text: string): void {
        this.label.innerHTML = text
    }

    open_with_callback(callback: (result: string) => void): void {
        // Cache the callback
        this.callback = callback

        // Call our base implementation.
        super.open()
    }
    
    // This should be overridden in derived classes.
    get_value(): string {
        return ""
    }

    on_ok(): void {
        this.close()
        // Call the callback.
        if (this.callback) {
            this.callback(this.get_value())
        }
    };

    on_cancel(): void {
        this.close()
    };
}



//-------------------------------------------------------------------------------------
//Select Input Popup.
//-------------------------------------------------------------------------------------

class SelectInputPopup extends InputPopup {
    // The HTMLSelectElement which prompts user for input.
    input: HTMLSelectElement
    constructor() {
        super()
    }

    initialize(): void {
        super.initialize()

        // Setup the input element.
        this.input = document.createElement('select')
        this.set_input_element(this.input)
    }

    set_options(option_values: string[], option_texts: string[]): void {
        // Remove any existing select options.
        for (let i = this.input.options.length - 1; i >= 0; i--) {
            //this.input.remove(this.input.options[i]);
            this.input.remove(i);
        }
        // Add the new select options.
        for (let i = 0; i < option_values.length; i++) {
            let option = document.createElement('option');
            option.value = option_values[i];
            option.innerHTML = option_texts[i];
            this.input.appendChild(option);
        }
    }
    
    get_value(): string {
        let HTMLOptionElement = <HTMLOptionElement>this.input.options.item(this.input.selectedIndex)
        return HTMLOptionElement.text
    }
}



//-------------------------------------------------------------------------------------
//Text Input Popup.
//-------------------------------------------------------------------------------------

class TextInputPopup extends InputPopup {
    // The HTMLInputElement which prompts user for input.
    input: HTMLInputElement
    constructor() {
        super()
        this.input = null
    }
    initialize() {
        super.initialize()

        // Setup the input element.
        this.input = document.createElement('input')
        this.input.classList.add('smash_browse_text_input')
        this.input.classList.add('smash_browse_text')
        this.input.setAttribute('type', 'text')
        this.input.setAttribute('value', '')
        this.set_input_element(this.input)
        //this.set_input_html('<input id="smash_browse_popup_input" class="smash_browse_text_input smash_browse_text" type="text" value="">')
    }
    
    open_with_callback(callback: (result: string) => void): void {
        this.clear_value()
        super.open_with_callback(callback)
    }
    
    clear_value(): void {
        this.input.value = ""
    }

    get_value(): string {
        return this.input.value
    }
}    







        