// left: 37, up: 38, right: 39, down: 40,
// spacebar: 32, pageup: 33, pagedown: 34, end: 35, home: 36
var keys = {37: 1, 38: 1, 39: 1, 40: 1};

function preventDefault(e) {
  e = e || window.event;
  if (e.preventDefault)
      e.preventDefault();
  e.returnValue = false;  
}

function preventDefaultForScrollKeys(e) {
    if (keys[e.keyCode]) {
        preventDefault(e);
        return false;
    }
}

function disableScroll() {
  if (window.addEventListener) // older FF
      window.addEventListener('DOMMouseScroll', preventDefault, false);
  window.onwheel = preventDefault; // modern standard
  window.onmousewheel = document.onmousewheel = preventDefault; // older browsers, IE
  window.ontouchmove  = preventDefault; // mobile
  document.onkeydown  = preventDefaultForScrollKeys;
}

function enableScroll() {
    if (window.removeEventListener)
        window.removeEventListener('DOMMouseScroll', preventDefault, false);
    window.onmousewheel = document.onmousewheel = null; 
    window.onwheel = null; 
    window.ontouchmove = null;  
    document.onkeydown = null;  
}

//------------------------------------------------------------------------------------------------
//Event Blocking/Unblocking.
//------------------------------------------------------------------------------------------------

// Basically everything except the contextmenu event (right click).
//var event_types = 'message blur change focus focusin focusout hover scroll submit click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave pointerup pointerdown pointerenter pointerleave pointermove pointerover pointerout keydown keypress keyup input'.split(' ');
var event_types = 'message contextmenu scroll submit wheel click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave pointerup pointerdown pointerenter pointerleave pointermove pointerover pointerout keydown keypress keyup input'.split(' ');
//blur change focus focusin focusout hover message  scroll
//var event_types = ["deviceorientation", "transitionend", "webkittransitionend", "webkitanimationstart", "webkitanimationiteration", "webkitanimationend", "search", "reset", "waiting", "volumechange", "unload", "timeupdate", "suspend", "submit", "storage", "stalled", "select", "seeking", "seeked", "scroll", "resize", "ratechange", "progress", "popstate", "playing", "play", "pause", "pageshow", "pagehide", "online", "offline", "mousewheel", "mouseup", "mouseover", "mouseout", "mousemove", "mousedown", "message", "loadstart", "loadedmetadata", "loadeddata", "load", "keyup", "keypress", "keydown", "invalid", "input", "hashchange", "focus", "error", "ended", "emptied", "durationchange", "drop", "dragstart", "dragover", "dragleave", "dragenter", "dragend", "drag", "dblclick", "contextmenu", "click", "change", "canplaythrough", "canplay", "blur", "beforeunload", "abort"]

function block_event(event) {
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