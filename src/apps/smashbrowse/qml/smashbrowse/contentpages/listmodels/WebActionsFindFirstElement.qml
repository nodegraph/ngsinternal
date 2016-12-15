import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "With Specific Text"
        description: "Find the first element with the text under the crosshair."
        script: "web_recorder.record_find_element_by_text();
        		 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "With Specific Images"
        description: "Find the first element with the images under the crosshair."
        script: "web_recorder.record_find_element_by_images()
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "With Any Text"
        description: "Find the first element that has any text."
        script: "web_recorder.record_find_element_by_text_type();
        		main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "With Any Image"
        description: "Find the first element that has any image."
        script: "web_recorder.record_find_element_by_image_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "With Any Input"
        description: "Find the first element that has any input field"
        script: "web_recorder.record_find_element_by_input_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "With Any Select"
        description: "Find the first element that has any select/dropdown combo box."
        script: "web_recorder.record_find_element_by_select_type();
        		main_bar.switch_to_current_mode();"
    }

}
