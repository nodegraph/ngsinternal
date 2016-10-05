import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Above"
        description: "Shift to text input elements above."
        script: "app_recorder.record_shift_to_inputs_above()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Below"
        description: "Shift to text input elements below."
        script: "app_recorder.record_shift_to_inputs_below()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "On Left"
        description: "Shift to text input elements on left."
        script: "app_recorder.record_shift_to_inputs_on_left()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "On Right"
        description: "Shift to text input elements on right."
        script: "app_recorder.record_shift_to_inputs_on_right()"
    }

}
