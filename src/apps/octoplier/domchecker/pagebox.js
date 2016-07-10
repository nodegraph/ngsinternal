
var PageBox = function(left=0, right=0, top=0, bottom=0) {
    this.left = left
    this.right = right
    this.top = top
    this.bottom = bottom
}

PageBox.prototype.set_from_client_rect = function(rect) {
    this.left = rect.left + window.scrollX
    this.right = rect.right + window.scrollX
    this.top = rect.top + window.scrollY
    this.bottom = rect.bottom + window.scrollY
}

PageBox.prototype.page_box_contains = function(inner) {
    if ((inner.left >= this.left) && 
            (inner.right <= this.right) && 
            (inner.top >= this.top) &&
            (inner.bottom <= this.bottom) 
    ) {
        return true
    }
    return false
}

PageBox.prototype.contains_page_point = function(page_box, page_x, page_y) {
    if ((page_x >= this.left) && (page_x <= this.right) && 
            (page_y >=this.top) && (page_y <= this.bottom)) {
        return true
    }
    return false
}

PageBox.prototype.page_box_intersects = function(other) {
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
