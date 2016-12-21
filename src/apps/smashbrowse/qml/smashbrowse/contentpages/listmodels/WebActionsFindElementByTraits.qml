import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Using Text Under Crosshair"
        description: "Find the top-leftmost element containing the text under the crosshair."
        script: "web_recorder.record_find_element_by_text();
        		 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Using Images Under Crosshair"
        description: "Find the top-leftmost element containing the images under the crosshair."
        script: "web_recorder.record_find_element_by_images()
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "With Any Text"
        description: "Find the top-leftmost text element."
        script: "web_recorder.record_find_element_by_text_type();
        		main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "With Any Image"
        description: "Find the top-leftmost image element."
        script: "web_recorder.record_find_element_by_image_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "With Any Input Field"
        description: "Find the top-leftmost input field."
        script: "web_recorder.record_find_element_by_input_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "With Any Drop Down Field"
        description: "Find the top-leftmost drop down field."
        script: "web_recorder.record_find_element_by_select_type();
        		main_bar.switch_to_current_mode();"
    }
}
