import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Topmost Element"
        description: "Shrink set to topmost element."
        script: "app_recorder.record_shrink_set_to_topmost()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Bottommost Element"
        description: "Shrink set to bottommost element."
        script: "app_recorder.record_shrink_set_to_bottommost()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Leftmost Element"
        description: "Shrink set to leftmost element."
        script: "app_recorder.record_shrink_set_to_leftmost()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Rightmost Element"
        description: "Shrink set to rightmost element."
        script: "app_recorder.record_shrink_set_to_rightmost()"
    }

}
