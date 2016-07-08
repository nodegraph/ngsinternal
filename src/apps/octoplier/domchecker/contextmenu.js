//------------------------------------------------------------------------------------------------
//Smash Browse Context Menu.
//------------------------------------------------------------------------------------------------

//1) The smash_browse prefix prevents collisions with js in the target web page.
//2) Elements are described by their bounds in page space so that they can be passed back and forth
//between javascript and c++. In the javascript environment they can be easily converted back to elements.
//We call these bounds page boxes.
//page_box == [left, right, top, bottom] == [min_x, max_x, min_y, max_y]

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
    this.text_box_overlay = new Overlay('smash_browse_text_box', null)
    this.image_box_overlay = new Overlay('smash_browse_image_box', null)
}

ContextMenu.prototype.create_menu = function (top_menu) {
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
    this.text_element = get_first_text_element_at_point(this.page_x, this.page_y)
    this.image_element = get_first_image_element_at_point(this.page_x, this.page_y)
    
    //console.log('mouse over text: ' + get_xpath(this.text_element))
    //console.log('mouse over image: ' + get_xpath(this.image_element))
    
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
        var elements = find_elements_by_tag_name('img')
        var msg = "Find elements with type img: " + elements.length
        console.log(msg)
        g_selection_sets.create_set(elements)
    } else if (this.find_by_position.contains(menu_target)) {
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
    var text_element2 = get_first_text_element_at_point(page_event.pageX, page_event.pageY)
    var image_element2 = get_first_image_element_at_point(page_event.pageX, page_event.pageY)
    this.update_text_box_overlay(text_element2)
    this.update_image_box_overlay(image_element2)
}

ContextMenu.prototype.update_text_box_overlay = function(element) {
    if (!element) {
        var page_box = [-99, -99, -99, -99]
        this.text_box_overlay.update(page_box)
    } else {
        var client_rect = element.getBoundingClientRect()
        var page_box = get_page_box_from_client_rect(client_rect)
        this.text_box_overlay.update(page_box)
    }
}

ContextMenu.prototype.update_image_box_overlay = function(element) {
    if (!element) {
        var page_box = [-99, -99, -99, -99]
        this.image_box_overlay.update(page_box)
    } else {
        var client_rect = element.getBoundingClientRect()
        var page_box = get_page_box_from_client_rect(client_rect)
        this.image_box_overlay.update(page_box)
    }
}

var g_context_menu = new ContextMenu()

// The context menu can only be initialized after the dom is loaded because
// it needs access to the window.document in order to add the context menu
// elements to the dom.
document.addEventListener ('DOMContentLoaded', on_loaded, false);
function on_loaded () {
    g_context_menu.initialize()
    window.document.addEventListener("contextmenu", g_context_menu.on_context_menu.bind(g_context_menu), true)
    disable_hover()
}

