import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Click"
        description: "Click on an element."
        script: "app_worker.perform_click()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Type Text"
        description: "Type text into a text input element."
        script: "web_action_stack_page.on_type_text()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Press Enter/Submit"
        description: "Type enter or submit into a text input element."
        script: "app_worker.type_enter()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Extract Text"
        description: "Extract text from an element."
        script: "app_worker.extract_text()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Select from Dropdown"
        description: "Select a choice from a dropdown."
        script: "web_action_stack_page.on_select_from_dropdown()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Hover the Mouse"
        description: "Hover the mouse over the crosshair."
        script: "app_worker.perform_mouse_over()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Perform Scroll"
        description: "Scroll along a direction."
        script: "push_by_names('Scroll', 'WebActionPage', 'WebActionsPerformScroll')"
    }


}

//this.perform_click = this.add_item(this.perform_menu, 'click')
//this.perform_type = this.add_item(this.perform_menu, 'type text')
//this.perform_enter = this.add_item(this.perform_menu, 'press enter/submit')
//this.perform_extract = this.add_item(this.perform_menu, 'extract text')
//this.perform_select_option = this.add_item(this.perform_menu, 'select option')
//this.perform_scroll_down = this.add_item(this.perform_menu, 'scroll down')
//this.perform_scroll_up = this.add_item(this.perform_menu, 'scroll up')
//this.perform_scroll_right = this.add_item(this.perform_menu, 'scroll right')
//this.perform_scroll_left = this.add_item(this.perform_menu, 'scroll left')
