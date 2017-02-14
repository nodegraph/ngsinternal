import QtQuick 2.6

ListModel {

	ListElement {
        image_url: "qrc:///icons/ic_wallpaper_white_48dp.png"
        title: "Download Video From the Current Element"
        description: "Download the best quality video and audio from the current element's link."
        script: "browser_recorder.record_download_video(true);
                 main_bar.switch_to_node_graph();"
    }
    
   	ListElement {
        image_url: "qrc:///icons/ic_wallpaper_white_48dp.png"
        title: "Download Video From Page"
        description: "Download the the best quality video and audio from the current page."
        script: "browser_recorder.record_download_video(false);
                 main_bar.switch_to_node_graph();"
    }

	ListElement {
        image_url: "qrc:///icons/ic_title_white_48dp.png"
        title: "Get Text"
        description: "Get the text from the current element."
        script: "browser_recorder.record_extract_text();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_link_white_48dp.png"
        title: "Get Current URL"
        description: "Get current url."
        script: "browser_recorder.record_get_current_url();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_title_white_48dp.png"
        title: "Get Current Title"
        description: "Get Current Title of Browser"
        script: "browser_recorder.record_get_browser_title();
                 main_bar.switch_to_node_graph();"
    }
    

}