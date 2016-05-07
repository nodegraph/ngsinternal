import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {
    ListElement {
        navigator_title: "Node Actions"
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Toggle Selected State"
        description: "Toggle the selected state of the item under your screen press."
        script: "node_graph_page.node_graph.toggle_selection_under_long_press()"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "View"
        description: "View the output data from this node."
        next_model: "qrc:///qml/octoplier/data/NodeTypeChoices.qml"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Edit"
        description: "Edit the parameters of this node"
        next_model: "qrc:///qml/octoplier/data/NodeTypeChoices.qml"
    }
}

