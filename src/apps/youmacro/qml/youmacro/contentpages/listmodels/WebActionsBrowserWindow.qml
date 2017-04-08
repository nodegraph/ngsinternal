import QtQuick 2.7

ListModel {
    
    ListElement {
    	letters: "Ot"
        title: "Open New Tab"
        description: "Open a new Tab."
        script: "browser_recorder.record_open_tab();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
    	letters: "Ct"
        title: "Close Current Tab"
        description: "Close the current tab and switch to a previous tab."
        script: "browser_recorder.record_destroy_current_tab();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
    	letters: "Rb"
        title: "Release Browser"
        description: "Release the browser from the app's control."
        script: "browser_recorder.record_release_browser();
                 main_bar.switch_to_node_graph();"
    }
    
    //ListElement {
    //	letters: "Ad"
    //    title: "Accept Save Dialog"
    //    description: "Accepts a Save As dialog, and will confirm overwriting the file if it exists."
    //    script: "browser_recorder.record_accept_save_dialog();
    //             main_bar.switch_to_node_graph();"
    //}
    
    //ListElement {
    //    letters: "Rz"
    //    title: "Resize Browser"
    //    description: "Resize the browser."
    //    script: "browser_recorder.record_resize_browser();
    //             main_bar.switch_to_node_graph();"
    //}
    
    //ListElement {
    //	letters: "Ob"
    //    title: "Open Browser"
    //    description: "Opens a browser window if one is not open already."
    //    script: "browser_recorder.record_open_browser();
    //             main_bar.switch_to_node_graph();"
    //}
    
}