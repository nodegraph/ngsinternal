import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    property var title: "Node Actions"

    function update(props) {}

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "View"
        description: "View the output data from this node."
        script: "node_graph_page.node_graph.view_node(); menu_stack_page.visible = false"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Edit"
        description: "Edit the parameters of this node"
        script: "node_graph_page.node_graph.edit_node(); menu_stack_page.visible = false"
    }

}

