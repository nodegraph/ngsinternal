import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    ListElement {
        navigator_title: "Node Graph Actions"

        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Lock Node Graph"
        description: "Lock node graph, preventing any modifications."
        script: "node_graph_page.node_graph.surface()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Surface"
        description: "Surface out of the current group."
        script: "node_graph_page.node_graph.surface()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Create..."
        description: "Create a new node."
        next_model: "CreateNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Frame..."
        description: "Frame nodes so that they fill the screen."
        next_model: "FrameNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Select..."
        description: "Select or deselect all nodes."
        next_model: "SelectNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Copy and Paste..."
        description: "Copy, cut, paste or destroy nodes."
        next_model: "CopyPasteNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Explode and Collapse..."
        description: "Explode group nodes or collapse some nodes."
        next_model: "ExplodeCollapseNodeActions"
    }

}


