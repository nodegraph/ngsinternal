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

ContextMenu.prototype.initialize = function() {
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
    this.text_box_overlay = new Overlay('smash_browse_text_box', g_distinct_colors.text_color, -1, null)
    this.image_box_overlay = new Overlay('smash_browse_image_box', g_distinct_colors.image_color, -1, null)
}

ContextMenu.prototype.create_menu = function (top_menu) {
    // The find specific menu.
    this.find_menu = this.add_sub_menu(top_menu, 'Find elements using info from mouse hover')
    this.find_by_text = this.add_item(this.find_menu, 'text')
    this.find_by_image = this.add_item(this.find_menu, 'image')
    this.find_by_position = this.add_item(this.find_menu, 'position')
    this.find_by_xpath = this.add_item(this.find_menu, 'xpath')
    
    // The find all menu.
    this.find_all_menu = this.add_sub_menu(top_menu, 'Find all elements by type')
    this.get_elem_wraps_with_inputs = this.add_item(this.find_all_menu, 'has an input (text field)')
    this.get_elem_wraps_with_selects = this.add_item(this.find_all_menu, 'has a select (drop down)')
    this.get_elem_wraps_with_text = this.add_item(this.find_all_menu, 'has text')
    this.get_elem_wraps_with_images = this.add_item(this.find_all_menu, 'has an image')
//    this.get_elem_wraps_with_vertical_scroll_bars = this.add_item(this.find_all_menu, 'vertical scroll bar')
//    this.get_elem_wraps_with_horizontal_scroll_bars = this.add_item(this.find_all_menu, 'horizontal scroll bar')
//    this.get_elem_wraps_with_any_scroll_bars = this.add_item(this.find_all_menu, 'any scroll bar')
    
    // Spacer.
    this.add_spacer(top_menu)

    // The shift elements menu.
    this.shift_to_text_menu = this.add_sub_menu(top_menu, 'Shift set elements to text')
    this.shift_down_by_text = this.add_item(this.shift_to_text_menu, 'below')
    this.shift_up_by_text = this.add_item(this.shift_to_text_menu, 'above')
    this.shift_left_by_text = this.add_item(this.shift_to_text_menu, 'on left')
    this.shift_right_by_text = this.add_item(this.shift_to_text_menu, 'on right')
    
    this.shift_to_image_menu = this.add_sub_menu(top_menu, 'Shift set elements to image')
    this.shift_down_by_image = this.add_item(this.shift_to_image_menu, 'below')
    this.shift_up_by_image = this.add_item(this.shift_to_image_menu, 'above')
    this.shift_left_by_image = this.add_item(this.shift_to_image_menu, 'on left')
    this.shift_right_by_image = this.add_item(this.shift_to_image_menu, 'on right')
    
    this.shift_to_input_menu = this.add_sub_menu(top_menu, 'Shift set elements to input')
    this.shift_down_by_input = this.add_item(this.shift_to_input_menu, 'below')
    this.shift_up_by_input = this.add_item(this.shift_to_input_menu, 'above')
    this.shift_left_by_input = this.add_item(this.shift_to_input_menu, 'on left')
    this.shift_right_by_input = this.add_item(this.shift_to_input_menu, 'on right')
    
    this.shift_to_selector_menu = this.add_sub_menu(top_menu, 'Shift set elements to selector')
    this.shift_down_by_selector = this.add_item(this.shift_to_selector_menu, 'below')
    this.shift_up_by_selector = this.add_item(this.shift_to_selector_menu, 'above')
    this.shift_left_by_selector = this.add_item(this.shift_to_selector_menu, 'on left')
    this.shift_right_by_selector = this.add_item(this.shift_to_selector_menu, 'on right')
    
    // Spacer.
    this.add_spacer(top_menu)
        
    // The expand menu.
    this.expand_menu = this.add_sub_menu(top_menu, 'Expand set to similar elements')
    this.expand_above = this.add_item(this.expand_menu, 'above us')
    this.expand_below = this.add_item(this.expand_menu, 'below us')
    this.expand_above_and_below = this.add_item(this.expand_menu, 'above and below us')
    this.expand_left = this.add_item(this.expand_menu, 'to the left of us')
    this.expand_right = this.add_item(this.expand_menu, 'to the right of us')
    this.expand_left_and_right = this.add_item(this.expand_menu, 'to the left and right of us')
    
    // Spacer.
    this.add_spacer(top_menu)
    
    // Mark sets.
    this.mark_set = this.add_item(this.top_menu, 'Mark set under mouse (LMB)')
    this.unmark_set = this.add_item(this.top_menu, 'Unmark set under mouse (LMB)')
    
    // The constrain menu.
    this.constrain_menu = this.add_sub_menu(top_menu, 'Constrain set elements to marked set' )
    this.constrain_above = this.add_item(this.constrain_menu, 'above marked set elements')
    this.constrain_below = this.add_item(this.constrain_menu, 'below marked set elements')
    this.constrain_above_and_below = this.add_item(this.constrain_menu, 'above and below marked set elements')
    this.constrain_left = this.add_item(this.constrain_menu, 'left of marked set elements')
    this.constrain_right = this.add_item(this.constrain_menu, 'right of marked set elements')
    this.constrain_left_and_right = this.add_item(this.constrain_menu, 'left and right of makred set elements')
    
    // Distance constraint menu.
    this.distance_menu = this.add_sub_menu(top_menu, 'Constrain all elements by distance' )
    this.distance_closest = this.add_item(this.distance_menu, 'closest elements from each set')
    this.distance_closest = this.add_item(this.distance_menu, 'topmost element')
    this.distance_closest = this.add_item(this.distance_menu, 'bottommost element')
    this.distance_closest = this.add_item(this.distance_menu, 'leftmost element')
    this.distance_closest = this.add_item(this.distance_menu, 'rightmost element')
    
    // Spacer.
    this.spacer = this.add_spacer(this.top_menu)
    
    // Act on element.
    this.perform_menu = this.add_sub_menu(this.top_menu, 'Perform action on element')
    this.perform_click = this.add_item(this.perform_menu, 'click')
    this.perform_type = this.add_item(this.perform_menu, 'type text')
    this.perform_enter = this.add_item(this.perform_menu, 'type enter/submit')
    this.perform_extract = this.add_item(this.perform_menu, 'extract text')
    
    // Spacer.
    this.spacer = this.add_spacer(this.top_menu)
    
    // Delete
    this.delete_set = this.add_item(this.top_menu, 'Delete set under mouse')
    
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

ContextMenu.prototype.element_is_part_of_menu = function(element) {
    if (this.top_menu.contains(element)) {
        return true
    }
    return false
}

ContextMenu.prototype.on_context_menu = function(page_event) {
    this.page_x = page_event.pageX
    this.page_y = page_event.pageY
    this.text_element = g_page_wrap.get_top_text_elem_wrap_at(this.page_x, this.page_y)
    this.image_element = g_page_wrap.get_top_image_elem_wrap_at(this.page_x, this.page_y)
    
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
    console.log('clicked')
    
    // --------------------------------------------------------------
    // Find elements by using specific values.
    // --------------------------------------------------------------
    
    // Find by text.
    if (this.find_by_text.contains(menu_target)) { 
        var text_values = g_page_wrap.get_text_values_at(this.page_x, this.page_y)
        console.log('element xpath: ' + this.text_element.get_xpath())
        console.log('text values: ' + JSON.stringify(text_values))
        // Check that we can find it too.
        var elements = g_page_wrap.get_elem_wraps_by_text_values(text_values)
        console.log('found num elements: ' + elements.length)
        for (var i=0; i<elements.length; i++) {
            console.log('xpath ['+i+']: ' + elements[i].get_xpath())
        }
        g_overlay_sets.add_set(new OverlaySet(elements))
    } 
    
    // Find by image.
    else if (this.find_by_image.contains(menu_target)) {
        var image_values = g_page_wrap.get_image_values_at(this.page_x, this.page_y)
        console.log('element xpath: ' + this.image_element.get_xpath())
        console.log('image values: ' + JSON.stringify(image_values))
        // Check that we can find it too.
        var found = g_page_wrap.get_elem_wraps_by_image_values(image_values)
        console.log('found num found: ' + found.length)
        for (var i=0; i<found.length; i++) {
            console.log('xpath ['+i+']: ' + found[i].get_xpath())
        }
        g_overlay_sets.add_set(new OverlaySet(found))
    } 
    
    // Find by position.
    else if (this.find_by_position.contains(menu_target)) {
        var pos = {x:this.page_x, y:this.page_y}
        console.log('pos: ' + JSON.stringify(pos))
        var top_element = g_page_wrap.get_top_text_or_image_elem_wrap_at(pos.x, pos.y)
        console.log('top elem: ' + top_element.get_xpath())
        g_overlay_sets.add_set(new OverlaySet([top_element]))
    } 
    
    // Find by xpath.
    else if (this.find_by_xpath.contains(menu_target)) {
        var pos = {x:this.page_x, y:this.page_y}
        console.log('pos: ' + JSON.stringify(pos))
        var top_element = g_page_wrap.get_top_text_or_image_elem_wrap_at(pos.x, pos.y)
        console.log('top elem: ' + top_element.get_xpath())
        var xpath = top_element.get_xpath()
        console.log('xpath: ' + xpath)
        var found = g_page_wrap.get_elem_wraps_by_xpath(xpath)
        console.log('num found: ' + found.length)
        for (var i=0; i<found.length; i++) {
            console.log('xpath ['+i+']: ' + found[i].get_xpath())
        }
        g_overlay_sets.add_set(new OverlaySet([top_element]))
    } 
    
    // --------------------------------------------------------------
    // Find all.
    // --------------------------------------------------------------
    
    // Find all inputs.
    else if (this.get_elem_wraps_with_inputs.contains(menu_target)) {
        var elem_wraps = g_page_wrap.get_elem_wraps_with_inputs()
        var msg = "Find elem wraps with inputs: " + elem_wraps.length
        console.log(msg)
        g_overlay_sets.add_set(new OverlaySet(elem_wraps))
    }
    
    // Find all inputs.
    else if (this.get_elem_wraps_with_selects.contains(menu_target)) {
        var elem_wraps = g_page_wrap.get_elem_wraps_with_selects()
        var msg = "Find elem wraps with selects: " + elem_wraps.length
        console.log(msg)
        g_overlay_sets.add_set(new OverlaySet(elem_wraps))
    }
    
    // Find all images.
    else if (this.get_elem_wraps_with_images.contains(menu_target)) {
        var elem_wraps = g_page_wrap.get_elem_wraps_with_images()
        var msg = "Find elem wraps with images: " + elem_wraps.length
        console.log(msg)
        g_overlay_sets.add_set(new OverlaySet(elem_wraps))
    } 
    
    // Find all text.
    else if (this.get_elem_wraps_with_text.contains(menu_target)) {
        var elem_wraps = g_page_wrap.get_elem_wraps_with_text()
        var msg = "Find elem wraps with text: " + elem_wraps.length
        console.log(msg)
        g_overlay_sets.add_set(new OverlaySet(elem_wraps))
    } 
    
    // --------------------------------------------------------------
    // Shift elements.
    // --------------------------------------------------------------
    
    // Text.
    else if (this.shift_up_by_text.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.up, ElemWrap.prototype.wrap_type.text)
    }
    
    else if (this.shift_down_by_text.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.down, ElemWrap.prototype.wrap_type.text)
    }
    
    else if (this.shift_left_by_text.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.left, ElemWrap.prototype.wrap_type.text)
    }
    
    else if (this.shift_right_by_text.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.right, ElemWrap.prototype.wrap_type.text)
    }
    
    // Image.
    else if (this.shift_up_by_image.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.up, ElemWrap.prototype.wrap_type.image)
    }
    
    else if (this.shift_down_by_image.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.down, ElemWrap.prototype.wrap_type.image)
    }
    
    else if (this.shift_left_by_image.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.left, ElemWrap.prototype.wrap_type.image)
    }
    
    else if (this.shift_right_by_image.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.right, ElemWrap.prototype.wrap_type.image)
    }
    
    // Input.
    else if (this.shift_up_by_input.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.up, ElemWrap.prototype.wrap_type.input)
    }
    
    else if (this.shift_down_by_input.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.down, ElemWrap.prototype.wrap_type.input)
    }
    
    else if (this.shift_left_by_input.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.left, ElemWrap.prototype.wrap_type.input)
    }
    
    else if (this.shift_right_by_input.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.right, ElemWrap.prototype.wrap_type.input)
    }
    
    // Select.
    else if (this.shift_up_by_selector.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.up, ElemWrap.prototype.wrap_type.select)
    }
    
    else if (this.shift_down_by_selector.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.down, ElemWrap.prototype.wrap_type.select)
    }
    
    else if (this.shift_left_by_selector.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.left, ElemWrap.prototype.wrap_type.select)
    }
    
    else if (this.shift_right_by_selector.contains(menu_target)) {
        g_overlay_sets.shift(this.page_x, this.page_y, ElemWrap.prototype.direction.right, ElemWrap.prototype.wrap_type.select)
    }
    
    // --------------------------------------------------------------
    // Constrain elements.
    // --------------------------------------------------------------
    
    // Mark set.
    else if (this.mark_set.contains(menu_target)) {
        g_overlay_sets.mark(this.page_x, this.page_y)
    } 
    
    // Unmark set.
    else if (this.unmark_set.contains(menu_target)) {
        g_overlay_sets.unmark(this.page_x, this.page_y)
    } 
    

    
//    // Find all vertical scroll bars.
//    else if (this.get_elem_wraps_with_vertical_scroll_bars.contains(menu_target)){
//        var elem_wraps = g_page_wrap.get_elem_wraps_with_vertical_scroll_bars()
//        var msg = "Find elem_wraps with v scroll bars: " + elem_wraps.length
//        console.log(msg)
//        g_overlay_sets.add_set(new OverlaySet(elem_wraps))
//    }    
    
//    // Find all horizontal scroll bars.
//    else if (this.get_elem_wraps_with_horizontal_scroll_bars.contains(menu_target)){
//        var elem_wraps = g_page_wrap.get_elem_wraps_with_horizontal_scroll_bars()
//        var msg = "Find elem_wraps with h scroll bars: " + elem_wraps.length
//        console.log(msg)
//        g_overlay_sets.add_set(new OverlaySet(elem_wraps))
//    }    
    
//    // Find any scroll bars.
//    else if (this.get_elem_wraps_with_any_scroll_bars.contains(menu_target)){
//        var elem_wraps = g_page_wrap.get_elem_wraps_with_any_scroll_bars()
//        var msg = "Find elem_wraps with any scroll bars: " + elem_wraps.length
//        console.log(msg)
//        g_overlay_sets.add_set(new OverlaySet(elem_wraps))
//    }    
    
    // Delete set.
    else if (this.delete_set.contains(menu_target)) {
        g_overlay_sets.destroy_set(this.page_x, this.page_y)
    }
    
    // Find by position.
    else if (this.find_by_position.contains(menu_target)) {
        var msg = "Find elem_wraps with position, " + this.position
        console.log(msg)
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

var g_context_menu = new ContextMenu()

// The context menu can only be initialized after the dom is loaded because
// it needs access to the window.document in order to add the context menu
// elem_wraps to the dom.
document.addEventListener ('DOMContentLoaded', on_loaded, false);
function on_loaded () {
    g_context_menu.initialize()
    window.document.addEventListener("contextmenu", g_context_menu.on_context_menu.bind(g_context_menu), true)
    g_page_wrap.disable_hover()
}

