//------------------------------------------------------------------------------------------------
//Smash Browse Context Menu.
//------------------------------------------------------------------------------------------------

//1) The smash_browse prefix prevents collisions with js in the target web page.
//2) Elements are described by their bounds in page space so that they can be passed back and forth
//between javascript and c++. In the javascript environment they can be easily converted back to elements.

class ContextMenu {
    // Our Dependencies.
    handler: ContextMenuHandler
    
    // Our Members.
    top_menu: HTMLMenuElement
    on_click_bound: EventListener
    visible: boolean
    text_box_overlay: Overlay
    image_box_overlay: Overlay
    page_pos: Point
    
    // Menu Elements.
    test2: HTMLLIElement
    test3: HTMLLIElement
    test4: HTMLLIElement
    test5: HTMLLIElement
    
    // Navigate Menu.
    navigate_menu: HTMLMenuElement
    navigate_to_url: HTMLLIElement
    navigate_back: HTMLLIElement
    navigate_forward: HTMLLIElement
    navigate_refresh: HTMLLIElement
    
    // The create set by matching menu.
    create_set_by_matching_menu: HTMLMenuElement
    create_set_by_matching_text: HTMLLIElement
    create_set_by_matching_image: HTMLLIElement
    create_set_by_matching_position: HTMLLIElement
    create_set_by_matching_xpath: HTMLLIElement
    
    // The create set by type menu.
    create_set_by_type_menu: HTMLMenuElement
    create_set_from_inputs: HTMLLIElement
    create_set_from_selects: HTMLLIElement
    create_set_from_texts: HTMLLIElement
    create_set_from_images: HTMLLIElement
    
    // Delete
    delete_set: HTMLLIElement

    // The shift elements menu.
    shift_to_text_menu: HTMLMenuElement
    shift_up_by_text: HTMLLIElement
    shift_down_by_text: HTMLLIElement
    shift_left_by_text: HTMLLIElement
    shift_right_by_text: HTMLLIElement

    shift_to_image_menu: HTMLMenuElement
    shift_up_by_image: HTMLLIElement
    shift_down_by_image: HTMLLIElement
    shift_left_by_image: HTMLLIElement
    shift_right_by_image: HTMLLIElement

    shift_to_input_menu: HTMLMenuElement
    shift_up_by_input: HTMLLIElement
    shift_down_by_input: HTMLLIElement
    shift_left_by_input: HTMLLIElement
    shift_right_by_input: HTMLLIElement

    shift_to_selector_menu: HTMLMenuElement
    shift_up_by_select: HTMLLIElement
    shift_down_by_select: HTMLLIElement
    shift_left_by_select: HTMLLIElement
    shift_right_by_select: HTMLLIElement

    // The expand menu.
    expand_menu: HTMLMenuElement
    expand_above: HTMLLIElement
    expand_below: HTMLLIElement
    expand_left: HTMLLIElement
    expand_right: HTMLLIElement

    // Mark sets.
    mark_set: HTMLLIElement
    unmark_set: HTMLLIElement

    // Merge marked sets.
    merge_marked_sets: HTMLLIElement

    // The shrink to marked menu.
    shrink_set_to_marked_menu: HTMLMenuElement
    shrink_set_above_marked: HTMLLIElement
    shrink_set_below_marked: HTMLLIElement
    shrink_set_above_and_below_marked: HTMLLIElement
    shrink_set_left_of_marked: HTMLLIElement
    shrink_set_right_of_marked: HTMLLIElement
    shrink_set_left_and_right_of_marked: HTMLLIElement

    // The shrink down all sets menu.
    shrink_set_menu: HTMLMenuElement
    shrink_set_to_topmost: HTMLLIElement
    shrink_set_to_bottommost: HTMLLIElement
    shrink_set_to_leftmost: HTMLLIElement
    shrink_set_to_rightmost: HTMLLIElement

    // Act on element.
    perform_menu: HTMLMenuElement
    perform_click: HTMLLIElement
    perform_type: HTMLLIElement
    perform_enter: HTMLLIElement
    perform_extract: HTMLLIElement
    perform_select_option: HTMLLIElement
    perform_scroll_down: HTMLLIElement
    perform_scroll_up: HTMLLIElement
    perform_scroll_right: HTMLLIElement
    perform_scroll_left: HTMLLIElement

    // Cancel.
    cancel: HTMLLIElement
    
    
    constructor() {
        
        // Dependencies.
        
        
        // Members.
        this.top_menu = null
        this.on_click_bound = null
        this.visible = false
    }

    set_handler(handler: ContextMenuHandler) {
        this.handler = handler
    }

    get_page_pos(): Point {
        return this.page_pos
    }

    initialized(): boolean {
        if (this.top_menu) {
            return true
        }
        return false
    }

    initialize(): void {
        if (this.initialized()) {
            return
        }

        // Create the top level menu element.
        this.top_menu = document.createElement("menu")
        this.top_menu.classList.add('smash_browse_menu')
        document.body.appendChild(this.top_menu)

        // Bind the click handler to ourself.
        this.on_click_bound = this.on_click.bind(this)

        // Create the menu as dom elements.
        this.create_menu(this.top_menu)

        // Create our mouse overlays.
        this.text_box_overlay = new Overlay('smash_browse_text_box', DistinctColors.text_color, -1, false, null)
        this.image_box_overlay = new Overlay('smash_browse_image_box', DistinctColors.image_color, -1, false, null)
    }

    create_menu(top_menu: HTMLMenuElement): void {

        this.test2 = this.add_item(this.top_menu, 'TEST get all cookies')
        this.test3 = this.add_item(this.top_menu, 'TEST clear all cookies')
        this.test4 = this.add_item(this.top_menu, 'TEST set all cookies')
        this.test5 = this.add_item(this.top_menu, 'TEST update overlays')

        // The navigate menu.
        this.navigate_menu = this.add_sub_menu('Navigate')
        this.navigate_to_url = this.add_item(this.navigate_menu, 'to url')
        //    this.navigate_back = this.add_item(this.navigate_menu, 'back')
        //    this.navigate_forward = this.add_item(this.navigate_menu, 'forward')
        this.navigate_refresh = this.add_item(this.navigate_menu, 'refresh')

        // Spacer.
        this.add_spacer(top_menu)

        // The create set by matching menu.
        this.create_set_by_matching_menu = this.add_sub_menu('Create set from values under mouse')
        this.create_set_by_matching_text = this.add_item(this.create_set_by_matching_menu, 'text')
        this.create_set_by_matching_image = this.add_item(this.create_set_by_matching_menu, 'image')
        //    this.create_set_by_matching_position = this.add_item(this.create_set_by_matching_menu, 'position')
        //    this.create_set_by_matching_xpath = this.add_item(this.create_set_by_matching_, 'xpath')
    
        // The create set by type menu.
        this.create_set_by_type_menu = this.add_sub_menu('Create set from elements of type')
        this.create_set_from_inputs = this.add_item(this.create_set_by_type_menu, 'has an input (text field)')
        this.create_set_from_selects = this.add_item(this.create_set_by_type_menu, 'has a select (drop down)')
        this.create_set_from_texts = this.add_item(this.create_set_by_type_menu, 'has text')
        this.create_set_from_images = this.add_item(this.create_set_by_type_menu, 'has an image')

        // Delete
        this.delete_set = this.add_item(this.top_menu, 'Delete set')

        // Spacer.
        this.add_spacer(top_menu)

        // The shift elements menu.
        this.shift_to_text_menu = this.add_sub_menu('Shift set elements to text')
        this.shift_up_by_text = this.add_item(this.shift_to_text_menu, 'above')
        this.shift_down_by_text = this.add_item(this.shift_to_text_menu, 'below')
        this.shift_left_by_text = this.add_item(this.shift_to_text_menu, 'on left')
        this.shift_right_by_text = this.add_item(this.shift_to_text_menu, 'on right')

        this.shift_to_image_menu = this.add_sub_menu('Shift set elements to images')
        this.shift_up_by_image = this.add_item(this.shift_to_image_menu, 'above')
        this.shift_down_by_image = this.add_item(this.shift_to_image_menu, 'below')
        this.shift_left_by_image = this.add_item(this.shift_to_image_menu, 'on left')
        this.shift_right_by_image = this.add_item(this.shift_to_image_menu, 'on right')

        this.shift_to_input_menu = this.add_sub_menu('Shift set elements to inputs')
        this.shift_up_by_input = this.add_item(this.shift_to_input_menu, 'above')
        this.shift_down_by_input = this.add_item(this.shift_to_input_menu, 'below')
        this.shift_left_by_input = this.add_item(this.shift_to_input_menu, 'on left')
        this.shift_right_by_input = this.add_item(this.shift_to_input_menu, 'on right')

        this.shift_to_selector_menu = this.add_sub_menu('Shift set elements to selects')
        this.shift_up_by_select = this.add_item(this.shift_to_selector_menu, 'above')
        this.shift_down_by_select = this.add_item(this.shift_to_selector_menu, 'below')
        this.shift_left_by_select = this.add_item(this.shift_to_selector_menu, 'on left')
        this.shift_right_by_select = this.add_item(this.shift_to_selector_menu, 'on right')

        // Spacer.
        this.add_spacer(top_menu)

        // The expand menu.
        this.expand_menu = this.add_sub_menu('Expand set to similar elements')
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
        this.shrink_set_to_marked_menu = this.add_sub_menu('Shrink set w.r.t. marked sets')
        this.shrink_set_above_marked = this.add_item(this.shrink_set_to_marked_menu, 'above marked set element')
        this.shrink_set_below_marked = this.add_item(this.shrink_set_to_marked_menu, 'below marked set elements')
        this.shrink_set_above_and_below_marked = this.add_item(this.shrink_set_to_marked_menu, 'above and below marked set elements')
        this.shrink_set_left_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'left of marked set elements')
        this.shrink_set_right_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'right of marked set elements')
        this.shrink_set_left_and_right_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'left and right of marked set elements')

        // The shrink down all sets menu.
        this.shrink_set_menu = this.add_sub_menu('Shrink set to')
        this.shrink_set_to_topmost = this.add_item(this.shrink_set_menu, 'topmost element')
        this.shrink_set_to_bottommost = this.add_item(this.shrink_set_menu, 'bottommost element')
        this.shrink_set_to_leftmost = this.add_item(this.shrink_set_menu, 'leftmost element')
        this.shrink_set_to_rightmost = this.add_item(this.shrink_set_menu, 'rightmost element')

        // Spacer.
        this.add_spacer(this.top_menu)

        // Act on element.
        this.perform_menu = this.add_sub_menu('Perform action on set with one element')
        this.perform_click = this.add_item(this.perform_menu, 'click')
        this.perform_type = this.add_item(this.perform_menu, 'type text')
        this.perform_enter = this.add_item(this.perform_menu, 'press enter/submit')
        this.perform_extract = this.add_item(this.perform_menu, 'extract text')
        this.perform_select_option = this.add_item(this.perform_menu, 'select option')
        this.perform_scroll_down = this.add_item(this.perform_menu, 'scroll down')
        this.perform_scroll_up = this.add_item(this.perform_menu, 'scroll up')
        this.perform_scroll_right = this.add_item(this.perform_menu, 'scroll right')
        this.perform_scroll_left = this.add_item(this.perform_menu, 'scroll left')

        // Spacer.
        this.add_spacer(this.top_menu)

        // Cancel.
        this.cancel = this.add_item(this.top_menu, 'Cancel')
    }

    add_item(menu: HTMLMenuElement, item_text: string): HTMLLIElement {
        let li = document.createElement('li')
        li.classList.add('smash_browse_menu_item')
        menu.appendChild(li)

        let button = document.createElement('button')
        button.classList.add('smash_browse_menu_button')
        button.type = 'button'
        li.appendChild(button)

        let span = document.createElement('span')
        span.classList.add('smash_browse_menu_text')
        button.appendChild(span)

        let text = document.createTextNode(item_text)
        span.appendChild(text)
        return li
    }

    add_sub_menu(item_text: string): HTMLMenuElement {
        let li = document.createElement('li')
        li.classList.add('smash_browse_menu_item')
        li.classList.add('smash_browse_submenu')
        this.top_menu.appendChild(li)

        let button = document.createElement('button')
        button.classList.add('smash_browse_menu_button')
        button.type = 'button'
        li.appendChild(button)

        let span = document.createElement('span')
        span.classList.add('smash_browse_menu_text')
        button.appendChild(span)

        let text = document.createTextNode(item_text)
        span.appendChild(text)

        let menu = document.createElement("menu");
        menu.classList.add('smash_browse_menu')
        li.appendChild(menu)
        return menu
    }

    remove_sub_menu(menu: HTMLMenuElement, sub_menu: HTMLMenuElement): void {
        menu.removeChild(sub_menu)
    }

    add_spacer(menu: HTMLMenuElement): HTMLLIElement {
        let li = document.createElement('li')
        li.classList.add('smash_browse_menu_separator')
        menu.appendChild(li)
        return li
    }

    disable_item(item: HTMLLIElement): void {
        item.classList.add('smash_browse_disabled')
    }

    enable_item(item: HTMLLIElement): void {
        item.classList.remove('smash_browse_disabled')
    }

    show(point: Point): void {
        if (!this.initialized()) {
            this.initialize()
        }

        this.top_menu.style.left = point.x + 'px'
        this.top_menu.style.top = point.y + 'px'
        this.top_menu.classList.add('smash_browse_menu_show')
        this.visible = true
    }

    hide(): void {
        if (!this.initialized()) {
            return
        }
        this.top_menu.classList.remove('smash_browse_menu_show')
        this.visible = false
    }

    contains_element(element: Node): boolean {
        if (this.top_menu && this.top_menu.contains(element)) {
            return true
        }
        return false
    }
    
    on_context_menu(page_event: MouseEvent, text_values: string[], image_values: string[]): boolean {
        this.page_pos = new Point({x: page_event.pageX, y: page_event.pageY})

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
        this.show(this.page_pos)

        // Listen to clicks to perform the according action and close the menu.
        document.addEventListener('click', this.on_click_bound, true);

        page_event.preventDefault();
        return false
    }

    on_click(menu_event: MouseEvent): void {
        this.hide();
        this.handler.handle_menu_click(menu_event)
        document.removeEventListener('click', this.on_click_bound, true)
    }

    on_mouse_over(text_elem_wrap: ElemWrap, image_elem_wrap: ElemWrap): void {
        if (!this.text_box_overlay || !this.image_box_overlay) {
            return
        }
        this.update_text_box_overlay(text_elem_wrap)
        this.update_image_box_overlay(image_elem_wrap)
    }

    update_text_box_overlay(elem_wrap: ElemWrap): void {
        this.text_box_overlay.elem_wrap = elem_wrap
        this.text_box_overlay.update(DistinctColors.text_color, -1)
    }

    update_image_box_overlay(elem_wrap: ElemWrap): void {
        this.image_box_overlay.elem_wrap = elem_wrap
        this.image_box_overlay.update(DistinctColors.image_color, -1)
    }

}





