import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Topmost Element"
        description: "Shrink set to topmost element."
        script: "web_recorder.record_shrink_set_to_topmost();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Bottommost Element"
        description: "Shrink set to bottommost element."
        script: "web_recorder.record_shrink_set_to_bottommost();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Leftmost Element"
        description: "Shrink set to leftmost element."
        script: "web_recorder.record_shrink_set_to_leftmost();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Rightmost Element"
        description: "Shrink set to rightmost element."
        script: "web_recorder.record_shrink_set_to_rightmost();
        		main_bar.switch_to_current_mode();"
    }

}
