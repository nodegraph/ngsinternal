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
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Control Click"
        description: "Control click on an element to open it in a new tab."
        script: "web_recorder.record_ctrl_click();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Type Text"
        description: "Type text into an input field."
        script: "web_menu_list_stack_page.on_type_text();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Press Enter/Submit"
        description: "Type enter or submit into an input field."
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
        title: "Choose from Drop Down"
        description: "Choose an option from a drop down field."
        script: "web_menu_list_stack_page.on_choose_from_dropdown();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Perform Scroll"
        description: "Scroll along a direction."
        script: "push_by_names('Scroll', 'WebMenuListPage', 'WebActionsPerformScroll')"
    }

    // Eventually we want to automatically and persistently hover over the current element.
    // But this hasn't been fully implemented and tested.
    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Hover the Mouse"
        description: "Hovers the mouse over the element."
        script: "web_recorder.record_mouse_over();
                main_bar.switch_to_current_mode();"
    }

}
