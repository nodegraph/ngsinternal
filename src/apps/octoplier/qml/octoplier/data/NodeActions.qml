import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {}

    ListElement {
        navigator_title: "Node Actions"
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "View"
        description: "View the output data from this node."
        next_model: "CreateNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Edit"
        description: "Edit the parameters of this node"
        next_model: "CreateNodeActions"
    }

}

