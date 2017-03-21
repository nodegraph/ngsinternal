import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    ListElement {
        image_url: "qrc:///icons/ic_navigation_white_48dp.png"
        title: "Navigate..."
        description: "Navigate to another page."
        script: "push_by_names('Navigate', 'WebMenuListPage', 'WebActionsNavigate')"
    }

	ListElement {
        image_url: "qrc:///icons/ic_picture_in_picture_white_48dp.png"
        title: "Find and Filter Elements..."
        description: "Find and Filter Elements from the current web page."
        script: "push_by_names('Filter Elements', 'NodeMenuListPage', 'FilterElements')"
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

