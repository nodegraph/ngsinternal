import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "get all cookies"
        description: "get all cookies"
        script: "web_worker.queue_get_all_cookies()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "clear all cookies"
        description: "clear all cookies"
        script: "web_worker.queue_clear_all_cookies()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "set all cookies"
        description: "set all cookies"
        script: "web_worker.queue_set_all_cookies()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "update overlays"
        description: "update overlays"
        script: "web_worker.queue_update_overlays()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "open browser"
        description: "Open the browser."
        script: "web_worker.queue_check_browser_is_open()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "close browser"
        description: "Close the browser."
        script: "web_worker.queue_close_browser()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "test busy"
        description: "Close the browser."
        script: "web_menu_list_stack_page.on_busy()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Reset Browser"
        description: "Reset and Restart the Browser."
        script: "web_worker.queue_reset()"
    }

}
