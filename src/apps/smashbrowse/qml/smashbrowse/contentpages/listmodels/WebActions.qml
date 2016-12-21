import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Navigate..."
        description: "Navigate to another page."
        script: "push_by_names('Navigate', 'WebMenuListPage', 'WebActionsNavigate')"
    }

	ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Find Element By Position..."
        description: "Find an element by position."
        script: "push_by_names('Find Element By Position', 'WebMenuListPage', 'WebActionsFindElementByPosition')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Find Element By Traits..."
        description: "Find an element using traits."
        script: "push_by_names('Find Element By Traits', 'WebMenuListPage', 'WebActionsFindElementByTraits')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Shift Element..."
        description: "Shift to another element."
        script: "push_by_names('Shift Element', 'WebMenuListPage', 'WebActionsShift')"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Perform Action..."
        description: "Perform an action at the current element."
        script: "push_by_names('Perform Action', 'WebMenuListPage', 'WebActionsPerformAction')"
    }

}

