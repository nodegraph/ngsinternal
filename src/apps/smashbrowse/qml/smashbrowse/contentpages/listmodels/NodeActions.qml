import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "View"
        description: "View the output data from this node."
        script: "node_graph_item.view_node();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Edit"
        description: "Edit the parameters of this node"
        script: "node_graph_item.edit_node();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_build_white_48dp.png"
        title: "Rename"
        description: "Rename a node."
		script: "ng_menu_list_stack_page.rename_node()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Clean"
        description: "Clean this node and all upstream nodes."
        script: "node_graph_item.clean_node();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Dirty"
        description: "Dirty this node and all downstream nodes."
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

