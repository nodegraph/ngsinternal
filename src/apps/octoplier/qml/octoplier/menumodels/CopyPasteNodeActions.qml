import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    property var title: "Copy and Paste"

    function update(props) {}

    ListElement {
        image_url: "qrc:///icons/ic_content_copy_white_48dp.png"
        title: "Copy"
        description: "Copy all the selected nodes."
        script: "node_graph_page.node_graph.copy(); menu_stack_page.visible = false"
    }
    ListElement {
        image_url: "qrc:///icons/ic_content_cut_white_48dp.png"
        title: "Cut"
        description: "Cut all the selected nodes."
        script: "node_graph_page.node_graph.cut(); menu_stack_page.visible = false"
    }
    ListElement {
        image_url: "qrc:///icons/ic_content_paste_white_48dp.png"
        title: "Paste"
        description: "Paste nodes copied from the last copy."
        script: "node_graph_page.node_graph.paste(center_new_nodes); menu_stack_page.visible = false"
    }
}

