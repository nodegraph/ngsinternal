import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Test Features"
        description: "Test Features"
        script: "push_by_names('Test Features', 'WebActionPage', 'WebActionsTest')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "222"
        description: "Edit the parameters of this node"
        script: "node_graph_item.edit_node();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "333"
        description: "Select this node"
        script: "node_graph_item.select_last_press();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "444"
        description: "Deselect this node."
        script: "node_graph_item.deselect_last_press();
                 main_bar.switch_to_current_mode();"
    }

}





//// The navigate menu.
//this.navigate_menu = this.add_sub_menu('Navigate')
//this.navigate_to_url = this.add_item(this.navigate_menu, 'to url')
////    this.navigate_back = this.add_item(this.navigate_menu, 'back')
////    this.navigate_forward = this.add_item(this.navigate_menu, 'forward')
//this.navigate_refresh = this.add_item(this.navigate_menu, 'refresh')

//// Spacer.
//this.add_spacer(top_menu)

//// The create set by matching menu.
//this.create_set_by_matching_menu = this.add_sub_menu('Create set from values under mouse')
//this.create_set_by_matching_text = this.add_item(this.create_set_by_matching_menu, 'text')
//this.create_set_by_matching_image = this.add_item(this.create_set_by_matching_menu, 'image')
////    this.create_set_by_matching_position = this.add_item(this.create_set_by_matching_menu, 'position')
////    this.create_set_by_matching_xpath = this.add_item(this.create_set_by_matching_, 'xpath')

//// The create set by type menu.
//this.create_set_by_type_menu = this.add_sub_menu('Create set from elements of type')
//this.create_set_from_inputs = this.add_item(this.create_set_by_type_menu, 'has an input (text field)')
//this.create_set_from_selects = this.add_item(this.create_set_by_type_menu, 'has a select (drop down)')
//this.create_set_from_texts = this.add_item(this.create_set_by_type_menu, 'has text')
//this.create_set_from_images = this.add_item(this.create_set_by_type_menu, 'has an image')

//// Delete
//this.delete_set = this.add_item(this.top_menu, 'Delete set')

//// Spacer.
//this.add_spacer(top_menu)

//// The shift elements menu.
//this.shift_to_text_menu = this.add_sub_menu('Shift set elements to text')
//this.shift_up_by_text = this.add_item(this.shift_to_text_menu, 'above')
//this.shift_down_by_text = this.add_item(this.shift_to_text_menu, 'below')
//this.shift_left_by_text = this.add_item(this.shift_to_text_menu, 'on left')
//this.shift_right_by_text = this.add_item(this.shift_to_text_menu, 'on right')

//this.shift_to_image_menu = this.add_sub_menu('Shift set elements to images')
//this.shift_up_by_image = this.add_item(this.shift_to_image_menu, 'above')
//this.shift_down_by_image = this.add_item(this.shift_to_image_menu, 'below')
//this.shift_left_by_image = this.add_item(this.shift_to_image_menu, 'on left')
//this.shift_right_by_image = this.add_item(this.shift_to_image_menu, 'on right')

//this.shift_to_input_menu = this.add_sub_menu('Shift set elements to inputs')
//this.shift_up_by_input = this.add_item(this.shift_to_input_menu, 'above')
//this.shift_down_by_input = this.add_item(this.shift_to_input_menu, 'below')
//this.shift_left_by_input = this.add_item(this.shift_to_input_menu, 'on left')
//this.shift_right_by_input = this.add_item(this.shift_to_input_menu, 'on right')

//this.shift_to_selector_menu = this.add_sub_menu('Shift set elements to selects')
//this.shift_up_by_select = this.add_item(this.shift_to_selector_menu, 'above')
//this.shift_down_by_select = this.add_item(this.shift_to_selector_menu, 'below')
//this.shift_left_by_select = this.add_item(this.shift_to_selector_menu, 'on left')
//this.shift_right_by_select = this.add_item(this.shift_to_selector_menu, 'on right')

//// Spacer.
//this.add_spacer(top_menu)

//// The expand menu.
//this.expand_menu = this.add_sub_menu('Expand set to similar elements')
//this.expand_above = this.add_item(this.expand_menu, 'above')
//this.expand_below = this.add_item(this.expand_menu, 'below')
//this.expand_left = this.add_item(this.expand_menu, 'on left')
//this.expand_right = this.add_item(this.expand_menu, 'on right')

//// Spacer.
//this.add_spacer(top_menu)

//// Mark sets.
//this.mark_set = this.add_item(this.top_menu, 'Mark set')
//this.unmark_set = this.add_item(this.top_menu, 'Unmark set')

//// Merge marked sets.
//this.merge_marked_sets = this.add_item(this.top_menu, 'Merge marked sets')

//// The shrink to marked menu.
//this.shrink_set_to_marked_menu = this.add_sub_menu('Shrink set w.r.t. marked sets')
//this.shrink_set_above_marked = this.add_item(this.shrink_set_to_marked_menu, 'above marked set element')
//this.shrink_set_below_marked = this.add_item(this.shrink_set_to_marked_menu, 'below marked set elements')
//this.shrink_set_above_and_below_marked = this.add_item(this.shrink_set_to_marked_menu, 'above and below marked set elements')
//this.shrink_set_left_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'left of marked set elements')
//this.shrink_set_right_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'right of marked set elements')
//this.shrink_set_left_and_right_of_marked = this.add_item(this.shrink_set_to_marked_menu, 'left and right of marked set elements')

//// The shrink down all sets menu.
//this.shrink_set_menu = this.add_sub_menu('Shrink set to')
//this.shrink_set_to_topmost = this.add_item(this.shrink_set_menu, 'topmost element')
//this.shrink_set_to_bottommost = this.add_item(this.shrink_set_menu, 'bottommost element')
//this.shrink_set_to_leftmost = this.add_item(this.shrink_set_menu, 'leftmost element')
//this.shrink_set_to_rightmost = this.add_item(this.shrink_set_menu, 'rightmost element')

//// Spacer.
//this.add_spacer(this.top_menu)

//// Act on element.
//this.perform_menu = this.add_sub_menu('Perform action on set with one element')
//this.perform_click = this.add_item(this.perform_menu, 'click')
//this.perform_type = this.add_item(this.perform_menu, 'type text')
//this.perform_enter = this.add_item(this.perform_menu, 'press enter/submit')
//this.perform_extract = this.add_item(this.perform_menu, 'extract text')
//this.perform_select_option = this.add_item(this.perform_menu, 'select option')
//this.perform_scroll_down = this.add_item(this.perform_menu, 'scroll down')
//this.perform_scroll_up = this.add_item(this.perform_menu, 'scroll up')
//this.perform_scroll_right = this.add_item(this.perform_menu, 'scroll right')
//this.perform_scroll_left = this.add_item(this.perform_menu, 'scroll left')
