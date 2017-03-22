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
        title: "Filter By Type"
        description: "Filter the elements by the element type."
        script: "browser_recorder.record_filter_by_type();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fv"
        title: "Filter By Value"
        description: "Filter the elements by the element value."
        script: "browser_recorder.record_filter_by_value();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fp"
        title: "Filter By Position"
        description: "Filter the elements by whether they contain the specified point."
        script: "browser_recorder.record_filter_by_position();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fd"
        title: "Filter By Dimensions"
        description: "Filter the elements by their dimensions."
        script: "browser_recorder.record_filter_by_dimensions();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fv"
        title: "Filter By Viewport"
        description: "Filter the elements by whether they are overlapping the current viewport."
        script: "browser_recorder.record_filter_by_viewport();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Sc"
        title: "Sort By Distance."
        description: "Sort elements by distance to the specified anchoring elements."
        script: "browser_recorder.record_find_closest();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fl"
        title: "Filter By Left Edge."
        description: "Filter the elements by whether they lie on the left edge."
        script: "browser_recorder.record_filter_to_leftmost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fr"
        title: "Filter By Right Edge."
        description: "Filter the elements by whether they lie on the right edge."
        script: "browser_recorder.record_filter_to_rightmost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Ft"
        title: "Filter By Top Edge."
        description: "Filter the elements by whether they lie on the top edge."
        script: "browser_recorder.record_filter_to_topmost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fb"
        title: "Filter By Bottom Edge."
        description: "Filter the elements by whether they lie on the bottom edge."
        script: "browser_recorder.record_filter_to_bottommost();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Fi"
        title: "Filter By Index."
        description: "Isolate elements with indices in the specified range."
        script: "browser_recorder.record_filter_by_index();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
    	letters: "Je"
        title: "Join Elements."
        description: "Join two sets of elements together."
        script: "browser_recorder.record_join_elements();
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
