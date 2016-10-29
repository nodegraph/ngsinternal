import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Down"
        description: "Scroll down"
        script: "web_recorder.record_scroll_down();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Up"
        description: "Scroll up."
        script: "web_recorder.record_scroll_up();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Right"
        description: "Scroll right."
        script: "web_recorder.record_scroll_right();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Left"
        description: "Scroll left."
        script: "web_recorder.record_scroll_left();
        		main_bar.switch_to_current_mode();"
    }

}
