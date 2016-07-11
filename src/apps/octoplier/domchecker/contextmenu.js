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
    this.find_all_inputs = this.add_item(this.find_all_menu, 'input')
    this.get_elem_wraps_with_text = this.add_item(this.find_all_menu, 'text')
    this.get_elem_wraps_with_images = this.add_item(this.find_all_menu, 'image')
    
    // Spacer.
    this.add_spacer(top_menu)

    // The shift next text menu.
    this.shift_next_text_menu = this.add_sub_menu(top_menu, 'Shift set elements to next text')
    this.shift_down_next_text = this.add_item(this.shift_next_text_menu, 'down')
    this.shift_up_next_text = this.add_item(this.shift_next_text_menu, 'up')
    this.shift_left_next_text = this.add_item(this.shift_next_text_menu, 'left')
    this.shift_right_next_text = this.add_item(this.shift_next_text_menu, 'right')
    
    // The shift next image menu.
    this.shift_next_image_menu = this.add_sub_menu(top_menu, 'Shift set elements to next image')
    this.shift_down_next_image = this.add_item(this.shift_next_image_menu, 'down')
    this.shift_up_next_image = this.add_item(this.shift_next_image_menu, 'up')
    this.shift_left_next_image = this.add_item(this.shift_next_image_menu, 'left')
    this.shift_right_next_image = this.add_item(this.shift_next_image_menu, 'right')
    
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
    
    if (this.text_element) {
        console.log('mouse over text: ' + this.text_element.get_xpath())
    }
    if (this.image_element) {
        console.log('mouse over image: ' + this.image_element.get_xpath())
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
    console.log('clicked')
    
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
        var elements = g_page_wrap.get_elem_wraps_by_image_values(image_values)
        console.log('found num elements: ' + elements.length)
        for (var i=0; i<elements.length; i++) {
            console.log('xpath ['+i+']: ' + elements[i].get_xpath())
        }
        g_overlay_sets.add_set(new OverlaySet(elements))
    } 
    
    // Mark set.
    else if (this.mark_set.contains(menu_target)) {
        g_overlay_sets.mark(this.page_x, this.page_y)
    } 
    
    // Unmark set.
    else if (this.unmark_set.contains(menu_target)) {
        g_overlay_sets.unmark(this.page_x, this.page_y)
    } 
    
    // Find all images.
    else if (this.get_elem_wraps_with_images.contains(menu_target)) {
        var elements = g_page_wrap.get_elem_wraps_with_images()
        var msg = "Find elements with type img: " + elements.length
        console.log(msg)
        g_overlay_sets.add_set(new OverlaySet(elements))
    } 
    
    // Find all text.
    else if (this.get_elem_wraps_with_text.contains(menu_target)) {
        var elements = g_page_wrap.get_elem_wraps_with_text()
        var msg = "Find elements with type img: " + elements.length
        console.log(msg)
        g_overlay_sets.add_set(new OverlaySet(elements))
    } 
    
    // Delete set.
    else if (this.delete_set.contains(menu_target)) {
        g_overlay_sets.destroy_set(this.page_x, this.page_y)
    }
    
    // Find by position.
    else if (this.find_by_position.contains(menu_target)) {
        var msg = "Find elements with position, " + this.position
        console.log(msg)
    }
    document.removeEventListener('click', this.on_click_bound, true);
    
    // Remove previous overlays.
    //this.destroy_selected_overlays()
    
    // Create current overlays.
    //this.create_selected_overlays()
}

ContextMenu.prototype.on_mouse_over = function(page_event) {
    var text_element2 = g_page_wrap.get_top_text_elem_wrap_at(page_event.pageX, page_event.pageY)
    var image_element2 = g_page_wrap.get_top_image_elem_wrap_at(page_event.pageX, page_event.pageY)
    
    if (text_element2) {
        console.log('text element: ' + text_element2.get_xpath())
    }
//    if (image_element2) {
//        console.log('image element: ' + image_element2.get_xpath())
//    }
    
    this.update_text_box_overlay(text_element2)
    this.update_image_box_overlay(image_element2)
}

ContextMenu.prototype.update_text_box_overlay = function(element_wrapper) {
    this.text_box_overlay.update_page_box_from_element(element_wrapper)
}

ContextMenu.prototype.update_image_box_overlay = function(element_wrapper) {
    this.image_box_overlay.update_page_box_from_element(element_wrapper)
}

var g_context_menu = new ContextMenu()

// The context menu can only be initialized after the dom is loaded because
// it needs access to the window.document in order to add the context menu
// elements to the dom.
document.addEventListener ('DOMContentLoaded', on_loaded, false);
function on_loaded () {
    g_context_menu.initialize()
    window.document.addEventListener("contextmenu", g_context_menu.on_context_menu.bind(g_context_menu), true)
    g_page_wrap.disable_hover()
}

