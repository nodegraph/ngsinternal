import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Down"
        description: "Scroll down"
        script: "app_recorder.record_scroll_down()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Up"
        description: "Scroll up."
        script: "app_recorder.record_scroll_up()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Right"
        description: "Scroll right."
        script: "app_recorder.record_scroll_right()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Left"
        description: "Scroll left."
        script: "app_recorder.record_scroll_left()"
    }

}
