import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Click"
        description: "Click on an element."
        script: "web_recorder.record_click();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Type Text"
        description: "Type text into a text input element."
        script: "web_menu_list_stack_page.on_type_text();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Press Enter/Submit"
        description: "Type enter or submit into a text input element."
        script: "web_recorder.record_type_enter();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Extract Text"
        description: "Extract text from an element."
        script: "web_recorder.record_extract_text();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Select from Dropdown"
        description: "Select a choice from a dropdown."
        script: "web_menu_list_stack_page.on_select_from_dropdown();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Hover the Mouse"
        description: "Hover the mouse over the crosshair."
        script: "web_recorder.record_mouse_over();
        		main_bar.switch_to_current_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Start Hovering the Mouse"
        description: "Starts hovering the mouse over the crosshair."
        script: "web_recorder.record_start_mouse_hover();
        		main_bar.switch_to_current_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Stop Hovering the Mouse"
        description: "Stops hovering the mouse over the crosshair."
        script: "web_recorder.record_stop_mouse_hover();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Perform Scroll"
        description: "Scroll along a direction."
        script: "push_by_names('Scroll', 'WebMenuListPage', 'WebActionsPerformScroll')"
    }


}
