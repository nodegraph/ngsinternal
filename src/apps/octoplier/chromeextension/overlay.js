// This class represents the rectangular overlays that we see overlayed on
// the target web page. These boxes ares used to represent what elements 
// are currently under the mouse as well as what elements have already been
// selected by the user.

var Overlay = function(class_name, color, color_index, mark = false, elem_wrap=null) {
    //Primary roperty.
    this.elem_wrap = elem_wrap // Note the elem wrap is allowed to be null as well.
    
    // Our dom elements.
    this.left = null
    this.right = null
    this.top = null
    this.bottom = null
    this.marker = null
    
    // Setup.
    this.create_dom_elements(class_name)
    this.update_dom_elements(color, color_index)
    this.mark(mark)
}

// The thickness of the edges of the box overlay.
Overlay.prototype.thickness = 0

// Amount to oversize the box around the dom element.
Overlay.prototype.enlarge = 0

// Update all internal state.
Overlay.prototype.update = function(color, color_index, mark) {
    if (this.elem_wrap) {
        this.elem_wrap.update()
    }
    this.update_dom_elements(color, color_index, mark)
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
    document.body.removeChild(this.marker)
    // Set our dom references to null.
    this.left = null
    this.right = null
    this.top = null
    this.bottom = null
    this.marker = null
}

Overlay.prototype.contains_point = function(page_x, page_y) {
    return this.elem_wrap.contains_point(page_x, page_y)
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
    
    this.marker = document.createElement("div")
    this.marker.classList.add(class_name)
    this.marker.style.position = "absolute"
    document.body.appendChild(this.marker)
}

//Updates the dom elements to reflect new position and size.
Overlay.prototype.update_dom_elements = function(color, color_index) {    
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
        
        this.marker.style.left = '0px'
        this.marker.style.top = '0px'
        this.marker.style.width = '0px'
        this.marker.style.height = '0px'
        return
    }
    
    // Otherwise if we have an elem wrap.
    
    // Set the color of the dom elements.
    this.left.style.outlineColor = color
    this.right.style.outlineColor = color
    this.top.style.outlineColor = color
    this.bottom.style.outlineColor = color
    this.marker.style.outlineColor = color
    
    // Set the geometry of the dom elements.
    var page_box = this.elem_wrap.page_box
    var width = page_box.right - page_box.left
    var height = page_box.bottom - page_box.top
    var t = this.thickness
    
    // Left.
    this.left.style.left = (page_box.left-this.enlarge-t)+'px'
    this.left.style.top = (page_box.top-this.enlarge-t)+'px'
    this.left.style.width = t+'px'
    this.left.style.height = (height+2*this.enlarge+2*t)+'px'
    
    // Right.
    this.right.style.left = (page_box.right+this.enlarge)+'px'
    this.right.style.top = (page_box.top-this.enlarge-t)+'px'
    this.right.style.width = t+'px'
    this.right.style.height = (height+2*this.enlarge+2*t)+'px'
    
    // Top.
    this.top.style.left = (page_box.left-this.enlarge-t)+'px'
    this.top.style.top = (page_box.top-this.enlarge-t)+'px'
    this.top.style.width = (width+2*this.enlarge+2*t)+'px'
    this.top.style.height = t+'px'
    
    // Bottom.
    this.bottom.style.left = (page_box.left-this.enlarge-t)+'px'
    this.bottom.style.top = page_box.bottom+this.enlarge+ 'px'
    this.bottom.style.width = (width+2*this.enlarge+2*t)+'px'
    this.bottom.style.height = t+'px'
    
    // Mark.
    this.marker.style.left = (page_box.left-this.enlarge-t + (color_index+1)*8)+'px'
    this.marker.style.top = (page_box.top-this.enlarge-t)+'px'
    this.marker.style.width = t+'px'
    this.marker.style.height = (height+2*this.enlarge+2*t)+'px'
}

//Updates the css style of the dom elements to reflect new color.
Overlay.prototype.mark = function(mark) {
    if (mark) {
        this.marker.style.outlineWidth = "4px"
    } else {
        this.marker.style.outlineWidth = "2px"
    }
}



