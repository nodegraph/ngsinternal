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
    	letters: "Ft"
        title: "By Type"
        description: "Filter the elements by the element type."
        script: "browser_recorder.record_filter_by_type();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fv"
        title: "By Value"
        description: "Filter the elements by the element value."
        script: "browser_recorder.record_filter_by_value();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fp"
        title: "By Position"
        description: "Filter the elements by whether they contain the specified point."
        script: "browser_recorder.record_filter_by_position();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fd"
        title: "By Dimensions"
        description: "Filter the elements by their dimensions."
        script: "browser_recorder.record_filter_by_dimensions();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fv"
        title: "By Viewport"
        description: "Filter the elements by whether they are overlapping the current viewport."
        script: "browser_recorder.record_filter_by_viewport();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fc"
        title: "Find closest element."
        description: "Find closest element to the specified anchoring elements."
        script: "browser_recorder.record_find_closest();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fl"
        title: "Filter to leftmost elements."
        description: "Filter the elements to only the leftmost elements."
        script: "browser_recorder.record_filter_to_leftmost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fr"
        title: "Filter to rightmost elements."
        description: "Filter the elements to only the rightmost elements."
        script: "browser_recorder.record_filter_to_rightmost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Ft"
        title: "Filter to topmost elements."
        description: "Filter the elements to only the topmost elements."
        script: "browser_recorder.record_filter_to_topmost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fb"
        title: "Filter to bottommost elements."
        description: "Filter the elements to only the bottommost elements."
        script: "browser_recorder.record_filter_to_bottommost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Ie"
        title: "Filter by index."
        description: "Isolate elements with indices in the specified range."
        script: "browser_recorder.record_filter_by_index();
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
