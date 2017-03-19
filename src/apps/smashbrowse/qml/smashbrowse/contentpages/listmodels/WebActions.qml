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
        description: "Find an element by position."
        script: "push_by_names('Filter Elements', 'NodeMenuListPage', 'FilterElements')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_picture_in_picture_alt_white_48dp.png"
        title: "Find Element By Traits..."
        description: "Find an element using traits."
        script: "push_by_names('Find Element By Traits', 'WebMenuListPage', 'WebActionsFindElementByTraits')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_arrow_forward_white_48dp.png"
        title: "Shift Element..."
        description: "Shift to another element."
        script: "push_by_names('Shift Element', 'WebMenuListPage', 'WebActionsShift')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_arrow_forward_white_48dp.png"
        title: "Shift Element Along Rows..."
        description: "Shift to the next element on this row or the following rows."
        script: "push_by_names('Shift Element Along Rows', 'WebMenuListPage', 'WebActionsShiftAlongRows')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_arrow_forward_white_48dp.png"
        title: "Shift Element Along Columns..."
        description: "Shift to the next element on this column."
        script: "push_by_names('Shift Element Along Columns', 'WebMenuListPage', 'WebActionsShiftAlongColumns')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_gavel_white_48dp.png"
        title: "Perform Action on Element..."
        description: "Perform an action on the current element."
        script: "push_by_names('Perform Action', 'WebMenuListPage', 'WebActionsPerformAction')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_format_list_bulleted_white_48dp.png"
        title: "Extract Text, Video and Images"
        description: "Extract text, videos and images from the current element or browser window."
        script: "push_by_names('Perform Action', 'WebMenuListPage', 'WebActionsExtract')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_gavel_white_48dp.png"
        title: "Perform Action on Browser Window..."
        description: "Perform an action on the browser."
        script: "push_by_names('Perform Action', 'NodeMenuListPage', 'WebActionsBrowserWindow')"
    }
    
}

