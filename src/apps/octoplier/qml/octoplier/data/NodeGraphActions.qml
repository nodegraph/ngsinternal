import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
        if (props["links_are_locked"] == true) {
            get(0).title="Unlock Links"
            get(0).description="Unlocking links allows new link creation and deletion."
            get(0).script="node_graph_page.node_graph.lock_links(false)"
        } else {
            get(0).title="Lock Links"
            get(0).description="Locking links prevents new link creation and deletion."
            get(0).script="node_graph_page.node_graph.lock_links(true)"
        }
    }

    ListElement {
        navigator_title: "Node Graph Actions"

        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Lock Links"
        description: "Locking links prevents new link creation and deletion."
        script: "node_graph_page.node_graph.lock_links(true)"
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


