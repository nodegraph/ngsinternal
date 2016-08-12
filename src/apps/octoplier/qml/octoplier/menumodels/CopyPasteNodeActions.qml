import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {}

    ListElement {
        image_url: "qrc:///icons/ic_content_copy_white_48dp.png"
        title: "Copy"
        description: "Copy all the selected nodes."
        script: "node_graph_page.node_graph.copy();
                 main_bar.switch_to_current_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_content_cut_white_48dp.png"
        title: "Cut"
        description: "Cut all the selected nodes."
        script: "node_graph_page.node_graph.cut();
                 main_bar.switch_to_current_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_content_paste_white_48dp.png"
        title: "Paste"
        description: "Paste nodes copied from the last copy."
        script: "node_graph_page.node_graph.paste(center_new_nodes);
                 main_bar.switch_to_current_mode();"
    }
}

