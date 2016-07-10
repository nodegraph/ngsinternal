// We use a predefined set of distinct colors.
// These were obtained from stack overflow.
// http://stackoverflow.com/questions/470690/how-to-automatically-generate-n-distinct-colors

var DistinctColors = function () {
    var used = []
}

//A set of distinct colors to indicate separate sets.
DistinctColors.prototype.used_colors = {
        "#FF0000": false, // red
        "#0000FF": false, // blue
        "#00FF00": false, // green
        "#FFFF00": false, // yellow
        "#00FFFF": false, // turqoise
        "#FF00FF": false, // purple
        // The following were obtained from stack overflow.
        "#023FA5": false,
        "#7D87B9": false,
        "#BEC1D4": false,
        "#D6BCC0": false,
        "#BB7784": false,
        "#FFFFFF": false,
        "#4A6FE3": false,
        "#8595E1": false,
        "#B5BBE3": false,
        "#E6AFB9": false,
        "#E07B91": false,
        "#D33F6A": false,
        "#11C638": false,
        "#8DD593": false,
        "#C6DEC7": false,
        "#EAD3C6": false,
        "#F0B98D": false,
        "#EF9708": false,
        "#0FCFC0": false,
        "#9CDED6": false,
        "#D5EAE7": false,
        "#F3E1EB": false,
        "#F6C4E1": false,
        "#F79CD4": false 
}

DistinctColors.prototype.obtain_color = function() {
    var count = 0
    for (var key in this.used_colors) {
        if (this.used_colors.hasOwnProperty(key)) {
            if (this.used_colors[key] == false) {
                this.used_colors[key] = true
                return {color:key,index:count}
            }
            count += 1
        }
    }
    return {}
}

DistinctColors.prototype.request_color = function(color) {
    if (this.used_colors[color] == false) {
        this.used_colors[color] = true
        return true
    }
    return false
}

DistinctColors.prototype.release_color = function(color) {
    this.used_colors[color] = false
}

var g_distinct_colors = new DistinctColors()