import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Above"
        description: "Shift to dropdown elements above."
        script: "app_worker.shift_to_selects_above()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Below"
        description: "Shift to dropdown elements below."
        script: "app_worker.shift_to_selects_below()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "On Left"
        description: "Shift to dropdown elements on left."
        script: "app_worker.shift_to_selects_on_left()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "On Right"
        description: "Shift to dropdown elements on right."
        script: "app_worker.shift_to_selects_on_right()"
    }

}
