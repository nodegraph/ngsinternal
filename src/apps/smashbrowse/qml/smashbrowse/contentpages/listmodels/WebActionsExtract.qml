import QtQuick 2.6

ListModel {

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Get Current URL"
        description: "Get current url."
        script: "web_recorder.record_get_current_url();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Get Current Title"
        description: "Get Current Title of Browser"
        script: "web_recorder.record_get_browser_title();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Download Video"
        description: "Download the largest video file among all the tabs."
        script: "web_recorder.record_download_video();
                 main_bar.switch_to_node_graph();"
    }

}