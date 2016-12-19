import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Above"
        description: "Shift to drop down fields above."
        script: "web_recorder.record_shift_to_select_type_above();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Below"
        description: "Shift to drop down fields below."
        script: "web_recorder.record_shift_to_select_type_below();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "On Left"
        description: "Shift to drop down fields on left."
        script: "web_recorder.record_shift_to_select_type_on_left();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "On Right"
        description: "Shift to drop down fields on right."
        script: "web_recorder.record_shift_to_select_type_on_right();
        		main_bar.switch_to_current_mode();"
    }

}
