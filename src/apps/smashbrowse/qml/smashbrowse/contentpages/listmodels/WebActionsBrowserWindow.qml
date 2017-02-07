import QtQuick 2.6

ListModel {
    
	ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Resize"
        description: "Resize the browser."
        script: "web_recorder.record_resize_browser();
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
        title: "Accept Save Dialog"
        description: "Accepts a Save As dialog, and will confirm overwriting the file if it exists."
        script: "web_recorder.record_accept_save_dialog();
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
    
}