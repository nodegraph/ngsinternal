import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Navigate..."
        description: "Navigate to another web page."
        script: "push_by_names('Navigate', 'WebMenuListPage', 'WebActionsNavigate')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Find First Element..."
        description: "Find the first element."
        script: "push_by_names('Find First Element', 'WebMenuListPage', 'WebActionsFindFirstElement')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Shift Element..."
        description: "Shift the current element."
        script: "push_by_names('Shift Set Elements', 'WebMenuListPage', 'WebActionsShift')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Perform Action"
        description: "Perform an action at the current element."
        script: "push_by_names('Perform Action', 'WebMenuListPage', 'WebActionsPerformAction')"
    }

}


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
