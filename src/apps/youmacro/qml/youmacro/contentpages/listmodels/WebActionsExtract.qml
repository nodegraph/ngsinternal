import QtQuick 2.7

ListModel {

	ListElement {
        image_url: "qrc:///icons/ic_wallpaper_white_48dp.png"
        title: "Download Videos"
        description: "Download videos from the elements in our main input or on this page."
        script: "browser_recorder.record_download_video();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_wallpaper_white_48dp.png"
        title: "Download Images"
        description: "Download images from the elements in our main input."
        script: "browser_recorder.record_download_image();
                 main_bar.switch_to_node_graph();"
    }
    
	//ListElement {
    //    image_url: "qrc:///icons/ic_title_white_48dp.png"
    //    title: "Get Text"
    //    description: "Get the text from the current element."
    //    script: "browser_recorder.record_extract_text();
    //    		main_bar.switch_to_current_mode();"
    //}

    ListElement {
        image_url: "qrc:///icons/ic_link_white_48dp.png"
        title: "Get URL"
        description: "Get the url of the current tab."
        script: "browser_recorder.record_get_url();
                 main_bar.switch_to_node_graph();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_title_white_48dp.png"
        title: "Get Title"
        description: "Get the title of the current tab."
        script: "browser_recorder.record_get_title();
                 main_bar.switch_to_node_graph();"
    }
    
    //ListElement {
    //    letters: "Rz"
    //    title: "Get Browser Size"
    //    description: "Get the current size of the browser."
    //    script: "browser_recorder.record_get_browser_size();
    //             main_bar.switch_to_node_graph();"
    //}

}