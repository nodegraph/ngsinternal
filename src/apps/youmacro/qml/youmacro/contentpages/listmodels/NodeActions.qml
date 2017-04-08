import QtQuick 2.7

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_format_list_bulleted_white_48dp.png"
        title: "View (V)"
        description: "View the output data from this node."
        script: "node_graph_item.view_node();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_format_list_numbered_white_48dp.png"
        title: "Edit (E)"
        description: "Edit the parameters of this node"
        script: "node_graph_item.edit_node();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Rename"
        description: "Rename this node."
		script: "ng_menu_list_stack_page.rename_node()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_check_white_48dp.png"
        title: "Clean (C)"
        description: "Clean this node."
        script: "node_graph_item.clean_node();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_refresh_white_48dp.png"
        title: "Reclean (R)"
        description: "Reclean this node."
        script: "node_graph_item.reclean_node();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_close_white_48dp.png"
        title: "Dirty (D)"
        description: "Dirty this node."
        script: "node_graph_item.dirty_node();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Select (CTRL+LMB)"
        description: "Select this node"
        script: "node_graph_item.select_last_press();
                 main_bar.switch_to_current_mode();"
    }
    ListElement {
        image_url: "qrc:///icons/ic_crop_16_9_white_48dp.png"
        title: "Deselect (CTRL+LMB)"
        description: "Deselect this node."
        script: "node_graph_item.deselect_last_press();
                 main_bar.switch_to_current_mode();"
    }

}

