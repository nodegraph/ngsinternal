
var EventBlocker = function() {
    this.events_are_blocked = false
    this.bound_listener = this.block_event.bind(this)
}

//// left: 37, up: 38, right: 39, down: 40,
//// spacebar: 32, pageup: 33, pagedown: 34, end: 35, home: 36
//var keys = {37: 1, 38: 1, 39: 1, 40: 1};
//
//function preventDefault(e) {
//  e = e || window.event;
//  if (e.preventDefault)
//      e.preventDefault();
//  e.returnValue = false;  
//}

//function preventDefaultForScrollKeys(e) {
//    if (keys[e.keyCode]) {
//        preventDefault(e);
//        return false;
//    }
//}

//function disableScroll() {
//  if (window.addEventListener) // older FF
//      window.addEventListener('DOMMouseScroll', preventDefault, false);
//  window.onwheel = preventDefault; // modern standard
//  window.onmousewheel = document.onmousewheel = preventDefault; // older browsers, IE
//  window.ontouchmove  = preventDefault; // mobile
//  document.onkeydown  = preventDefaultForScrollKeys;
//}
//
//function enableScroll() {
//    if (window.removeEventListener)
//        window.removeEventListener('DOMMouseScroll', preventDefault, false);
//    window.onmousewheel = document.onmousewheel = null; 
//    window.onwheel = null; 
//    window.ontouchmove = null;  
//    document.onkeydown = null;  
//}

//------------------------------------------------------------------------------------------------
//Event Blocking/Unblocking.
//------------------------------------------------------------------------------------------------

// The event that we will block.
EventBlocker.prototype.event_types = [
                                      'message',
                                      'contextmenu', 
                                      'scroll', 
                                      'submit', 
                                      'wheel', 
                                      'click', 
                                      'dblclick', 
                                      'mousedown', 
                                      'mouseup', 
                                      'mousemove', 
                                      'mouseover', 
                                      'mouseout', 
                                      'mouseenter', 
                                      'mouseleave', 
                                      'pointerup', 
                                      'pointerdown', 
                                      'pointerenter', 
                                      'pointerleave', 
                                      'pointermove', 
                                      'pointerover', 
                                      'pointerout', 
                                      'keydown', 
                                      'keypress', 
                                      'keyup', 
                                      'input']

EventBlocker.prototype.block_event = function(event) {
    // If the event is inside our context menu or popup dialog, let it through.
    if (window == window.top) {
        if (event.target && event.target.tagName) {
            if (g_context_menu.contains_element(event.target)) {
                return true
            }
            if (g_popup_dialog.contains_element(event.target)) {
                return true
            }
        }

        // Otherwise we selectively let the event through or block it.
        switch (event.type) {
            case 'contextmenu':
                if (g_context_menu.initialized()) {
                    g_context_menu.on_context_menu(event)
                }
                break
            case 'click':
                // Click outside a visible context menu will close it.
                if (g_context_menu.visible) {
                    g_context_menu.hide()
                } else {
                }
                break
            case 'mousemove':
                g_context_menu.on_mouse_over(event)
                break
            case 'mouseout':
                break
            case 'scroll':
                if (event.taget) {
                    var elem_wrap = new ElemWrap(event.target)
                    console.log('got scroll target: ' + elem_wrap.get_xpath())
                }
                g_overlay_sets.update()
                return true
            case 'message':
                return true
        }
    }
    
    // If we get here, then the event will be blocked from further propagation.
    event.stopPropagation();
    event.preventDefault();
    event.stopImmediatePropagation();
    return false;
}

EventBlocker.prototype.block_events = function() {
    if (this.events_are_blocked) {
        return
    }
    for (var i=0; i<this.event_types.length; i++) {
        window.addEventListener(this.event_types[i], this.bound_listener, true)
    }
    this.events_are_blocked = true
}

EventBlocker.prototype.unblock_events = function() {
    if (!this.events_are_blocked) {
        return
    }
    for (var i=0; i<this.event_types.length; i++) {
        window.removeEventListener(this.event_types[i], this.bound_listener, true);
    }
    this.events_are_blocked = false
}

var g_event_blocker = new EventBlocker();
g_event_blocker.block_events()