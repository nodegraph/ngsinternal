//This class represents a box in page space, not client rect space.
var Box = function(arg = null) {
    if (arg == null) {
        this.reset()
    } else if (Object.getPrototypeOf(arg) === Box.prototype) {
    	// Initializing from another Box.
        this.left = arg.left
        this.right = arg.right
        this.top = arg.top
        this.bottom = arg.bottom
    } else if (Object.getPrototypeOf(arg) === ClientRect.prototype) {
    	// When initializing from a client rect, the box is kept in client space.
        this.left = arg.left
        this.right = arg.right
        this.top = arg.top
        this.bottom = arg.bottom
    } else if (Object.getPrototypeOf(arg) === Array.prototype) {
    	// Initializing from an array.
        this.left = arg[0]
        this.right = arg[1]
        this.top = arg[2]
        this.bottom = arg[3]
    } else if (Object.getPrototypeOf(arg) === Object.prototype) {
    	// Initializing from an object.
        this.left = arg.left
        this.right = arg.right
        this.top = arg.top
        this.bottom = arg.bottom
    } else {
        this.reset()
    } 
}

//Reset the box to the origin (top left) with zero width and height.
Box.prototype.reset = function() {
    this.left = 0
    this.right = 0
    this.top = 0
    this.bottom = 0
}

Box.prototype.get_as_string = function() {
    var msg = this.left + ',' + this.right + ',' + this.top + ',' + this.bottom
    return msg
}

Box.prototype.get_center = function() {
    return new Point(this.get_mid_x(), this.get_mid_y())
}

Box.prototype.get_width = function() {
    return this.right - this.left
}

Box.prototype.get_height = function() {
    return this.bottom - this.top
}

Box.prototype.get_mid_x = function() {
    return (this.left + this.right) / 2.0
}

Box.prototype.get_mid_y = function() {
    return (this.top + this.bottom) /2.0
}

//Assumes we are in page space and converts to client space.
Box.prototype.to_client_space = function() {
	this.left -= window.scrollX
	this.right -= window.scrollX
	this.top -= window.scrollY
	this.top -= windows.scrollY
}

// Assumes we are in client space and convert to page space.
Box.prototype.to_page_space = function() {
	this.left += window.scrollX
	this.right += window.scrollX
	this.top += window.scrollY
	this.bottom += window.scrollY
}

//Returns true if and only if we contain the inner entirely.
Box.prototype.contains = function(inner) {
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
Box.prototype.contains_point = function(page_pos, sigma = 1.0) {
    if ((page_pos.x >= (this.left-sigma)) && (page_pos.x <= (this.right+sigma)) && 
            (page_pos.y >=(this.top-sigma)) && (page_pos.y <= (this.bottom+sigma))) {
        return true
    }
    return false
}

//Returns true if and only if we intersect the other page box.
Box.prototype.intersects = function(other) {
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
Box.prototype.is_oriented_on = function(side, of) {
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
Box.prototype.get_beam = function(side) {
    var beam = new Box(this)
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
Box.prototype.get_extreme = function(side) {
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
            console.log("Error: Box.get_extreme(side) was given an unknown side argument: " + side)
    }
    return null
}

