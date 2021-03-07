import QtQuick 2.7

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_keyboard_arrow_down_white_48dp.png"
        title: "Down"
        description: "Scroll down"
        script: "browser_recorder.record_scroll_down();
        		main_bar.switch_to_last_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_keyboard_arrow_up_white_48dp.png"
        title: "Up"
        description: "Scroll up."
        script: "browser_recorder.record_scroll_up();
        		main_bar.switch_to_last_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_keyboard_arrow_right_white_48dp.png"
        title: "Right"
        description: "Scroll right."
        script: "browser_recorder.record_scroll_right();
        		main_bar.switch_to_last_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_keyboard_arrow_left_white_48dp.png"
        title: "Left"
        description: "Scroll left."
        script: "browser_recorder.record_scroll_left();
        		main_bar.switch_to_last_mode();"
    }

}
