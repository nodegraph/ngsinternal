//------------------------------------------------------------------------------------------------
//Event Blocking/Unblocking.
//------------------------------------------------------------------------------------------------

// Basically everything except the contextmenu event (right click).
//var event_types = 'message blur change focus focusin focusout hover scroll submit click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave pointerup pointerdown pointerenter pointerleave pointermove pointerover pointerout keydown keypress keyup input'.split(' ');
var event_types = 'message scroll submit click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave pointerup pointerdown pointerenter pointerleave pointermove pointerover pointerout keydown keypress keyup input'.split(' ');
//blur change focus focusin focusout hover message  scroll
//var event_types = ["deviceorientation", "transitionend", "webkittransitionend", "webkitanimationstart", "webkitanimationiteration", "webkitanimationend", "search", "reset", "waiting", "volumechange", "unload", "timeupdate", "suspend", "submit", "storage", "stalled", "select", "seeking", "seeked", "scroll", "resize", "ratechange", "progress", "popstate", "playing", "play", "pause", "pageshow", "pagehide", "online", "offline", "mousewheel", "mouseup", "mouseover", "mouseout", "mousemove", "mousedown", "message", "loadstart", "loadedmetadata", "loadeddata", "load", "keyup", "keypress", "keydown", "invalid", "input", "hashchange", "focus", "error", "ended", "emptied", "durationchange", "drop", "dragstart", "dragover", "dragleave", "dragenter", "dragend", "drag", "dblclick", "contextmenu", "click", "change", "canplaythrough", "canplay", "blur", "beforeunload", "abort"]

function block_event(event) {
    // If the event is inside the context menu, let it through.
    if (event.target && event.target.tagName) {
        if (g_context_menu.visible){
            console.log('node: ' + JSON.stringify(event.target))
            if (g_context_menu.top_menu.contains(event.target)) {
                return true
            }
        }
    }
    // Otherwise we selectively let the event through or block it.
    switch (event.type) {
        case 'click':
            // Click outside a visible context menu will close it.
            if (g_context_menu.visible) {
                g_context_menu.hide()
            } else {
                return true
            }
            break
        case 'mousemove':
            g_context_menu.on_mouse_over(event)
            break
        case 'mouseout':
            break
        case 'scroll':
            return true
        case 'message':
            return true
    }
    // If we get here, then the event will be blocked from further propagation.
    event.stopPropagation();
    event.preventDefault();
    event.stopImmediatePropagation();
    return false;
}

function block_events() {
    event_types.forEach(function (eventType) {
        window.addEventListener(eventType, block_event, true)
    })
}

function unblock_events() {
    EVENT_TYPES.forEach(function (eventType) {
        window.removeEventListener(eventType, block_event, true);
    });
}

block_events()