
// This class represents a 2D point.
var Point = function() {
    if (arguments.length == 0) {
        // With zero arguments we reset to zero.
        this.reset()
    } else if (arguments.length == 1) {
        // With one argument we expect it to be another Point instance.
        this.x = arguments[0].x
        this.y = arguments[0].y
    } else if (arguments.length == 2) {
        // With two arguments we expect them to be the x and y coordinates.
        this.x = arguments[0]
        this.y = arguments[1]
    }
}

Point.prototype.assign = function(other) {
    this.x = other.x
    this.y = other.y
}

Point.prototype.reset = function(other) {
    this.x = 0
    this.y = 0
}

Point.prototype.increment = function(other) {
    this.x += other.x
    this.y += other.y
}

Point.prototype.decrement = function(other) {
    this.x -= other.x
    this.y -= other.y
}

Point.prototype.add = function(other) {
    return new Point(this.x+other.x, this.y+other.y)
}

Point.prototype.subtract = function(other) {
    return new Point(this.x-other.x, this.y-other.y)
}

Point.prototype.divide_elements = function(factor) {
    this.x /= factor
    this.y /= factor
}

Point.prototype.multiply_elements = function(factor) {
    this.x *= factor
    this.y *= factor
}

Point.prototype.lte = function(other) {
    return (this.x < other.x) && (this.y < other.y)
}

Point.prototype.lte = function(other) {
    return (this.x <= other.x) && (this.y <= other.y)
}

Point.prototype.gte = function(other) {
    return (this.x >= other.x) && (this.y >= other.y)
}

Point.prototype.gt = function(other) {
    return (this.x > other.x) && (this.y > other.y)
}


