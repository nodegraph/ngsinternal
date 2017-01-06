import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Similar Text Element"
        description: "Shift to a neighboring text element with the same text."
        script: "web_recorder.record_shift_to_text_values();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Similar Image Element"
        description: "Shift to a neighboring image element with the same images."
        script: "web_recorder.record_shift_to_image_values();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Text Element"
        description: "Shift to a neighboring text element in the direction specified by the crosshair."
        script: "web_recorder.record_shift_to_text_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Image Element"
        description: "Shift to a neighboring image element in the direction specified by the crosshair."
        script: "web_recorder.record_shift_to_image_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Input Field Element"
        description: "Shift to a neighboring input field in the direction specified by the crosshair."
        script: "web_recorder.record_shift_to_input_type();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Drop Down Field Element"
        description: "Shift to a neighboring drop down field in the direction specified by the crosshair."
        script: "web_recorder.record_shift_to_select_type();
        		main_bar.switch_to_current_mode();"
    }

}


