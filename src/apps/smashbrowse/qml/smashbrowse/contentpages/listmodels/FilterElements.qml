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
    	letters: "He"
        title: "Highlight Elements"
        description: "Highlight the elements with a border in the browser."
        script: "browser_recorder.record_highlight_elements();
        		 main_bar.switch_to_current_mode();"
    }
}
