import QtQuick 2.6

ListModel {

	ListElement {
        image_url: "qrc:///icons/ic_wallpaper_white_48dp.png"
        title: "Download Video"
        description: "Download the best quality video and audio from the elements in the main input or on this page."
        script: "browser_recorder.record_download_video();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_wallpaper_white_48dp.png"
        title: "Download Image"
        description: "Download images from the elements in the main input."
        script: "browser_recorder.record_download_image();
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
    
    ListElement {
        letters: "Rz"
        title: "Get Browser Size"
        description: "Get the current size of the browser."
        script: "browser_recorder.record_get_browser_size();
                 main_bar.switch_to_node_graph();"
    }

}