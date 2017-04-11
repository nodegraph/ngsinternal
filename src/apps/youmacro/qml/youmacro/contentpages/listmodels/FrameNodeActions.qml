import QtQuick 2.7
import GUITypes 1.0

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_crop_free_white_48dp.png"
        title: "Frame All Nodes (F)"
        description: "Frame all nodes to fit the screen."
        script: "node_graph_item.frame_all();
                 main_bar.switch_to_last_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_filter_center_focus_white_48dp.png"
        title: "Frame Selected Nodes (CTRL+F)"
        description: "Frame selected nodes to fit the screen."
        script: "node_graph_item.frame_selected();
                 main_bar.switch_to_last_mode();"
    }
}
