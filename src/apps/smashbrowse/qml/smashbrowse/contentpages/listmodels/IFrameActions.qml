import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "By Matching Text"
        description: "Find and switch to an iframe by looking for text."
        script: "web_recorder.record_switch_to_iframe_by_matching_text();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "By Matching Images"
        description: "Find and switch to an iframe by looking for images."
        script: "web_recorder.record_switch_to_iframe_by_matching_images();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "By Matching Position"
        description: "Find and switch to an iframe by looking at its position."
        script: "web_recorder.record_switch_to_iframe_by_matching_positions();
                 main_bar.switch_to_current_mode();"
    }
}


