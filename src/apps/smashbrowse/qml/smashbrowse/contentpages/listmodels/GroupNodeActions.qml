import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_arrow_downward_white_48dp.png"
        title: "Dive"
        description: "Dive into the group node."
        script: "ng_controller.dive();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "View"
        description: "View the output data from the group node."
        script: "node_graph_item.view_node();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Edit"
        description: "Edit the parameters of the group node"
        script: "node_graph_item.edit_node();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Clean"
        description: "Clean this group node."
        script: "node_graph_item.clean_node();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Reclean"
        description: "Clean this group node."
        script: "node_graph_item.reclean_node();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Dirty"
        description: "Dirty this group node."
        script: "node_graph_item.dirty_node();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Select"
        description: "Select this node"
        script: "node_graph_item.select_last_press();
                 main_bar.switch_to_current_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_border_clear_white_48dp.png"
        title: "Deselect"
        description: "Deselect this node."
        script: "node_graph_item.deselect_last_press();
                 main_bar.switch_to_current_mode();"
    }
}
