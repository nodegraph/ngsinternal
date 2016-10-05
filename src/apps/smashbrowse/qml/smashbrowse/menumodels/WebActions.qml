import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Test..."
        description: "Test Features."
        script: "push_by_names('Test Features', 'WebActionPage', 'WebActionsTest')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Navigate..."
        description: "Navigate to a url."
        script: "push_by_names('Navigate', 'WebActionPage', 'WebActionsNavigate')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Create Set From Values..."
        description: "Create set from values under crosshair."
        script: "push_by_names('Create Set', 'WebActionPage', 'WebActionsCreateByMatchingValues')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Create Set From Types..."
        description: "Create set from element types."
        script: "push_by_names('Create Set', 'WebActionPage', 'WebActionsCreateFromWrapType')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Shift Set Elements..."
        description: "Shift set elements along direction."
        script: "push_by_names('Shift Set Elements', 'WebActionPage', 'WebActionsShift')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Expand Set..."
        description: "Expand set to similar elements."
        script: "push_by_names('Expand Set', 'WebActionPage', 'WebActionsExpand')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Shrink Set..."
        description: "Shrink sets to one side."
        script: "push_by_names('Shrink Set', 'WebActionPage', 'WebActionsShrink')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Mark Sets..."
        description: "Mark and unmark sets."
        script: "push_by_names('Mark Set', 'WebActionPage', 'WebActionsMark')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Shrink Set Against Marked..."
        description: "Shrink sets against other marked sets."
        script: "push_by_names('Shrink Set', 'WebActionPage', 'WebActionsShrinkAgainstMarked')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Delete Set"
        description: "Delete set under crosshair."
        script: "app_recorder.record_delete_set()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Perform Action"
        description: "Perform action on element under crosshair."
        script: "push_by_names('Perform Action', 'WebActionPage', 'WebActionsPerformAction')"
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
