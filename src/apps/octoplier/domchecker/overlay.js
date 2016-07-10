// This class represents the rectangular overlays that we see overlayed on
// the target web page. These boxes ares used to represent what elements 
// are currently under the mouse as well as what elements have already been
// selected by the user.

var Overlay = function(class_name, page_box=null, color=null, enlarge=0) {
    this.marked = false
    this.page_box = page_box
    this.color = color
    this.enlarge = enlarge 
    
    this.create_dom_elements(class_name)
    this.update_dom_elements()
    this.update_dom_color()
}

// The thickness of the edges of the box overlay.
Overlay.prototype.thickness = 0

//Destroy our dom elements.
Overlay.prototype.destroy = function() {
    // Release the page box
    this.page_box = null
    // Destroy the dom elments.
    document.body.removeChild(this.left)
    document.body.removeChild(this.right)
    document.body.removeChild(this.top)
    document.body.removeChild(this.bottom)
    this.left = null
    this.right = null
    this.top = null
    this.bottom = null
}

Overlay.prototype.move_off_page() {
    this.page_box.left = -99
    this.page_box.right = -99
    this.page_box.top = -99
    this.page_box.bottom = -99
    this.update_dom_elements()
}

//Update the dimensions of this overlay.
Overlay.prototype.update_page_box = function(page_box) {
    // Keep a reference to the page_box.
    this.page_box = page_box
    // Update the dom.
    this.update_dom_elements()
}

//Update the dimensions of this overlay using an element.
Overlay.prototype.update_page_box_from_element = function(element) {
    var client_rect = element.getBoundingClientRect()
    // Keep a reference to the page_box.
    this.page_box.set_from_client_rect(client_rect)
    // Update the dom.
    this.update_dom_elements()
}

//Returns true if this overlay contains a page point.
Overlay.prototype.contains = function(page_x, page_y) {
    return page_box_contains_point(this.page_box, page_x, page_y)
}

//Mark the overlay with visual emphasis.
Overlay.prototype.mark = function() {
    this.marked = true
    this.update_dom_color()
}

//Unmark the overlay from visual emphasis.
Overlay.prototype.unmark = function() {
    this.marked = false
    this.update_dom_color()
}

// ----------------------------------------------------------------------------------
// Private methods.
//----------------------------------------------------------------------------------

//Creates dom elements representing the four edges of the box overlay.
Overlay.prototype.create_dom_elements = function(class_name) {
    this.left = document.createElement("div")
    this.left.classList.add(class_name)
    this.left.style.position = "absolute"
    document.body.appendChild(this.left)
        
    this.right = document.createElement("div")
    this.right.classList.add(class_name)
    this.right.style.position = "absolute"
    document.body.appendChild(this.right)
    
    this.top = document.createElement("div")
    this.top.classList.add(class_name)
    this.top.style.position = "absolute"
    document.body.appendChild(this.top)
    
    this.bottom = document.createElement("div")
    this.bottom.classList.add(class_name)
    this.bottom.style.position = "absolute"
    document.body.appendChild(this.bottom)
}

//Updates the dom elements to reflect new position and size.
Overlay.prototype.update_dom_elements = function() {
    if (!this.page_box) {
        return
    }
    
    var width = this.page_box.right - this.page_box.left
    var height = this.page_box.bottom - this.page_box.top
    var t = this.thickness
    
    this.left.style.left = (this.page_box.left-this.enlarge-t)+'px'
    this.left.style.top = (this.page_box.top-this.enlarge-t)+'px'
    this.left.style.width = t+'px'
    this.left.style.height = (height+2*this.enlarge+2*t)+'px'
    
    this.right.style.left = (this.page_box.right+this.enlarge)+'px'
    this.right.style.top = (this.page_box.top-this.enlarge-t)+'px'
    this.right.style.width = t+'px'
    this.right.style.height = (height+2*this.enlarge+2*t)+'px'
    
    this.top.style.left = (this.page_box.left-this.enlarge-t)+'px'
    this.top.style.top = (this.page_box.top-this.enlarge-t)+'px'
    this.top.style.width = (width+2*this.enlarge+2*t)+'px'
    this.top.style.height = t+'px'
    
    this.bottom.style.left = (this.page_box.left-this.enlarge-t)+'px'
    this.bottom.style.top = this.page_box.bottom+this.enlarge+ 'px'
    this.bottom.style.width = (width+2*this.enlarge+2*t)+'px'
    this.bottom.style.height = t+'px'
}

//Updates the css style of the dom elements to reflect new color.
Overlay.prototype.update_dom_color = function() {
    if (this.marked) {
        this.left.style.outline = "5px solid " + this.color
        this.right.style.outline = "5px solid " + this.color
        this.top.style.outline = "5px solid " + this.color
        this.bottom.style.outline = "5px solid " + this.color
    }
    else if (this.color) {
        this.left.style.outline = "2px solid " + this.color
        this.right.style.outline = "2px solid " + this.color
        this.top.style.outline = "2px solid " + this.color
        this.bottom.style.outline = "2px solid " + this.color
        return
    }
}




