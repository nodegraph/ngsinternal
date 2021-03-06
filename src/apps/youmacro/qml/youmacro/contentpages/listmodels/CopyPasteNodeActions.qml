import QtQuick 2.7

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_content_copy_white_48dp.png"
        title: "Copy"
        description: "Copy all the selected nodes."
        script: "node_graph_item.copy();
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_content_cut_white_48dp.png"
        title: "Cut"
        description: "Cut all the selected nodes."
        script: "node_graph_item.cut();
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_content_paste_white_48dp.png"
        title: "Paste"
        description: "Paste nodes copied from the last copy."
        script: "node_graph_item.paste(center_new_nodes);
                 main_bar.switch_to_last_mode();"
    }
}

