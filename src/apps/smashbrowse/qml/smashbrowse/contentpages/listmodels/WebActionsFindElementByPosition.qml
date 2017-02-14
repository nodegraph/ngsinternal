import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_title_white_48dp.png"
        title: "Text"
        description: "Find the text element at the crosshair."
        script: "browser_recorder.record_find_text_element_by_position();
        		 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_crop_original_white_48dp.png"
        title: "Image"
        description: "Find the image element at the crosshair."
        script: "browser_recorder.record_find_image_element_by_position()
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_text_fields_white_48dp.png"
        title: "Input Field"
        description: "Find the input field at the crosshair."
        script: "browser_recorder.record_find_input_element_by_position();
        		main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_format_line_spacing_white_48dp.png"
        title: "Drop Down Field"
        description: "Find the drop down field at the crosshair."
        script: "browser_recorder.record_find_drop_down_element_by_position();
        		main_bar.switch_to_current_mode();"
    }
    
}
