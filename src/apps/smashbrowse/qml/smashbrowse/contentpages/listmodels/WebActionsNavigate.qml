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
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Get Current URL"
        description: "Get current url."
        script: "web_recorder.record_get_current_url();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Close Current Tab"
        description: "Close the current tab and switch to a previous tab."
        script: "web_recorder.record_destroy_current_tab();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Open Tab"
        description: "Open Tab."
        script: "web_recorder.record_open_tab();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Download Video"
        description: "Download the largest video file among all the tabs."
        script: "web_recorder.record_download_video();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Release Browser"
        description: "Release the current browser from our control."
        script: "web_recorder.record_release_browser();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Open Browser"
        description: "Opens a browser window if one is not open already."
        script: "web_recorder.record_open_browser();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Get Title"
        description: "Get Current Title of Browser"
        script: "web_recorder.record_get_browser_title();
                 main_bar.switch_to_node_graph();"
    }
}
