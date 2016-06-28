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
            this.top_menu.classList.add('menu')
            document.body.appendChild(this.top_menu)
            
            // Bind the click handler to ourself.
            this.on_click_bound = this.on_click.bind(this)
            
            // Create the menu as dom elements.
            this.create_menu(this.top_menu)
            
            // Create our mouse overlays.
            this.mouse_overlay = this.create_overlay('smart_browse_mouse_overlay')
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
            li.classList.add('menu-item')
            menu.appendChild(li)
            
            var button = document.createElement('button')
            button.classList.add('menu-btn')
            button.type = 'button'
            li.appendChild(button)
            
            var span = document.createElement('span')
            span.classList.add('menu-text')
            button.appendChild(span)
            
            var text = document.createTextNode(item_text)
            span.appendChild(text)
            return li
        },
        add_sub_menu : function(menu, item_text) {
            var li = document.createElement('li')
            li.classList.add('menu-item')
            li.classList.add('submenu')
            this.top_menu.appendChild(li)
            
            var button = document.createElement('button')
            button.classList.add('menu-btn')
            button.type = 'button'
            li.appendChild(button)
            
            var span = document.createElement('span')
            span.classList.add('menu-text')
            button.appendChild(span)
            
            var text = document.createTextNode(item_text)
            span.appendChild(text)
            
            var menu = document.createElement("menu");
            menu.classList.add('menu')
            li.appendChild(menu)
            return menu
        },
        remove_sub_menu : function(menu, sub_menu) {
            menu.removeChild(sub_menu)
        },
        add_spacer : function(menu) {
            var li = document.createElement('li')
            li.classList.add('menu-separator')
            menu.appendChild(li)
            return li
        },
        disable_item : function(item) {
            item.classList.add('disabled')
        },
        enable_item : function(item) {
            item.classList.remove('disabled')
        },
        show : function(x,y) {
            this.top_menu.style.left = x + 'px'
            this.top_menu.style.top = y + 'px'
            this.top_menu.classList.add('show-menu')
            this.visible = true
        },
        hide : function(x,y) {
            this.top_menu.classList.remove('show-menu')
            this.visible = false
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
            // Gather information about the page event.
            this.text_values = get_text_values(page_event.target)
            this.image_values = get_image_values(page_event.target)
            this.type = page_event.target.tagName
            this.position = [page_event.pageX, page_event.pageY]
            this.rect = page_event.target.getBoundingClientRect()
            
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
            if (this.find_by_text.contains(menu_target)) { 
                var msg = "Find elements with text value, " + this.text_values
                //var elements = find_elements_by_text(this.text_value)
                //this.set_selected_elements(elements)
                console.log(msg)
            } else if (this.find_by_image.contains(menu_target)) {
                var msg = "Find elements with image values, " + this.image_values
                console.log(msg)
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
            var inner_elem = get_visible_inner_element(page_event.pageX, page_event.pageY)
            var rect = inner_elem.getBoundingClientRect()
            var page_box = get_page_box_from_client_rect(rect)
            this.update_overlay(this.mouse_overlay, page_box)
        },
        create_overlay: function(class_name) {
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
        create_selected_overlays: function() {
            for (var i=0; i<this.selected_page_boxes.length; i++) {
                var page_box = this.selected_page_boxes[i]
                var overlay = this.create_overlay('smart_browse_selected_overlay')
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
}

//------------------------------------------------------------------------------------------------
//Event Blocking/Unblocking.
//------------------------------------------------------------------------------------------------

// Basically everything except the contextmenu event (right click).
var EVENT_TYPES = 'message scroll submit click dblclick mousedown mousemove mouseup mouseover mouseenter mouseout keydown keypress keyup input'.split(' ');

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
                        //event.target.classList.add('smash_browse_hover')
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
    EVENT_TYPES.forEach(function (eventType) {
        window.addEventListener(eventType, block_event, true);
    });
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
    perform_image_check(event, data)
}

function perform_position_check(event, data) {
    console.log("target xpath: " + data.target)
    console.log("pageX,pageY: " + event.pageX + "," + event.pageY)
    var element = document.elementFromPoint(event.pageX-window.scrollX, event.pageY-window.scrollY)
    var xpath
    if (element) {
        xpath = get_xpath(element)
    }
    console.log("xpath: " + xpath)
    var results = find_elements_by_xpath(xpath)
    console.log("num found: " + results.length)
    dump_array_of_elements(results)
    console.log("pass: " + (xpath == data.target) && (results.length == 1))
}

function perform_image_check(event, data) {
    //console.log("target xpath: " + data.target)
    //console.log("pageX,pageY: " + event.pageX + "," + event.pageY)
    var image_path = get_image_path(event.target)
    var xpath = get_xpath(event.target)
    console.log("xpath: " + xpath)
    console.log("image_path: " + image_path)
    if (image_path.length) {
        var elements = find_elements_by_image_path(image_path)
        var xpath2 = get_xpath(elements[0])
        console.log("num paths found: " + elements.length)
        console.log("passed: " + (xpath == xpath2) && (elements.length == 1))
    } else {
        console.log("passed: false")
    }
    
}

function perform_text_path_check(event, data) {
    var element = event.target
    var results = []
    
    console.log('*********************************************************')
    console.log('Element Info:')
    console.log('*********************************************************')
    
    console.log("full xpath: " + data.target)
    
    var text_path = get_text_structure(element).text_path
    var text = get_text_value(element)
    var xpath = form_text_match_xpath(text_path[text_path.length-1])
    
    console.log("text path: " + JSON.stringify(text_path))
    console.log("xpath: " + xpath)
    get_similar_text_paths(element, false)
    
    var elements =  find_elements_by_xpath(xpath)
    console.log('-----------------------------------------------------------')
    console.log("num elements found by xpath: " + elements.length)
    console.log('-----------------------------------------------------------')
    for (var i=0; i<elements.length; i++) {
        console.log("--xpath match[" + i + "] " + get_xpath(elements[i]))
        console.log("--text path[" + i + "] " + get_text_structure(elements[i]).text_path)
    }
    
    elements = find_elements_by_text_path(text_path)
    console.log('-----------------------------------------------------------')
    console.log("num elements found by text path: " + elements.length)
    console.log('-----------------------------------------------------------')
    for (var i=0; i<elements.length; i++) {
        console.log("--match[" + i + "] " + get_xpath(elements[i]))
    }
    console.log('-----------------------------------------------------------')
    console.log("check passes: " + ((elements.length == 1) && (elements[0] == element)))
    console.log('-----------------------------------------------------------')
    
}

//Form an xpath expression which will match any node with a 
//textnode inside of them whose text matches that given.
function form_text_match_xpath(text) {
    // We need to break out single quotes and concat them
    // to form a proper xpath.
    var splits = text.split("'")
    if (splits.length>1) {
        // Concat the splits with single quotes.
        text = "concat(";
        for (var s=0; s<splits.length; s++) {
            if (s!=0){
                text += "," + "\"'\"" + ","  // single quote is added here
            }
            text += "'" + splits[s] + "'"
        }
        text += ")"
    } else {
        // The text has no single quotes here.
        text = "'" + text + "'"
    }
    return "//*[text()[string(.) = "+ text +"]]"
}



// Gets the text value from an element's immediate textnode child.
function get_text_value(element) {
    // Get the element's children.
    var children = element.childNodes
    // Loop through them looking for a text node.
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        if (child.nodeType == Node.TEXT_NODE) {
            var text = child.nodeValue
            // If the text is not all white space, then we have found our text.
            if (!is_all_whitespace(text)) {
                return text
            }
        }
    }
    // Otherwise this may be an anchor with a title attribute.
    var attrs = element.attributes
    for (var i=0; i<attrs.length; i++) {
        if (attrs[i].name.toLowerCase() == 'title') {
            return attrs[i].value
        }
    }
    // Otherwise we can take the 'href' attribute.
    for (var i=0; i<attrs.length; i++) {
        if (attrs[i].name.toLowerCase() == 'href') {
            return attrs[i].value
        }
    }
    
    // Otherwise we didn't find any text for this element.
    return ""
}


function get_visible_inner_element(page_x, page_y) {
    // Build xpath to find all elements.
    var xpath = "//*"
    var elements = find_elements_by_xpath(xpath)
    console.log("determining visible inner from num elememnts: " + elements.length)
    console.log('page pos: ' + page_x + "," + page_y)
    
    // Loop over the elements.
    var smallest_elem = null
    var smallest_area = null
    var elem = null
    var rect = null
    for (var i=0; i<elements.length; i++) {
        elem = elements[i]
        
        // Make sure the element is visible.
        if ((elem.offsetWidth == 0) || (elem.offsetHeight == 0)) {
            continue
        }
        
        // Make sure the element intersects the internal element.
        if (!element_contains_point(elem, page_x, page_y)) {
            continue
        }
        
        console.log ('aaaa')
        if (!smallest_elem) {
            smallest_elem = elem
            rect = elem.getBoundingClientRect()
            smallest_area = rect.width * rect.height
        } else {
            rect = elem.getBoundingClientRect()
            if ((rect.width * rect.height) < smallest_area) {
                smallest_elem = elem
                smallest_area = rect.width * rect.height
            }
        }
    }
    return smallest_elem
}

function get_text_values(element) {
    var original_element = element
    
    // Find images which overlap the internal rect.
    var texts = []
    
    // Build xpath to find all elements.
    var xpath = "//*"
    var elements = find_elements_by_xpath(xpath)
    console.log("determining text values from num elememnts: " + elements.length)
    
    // Loop over the elements.
    var elem = null
    for (var i=0; i<elements.length; i++) {
        elem = elements[i]
        
        // Make sure the element is visible.
        if ((elem.offsetWidth == 0) || (elem.offsetHeight == 0)) {
            continue
        }
        
        // Make sure the element intersects the internal element.
        if (!element_intersects(elem, original_element)) {
            continue
        }
        
        // Loop through children accumulating text node values.
        for (var c=0; c<elem.childNodes.length; c++) {
            var child = elem.childNodes[c]
            if (child.nodeType == Node.TEXT_NODE) {
                var text = child.nodeValue
                // Add text if it's not all whitespace.
                if (!is_all_whitespace(text)) {
                    texts.push(text)
                }
            }
        }
    }
    return texts
}

// Returns an array of image urls which are all overlapping the given element.
function get_image_values(element) {
    var original_element = element
    
    // Find images which overlap the internal rect.
    var images = []
    
    // Build xpath to find all elements.
    var xpath = "//*"
    var elements = find_elements_by_xpath(xpath)
    console.log("determining image values from num elememnts: " + elements.length)
    
    // Loop over the elements.
    var elem = null
    for (var i=0; i<elements.length; i++) {
        elem = elements[i]
        
        // Make sure the element is visible.
        if ((elem.offsetWidth == 0) || (elem.offsetHeight == 0)) {
            continue
        }
        
        // Make sure the element intersects the internal element.
        if (!element_intersects(elem, original_element)) {
            continue
        }
        
        // Get the tag name.
        var tag_name = elem.tagName.toLowerCase()
        
        // Return the src for images and video elements.
        if ((tag_name === 'img') || (tag_name === 'video')) {
            if (images.indexOf(elem.src)<0) {
                //console.log("elem source: " + elem.src)
                images.push(elem.src)
            }
        }
        
        // Otherwise we query the computed style for the background image.
        var style = elem.currentStyle || window.getComputedStyle(elem, false)
        if (style) {
            // console.log("ssssstyle: " + JSON.stringify(style))
            if (style.backgroundImage.indexOf('url(') == 0) {
                var background_image = style.backgroundImage.slice(4, -1);
                if (background_image != "") {
                    if (images.indexOf(background_image)<0) {
                        //console.log("background image: " + background_image)
                        images.push(background_image)
                    }
                }
            }
        }
    }
    return images
}

function is_text_child(child) {
    // Skip unwanted elements.
    var tagName = child.tagName
    if (tagName) {
        tagName = tagName.toLowerCase()
        if ((tagName == 'script') || (tagName == 'noscript') || (tagName == 'style')) {
            return false
        }
    }
    // Skip comment nodes.
    if (child.nodeType == Node.COMMENT_NODE) {
        return false
    }
    // Skip elements with empty text.
    if (get_text_value(child) == '') {
        return false
    }
    // Skip elements which are not visible.
    if ((child.offsetWidth == 0) || (child.offsetHeight == 0)) {
        return false
    }
    return true
}

// Recurses to find text embedded in an element which is acting like a label.
function get_top_left_text_child(element) {
    var children = find_text_elements(element)
    // The elements are in document order. Usually this implies visual order as well.
    // We take the first element we can find meeting our criteria.
    // The top left child could be the element argument itself.
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        if (is_text_child(child)) {
            return  child
            break
        }
    }
    return null
}

function get_bottom_left_text_child(element) {
    var children = find_text_elements(element)
    // The elements are in document order. Usually this implies visual order as well.
    // We take the last element we can find meeting our criteria.
    // The bottom left child could be the element argument itself.
    for (var i=children.length-1; i>=0; i--) {
        var child = children[i]
        if (is_text_child(child)) {
            return child
            break
        }
    }
    return null
}

function get_text_siblings(element) {
    var children = find_text_elements(element)
    var siblings = []
    // The elements are in document order. Usually this implies visual order as well.
    // We take the last element we can find meeting our criteria.
    // The bottom left child could be the element argument itself.
    for (var i=children.length-1; i>=0; i--) {
        var child = children[i]
        if (is_text_child(child)) {
            siblings.push(child)
        }
    }
    return siblings
}

//Returns an array of text which will uniquely identify an element which has a textnode.
function get_text_structure(element) {
    // Our text path.
    var text_path = []
    var text_path_elements =[]
    // Our last top left child.
    var last_top_left_child = null
    // Start visiting ancestors from the element.
    while(element) {
        // Get the top left text inside the element.
        var top_left_child = get_top_left_text_child(element)
        if (top_left_child && top_left_child != last_top_left_child) {
            var text = get_text_value(top_left_child)
            if (text != '') {
                text_path.unshift(text)
                text_path_elements.unshift(element)
            }
            last_top_left_child = top_left_child
        }
        element = element.parentNode
    }
    
    return {
        text_path: text_path,
        text_path_elements: text_path_elements
    }
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
        var style = element.currentStyle || window.getComputedStyle(element, false)
        var target_style = target_element.currentStyle || window.getComputedStyle(target_element, false)

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

//Get the full xpath for an element, anchored at the html document.
//For simplicity each path element always has a [] index suffix.
function get_xpath(element) {
    var path = [];
    while(element) {
        if (element.parentNode) {
            var node_name = element.nodeName;
            var sibling = element
            var count = 0;
            while (sibling) {
                if (sibling.nodeName == node_name) {
                    ++count;
                }
                sibling = sibling.previousElementSibling
            }
            path.unshift(node_name + '[' + count + ']');
        }
        element = element.parentNode
    }
    return '/'+path.join('/');
}

// Get image path.
function get_image_path(element) {
    var image_path = get_text_structure(element).text_path
    var urls = get_image_values(element)
    if (urls.length == 0) {
        return []
    }
    image_path.push(urls)
    return image_path
}

//------------------------------------------------------------------------------------------------
//Utilities.
//------------------------------------------------------------------------------------------------

function find_elements_by_text(text) {
    var xpath = "//*[count(text()) > 0]"
    var elements = find_elements_by_xpath(xpath)
    var results = []
    for (var i=0; i<elements.length; i++) {
        var element = elements[i]
        if (text == get_text_value(element)) {
            results.push(element)
        }
    }
    return results
}

//Returns an array of elements which have text nodes.
//The elements under consideration are the element argument and its children.
function find_text_elements(element) {
    var element_xpath = get_xpath(element)
    
    // Build xpath to find all elements with text nodes under the element xpath.
    var xpath = element_xpath;
    if (xpath != '/') {
        xpath += '/'
    }
    xpath += "/*[count(text()) > 0]";
    return find_elements_by_xpath(xpath)
}

// Returns an array of elements which have background images or is an image or video itself.
function find_image_elements(text_path, grouping_rect, urls) {
    var elements = []
    var xpath = "//*"
    
    // Look for elements containing the image url.
    // We look backwards so that we get the deepest and latest elements first in document order.
    var children = find_elements_by_xpath(xpath)
    console.log("num children to look at: " + children.length)
    for (var i=0; i<children.length; i++) {
        var child = children[i]
        
        // If the child isn't visible skip it.
        if ((child.offsetWidth == 0) || (child.offsetHeight == 0)) {
            continue
        }
        
        // If the child doesn't intersect the grouping_rect skip it.
        if (!intersects(child.getBoundingClientRect(), grouping_rect)) {
            continue
        }
        
        // Make sure we're getting top most element, when elements are overlapping. (like divs)
        child = get_top_element(child)
        if (!child) {
            continue
        }
        
        // Get the text path of the child.
        var path = get_text_structure(child).text_path
        if (!arrays_are_equal(text_path, path)) {
            continue
        }
        
        // Otherwise try getting the image value.
        var image_urls = get_image_values(child)
        
        // If it matches our url we've found one.
        if (arrays_are_equal(image_urls,urls) ) {
            if (elements.indexOf(child) <0) {
                elements.push(child)
            }
        }
    }
    return elements
}

//Returns an array of elements matching the text path.
function find_elements_by_text_path(text_path) {
    var text_path_tail = text_path[text_path.length-1]
    var xpath = form_text_match_xpath(text_path_tail)
    var xpath_results = find_elements_by_xpath(xpath)
    var elements = []
    for (var i=0; i<xpath_results.length; i++) {
        var element = xpath_results[i]
        var path = get_text_structure(element).text_path
        if (arrays_are_equal(path, text_path)) {
            elements.push(element)
        }
    }
    return elements
}

function find_elements_by_image_path(image_path) {
    var text_path = image_path
    var image_urls = text_path.pop()
    
    var image_elements = []
    var text_elements = find_elements_by_text_path(text_path)
    
    console.log("num text elements: " + text_elements.length)
    
    
    for (var e=0; e<text_elements.length; e++) {
        var text_element = text_elements[e]
        var obj = get_text_structure(text_element)
        var grouping_element = obj.text_path_elements[obj.text_path_elements.length-2]
        image_elements = image_elements.concat(find_image_elements(text_path, grouping_element.getBoundingClientRect(), image_urls))
    }
    
    for (var t=0; t<image_elements.length; t++) {
        console.log("ppp: " + get_xpath(image_elements[t]))
        console.log("qqq: " + get_image_path(image_elements[t]))
    }
    return image_elements
}

//Find elements by xpath. 
//This returns an array of elements.
function find_elements_by_xpath(xpath) {
    var set = document.evaluate(xpath, document, null, XPathResult.ORDERED_NODE_SNAPSHOT_TYPE, null)
    var elements = []
    for (var i=0; i<set.snapshotLength; i++) {
        elements.push(set.snapshotItem(i))
    }
    return elements
}

function get_top_element(element) {
    // Make sure the element is visible.
    if ((element.offsetWidth == 0) || (element.offsetHeight == 0)) {
        console.log('t')
        return null
    }
    
    var rect = element.getBoundingClientRect()
    var x = rect.left + 1
    var y = rect.top + 1
    return document.elementFromPoint(x, y)
}

//Dump the xpaths of an array of elements.
function dump_array_of_elements(elements) {
  for (var i=0; i<elements.length; i++) {
      console.log("found: " + get_xpath(elements[i]))
  }
}

//Returns true if the text is composed of all whitespace characters.
function is_all_whitespace(text) {
    var regex = /^\s+$/;
    return regex.test(text)
}

//Returns true if the contents of two arrays of primitives are equal.
function arrays_are_equal(a1, a2) {
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

function get_page_height() {
    var doc_body = document.body
    var doc_elem = document.documentElement
    return Math.max( doc_body.scrollHeight, doc_body.offsetHeight, 
            doc_elem.clientHeight, doc_elem.scrollHeight, doc_elem.offsetHeight );
}

function get_page_width() {
    var doc_body = document.body
    var doc_elem = document.documentElement
    return Math.max( doc_body.scrollWidth, doc_body.offsetWidth, 
            doc_elem.clientWidth, doc_elem.scrollWidth, doc_elem.offsetWidth );
}

//Dump the client rect to the console.
function dump_client_rect(rect) {
    console.log("client rect: " + rect.left + "," + rect.right + "," + rect.top + "," + rect.bottom)
}

//Returns true if the outer element contains the inner element.
function element_contains(outer, inner) {
    //Associate null with zero area element
    if (outer == null) {
        return false
    }
    if (inner == null) {
        return true
    }
    //Outer and inner must be elements.
    if (!outer.getBoundingClientRect) {
        return false
    }
    if (!inner.getBoundingClientRect) {
        return false
    }
    //Call out to our helper.
    var outer_rect = outer.getBoundingClientRect()
    var inner_rect = inner.getBoundingClientRect()
    return contains(outer_rect, inner_rect)
}

// Returns true if the outer rect contains the inner rect.
function contains(outer, inner) {
    if (inner.left >= outer.left && 
            inner.right <= outer.right && 
            inner.bottom <= outer.bottom && 
            inner.top >= outer.top) {
        return true
    }
    return false
}

function element_intersects(outer, inner) {
    //Associate null with zero area element
    if (outer == null) {
        return false
    }
    if (inner == null) {
        return false
    }
    //Outer and inner must be elements.
    if (!outer.getBoundingClientRect) {
        return false
    }
    if (!inner.getBoundingClientRect) {
        return false
    }
    //Call out to our helper.
    var outer_rect = outer.getBoundingClientRect()
    var inner_rect = inner.getBoundingClientRect()
    return intersects(outer_rect, inner_rect)
}
		
function intersects(rect_a, rect_b) {
    if (rect_a.right < rect_b.left) {
        return false
    } else if (rect_a.left > rect_b.right) {
        return false
    } else if (rect_a.bottom < rect_b.top) {
        return false
    } else if (rect_a.top > rect_b.bottom) {
        return false
    }
    return true
}

// Returns a bounding box in pages space as an array [min_x, max_x, min_y, max_y]
function get_page_box_from_client_rect(rect) {
    return [rect.left + window.scrollX,
            rect.right + window.scrollX,
            rect.top + window.scrollY,
            rect.bottom + window.scrollY]
}

function element_contains_point(element, page_x, page_y) {
    var rect = element.getBoundingClientRect()
    var page_box = get_page_box_from_client_rect(rect)
    if ((page_x >= page_box[0]) && (page_x <= page_box[1]) && 
            (page_y >=page_box[2]) && (page_y <= page_box[3])) {
        return true
    }
    return false
}



