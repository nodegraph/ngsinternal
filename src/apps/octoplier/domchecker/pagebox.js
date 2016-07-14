//This class represents a box in page space, not client rect space.
var PageBox = function(client_rect) {
    if (client_rect) {
        this.set_from_client_rect(client_rect)
    } else {
        this.reset()
    } 
}

//Reset the box to the origin (top left) with zero width and height.
PageBox.prototype.reset = function() {
    this.left = 0
    this.right = 0
    this.top = 0
    this.bottom = 0
}

PageBox.prototype.get_width = function() {
    return this.right - this.left
}

PageBox.prototype.get_height = function() {
    return this.bottom - this.top
}

PageBox.prototype.get_mid_x = function() {
    return (this.left + this.right) / 2.0
}

PageBox.prototype.get_mid_y = function() {
    return (this.top + this.bottom) /2.0
}

//Set the box to match the client rect but in page space.
PageBox.prototype.set_from_client_rect = function(rect) {
    this.left = rect.left + window.scrollX
    this.right = rect.right + window.scrollX
    this.top = rect.top + window.scrollY
    this.bottom = rect.bottom + window.scrollY
}

//Returns true if and only if we contain the inner entirely.
PageBox.prototype.contains = function(inner) {
    if ((inner.left >= this.left) && 
            (inner.right <= this.right) && 
            (inner.top >= this.top) &&
            (inner.bottom <= this.bottom) 
    ) {
        return true
    }
    return false
}

//Returns true if and only if we contain the given page point.
//Due the document.elementsFromPoint() precision on x and y,
//we need to do a containment test with a sigma of 1.0 by default.
PageBox.prototype.contains_point = function(page_x, page_y, sigma = 1.0) {
    if ((page_x >= (this.left-sigma)) && (page_x <= (this.right+sigma)) && 
            (page_y >=(this.top-sigma)) && (page_y <= (this.bottom+sigma))) {
        return true
    }
    return false
}

//Returns true if and only if we intersect the other page box.
PageBox.prototype.intersects = function(other) {
    if (this.right < other.left) {
        return false
    } else if (this.left > other.right) {
        return false
    } else if (this.bottom < other.top) {
        return false
    } else if (this.top > other.bottom) {
        return false
    }
    return true
}
