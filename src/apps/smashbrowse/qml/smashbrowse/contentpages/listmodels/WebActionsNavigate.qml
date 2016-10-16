import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Navigate..."
        description: "Navigate to a url."
        script: "web_menu_list_stack_page.on_url_entry()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Refresh"
        description: "Refresh web page."
        script: "web_recorder.record_navigate_refresh();
                 main_bar.switch_to_current_mode();"
    }

}
