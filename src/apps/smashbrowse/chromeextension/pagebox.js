//This class represents a box in page space, not client rect space.
var PageBox = function(arg = null) {
    if (arg == null) {
        this.reset()
    } else if (Object.getPrototypeOf(arg) === PageBox.prototype) {
        this.left = arg.left
        this.right = arg.right
        this.top = arg.top
        this.bottom = arg.bottom
    } else if (Object.getPrototypeOf(arg) === Array.prototype) {
        this.left = arg[0]
        this.right = arg[1]
        this.top = arg[2]
        this.bottom = arg[3]
    } else if (Object.getPrototypeOf(arg) === Object.prototype) {
        this.left = arg.left
        this.right = arg.right
        this.top = arg.top
        this.bottom = arg.bottom
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

PageBox.prototype.get_as_string = function() {
    var msg = this.left + ',' + this.right + ',' + this.top + ',' + this.bottom
    return msg
}

PageBox.prototype.get_center = function() {
    return new Point(this.get_mid_x(), this.get_mid_y())
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
    if (this.right <= other.left) {
        return false
    } else if (this.left >= other.right) {
        return false
    } else if (this.bottom <= other.top) {
        return false
    } else if (this.top >= other.bottom) {
        return false
    }
    return true
}

//Returns true if this page box is on one side of another page box.
PageBox.prototype.is_oriented_on = function(side, of) {
    switch(side) {
        case ElemWrap.prototype.direction.left:
            if (this.right < of.right) {
                return of.right - this.right
            }
            break
        case ElemWrap.prototype.direction.right:
            if (this.left > of.left) {
                return this.left - of.left
            }
            break
        case ElemWrap.prototype.direction.up:
            if (this.bottom < of.bottom) {
                return of.bottom - this.bottom
            }
            break
        case ElemWrap.prototype.direction.down:
            if (this.top > of.top) {
                return this.top - of.top
            }
            break
    }
    return 0
}

//Returns a beam to one side of us.
PageBox.prototype.get_beam = function(side) {
    var beam = new PageBox(this)
    switch(side) {
        case ElemWrap.prototype.direction.left:
            //beam.right = beam.left
            beam.left = 0
            break
        case ElemWrap.prototype.direction.right:
            //beam.left = beam.right
            beam.right = g_page_wrap.get_width()
            break
        case ElemWrap.prototype.direction.up:
            //beam.bottom = beam.top
            beam.top = 0
            break
        case ElemWrap.prototype.direction.down:
            //beam.top = beam.bottom
            beam.bottom = g_page_wrap.get_height()
            break
        default:
            console.log('Error: get_beam was passed an invalid side argument')
    }
    return beam
}

//Returns a side value.
PageBox.prototype.get_extreme = function(side) {
    switch(side) {
        case ElemWrap.prototype.direction.left:
            return this.left
        case ElemWrap.prototype.direction.right:
            return this.right
        case ElemWrap.prototype.direction.up:
            return this.top
        case ElemWrap.prototype.direction.down:
            return this.bottom
        default:
            console.log("Error: PageBox.get_extreme(side) was given an unknown side argument: " + side)
    }
    return null
}

