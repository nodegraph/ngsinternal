import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "get all cookies"
        description: "get all cookies"
        script: "app_comm.get_all_cookies()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "clear all cookies"
        description: "clear all cookies"
        script: "app_comm.clear_all_cookies()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "set all cookies"
        description: "set all cookies"
        script: "app_comm.set_all_cookies()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "update overlays"
        description: "update overlays"
        script: "app_comm.update_overlays()"
    }

}
