//----------------------------------------------------------------------------------------
//Arrays.
//----------------------------------------------------------------------------------------

//Returns true if the contents of two arrays of primitives are equal.
function arrays_are_equal(a1: any[], a2: any[]): boolean {
    if (a1.length != a2.length) {
        return false
    }
    for (var i=0; i<a1.length; i++) {
        if (a1[i] != a2[i]) {
            return false
        }
    }
    return true
}

function find_in_array_by_property(array: any[], attr: string, value: any): number {
    for(var i=0; i<array.length; i++) {
        if(array[i][attr] === value) {
            return i;
        }
    }
    return -1
}

function value_is_more_extreme(value: number, extreme: number, side: Direction) {
    switch(side) {
        case Direction.left:
            if (value < extreme) {
                return true
            }
            break
        case Direction.right:
            if (value > extreme) {
                return true
            }
            break
        case Direction.up:
            if (value < extreme) {
                return true
            }
            break
        case Direction.down:
            if (value > extreme) {
                return true
            }
            break
        default:
            console.log("Error: value_is_more_extreme(value, extreme, side) was given an unknown side argument")
    }
    return false
}

//----------------------------------------------------------------------------------------
//String Processing.
//----------------------------------------------------------------------------------------

//Returns true if the text is composed of all whitespace characters.
function is_all_whitespace(text: string): boolean {
    let regex = /^\s+$/;
    return regex.test(text)
}

function strip_quotes(text: string): string {
    if (text.length == 0) {
        return text
    }
    if ((text[0] == '"') && (text[text.length-1] == '"')){
        return text.slice(1,-1)
    }
    return text
}

//----------------------------------------------------------------------------------------
//Element Transforms.
//----------------------------------------------------------------------------------------

//function get_clickable_element(element) {
//    // Calculate a click point somewhat inside the element.
//    var rect = element.getBoundingClientRect()
//    var offset = Math.min(rect.width, rect.height)/2.0
//    var x = rect.left + offset
//    var y = rect.top + offset
//    // Get the element at that click point.
//    return document.elementFromPoint(x, y)
//}

