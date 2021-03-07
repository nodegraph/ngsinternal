import QtQuick 2.7

ListModel {

    function update(props) {
    }
        
    ListElement {
    	letters: "Ty"
        title: "Isolate By Type"
        description: "Isolate the elements with a specific type."
        script: "browser_recorder.record_isolate_by_type();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Va"
        title: "Isolate By Value"
        description: "Isolate the elements with a specific value."
        script: "browser_recorder.record_isolate_by_value();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Po"
        title: "Isolate By Position"
        description: "Isolate the elements which contain a specific point."
        script: "browser_recorder.record_isolate_by_position();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Dm"
        title: "Isolate By Dimensions"
        description: "Isolate the elements with specific dimensions with some tolerance."
        script: "browser_recorder.record_isolate_by_dimensions();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Vp"
        title: "Isolate By Viewport"
        description: "Isolate the elements which are in the current viewport."
        script: "browser_recorder.record_isolate_by_viewport();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
        letters: "Si"
        title: "Isolate To One Side..."
        description: "Isolate the elements which are off to one side of the anchor elements."
        script: "push_by_names('Isolate to One Side', 'WebMenuListPage', 'WebActionsIsolateToOneSide')"
    }
    
    ListElement {
        letters: "Bd"
        title: "Isolate Boundary Elements..."
        description: "Isolate boundary elements in the current set of elements."
        script: "push_by_names('Isolate Boundary Elements', 'WebMenuListPage', 'WebActionsIsolateBoundaryElements')"
    }
    
    ListElement {
    	letters: "Di"
        title: "Sort By Distance."
        description: "Sort elements by distance to the specified anchoring elements."
        script: "browser_recorder.record_sort_by_distance_to_anchors();
        		 main_bar.switch_to_last_mode();"
    }
        
    ListElement {
    	letters: "Ix"
        title: "Isolate By Index."
        description: "Isolate elements with indices in a specific range."
        script: "browser_recorder.record_isolate_by_index();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Jo"
        title: "Join Elements."
        description: "Join two arrays of elements into one."
        script: "browser_recorder.record_join_elements();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Pr"
        title: "Prune Elements."
        description: "Prune specific elements."
        script: "browser_recorder.record_prune_elements();
        		 main_bar.switch_to_last_mode();"
    }
    
    ListElement {
    	letters: "Pd"
        title: "Prune Duplicates."
        description: "Prune duplicate elements."
        script: "browser_recorder.record_prune_duplicates();
        		 main_bar.switch_to_last_mode();"
    }

}
