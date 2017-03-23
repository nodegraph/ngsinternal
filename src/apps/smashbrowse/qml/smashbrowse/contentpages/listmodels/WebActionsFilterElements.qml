import QtQuick 2.6

ListModel {

    function update(props) {
    }
    
    ListElement {
        letters: "Fa"
        title: "Find All Elements"
        description: "Find all elements on the page."
        script: "browser_recorder.record_get_all_elements();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Tp"
        title: "Isolate By Type"
        description: "Isolate the elements with a specific type."
        script: "browser_recorder.record_isolate_by_type();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Vl"
        title: "Isolate By Value"
        description: "Isolate the elements with a specific value."
        script: "browser_recorder.record_isolate_by_value();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Pn"
        title: "Isolate By Position"
        description: "Isolate the elements which contain a specific point."
        script: "browser_recorder.record_isolate_by_position();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Dm"
        title: "Isolate By Dimensions"
        description: "Isolate the elements with specific dimensions with some tolerance."
        script: "browser_recorder.record_isolate_by_dimensions();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Vp"
        title: "Isolate By Viewport"
        description: "Isolate the elements which are in the current viewport."
        script: "browser_recorder.record_isolate_by_viewport();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Di"
        title: "Sort By Distance."
        description: "Sort elements by distance to the specified anchoring elements."
        script: "browser_recorder.record_sort_by_distance_to_anchors();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Le"
        title: "Isolate Leftmost."
        description: "Isolate the leftmost elements."
        script: "browser_recorder.record_isolate_leftmost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Re"
        title: "Isolate Rightmost."
        description: "Isolate the rightmost elements."
        script: "browser_recorder.record_isolate_rightmost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Te"
        title: "Isolate Topmost."
        description: "Isolate the topmost elements."
        script: "browser_recorder.record_isolate_topmost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Be"
        title: "Isolate Bottommost."
        description: "Isolate the bottommost elements."
        script: "browser_recorder.record_isolate_bottommost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Le"
        title: "Isolate On Left."
        description: "Isolate elements to the left of anchor elements."
        script: "browser_recorder.record_isolate_on_left();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Re"
        title: "Isolate On Right."
        description: "Isolate elements to the right of anchor elements."
        script: "browser_recorder.record_isolate_on_right();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Te"
        title: "Isolate On Top."
        description: "Isolate elements above anchor elements."
        script: "browser_recorder.record_isolate_on_top();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Be"
        title: "Isolate On Bottom."
        description: "Isolate elements below anchor elements."
        script: "browser_recorder.record_isolate_on_bottom();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Ix"
        title: "Isolate By Index."
        description: "Isolate elements with indices in a specific range."
        script: "browser_recorder.record_isolate_by_index();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Jo"
        title: "Join Elements."
        description: "Join two arrays of elements into one."
        script: "browser_recorder.record_join_elements();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Pr"
        title: "Prune Elements."
        description: "Prune specific elements."
        script: "browser_recorder.record_prune_elements();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Pd"
        title: "Prune Duplicates."
        description: "Prune duplicate elements."
        script: "browser_recorder.record_prune_duplicates();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "He"
        title: "Highlight Elements"
        description: "Highlight the elements with a border in the browser."
        script: "browser_recorder.record_highlight_elements();
        		 main_bar.switch_to_current_mode();"
    }
}
