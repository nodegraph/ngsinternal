import QtQuick 2.7

ListModel {

    function update(props) {
    }

    ListElement {
    	image_url: "qrc:///icons/ic_arrow_back_white_48dp.png"
        title: "Isolate Leftmost."
        description: "Isolate the leftmost elements."
        script: "browser_recorder.record_isolate_leftmost();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	image_url: "qrc:///icons/ic_arrow_forward_white_48dp.png"
        title: "Isolate Rightmost."
        description: "Isolate the rightmost elements."
        script: "browser_recorder.record_isolate_rightmost();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	image_url: "qrc:///icons/ic_arrow_upward_white_48dp.png"
        title: "Isolate Topmost."
        description: "Isolate the topmost elements."
        script: "browser_recorder.record_isolate_topmost();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	image_url: "qrc:///icons/ic_arrow_downward_white_48dp.png"
        title: "Isolate Bottommost."
        description: "Isolate the bottommost elements."
        script: "browser_recorder.record_isolate_bottommost();
        		 main_bar.switch_to_last_mode();"
    }


}