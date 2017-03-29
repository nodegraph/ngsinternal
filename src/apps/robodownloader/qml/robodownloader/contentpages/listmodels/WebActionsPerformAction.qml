import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_mouse_white_48dp.png"
        title: "Click"
        description: "Click on an element."
        script: "browser_recorder.record_click();
        		main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_mouse_white_48dp.png"
        title: "Control Click"
        description: "Control click on an element to open it in a new tab."
        script: "browser_recorder.record_ctrl_click();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_keyboard_white_48dp.png"
        title: "Type Text"
        description: "Type text into an input field."
        script: "web_menu_list_stack_page.on_type_text();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_keyboard_white_48dp.png"
        title: "Press Enter/Submit"
        description: "Type enter or submit into an input field."
        script: "browser_recorder.record_type_enter();
        		main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_keyboard_white_48dp.png"
        title: "Type Password"
        description: "Type a password into an input field."
        script: "web_menu_list_stack_page.on_type_password();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_format_line_spacing_white_48dp.png"
        title: "Choose from Drop Down"
        description: "Choose an option from a drop down field."
        script: "web_menu_list_stack_page.on_choose_from_dropdown();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mouse_white_48dp.png"
        title: "Perform Scroll"
        description: "Scroll along a direction."
        script: "push_by_names('Scroll', 'WebMenuListPage', 'WebActionsPerformScroll')"
    }

    // Eventually we want to automatically and persistently hover over the current element.
    // But this hasn't been fully implemented and tested.
    ListElement {
        image_url: "qrc:///icons/ic_mouse_white_48dp.png"
        title: "Hover the Mouse"
        description: "Hovers the mouse over the element."
        script: "browser_recorder.record_mouse_over();
                main_bar.switch_to_current_mode();"
    }

}
