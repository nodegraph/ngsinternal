import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_bubble_chart_white_48dp.png"
        title: "Explode (X)"
        description: "Explode the selected group node into its contents."
        script: "node_graph_item.explode_group();
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }
    ListElement {
        image_url: "qrc:///icons/ic_group_work_white_48dp.png"
        title: "Package (P)"
        description: "Package selected nodes into a new group node."
        script: "node_graph_item.collapse_to_group();
                 main_bar.switch_to_mode(app_settings.node_graph_mode);"
    }

}

