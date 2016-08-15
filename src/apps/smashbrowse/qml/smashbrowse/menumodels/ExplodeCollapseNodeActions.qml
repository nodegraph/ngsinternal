import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    ListElement {
        image_url: "qrc:///icons/ic_zoom_out_map_white_48dp.png"
        title: "Explode"
        description: "Explode the selected group node into its contents."
        script: "node_graph_page.node_graph.explode_group();
                 main_bar.switch_to_current_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Collapse"
        description: "Collapse selected nodes into a new group node."
        script: "node_graph_page.node_graph.collapse_to_group();
                 main_bar.switch_to_current_mode();"
    }

}

