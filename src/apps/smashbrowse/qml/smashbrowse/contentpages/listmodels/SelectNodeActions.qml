import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Select All"
        description: "Select all nodes in this group."
        script: "node_graph_item.select_all();
                 main_bar.switch_to_current_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_crop_16_9_white_48dp.png"
        title: "Deselect All"
        description: "Deselect all nodes."
        script: "node_graph_item.deselect_all();
                 main_bar.switch_to_current_mode();"
    }
}
