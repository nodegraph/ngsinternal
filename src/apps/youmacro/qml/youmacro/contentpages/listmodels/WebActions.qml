import QtQuick 2.7

// The first element contains the title to display in the stack view navigator.

ListModel {

    ListElement {
        image_url: "qrc:///icons/ic_navigation_white_48dp.png"
        title: "Navigate..."
        description: "Navigate to another page."
        script: "push_by_names('Navigate', 'WebMenuListPage', 'WebActionsNavigate')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_location_searching_white_48dp.png"
        title: "Get All Elements"
        description: "Get all elements on the current page."
        script: "browser_recorder.record_get_all_elements();
        		 main_bar.switch_to_current_mode();"
    }
    
	ListElement {
        image_url: "qrc:///icons/ic_picture_in_picture_white_48dp.png"
        title: "Filter Elements..."
        description: "Filter the current set of elements."
        script: "push_by_names('Filter Elements', 'NodeMenuListPage', 'WebActionsFilterElements')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_star_border_white_48dp.png"
        title: "Highlight Elements"
        description: "Highlight the current set of elements in the browser."
        script: "browser_recorder.record_highlight_elements();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_star_border_white_48dp.png"
        title: "Clear Element Highlights"
        description: "Clear the current set of element highlights in the browser."
        script: "browser_recorder.record_clear_element_highlights();
        		 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_gavel_white_48dp.png"
        title: "Perform Element Actions..."
        description: "Perform an action on the current element."
        script: "push_by_names('Perform Action', 'WebMenuListPage', 'WebActionsPerformAction')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_gavel_white_48dp.png"
        title: "Perform Browser/Tab Actions..."
        description: "Perform an action on the browser and its tabs."
        script: "push_by_names('Perform Action', 'NodeMenuListPage', 'WebActionsBrowserWindow')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_format_list_bulleted_white_48dp.png"
        title: "Extract Text, Video and Images"
        description: "Extract text, videos and images from the current element or browser window."
        script: "push_by_names('Perform Action', 'WebMenuListPage', 'WebActionsExtract')"
    }
    
}

