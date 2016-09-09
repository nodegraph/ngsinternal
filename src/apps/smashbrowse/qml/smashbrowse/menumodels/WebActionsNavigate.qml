import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Navigate..."
        description: "Navigate to a url."
        script: "web_action_stack_page.on_url_entry()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Refresh"
        description: "Refresh web page."
        script: "app_comm.navigate_refresh();
                 main_bar.switch_to_current_mode();"
    }

}
