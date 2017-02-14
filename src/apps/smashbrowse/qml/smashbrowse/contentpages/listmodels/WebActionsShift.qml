import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_title_white_48dp.png"
        title: "To Element With Same Text"
        description: "Shift to a neighboring element with the same text."
        script: "browser_recorder.record_shift_to_text_values();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_crop_original_white_48dp.png"
        title: "To Element With Same Image"
        description: "Shift to a neighboring element with the same image."
        script: "browser_recorder.record_shift_to_image_values();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_title_white_48dp.png"
        title: "To Text Element"
        description: "Shift to a neighboring text element in the direction specified by the crosshair."
        script: "browser_recorder.record_shift_to_text_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_crop_original_white_48dp.png"
        title: "To Image Element"
        description: "Shift to a neighboring image element in the direction specified by the crosshair."
        script: "browser_recorder.record_shift_to_image_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_text_fields_white_48dp.png"
        title: "To Input Field Element"
        description: "Shift to a neighboring input field in the direction specified by the crosshair."
        script: "browser_recorder.record_shift_to_input_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_format_line_spacing_white_48dp.png"
        title: "To Drop Down Field Element"
        description: "Shift to a neighboring drop down field in the direction specified by the crosshair."
        script: "browser_recorder.record_shift_to_select_type();
        		main_bar.switch_to_current_mode();"
    }

}


