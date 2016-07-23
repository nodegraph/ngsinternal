

//Class which handles communication between:
//1) this content script and the background script.
var ContentCommunication = function () {
    this.connect_to_bg()
}

//Setup communication channel with chrome runtime.
ContentCommunication.prototype.connect_to_bg = function() {
    chrome.runtime.onMessage.addListener(this.receive_message_from_bg.bind(this))
}

//Send a message to the bg script.
ContentCommunication.prototype.send_message_to_bg = function(socket_message) {
    chrome.runtime.sendMessage(socket_message)
}

//Receive a message from the bg script.
ContentCommunication.prototype.receive_message_from_bg = function(request, sender, send_response) {
    console.log('content script received message from bg: ' + JSON.stringify(request))
    switch(request.request) {
        case 'create_set_from_match_values':
            switch(request.wrap_type) {
                case ElemWrap.prototype.wrap_type.text:
                    var elem_wraps = g_page_wrap.get_elem_wraps_by_text_values(request.match_values)
                    console.log('found num elem_wraps: ' + elem_wraps.length)
                    for (var i=0; i<elem_wraps.length; i++) {
                        console.log('xpath ['+i+']: ' + elem_wraps[i].get_xpath())
                        var elem_cache = new ElemCache(elem_wraps[i])
                        console.log('scrolls: ' + elem_cache.scroll_amounts)
                    }
                    g_overlay_sets.add_set(new OverlaySet(elem_wraps))
                    break
                case ElemWrap.prototype.wrap_type.image:
                    var found = g_page_wrap.get_elem_wraps_by_image_values(request.match_values)
                    console.log('found num found: ' + found.length)
                    for (var i=0; i<found.length; i++) {
                        console.log('xpath ['+i+']: ' + found[i].get_xpath())
                    }
                    g_overlay_sets.add_set(new OverlaySet(found))
                    break
            }
            break
        case 'create_set_from_wrap_type':
            var elem_wraps = g_page_wrap.get_elem_wraps_from_wrap_type(request.wrap_type)
            g_overlay_sets.add_set(new OverlaySet(elem_wraps))
            break
        case 'delete_set':
            g_overlay_sets.destroy_set_by_index(request.set_index)
            break
        case 'shift_set':
            g_overlay_sets.shift(request.set_index, request.direction, request.wrap_type)
            break
        case 'expand_set':
            var match_criteria = new MatchCriteria(request.match_criteria)
            g_overlay_sets.expand(request.set_index, request.direction, match_criteria)
            break
        case 'mark_set':
            g_overlay_sets.mark_set(request.set_index)
            break
        case 'unmark_set':
            g_overlay_sets.unmark_set(request.set_index)
            break
        case 'merge_marked_sets':
            g_overlay_sets.merge_marked_sets()
            break
        case 'shrink_set_to_marked':
            g_overlay_sets.shrink_set_to_marked(request.set_index, request.directions)
            break
        case 'shrink_set':
            g_overlay_sets.shrink_to_extreme(request.set_index, request.direction)
            break
        case 'perform_action':
            // The perform action request are usually handled by the nodejs controller.
            // However then can get send to this content script if the xpath in the request
            // has not been resolved. Once we resolve the xpath from the set_index, we reissue
            // the request, instead of sending a response. This means that these perform_action
            // requests will ultimately get sent twice from the app.
            
            // Add the xpath into the request, and re-issue it.
            var xpath = g_overlay_sets.get_xpath(request.set_index, request.overlay_index)
            request.xpath = xpath
            
            // We also unblock events to allow a webdriver to perform an action on an element at the above xpath.
            g_event_blocker.unblock_events()
            
            // Send the modified request back to the app.
            this.send_message_to_bg(request)
            return 
        case 'block_events':
            g_event_blocker.block_events()
            // No respone message is used for this request, as it procedurally added in to
            // handle unblocking events before performing webdriver actions, and then blocking events back again.
            return
//        case 'get_xpath_from_overlay':
//            var xpath = g_overlay_sets.get_xpath(request.set_index, request.overlay_index)
//            var response = {response: xpath}
//            this.send_message_to_bg(response)
//            return
//        case 'get_xpath_from_elem_cache':
//            var page_box = new PageBox(request.bounds)
//            var elem_cache = new ElemCache(page_box, request.scroll_amounts)
//            var elem_wrap = elem_cache.get_elem_wrap()
//            var xpath = elem_wrap.get_xpath()
//            // Send the result in a response message.
//            var response = {response: xpath}
//            this.send_message_to_bg(response)
//            return
    }
    this.send_message_to_bg(new ResponseMessage(true))
}


//------------------------------------------------------------------------------------------------
//Globals.
//------------------------------------------------------------------------------------------------

var g_content_comm=null

//We only create on the top window, not in other iframes.
if (window == window.top) {
    g_content_comm = new ContentCommunication()
}















