//------------------------------------------------------------------------------------------------
//Smash Browse Context Menu.
//------------------------------------------------------------------------------------------------

//Elements are described by their bounds in page space so that they can be passed back and forth
//between javascript and c++. In the javascript environment they can be easily converted back to elements.
//We call these bounds page boxes.
//page_box == [left, right, top, bottom] == [min_x, max_x, min_y, max_y]

smash_browse_context_menu = {
        top_menu : null,
        on_click_bound : null,
        visible : false,
        page_event : null,
        // Overlay settings.
        overlay_thickness : 0,
        // The selected elements.
        selected_page_boxes : [],
        selected_overlays : [],
        // The mouse.
        mouse_overlay : {},
        initialize : function() {
            if (this.top_menu) {
                return
            }
            // Create the top level menu element.
            this.top_menu = document.createElement("menu");
            this.top_menu.classList.add('smash_browse_menu')
            document.body.appendChild(this.top_menu)
            
            // Bind the click handler to ourself.
            this.on_click_bound = this.on_click.bind(this)
            
            // Create the menu as dom elements.
            this.create_menu(this.top_menu)
            
            // Create our mouse overlays.
            this.text_box_overlay = this.create_box_overlay('smash_browse_text_box')
            this.image_box_overlay = this.create_box_overlay('smash_browse_image_box')
        },
        create_menu : function (top_menu) {
            // The find menu.
            this.find_menu = this.add_sub_menu(top_menu, 'Select elements')
            this.find_by_text = this.add_item(this.find_menu, 'by text')
            this.find_by_image = this.add_item(this.find_menu, 'by image')
            this.find_by_type = this.add_item(this.find_menu, 'by type')
            this.find_by_position = this.add_item(this.find_menu, 'by position')
            this.find_by_xpath = this.add_item(this.find_menu, 'by xpath')

            // The shift menu.
            this.shift_menu = this.add_sub_menu(top_menu, 'Shift selected elements')
            this.shift_down = this.add_item(this.shift_menu, 'down')
            this.shift_up = this.add_item(this.shift_menu, 'up')
            this.shift_left = this.add_item(this.shift_menu, 'left')
            this.shift_right = this.add_item(this.shift_menu, 'right')
            
            // The expand menu.
            this.expand_menu = this.add_sub_menu(top_menu, 'Expand selected to similar elements')
            this.expand_above = this.add_item(this.expand_menu, 'above us')
            this.expand_below = this.add_item(this.expand_menu, 'below us')
            this.expand_above_and_below = this.add_item(this.expand_menu, 'above and below us')
            this.expand_left = this.add_item(this.expand_menu, 'to the left of us')
            this.expand_right = this.add_item(this.expand_menu, 'to the right of us')
            this.expand_left_and_right = this.add_item(this.expand_menu, 'to the left and right of us')

            // The constrain menu.
            this.constrain_menu = this.add_sub_menu(top_menu, 'Constrain selected to closest elements' )
            this.constrain_above = this.add_item(this.constrain_menu, 'above another element set')
            this.constrain_below = this.add_item(this.constrain_menu, 'below another element set')
            this.constrain_above_and_below = this.add_item(this.constrain_menu, 'above and below another element set')
            this.constrain_left = this.add_item(this.constrain_menu, 'left of another element set')
            this.constrain_right = this.add_item(this.constrain_menu, 'right of another element set')
            this.constrain_left_and_right = this.add_item(this.constrain_menu, 'left and right of another element set')
            
            // Act on element.
            this.perform_menu = this.add_sub_menu(this.top_menu, 'Perform action on element')
            this.perform_click = this.add_item(this.perform_menu, 'click')
            this.perform_type = this.add_item(this.perform_menu, 'type text')
            this.perform_enter = this.add_item(this.perform_menu, 'type enter/submit')
            this.perform_extract = this.add_item(this.perform_menu, 'extract text')
            
            // Spacer.
            this.spacer = this.add_spacer(this.top_menu)
            
            // Cancel.
            this.cancel = this.add_item(this.top_menu, 'Cancel')
        },
        add_item : function(menu, item_text) {
            var li = document.createElement('li')
            li.classList.add('smash_browse_menu_item')
            menu.appendChild(li)
            
            var button = document.createElement('button')
            button.classList.add('smash_browse_menu_button')
            button.type = 'button'
            li.appendChild(button)
            
            var span = document.createElement('span')
            span.classList.add('smash_browse_menu_text')
            button.appendChild(span)
            
            var text = document.createTextNode(item_text)
            span.appendChild(text)
            return li
        },
        add_sub_menu : function(menu, item_text) {
            var li = document.createElement('li')
            li.classList.add('smash_browse_menu_item')
            li.classList.add('smash_browse_submenu')
            this.top_menu.appendChild(li)
            
            var button = document.createElement('button')
            button.classList.add('smash_browse_menu_button')
            button.type = 'button'
            li.appendChild(button)
            
            var span = document.createElement('span')
            span.classList.add('smash_browse_menu_text')
            button.appendChild(span)
            
            var text = document.createTextNode(item_text)
            span.appendChild(text)
            
            var menu = document.createElement("menu");
            menu.classList.add('smash_browse_menu')
            li.appendChild(menu)
            return menu
        },
        remove_sub_menu : function(menu, sub_menu) {
            menu.removeChild(sub_menu)
        },
        add_spacer : function(menu) {
            var li = document.createElement('li')
            li.classList.add('smash_browse_menu_separator')
            menu.appendChild(li)
            return li
        },
        disable_item : function(item) {
            item.classList.add('smash_browse_disabled')
        },
        enable_item : function(item) {
            item.classList.remove('smash_browse_disabled')
        },
        show : function(x,y) {
            this.top_menu.style.left = x + 'px'
            this.top_menu.style.top = y + 'px'
            this.top_menu.classList.add('smash_browse_menu_show')
            this.visible = true
        },
        hide : function(x,y) {
            this.top_menu.classList.remove('smash_browse_menu_show')
            this.visible = false
        },
        element_is_part_of_menu : function(element) {
            if (this.top_menu.contains(element)) {
                return true
            }
            return false
        },
        set_selected_elements(elements) {
            var page_boxes = []
            for (var i=0; i<elements.length; i++) {
                var element = elements[i]
                var rect = element.getBoundingClientRect()
                page_boxes.push(get_page_box_from_client_rect(rect))
            }
            this.set_selected_page_boxes(page_boxes)
        },
        set_selected_page_boxes(page_boxes) {
            this.destroy_selected_overlays()
            this.selected_page_boxes = page_boxes
            this.create_selected_overlays()
        },
        on_context_menu: function(page_event) {
            this.page_x = page_event.pageX
            this.page_y = page_event.pageY
            this.text_element = get_first_text_element_at_point(this.page_x, this.page_y)
            this.image_element = get_first_image_element_at_point(this.page_x, this.page_y)
            
            console.log('mouse over text: ' + get_xpath(this.text_element))
            console.log('mouse over image: ' + get_xpath(this.image_element))
            
            // Show the menu.
            this.show(page_event.pageX, page_event.pageY)
            
            // Listen to clicks to perform the according action and close the menu.
            document.addEventListener('click', this.on_click_bound, true);
            
            page_event.preventDefault();
            return false
        },
        on_click: function(menu_event) {
            this.hide();
            var menu_target = menu_event.target
            console.log('clicked')
            
            if (this.find_by_text.contains(menu_target)) { 
                var text_values = get_text_values_from_point(this.page_x, this.page_y)
                console.log('element xpath: ' + get_xpath(this.text_element))
                console.log('text values: ' + JSON.stringify(text_values))
                // Check that we can find it too.
                var elements = find_elements_by_text_values(text_values)
                console.log('found num elements: ' + elements.length)
                for (var i=0; i<elements.length; i++) {
                    console.log('xpath ['+i+']: ' + get_xpath(elements[i]))
                }
                
            } else if (this.find_by_image.contains(menu_target)) {
                var image_values = get_image_values_from_point(this.page_x, this.page_y)
                console.log('element xpath: ' + get_xpath(this.image_element))
                console.log('image values: ' + JSON.stringify(image_values))
                // Check that we can find it too.
                var elements = find_elements_by_image_values(image_values)
                console.log('found num elements: ' + elements.length)
                for (var i=0; i<elements.length; i++) {
                    console.log('xpath ['+i+']: ' + get_xpath(elements[i]))
                }
            } else if (this.find_by_type.contains(menu_target)) {
                var msg = "Find elements with type, " + this.type
                console.log(msg)
            } else if (this.find_by_position.contains(menu_target)) {
                var msg = "Find elements with position, " + this.position
                console.log(msg)
            }
            document.removeEventListener('click', this.on_click_bound, true);
            
            // Remove previous overlays.
            //this.destroy_selected_overlays()
            
            // Create current overlays.
            //this.create_selected_overlays()
        },
        on_mouse_over: function(page_event) {
            console.log('mouse over')
            var text_element2 = get_first_text_element_at_point(page_event.pageX, page_event.pageY)
            var image_element2 = get_first_image_element_at_point(page_event.pageX, page_event.pageY)
            console.log('text element: ' + get_xpath(text_element2))
            console.log('image element: ' + get_xpath(image_element2))
            this.update_text_box_overlay(text_element2)
            this.update_image_box_overlay(image_element2)
        },
        create_box_overlay: function(class_name) {
            var left = document.createElement("div")
            left.classList.add(class_name)
            left.style.position = "absolute"
            document.body.appendChild(left)
                
            var right = document.createElement("div")
            right.classList.add(class_name)
            right.style.position = "absolute"
            document.body.appendChild(right)
            
            var top = document.createElement("div")
            top.classList.add(class_name)
            top.style.position = "absolute"
            document.body.appendChild(top)
            
            var bottom = document.createElement("div")
            bottom.classList.add(class_name)
            bottom.style.position = "absolute"
            document.body.appendChild(bottom)
            
            return {
                left: left,
                right: right,
                top: top,
                bottom: bottom
            }
        },
        update_overlay: function(overlay, page_box) {
            var width = page_box[1] - page_box[0]
            var height = page_box[3] - page_box[2]
            var left = overlay.left
            var right = overlay.right
            var top = overlay.top
            var bottom = overlay.bottom
            var t = this.overlay_thickness
            
            left.style.left = (page_box[0]-t)+'px'
            left.style.top = (page_box[2]-t)+'px'
            left.style.width = t+'px'
            left.style.height = (height+2*t)+'px'
            
            right.style.left = (page_box[1])+'px'
            right.style.top = (page_box[2]-t)+'px'
            right.style.width = t+'px'
            right.style.height = (height+2*t)+'px'
            
            top.style.left = (page_box[0]-t)+'px'
            top.style.top = (page_box[2]-t)+'px'
            top.style.width = (width+2*t)+'px'
            top.style.height = t+'px'
            
            bottom.style.left = (page_box[0]-t)+'px'
            bottom.style.top = page_box[3]+'px'
            bottom.style.width = (width+2*t)+'px'
            bottom.style.height = t+'px'
        },
        update_text_box_overlay: function(element) {
            if (!element) {
                var page_box = [-99, -99, -99, -99]
                this.update_overlay(this.text_box_overlay, page_box)
            } else {
                console.log('updating text box')
                var client_rect = element.getBoundingClientRect()
                var page_box = get_page_box_from_client_rect(client_rect)
                this.update_overlay(this.text_box_overlay, page_box)
            }
        },
        update_image_box_overlay: function(element) {
            if (!element) {
                var page_box = [-99, -99, -99, -99]
                this.update_overlay(this.image_box_overlay, page_box)
            } else {
                console.log('updating image box')
                var client_rect = element.getBoundingClientRect()
                var page_box = get_page_box_from_client_rect(client_rect)
                this.update_overlay(this.image_box_overlay, page_box)
            }
        },
        create_selected_overlays: function() {
            for (var i=0; i<this.selected_page_boxes.length; i++) {
                var page_box = this.selected_page_boxes[i]
                var overlay = this.create_box_overlay('smash_browse_selected_overlay')
                this.update_overlay(overlay, page_box)
                // Track the selected overlays.
                this.selected_overlays.push(overlay)
            }
        },
        destroy_selected_overlays: function() {
            console.log('num overlay divs: ' + this.selected_overlays.length)
            for (var i=0; i<this.selected_overlays.length; i++) {
                var overlay = this.selected_overlays[i]
                document.body.removeChild(overlay.left)
                document.body.removeChild(overlay.right)
                document.body.removeChild(overlay.top)
                document.body.removeChild(overlay.bottom)
            }
            this.selected_overlays.length = 0
        }
}



document.addEventListener ('DOMContentLoaded', on_loaded, false);
function on_loaded () {
    console.log("document is loaded!")
    smash_browse_context_menu.initialize()
    window.document.addEventListener("contextmenu", smash_browse_context_menu.on_context_menu.bind(smash_browse_context_menu), true)
    
    disable_hover()
}

//------------------------------------------------------------------------------------------------
//Event Blocking/Unblocking.
//------------------------------------------------------------------------------------------------

// Basically everything except the contextmenu event (right click).
var event_types = 'message blur change focus focusin focusout hover scroll submit click dblclick mousedown mouseup mousemove mouseover mouseout mouseenter mouseleave pointerup pointerdown pointerenter pointerleave pointermove pointerover pointerout keydown keypress keyup input'.split(' ');
//var event_types = ["deviceorientation", "transitionend", "webkittransitionend", "webkitanimationstart", "webkitanimationiteration", "webkitanimationend", "search", "reset", "waiting", "volumechange", "unload", "timeupdate", "suspend", "submit", "storage", "stalled", "select", "seeking", "seeked", "scroll", "resize", "ratechange", "progress", "popstate", "playing", "play", "pause", "pageshow", "pagehide", "online", "offline", "mousewheel", "mouseup", "mouseover", "mouseout", "mousemove", "mousedown", "message", "loadstart", "loadedmetadata", "loadeddata", "load", "keyup", "keypress", "keydown", "invalid", "input", "hashchange", "focus", "error", "ended", "emptied", "durationchange", "drop", "dragstart", "dragover", "dragleave", "dragenter", "dragend", "drag", "dblclick", "contextmenu", "click", "change", "canplaythrough", "canplay", "blur", "beforeunload", "abort"]

function block_event(event) {
    if (event.target && event.target.tagName) {
        if (smash_browse_context_menu.top_menu){
            if (smash_browse_context_menu.top_menu.contains(event.target)) {
                return true
            } else {
                switch (event.type) {
                    case 'click':
                        // Click outside a visible context menu will close it.
                        if (smash_browse_context_menu.visible) {
                            smash_browse_context_menu.hide()
                        } else {
                            return true
                        }
                        break
                    case 'mousemove' :
                        console.log('got mouse move')
                        smash_browse_context_menu.on_mouse_over(event)
                        break
                    case 'mouseout' :
                        //event.target.classList.remove('smash_browse_hover')
                        break
                }
            }
        }
    }
    event.stopPropagation();
    event.preventDefault();
    event.stopImmediatePropagation();
    return false;
}

function block_events() {
    event_types.forEach(function (eventType) {
        window.addEventListener(eventType, block_event, true)
    })
//    for(var i=0; i<event_types.length; i++) {
//        window.addEventListener(event_types[i], block_event, true)
//    }
}

function unblock_events() {
    EVENT_TYPES.forEach(function (eventType) {
        window.removeEventListener(eventType, block_event, true);
    });
}

block_events()

//------------------------------------------------------------------------------------------------
//The current set of elements.
//------------------------------------------------------------------------------------------------

var current_elements = document.getElementsByTagName("*");

//------------------------------------------------------------------------------------------------
//Our Context Menu.
//------------------------------------------------------------------------------------------------

//click dblclick mousedown mousemove mouseup mouseover keydown keypress keyup input
//var event_types = 'contextmenu'.split(' ');
//event_types.forEach(function (eventType) {
//    console.log('listening to: ' + eventType)
//    document.addEventListener(eventType, check_event, true);
//});

function check_event(event) {
    var target = event.target
    var rect = target.getBoundingClientRect ? target.getBoundingClientRect() : {};
    var data = {
            altKey: event.altKey,
            button: event.button,
            buttons: event.buttons,
            ctrlKey: event.ctrlKey,
            clientX: event.clientX,
            clientY: event.clientY,
            elementX: event.clientX - rect.left,
            elementY: event.clientY - rect.top,
            keyIdentifier: event.keyIdentifier,
            key: event.key,
            code: event.code,
            charCode: event.charCode,
            keyCode: event.keyCode,
            location: event.location,
            metaKey: event.metaKey,
            shiftKey: event.shiftKey,
            target: get_xpath(target),
            targetFrame: [],
            type: event.type,
    } 
    //perform_text_path_check(event, data)
    //perform_position_check(event, data)
    //perform_image_check(event, data)
}

function perform_position_check(event, data) {
//    console.log("target xpath: " + data.target)
//    console.log("pageX,pageY: " + event.pageX + "," + event.pageY)
//    var element = document.elementFromPoint(event.pageX-window.scrollX, event.pageY-window.scrollY)
//    var xpath
//    if (element) {
//        xpath = get_xpath(element)
//    }
//    console.log("xpath: " + xpath)
//    var results = find_elements_by_xpath(xpath)
//    console.log("num found: " + results.length)
//    dump_array_of_elements(results)
//    console.log("pass: " + (xpath == data.target) && (results.length == 1))
}

function perform_image_check(event, data) {
//    //console.log("target xpath: " + data.target)
//    //console.log("pageX,pageY: " + event.pageX + "," + event.pageY)
//    var image_path = get_image_path(event.target)
//    var xpath = get_xpath(event.target)
//    console.log("xpath: " + xpath)
//    console.log("image_path: " + image_path)
//    if (image_path.length) {
//        var elements = find_elements_by_image_path(image_path)
//        var xpath2 = get_xpath(elements[0])
//        console.log("num paths found: " + elements.length)
//        console.log("passed: " + (xpath == xpath2) && (elements.length == 1))
//    } else {
//        console.log("passed: false")
//    }
    
}

function perform_text_path_check(event, data) {
//    var element = event.target
//    var results = []
//    
//    console.log('*********************************************************')
//    console.log('Element Info:')
//    console.log('*********************************************************')
//    
//    console.log("full xpath: " + data.target)
//    
//    var text_path = get_text_structure(element).text_path
//    var text = get_text_value(element)
//    var xpath = form_text_match_xpath(text_path[text_path.length-1])
//    
//    console.log("text path: " + JSON.stringify(text_path))
//    console.log("xpath: " + xpath)
//    get_similar_text_paths(element, false)
//    
//    var elements =  find_elements_by_xpath(xpath)
//    console.log('-----------------------------------------------------------')
//    console.log("num elements found by xpath: " + elements.length)
//    console.log('-----------------------------------------------------------')
//    for (var i=0; i<elements.length; i++) {
//        console.log("--xpath match[" + i + "] " + get_xpath(elements[i]))
//        console.log("--text path[" + i + "] " + get_text_structure(elements[i]).text_path)
//    }
//    
//    elements = find_elements_by_text_path(text_path)
//    console.log('-----------------------------------------------------------')
//    console.log("num elements found by text path: " + elements.length)
//    console.log('-----------------------------------------------------------')
//    for (var i=0; i<elements.length; i++) {
//        console.log("--match[" + i + "] " + get_xpath(elements[i]))
//    }
//    console.log('-----------------------------------------------------------')
//    console.log("check passes: " + ((elements.length == 1) && (elements[0] == element)))
//    console.log('-----------------------------------------------------------')
}

//Returns true if the element matches the target element according
//to the given match criteria.
function elements_match(element, target_element, match_criteria) {
    var rect = element.getBoundingClientRect()
    var target_rect = target_element.getBoundingClientRect()

    // Test match.
    if (match_criteria.match_left && (rect.left != target_rect.left)) {
        return false
    }
    if (match_criteria.match_right && (rect.right != target_rect.right)) {
        return false
    }

    if (match_criteria.match_width && (rect.width != target_rect.width)) {
        return false
    }
    if (match_criteria.match_height && (rect.height != target_rect.height)) {
        return false
    }

    // Match xpath length.
    if (match_criteria.match_xpath_length) {
        xpath = get_xpath(element)
        target_xpath = get_xpath(target_element)
        if (xpath.split('/').length != target_xpath.split('/').length) {
            return false
        }
    }

    // Match font style.
    if (match_criteria.match_font || match_criteria.match_font_size) {
        var style = window.getComputedStyle(element, null)
        var target_style = window.getComputedStyle(target_element, null)

        if (match_criteria.match_font && (style.font != target_style.font)) {
            return false
        }
        if (match_criteria.match_font_size && (style.fontSize != target_style.fontSize)) {
            return false
        }
    }
    return true
}

// Returns an array starting with the target element and followed by all matching elements.
// The matching elements are found by scanning vertically from begin_y to end_y.
function get_matching_elements_on_y(target_element, match_criteria, begin_y, end_y) {
    // Results.
    var elements = [target_element]
    
    // Determine the y delta.
    var delta = -1
    if ((end_y - begin_y) >= 0) {
        delta = 1
    }
    
    // Determine the x sampling value.
    var target_rect = target_element.getBoundingClientRect()
    var target_rect_sample_x = target_rect.left + (Math.min(target_rect.width, target_rect.height) / 2.0)
    
    // Scan vertically.
    var h = begin_y
    while(((delta < 0) && (h > end_y)) || ((delta > 0) && (h < end_y))) {
        var element = document.elementFromPoint(target_rect_sample_x, h)
        h += delta
        
        // Continue if we don't have an element at this point.
        if (!element) {
            continue
        }
        
        // Continue if we already have this element.
        if (elements.indexOf(element) >= 0) {
            continue
        }
        
        // Does this element match the target element.
        if (!elements_match(element, target_element, match_criteria)){
            continue
        }
        
        // We have found a matching element.
        elements.push(element)
    }
    return elements
}

//Returns an array of elements which are similar to the target element 
//and intersect a vertical line passing through the target element.
//This array will include the target element, and will not be completely ordered.
//The array will start with the target element, then the elements above, then 
//the elements below.
function get_similar_elements_on_y(target_element) {
    var page_height = get_page_height()

    // Get properties of the element.
    var target_rect = target_element.getBoundingClientRect()
    var target_text = get_text_value(target_element)
    var target_urls = get_image_values(target_element)

    // The match properties object.
    var match_criteria = {
        match_left: true,
        match_right: false,
        match_width: false,
        match_height: false,
        match_font: false,
        match_font_size: false,
        match_xpath_length: true
    }

    // If we have images or video then match the width also.
    if (target_urls.length) {
        match_criteria.match_width = true;
        match_criteria.match_height = true;
    } else {
        match_criteria.match_font = true;
        match_criteria.match_font_size = true;
    }
    
    // Scan downwards.
    var down_elements = get_matching_elements_on_y(target_element, match_criteria, target_rect.bottom, page_height)

    // Scan upwards.
    var up_elements = get_matching_elements_on_y(target_element, match_criteria, target_rect.top, 0)

    //console.log('up down sizes: ' + up_elements.length + ',' + down_elements.length)

    // Join up and down elements.
    if (up_elements.length < 2) {
        if (down_elements.length < 2) {
            return [target_element]
        } else {
            return down_elements
        }
    } else {
        if (down_elements.length < 2) {
            return up_elements
        } else {
            down_elements.shift()
            return up_elements.concat(down_elements)
        }
    }
    // We will never reach this return statement.
    return [target_element]
}

function get_similar_horizontal(page_pos) {
    
}

//Get text paths similar to and including the element.
//This is intended to retrieve paths for elements in the same visual group as the element.
function get_similar_text_paths(element, horizontal) {
    var original_element = element
    var element_rect = element.getBoundingClientRect()

    // Find first ancestor with siblings that include the element argument.
    while(element) {
        var siblings = get_text_siblings(element)
        var filtered = []
        if (siblings.length>1) {
            for (var i=0; i<siblings.length; i++) {
                var sibling = siblings[i]
                var rect = sibling.getBoundingClientRect()
                if (horizontal) {
                    if (rect.bottom == element_rect.bottom) {
                        filtered.push(sibling)
                    }
                } else {
                    if (rect.left == element_rect.left) {
                        filtered.push(sibling)
                    }
                }
            }
        }
        // If we found more than one then return it.
        if (filtered.length > 1) {
            console.log("num filtered: " + filtered.length)
            for (var i=0; i<filtered.length; i++) {
                console.log("filtered["+i+"] "+get_xpath(filtered[i]))
                console.log("xpath["+i+"] " + get_text_structure(filtered[i]).text_path)
            }
            return filtered
        } else {
            filtered.length = 0
        }
        
        // Otherwise check our next parent.
        element = element.parentNode
    }
    
    console.log("num filtered: " + 1)
    console.log("filtered[0] original element")
    console.log("xpath[0] original element")

    return [original_element]
}











