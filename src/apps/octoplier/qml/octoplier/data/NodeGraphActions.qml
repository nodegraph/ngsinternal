import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    ListElement {
        navigator_title: "Node Graph Actions"
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Lock Node Graph"
        description: "Lock node graph, preventing accidently modifications."
        script: "node_graph_page.node_graph.surface()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Surface"
        description: "Surface out of the current group and into our parenting group."
        script: "node_graph_page.node_graph.surface()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Create Node..."
        description: "Create a new node."
        next_model: "CreateNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Frame Nodes..."
        description: "Frame the selected nodes so that they fill the screen."
        next_model: "FrameNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Select Nodes..."
        description: "Selection settings and choices to select or deselect all nodes."
        next_model: "SelectNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Copy Paste Nodes..."
        description: "Copy, cut, paste or destroy nodes."
        next_model: "CopyPasteNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Explode Collapse Nodes..."
        description: "Explode group nodes or collapse nodes."
        next_model: "ExplodeCollapseNodeActions"
    }

}


