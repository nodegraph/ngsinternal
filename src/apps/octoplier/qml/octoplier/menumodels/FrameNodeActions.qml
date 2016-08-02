import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    property var title: "Frame"

    function update(props) {}

    ListElement {
        image_url: "qrc:///icons/ic_crop_free_white_48dp.png"
        title: "Frame All Nodes"
        description: "Frame all nodes to fit the screen."
        script: "node_graph_page.node_graph.frame_all(); menu_stack_page.visible = false"
    }
    ListElement {
        image_url: "qrc:///icons/ic_filter_center_focus_white_48dp.png"
        title: "Frame Selected Nodes"
        description: "Frame selected nodes to fit the screen."
        script: "node_graph_page.node_graph.frame_selected(); menu_stack_page.visible = false"
    }
}
