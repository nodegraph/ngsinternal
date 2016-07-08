// We use a predefined set of distinct colors.
// These were obtained from stack overflow.
// http://stackoverflow.com/questions/470690/how-to-automatically-generate-n-distinct-colors

// This class represents the rectangular overlays that we see overlayed on
// the target web page. These boxes ares used to represent what elements 
// are currently under the mouse as well as what elements have already been
// selected by the user.

var Overlay = function(class_name, page_box) {
    this.page_box = page_box
    this.modify_dom(class_name)
    this.update_dom()
}

Overlay.prototype.thickness = 0
Overlay.prototype.colors = [
                            "#FFB300", // Vivid Yellow
                            "#803E75", // Strong Purple
                            "#FF6800", // Vivid Orange
                            "#A6BDD7", // Very Light Blue
                            "#C10020", // Vivid Red
                            "#CEA262", // Grayish Yellow
                            "#817066", // Medium Gray

                            // The following don't work well for people with defective color vision
                            "#007D34", // Vivid Green
                            "#F6768E", // Strong Purplish Pink
                            "#00538A", // Strong Blue
                            "#FF7A5C", // Strong Yellowish Pink
                            "#53377A", // Strong Violet
                            "#FF8E00", // Vivid Orange Yellow
                            "#B32851", // Strong Purplish Red
                            "#F4C800", // Vivid Greenish Yellow
                            "#7F180D", // Strong Reddish Brown
                            "#93AA00", // Vivid Yellowish Green
                            "#593315", // Deep Yellowish Brown
                            "#F13A13", // Vivid Reddish Orange
                            "#232C16", // Dark Olive Green
                            ]

Overlay.prototype.update = function(page_box) {
    // Keep a reference to the page_box.
    this.page_box = page_box
    // Update the dom.
    this.update_dom()
}

Overlay.prototype.destroy = function() {
    this.page_box = null
    document.body.removeChild(this.left)
    document.body.removeChild(this.right)
    document.body.removeChild(this.top)
    document.body.removeChild(this.bottom)
    this.left = null
    this.right = null
    this.top = null
    this.bottom = null
}

Overlay.prototype.modify_dom = function(class_name) {
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

Overlay.prototype.update_dom = function() {
    if (!this.page_box) {
        return
    }
    
    var width = this.page_box[1] - this.page_box[0]
    var height = this.page_box[3] - this.page_box[2]
    var t = this.thickness
    
    this.left.style.left = (this.page_box[0]-t)+'px'
    this.left.style.top = (this.page_box[2]-t)+'px'
    this.left.style.width = t+'px'
    this.left.style.height = (height+2*t)+'px'
    
    this.right.style.left = (this.page_box[1])+'px'
    this.right.style.top = (this.page_box[2]-t)+'px'
    this.right.style.width = t+'px'
    this.right.style.height = (height+2*t)+'px'
    
    this.top.style.left = (this.page_box[0]-t)+'px'
    this.top.style.top = (this.page_box[2]-t)+'px'
    this.top.style.width = (width+2*t)+'px'
    this.top.style.height = t+'px'
    
    this.bottom.style.left = (this.page_box[0]-t)+'px'
    this.bottom.style.top = this.page_box[3]+'px'
    this.bottom.style.width = (width+2*t)+'px'
    this.bottom.style.height = t+'px'
}

Overlay.prototype.update_color = function(color_index) {
    if (color_index < 0) {
        return
    }
    this.left.style.outline = "1px solid " + this.colors[color_index]
    this.right.style.outline = "1px solid " + this.colors[color_index]
    this.top.style.outline = "1px solid " + this.colors[color_index]
    this.bottom.style.outline = "1px solid " + this.colors[color_index]
}


