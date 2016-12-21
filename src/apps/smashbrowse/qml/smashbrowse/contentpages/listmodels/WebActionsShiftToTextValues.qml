import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Above"
        description: "Shift to similar text element above."
        script: "web_recorder.record_shift_to_text_values_above();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Below"
        description: "Shift to similar text element below."
        script: "web_recorder.record_shift_to_text_values_below();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "On Left"
        description: "Shift to similar text element on left."
        script: "web_recorder.record_shift_to_text_values_on_left();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "On Right"
        description: "Shift to similar text element on right."
        script: "web_recorder.record_shift_to_text_values_on_right();
        		main_bar.switch_to_current_mode();"
    }

}

