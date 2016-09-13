
class EventBlocker {
    // Our Dependencies.
    gui_collection: GUICollection // This is like our owning parent.
    
    // Our Members.
    events_are_blocked: boolean
    bound_listener: EventListener
    
    constructor(gc: GUICollection) {
        this.events_are_blocked = false
        this.bound_listener = this.block_event.bind(this)

        this.gui_collection = gc
        this.block_events()
    }

    // The events that we will block.
    static event_types = [
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

    block_event(event: UIEvent): boolean {
        
        if (event.target && (event.target instanceof Element)) { // event.target.tagName
            if (this.gui_collection.contains_element(<Element>event.target)) {
                return true
            }
        }

        // Otherwise we selectively let the event through or block it.
        switch (event.type) {
            case 'contextmenu':
                this.gui_collection.on_context_menu(<MouseEvent>event)
                break
            case 'click':
                // Click outside a visible context menu will close it.
                break
            case 'mousemove':
                this.gui_collection.on_mouse_over(<MouseEvent>event)
                break
            case 'mouseout':
                break
            case 'scroll':
                this.gui_collection.overlay_sets.update()
                break
            case 'message':
                return true
        }

        // If we get here, then the event will be blocked from further propagation.
        event.stopPropagation();
        event.preventDefault();
        event.stopImmediatePropagation();
        return false;
    }

    block_events(): void {
        if (this.events_are_blocked) {
            return
        }
        for (var i = 0; i < EventBlocker.event_types.length; i++) {
            window.addEventListener(EventBlocker.event_types[i], this.bound_listener, true)
        }
        this.events_are_blocked = true
    }

    unblock_events(): void {
        if (!this.events_are_blocked) {
            return
        }
        for (var i = 0; i < EventBlocker.event_types.length; i++) {
            window.removeEventListener(EventBlocker.event_types[i], this.bound_listener, true);
        }
        this.events_are_blocked = false
    }

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
