// This class represents the rectangular overlays that we see overlayed on
// the target web page. These boxes ares used to represent what elements 
// are currently under the mouse as well as what elements have already been
// selected by the user.

var Overlay = function(class_name, color, color_index, elem_wrap=null) {
    this.marked = false
    this.color = color
    this.color_index = color_index
    this.enlarge = 0
    this.elem_wrap = elem_wrap // Note the elem wrap is allowed to be null as well.
    // Setup.
    this.create_dom_elements(class_name)
    this.update_dom_elements()
    this.update_dom_color()
}

// The thickness of the edges of the box overlay.
Overlay.prototype.thickness = 0

// The length of the nub.
Overlay.prototype.nub_pushin = 2

// Update all internal state.
Overlay.prototype.update = function() {
    if (this.elem_wrap) {
        this.elem_wrap.update()
    }
    this.update_dom_elements()
    this.update_dom_color()
}

//Destroy our dom elements.
Overlay.prototype.destroy = function() {
    // Release the elem_wrap
    this.elem_wrap = null
    // Destroy the dom elments.
    document.body.removeChild(this.left)
    document.body.removeChild(this.right)
    document.body.removeChild(this.top)
    document.body.removeChild(this.bottom)
    document.body.removeChild(this.nub)
    // Set our dom references to null.
    this.left = null
    this.right = null
    this.top = null
    this.bottom = null
    this.nub = null
}

//Update the dimensions of this overlay using an element.
Overlay.prototype.update_with_elem_wrap = function(elem_wrap) {
    this.elem_wrap = elem_wrap
    // Update the dom.
    this.update_dom_elements()
}

//Returns true if this overlay contains a page point.
Overlay.prototype.contains = function(page_x, page_y) {
    return this.elem_wrap.contains_page_point(page_x, page_y)
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
    
    this.nub = document.createElement("div")
    this.nub.classList.add(class_name)
    this.nub.style.position = "absolute"
    document.body.appendChild(this.nub)
    
    this.left.style.outlineColor = this.color
    this.right.style.outlineColor = this.color
    this.top.style.outlineColor = this.color
    this.bottom.style.outlineColor = this.color
    this.nub.style.outlineColor = this.color
}

//Updates the dom elements to reflect new position and size.
Overlay.prototype.update_dom_elements = function() {
    if (!this.elem_wrap) {
        this.left.style.left = '0px'
        this.left.style.top = '0px'
        this.left.style.width = '0px'
        this.left.style.height = '0px'
        
        this.right.style.left = '0px'
        this.right.style.top = '0px'
        this.right.style.width = '0px'
        this.right.style.height = '0px'
        
        this.top.style.left = '0px'
        this.top.style.top = '0px'
        this.top.style.width = '0px'
        this.top.style.height = '0px'
        
        this.bottom.style.left = '0px'
        this.bottom.style.top = '0px'
        this.bottom.style.width = '0px'
        this.bottom.style.height = '0px'
        
        this.nub.style.left = '0px'
        this.nub.style.top = '0px'
        this.nub.style.width = '0px'
        this.nub.style.height = '0px'
        return
    }
    
    // Otherwise if we have an elem wrap.
    var page_box = this.elem_wrap.page_box
    var width = page_box.right - page_box.left
    var height = page_box.bottom - page_box.top
    var t = this.thickness
    
    this.left.style.left = (page_box.left-this.enlarge-t)+'px'
    this.left.style.top = (page_box.top-this.enlarge-t)+'px'
    this.left.style.width = t+'px'
    this.left.style.height = (height+2*this.enlarge+2*t)+'px'
    
    this.right.style.left = (page_box.right+this.enlarge)+'px'
    this.right.style.top = (page_box.top-this.enlarge-t)+'px'
    this.right.style.width = t+'px'
    this.right.style.height = (height+2*this.enlarge+2*t)+'px'
    
    this.top.style.left = (page_box.left-this.enlarge-t)+'px'
    this.top.style.top = (page_box.top-this.enlarge-t)+'px'
    this.top.style.width = (width+2*this.enlarge+2*t)+'px'
    this.top.style.height = t+'px'
    
    this.bottom.style.left = (page_box.left-this.enlarge-t)+'px'
    this.bottom.style.top = page_box.bottom+this.enlarge+ 'px'
    this.bottom.style.width = (width+2*this.enlarge+2*t)+'px'
    this.bottom.style.height = t+'px'
    
    this.nub.style.left = (page_box.left-this.enlarge-t + (this.color_index+1)*8)+'px'
    this.nub.style.top = (page_box.top-this.enlarge-t)+'px'
    this.nub.style.width = t+'px'
    this.nub.style.height = (height+2*this.enlarge+2*t)+'px'
}

//Updates the css style of the dom elements to reflect new color.
Overlay.prototype.update_dom_color = function() {
    if (this.marked) {
        this.nub.style.outlineWidth = "4px"
    } else {
        this.nub.style.outlineWidth = "2px"
    }
}




