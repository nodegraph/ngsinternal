import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {}

    ListElement {
        navigator_title: "Group Node Actions"

        image_url: "qrc:///icons/ic_arrow_downward_white_48dp.png"
        title: "Dive"
        description: "Dive into the group node."
        script: "node_graph_page.node_graph.dive(); menu_stack_page.visible = false"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Explode"
        description: "Explode the group node."
        script: "node_graph_page.node_graph.explode_group(); menu_stack_page.visible = false"
    }

    ListElement {
        navigator_title: "Group Node Actions"
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "View"
        description: "View the output data from the group node."
        next_model: "CreateNodeActions"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Edit"
        description: "Edit the parameters of the group node"
        next_model: "CreateNodeActions"
    }

}
