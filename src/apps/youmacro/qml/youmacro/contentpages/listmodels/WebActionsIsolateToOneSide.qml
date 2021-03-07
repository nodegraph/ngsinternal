import QtQuick 2.7

ListModel {

    function update(props) {
    }

    ListElement {
    	image_url: "qrc:///icons/ic_arrow_back_white_48dp.png"
        title: "Isolate On Left."
        description: "Isolate elements to the left of anchor elements."
        script: "browser_recorder.record_isolate_on_left();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	image_url: "qrc:///icons/ic_arrow_forward_white_48dp.png"
        title: "Isolate On Right."
        description: "Isolate elements to the right of anchor elements."
        script: "browser_recorder.record_isolate_on_right();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	image_url: "qrc:///icons/ic_arrow_upward_white_48dp.png"
        title: "Isolate On Top."
        description: "Isolate elements above anchor elements."
        script: "browser_recorder.record_isolate_on_top();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	image_url: "qrc:///icons/ic_arrow_downward_white_48dp.png"
        title: "Isolate On Bottom."
        description: "Isolate elements below anchor elements."
        script: "browser_recorder.record_isolate_on_bottom();
        		 main_bar.switch_to_last_mode();"
    }
    
    
}