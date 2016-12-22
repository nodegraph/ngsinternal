import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "URL..."
        description: "Go to the web page at the specified URL."
        script: "web_menu_list_stack_page.on_url_entry()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Back"
        description: "Go back in the browser history."
        script: "web_recorder.record_navigate_back();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Forward"
        description: "Go forward in the browser history."
        script: "web_recorder.record_navigate_forward();
                 main_bar.switch_to_node_graph();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Refresh"
        description: "Refresh the current web page."
        script: "web_recorder.record_navigate_refresh();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Resize"
        description: "Resize the browser."
        script: "web_recorder.record_resize_browser();
                 main_bar.switch_to_node_graph();"
    }

}
