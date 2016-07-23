//------------------------------------------------------------------------------------------------
//Smash Browse Context Menu.
//------------------------------------------------------------------------------------------------

//1) The smash_browse prefix prevents collisions with js in the target web page.
//2) Elements are described by their bounds in page space so that they can be passed back and forth
//between javascript and c++. In the javascript environment they can be easily converted back to elements.

var ContextMenu = function() {
    this.top_menu = null
    this.on_click_bound = null
    this.visible = false
}

ContextMenu.prototype.initialized = function() {
    if (this.top_menu) {
        return true
    }
    return false
}

ContextMenu.prototype.initialize = function() {
    if (this.initialized()) {
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
    this.text_box_overlay = new Overlay('smash_browse_text_box', g_distinct_colors.text_color, -1, null)
    this.image_box_overlay = new Overlay('smash_browse_image_box', g_distinct_colors.image_color, -1, null)
}

ContextMenu.prototype.create_menu = function (top_menu) {
    
    // The find specific menu.
    this.navigate_menu = this.add_sub_menu(top_menu, 'Navigate')
    this.navigate_to_url = this.add_item(this.navigate_menu, 'to url')
//    this.navigate_back = this.add_item(this.navigate_menu, 'back')
//    this.navigate_forward = this.add_item(this.navigate_menu, 'forward')
    this.navigate_refresh = this.add_item(this.navigate_menu, 'refresh')
    
    // Spacer.
    this.add_spacer(top_menu)
    
    // The find specific menu.
    this.create_set_by_matching_menu = this.add_sub_menu(top_menu, 'Create set from values under mouse')
    this.create_set_by_matching_text = this.add_item(this.create_set_by_matching_menu, 'text')
    this.create_set_by_matching_image = this.add_item(this.create_set_by_matching_menu, 'image')
//    this.create_set_by_matching_position = this.add_item(this.create_set_by_matching_menu, 'position')
//    this.create_set_by_matching_xpath = this.add_item(this.create_set_by_matching_menu, 'xpath')
    
    // The find all menu.
    this.create_set_by_type_menu = this.add_sub_menu(top_menu, 'Create set from elements of type')
    this.create_set_from_inputs = this.add_item(this.create_set_by_type_menu, 'has an input (text field)')
    this.create_set_from_selects = this.add_item(this.create_set_by_type_menu, 'has a select (drop down)')
    this.create_set_from_texts = this.add_item(this.create_set_by_type_menu, 'has text')
    this.create_set_from_images = this.add_item(this.create_set_by_type_menu, 'has an image')
    
    // Delete
    this.delete_set = this.add_item(this.top_menu, 'Delete set')
    
    // Spacer.
    this.add_spacer(top_menu)

    // The shift elements menu.
    this.shift_to_text_menu = this.add_sub_menu(top_menu, 'Shift set elements to text')
    this.shift_up_by_text = this.add_item(this.shift_to_text_menu, 'above')
    this.shift_down_by_text = this.add_item(this.shift_to_text_menu, 'below')
    this.shift_left_by_text = this.add_item(this.shift_to_text_menu, 'on left')
    this.shift_right_by_text = this.add_item(this.shift_to_text_menu, 'on right')
    
    this.shift_to_image_menu = this.add_sub_menu(top_menu, 'Shift set elements to images')
    this.shift_up_by_image = this.add_item(this.shift_to_image_menu, 'above')
    this.shift_down_by_image = this.add_item(this.shift_to_image_menu, 'below')
    this.shift_left_by_image = this.add_item(this.shift_to_image_menu, 'on left')
    this.shift_right_by_image = this.add_item(this.shift_to_image_menu, 'on right')
    
    this.shift_to_input_menu = this.add_sub_menu(top_menu, 'Shift set elements to inputs')
    this.shift_up_by_input = this.add_item(this.shift_to_input_menu, 'above')
    this.shift_down_by_input = this.add_item(this.shift_to_input_menu, 'below')
    this.shift_left_by_input = this.add_item(this.shift_to_input_menu, 'on left')
    this.shift_right_by_input = this.add_item(this.shift_to_input_menu, 'on right')

    this.shift_to_selector_menu = this.add_sub_menu(top_menu, 'Shift set elements to selects')
    this.shift_up_by_select = this.add_item(this.shift_to_selector_menu, 'above')
    this.shift_down_by_select = this.add_item(this.shift_to_selector_menu, 'below')
    this.shift_left_by_select = this.add_item(this.shift_to_selector_menu, 'on left')
    this.shift_right_by_select = this.add_item(this.shift_to_selector_menu, 'on right')
    
    // Spacer.
    this.add_spacer(top_menu)
        
    // The expand menu.
    this.expand_menu = this.add_sub_menu(top_menu, 'Expand set to similar elements')
    this.expand_above = this.add_item(this.expand_menu, 'above')
    this.expand_below = this.add_item(this.expand_menu, 'below')
    this.expand_left = this.add_item(this.expand_menu, 'on left')
    this.expand_right = this.add_item(this.expand_menu, 'on right')
    
    // Spacer.
    this.add_spacer(top_menu)
    
    // Mark sets.
    this.mark_set = this.add_item(this.top_menu, 'Mark set')
    this.unmark_set = this.add_item(this.top_menu, 'Unmark set')
    
    // Merge marked sets.
    this.merge_marked_sets = this.add_item(this.top_menu, 'Merge marked sets')
    
    // The shrink to marked menu.
    this.shrink_set_to_marked_menu = this.add_sub_menu(top_menu, 'Shrink set w.r.t. marked sets' )
    this.shrink_set_above_marked = this.add_item(this.shrink_set_to_marked_menu, 'above marked set element')
    this.shrink_set_below_marked = this.add_item(this.shrink_set_to_marked_menu, 'below marked set elements')
    this.shrink_set_above_and_below_marked = this.add_item(this.shrink_set_to_marked_menu, 'above and below marked set elements')
    this.shrink_set_left_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'left of marked set elements')
    this.shrink_set_right_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'right of marked set elements')
    this.shrink_set_left_and_right_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'left and right of marked set elements')
    
    // The shrink down all sets menu.
    this.shrink_set_menu = this.add_sub_menu(top_menu, 'Shrink set to' )
    this.shrink_set_to_topmost = this.add_item(this.shrink_set_menu, 'topmost element')
    this.shrink_set_to_bottommost= this.add_item(this.shrink_set_menu, 'bottommost element')
    this.shrink_set_to_leftmost = this.add_item(this.shrink_set_menu, 'leftmost element')
    this.shrink_set_to_rightmost = this.add_item(this.shrink_set_menu, 'rightmost element')
    
    // Spacer.
    this.spacer = this.add_spacer(this.top_menu)
    
    // Act on element.
    this.perform_menu = this.add_sub_menu(this.top_menu, 'Perform action on set with one element')
    this.perform_click = this.add_item(this.perform_menu, 'click')
    this.perform_type = this.add_item(this.perform_menu, 'type text')
    this.perform_enter = this.add_item(this.perform_menu, 'press enter/submit')
    this.perform_extract = this.add_item(this.perform_menu, 'extract text')
    this.perform_select_option = this.add_item(this.perform_menu, 'select option')
    
    // Spacer.
    this.spacer = this.add_spacer(this.top_menu)
    

    
    // Cancel.
    this.cancel = this.add_item(this.top_menu, 'Cancel')
}

ContextMenu.prototype.add_item = function(menu, item_text) {
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
}

ContextMenu.prototype.add_sub_menu = function(menu, item_text) {
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
}

ContextMenu.prototype.remove_sub_menu = function(menu, sub_menu) {
    menu.removeChild(sub_menu)
}

ContextMenu.prototype.add_spacer = function(menu) {
    var li = document.createElement('li')
    li.classList.add('smash_browse_menu_separator')
    menu.appendChild(li)
    return li
}

ContextMenu.prototype.disable_item = function(item) {
    item.classList.add('smash_browse_disabled')
}

ContextMenu.prototype.enable_item = function(item) {
    item.classList.remove('smash_browse_disabled')
}

ContextMenu.prototype.show = function(x,y) {
    this.top_menu.style.left = x + 'px'
    this.top_menu.style.top = y + 'px'
    this.top_menu.classList.add('smash_browse_menu_show')
    this.visible = true
}

ContextMenu.prototype.hide = function(x,y) {
    this.top_menu.classList.remove('smash_browse_menu_show')
    this.visible = false
}

ContextMenu.prototype.contains_element = function(element) {
    if (!this.top_menu) {
        return false
    }
    if (this.top_menu.contains(element)) {
        return true
    }
    return false
}

ContextMenu.prototype.on_context_menu = function(page_event) {
    this.page_x = page_event.pageX
    this.page_y = page_event.pageY
    
    var text_values = g_page_wrap.get_text_values_at(this.page_x, this.page_y)
    var image_values = g_page_wrap.get_image_values_at(this.page_x, this.page_y)
    
    if (text_values.length == 0) {
        this.disable_item(this.create_set_by_matching_text)
    } else {
        this.enable_item(this.create_set_by_matching_text)
    }
    if (image_values.length == 0) {
        this.disable_item(this.create_set_by_matching_image)
    } else {
        this.enable_item(this.create_set_by_matching_image)
    }
    
    // Show the menu.
    this.show(page_event.pageX, page_event.pageY)
    
    // Listen to clicks to perform the according action and close the menu.
    document.addEventListener('click', this.on_click_bound, true);
    
    page_event.preventDefault();
    return false
}

ContextMenu.prototype.on_click = function(menu_event) {
    this.hide();
    var menu_target = menu_event.target
    
    if (this.navigate_to_url.contains(menu_target)) { 
        function goto_url(url) {
            g_content_comm.send_message_to_bg({request: 'navigate_to', url: url})
        }
        g_text_input_popup.set_label_text("Enter URL")
        g_text_input_popup.open(goto_url)
    } 
    
//    else if (this.navigate_back.contains(menu_target)) { 
//        g_content_comm.send_message_to_bg({request: 'navigate_back'})
//    } 
//    
//    else if (this.navigate_forward.contains(menu_target)) { 
//        g_content_comm.send_message_to_bg({request: 'navigate_forward'})
//    } 
    
    else if (this.navigate_refresh.contains(menu_target)) { 
        g_content_comm.send_message_to_bg({request: 'navigate_refresh'})
    } 
    
    // --------------------------------------------------------------
    // Create Set From Match Values.
    // --------------------------------------------------------------
    
    // Create set from text values.
    else if (this.create_set_by_matching_text.contains(menu_target)) { 
        var text_values = g_page_wrap.get_text_values_at(this.page_x, this.page_y)
        var request = {
            request: 'create_set_from_match_values', 
            wrap_type: ElemWrap.prototype.wrap_type.text,
            match_values: text_values
        }
        g_content_comm.send_message_to_bg(request)
    } 
    
    // Create set from image values.
    else if (this.create_set_by_matching_image.contains(menu_target)) {
        var image_values = g_page_wrap.get_image_values_at(this.page_x, this.page_y)
        var request = {
            request: 'create_set_from_match_values', 
            wrap_type: ElemWrap.prototype.wrap_type.image,
            match_values: image_values
        }
        g_content_comm.send_message_to_bg(request)
    } 
    
    // --------------------------------------------------------------
    // Create Set from Type.
    // --------------------------------------------------------------
    
    // Find all inputs.
    else if (this.create_set_from_inputs.contains(menu_target)) {
        var request = {
                request: 'create_set_from_wrap_type',
                wrap_type: ElemWrap.prototype.wrap_type.input
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    // Find all inputs.
    else if (this.create_set_from_selects.contains(menu_target)) {
        var request = {
                request: 'create_set_from_wrap_type',
                wrap_type: ElemWrap.prototype.wrap_type.select
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    // Find all images.
    else if (this.create_set_from_images.contains(menu_target)) {
        var request = {
                request: 'create_set_from_wrap_type',
                wrap_type: ElemWrap.prototype.wrap_type.image
        }
        g_content_comm.send_message_to_bg(request)
    } 
    
    // Find all text.
    else if (this.create_set_from_texts.contains(menu_target)) {
        var request = {
                request: 'create_set_from_wrap_type',
                wrap_type: ElemWrap.prototype.wrap_type.text
        }
        g_content_comm.send_message_to_bg(request)
    } 
    
    // --------------------------------------------------------------
    // Delete Set.
    // --------------------------------------------------------------
    
    // Delete set.
    else if (this.delete_set.contains(menu_target)) {        
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {request: 'delete_set', set_index: set_index}
        g_content_comm.send_message_to_bg(request)
    }
    
    // --------------------------------------------------------------
    // Shift Set.
    // --------------------------------------------------------------
    
    // Text.
    else if (this.shift_up_by_text.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.up,
            wrap_type: ElemWrap.prototype.wrap_type.text
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_down_by_text.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.down,
            wrap_type: ElemWrap.prototype.wrap_type.text
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_left_by_text.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.left,
            wrap_type: ElemWrap.prototype.wrap_type.text
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_right_by_text.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.right,
            wrap_type: ElemWrap.prototype.wrap_type.text
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    // Image.
    else if (this.shift_up_by_image.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.up,
            wrap_type: ElemWrap.prototype.wrap_type.image
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_down_by_image.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.down,
            wrap_type: ElemWrap.prototype.wrap_type.image
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_left_by_image.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.left,
            wrap_type: ElemWrap.prototype.wrap_type.image
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_right_by_image.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.right,
            wrap_type: ElemWrap.prototype.wrap_type.image
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    // Input.
    else if (this.shift_up_by_input.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.up,
            wrap_type: ElemWrap.prototype.wrap_type.input
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_down_by_input.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.down,
            wrap_type: ElemWrap.prototype.wrap_type.input
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_left_by_input.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.left,
            wrap_type: ElemWrap.prototype.wrap_type.input
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_right_by_input.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.right,
            wrap_type: ElemWrap.prototype.wrap_type.input
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    // Select.
    else if (this.shift_up_by_select.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.up,
            wrap_type: ElemWrap.prototype.wrap_type.select
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_down_by_select.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.down,
            wrap_type: ElemWrap.prototype.wrap_type.select
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_left_by_select.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.left,
            wrap_type: ElemWrap.prototype.wrap_type.select
        };
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shift_right_by_select.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shift_set', 
            set_index: set_index, 
            direction: ElemWrap.prototype.direction.right,
            wrap_type: ElemWrap.prototype.wrap_type.select
        };
        g_content_comm.send_message_to_bg(request)
    }
        
    // --------------------------------------------------------------
    // Expand to similar elements.
    // --------------------------------------------------------------
    
    // Expand above.
    else if (this.expand_above.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var match_criteria = new MatchCriteria()
        match_criteria.match_left = true
        match_criteria.match_font = true
        match_criteria.match_font_size = true
        var request = {
            request: 'expand_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.up,
            match_criteria: match_criteria
        }
        g_content_comm.send_message_to_bg(request)
    }
    // Expand below.
    else if (this.expand_below.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var match_criteria = new MatchCriteria()
        match_criteria.match_left = true
        match_criteria.match_font = true
        match_criteria.match_font_size = true
        var request = {
            request: 'expand_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.down,
            match_criteria: match_criteria
        }
        g_content_comm.send_message_to_bg(request)
    }
    // Expand left.
    else if (this.expand_left.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var match_criteria = new MatchCriteria()
        match_criteria.match_top = true
        match_criteria.match_font = true
        match_criteria.match_font_size = true
        var request = {
            request: 'expand_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.left,
            match_criteria: match_criteria
        }
        g_content_comm.send_message_to_bg(request)
    }
    // Expand right.
    else if (this.expand_right.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var match_criteria = new MatchCriteria()
        match_criteria.match_top = true
        match_criteria.match_font = true
        match_criteria.match_font_size = true
        var request = {
            request: 'expand_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.right,
            match_criteria: match_criteria
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    // --------------------------------------------------------------
    // Shrink w.r.t. marked sets.
    // --------------------------------------------------------------
    
    // Mark set.
    else if (this.mark_set.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'mark_set',
            set_index: set_index
        }
        g_content_comm.send_message_to_bg(request)
    } 
    
    // Unmark set.
    else if (this.unmark_set.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'unmark_set',
            set_index: set_index
        }
        g_content_comm.send_message_to_bg(request)
    } 
    
    // Merge marked sets.
    else if (this.merge_marked_sets.contains(menu_target)) {
        var request = {
                request: 'merge_marked_sets',
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shrink_set_above_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.up]
        }
        g_content_comm.send_message_to_bg(request)
        
        //g_overlay_sets.shrink_set_to_marked(this.page_x, this.page_y, [ElemWrap.prototype.direction.up])
    }
    
    else if (this.shrink_set_below_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.down]
        }
        g_content_comm.send_message_to_bg(request)
        
        //g_overlay_sets.shrink_set_to_marked(this.page_x, this.page_y, [ElemWrap.prototype.direction.down])
    }
    
    else if (this.shrink_set_above_and_below_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.up, ElemWrap.prototype.direction.down]
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shrink_set_left_of_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.left]
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shrink_set_right_of_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.right]
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shrink_set_left_and_right_of_marked.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y, false)
        var request = {
            request: 'shrink_set_to_marked',
            set_index: set_index,
            directions: [ElemWrap.prototype.direction.left, ElemWrap.prototype.direction.right]
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    // --------------------------------------------------------------
    // Shrink all.
    // --------------------------------------------------------------
    
    else if (this.shrink_set_to_topmost.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shrink_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.up
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shrink_set_to_bottommost.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shrink_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.down
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shrink_set_to_leftmost.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shrink_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.left
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.shrink_set_to_rightmost.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'shrink_set',
            set_index: set_index,
            direction: ElemWrap.prototype.direction.right
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    // --------------------------------------------------------------
    // Perform action on set with one element.
    // --------------------------------------------------------------
    
    else if (this.perform_click.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'perform_action',
            set_index: set_index,
            overlay_index: 0,
            action: 'send_click'
        }
        g_content_comm.send_message_to_bg(request)
    }

    else if (this.perform_type.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'perform_action',
            set_index: set_index,
            overlay_index: 0,
            action: 'send_text',
            text: 'booya'
        }
        
        function send_message_callback(text) {
            request.text = text
            g_content_comm.send_message_to_bg(request)
        }
        g_text_input_popup.set_label_text('Enter text to type')
        g_text_input_popup.open(send_message_callback)
    }

    else if (this.perform_enter.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'perform_action',
            set_index: set_index,
            overlay_index: 0,
            action: 'send_enter',
            key: 'x'
        }
        g_content_comm.send_message_to_bg(request)
    }

    else if (this.perform_extract.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'perform_action',
            set_index: set_index,
            action: 'get_text',
            overlay_index: 0,
            variable_name: 'test'
        }
        g_content_comm.send_message_to_bg(request)
    }
    
    else if (this.perform_select_option.contains(menu_target)) {
        var set_index = g_overlay_sets.find_set_index(this.page_x, this.page_y)
        var request = {
            request: 'perform_action',
            set_index: set_index,
            action: 'select_option',
            overlay_index: 0,
            option_text: 'hello'
        }
        function send_message_callback(text) {
            request.option_text = text
            g_content_comm.send_message_to_bg(request)
        }
        
        var set = g_overlay_sets.sets[set_index]
        var element = set.overlays[0].elem_wrap.element
        var option_values = []
        var option_texts = []
        for (var i=0; i<element.options.length; i++) {
            option_values.push(element.options[i].value)
            option_texts.push(element.options[i].text)
            console.log('option value,text: ' + element.options[i].value + "," + element.options[i].text)
        }
        g_select_input_popup.set_label_text('Enter the text of the option to select')
        g_select_input_popup.set_options(option_values, option_texts)
        g_select_input_popup.open(send_message_callback)
    }

    
    document.removeEventListener('click', this.on_click_bound, true);
    
    // Remove previous overlays.
    //this.destroy_selected_overlays()
    
    // Create current overlays.
    //this.create_selected_overlays()
}

ContextMenu.prototype.on_mouse_over = function(page_event) {
    if (!this.text_box_overlay || !this.image_box_overlay) {
        return
    }
    var text_elem_wrap = g_page_wrap.get_top_text_elem_wrap_at(page_event.pageX, page_event.pageY)
    var image_elem_wrap = g_page_wrap.get_top_image_elem_wrap_at(page_event.pageX, page_event.pageY)
    this.update_text_box_overlay(text_elem_wrap)
    this.update_image_box_overlay(image_elem_wrap)
}

ContextMenu.prototype.update_text_box_overlay = function(elem_wrap) {
    this.text_box_overlay.update_with_elem_wrap(elem_wrap)
}

ContextMenu.prototype.update_image_box_overlay = function(elem_wrap) {
    this.image_box_overlay.update_with_elem_wrap(elem_wrap)
}


var g_context_menu = null

//We only create on the top window, not in other iframes.
if (window == window.top) {
    g_context_menu = new ContextMenu()
}

