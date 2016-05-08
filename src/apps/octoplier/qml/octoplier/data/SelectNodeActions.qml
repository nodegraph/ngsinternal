import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {
    ListElement {
        navigator_title: "Select Nodes"
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Select All"
        description: "Select all nodes in this group."
        script: "node_graph_page.node_graph.select_all()"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Deselect All"
        description: "Deselect all nodes."
        script: "node_graph_page.node_graph.deselect_all()"
    }
    ListElement {
        image_url: "qrc:///icons/ic_settings_white_48dp.png"
        title: "Enable accumulate selection"
        description: "A mode where new selections will accumulate into current selection."
        script: "node_graph_page.node_graph.enable_selection_accumulate()"
    }
}
