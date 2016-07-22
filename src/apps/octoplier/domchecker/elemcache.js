//The class represents an elements position and bounds as a serializeable/cacheable object.
//This is used to find elements again when returning back a web page, without having to 
//run all the procedural element finders again.
var ElemCache = function(elem_wrap) {
    if (arguments.length == 0) {
        this.page_box = null
        this.scroll_amounts = null
        this.wrap_type = null
    } else if (arguments.length == 1) {
        // If there's one argument we expect it to be an elem cache.
        var elem_cache = arguments[0]
        // Note the page_box is in page box space, which absolute and agnostic to scrolls.
        this.page_box = new PageBox(elem_wrap.page_box)
        this.scroll_amounts = elem_wrap.get_scroll_amounts()
        this.wrap_type = elem_wrap.get_wrap_type()
    } else if (arguments.length == 3) {
        this.page_box = arguments[0]
        this.scroll_amounts = arguments[1]
        this.wrap_type = arguments[2]
    }
}

//Positions scroll bars into proper position so that the real dom element lines up
//with the recorded page box. After this call, action's can be taken on the page box,
//area of the web page, thereby effectively taking action on the actual element.
ElemCache.prototype.position_scrolls = function() {
    if (this.scroll_amounts.length == 0) {
        return
    }
    
    // Get the elements at the center of our page_box.
    var center = this.page_box.get_center()
    var elem_wraps = g_page_wrap.get_visible_elem_wraps_at(center.x, center.y)
    
    // Note this is not fool proof, but should work in most of the cases.
    // We use the first elem_wrap with the right number of scroll bars that we need to set.
    var scrolls = null
    for (var i=0; i<elem_wraps.length; i++) {
        scrolls = elem_wraps[0].get_scroll_bars()
        if (scrolls.length == this.scroll_amounts.length) {
            break
        }
    }
    
    for (var i=0; i<scrolls.length; i++) {
        scrolls[i].set_scroll_amount(this.scroll_amounts[i])
    }
}

ElemCache.prototype.get_elem_wrap = function() {
    this.position_scrolls()
    var getter = ElemWrap.prototype.get_getter(this.wrap_type)
    var elem_wrap = this.get_first_elem_wrap_at(getter, page_x, page_y)
    return elem_wrap
}