import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        //image_url: "qrc:///icons/ic_arrow_upward_white_48dp.png"
        image_url: "qrc:///icons/ic_arrow_upward_white_48dp.png"
        title: "Surface"
        description: "Surface out of the current group."
        script: "ng_controller.surface_from_group();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_add_circle_outline_white_48dp.png"
        title: "Add"
        description: "Add a new node."
        script: "push_by_names('Create', 'NodeMenuListPage', 'CreateNodeActions')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_remove_circle_outline_white_48dp.png"
        title: "Remove"
        description: "Remove the selected nodes."
        script: "node_graph_item.destroy_selection();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_check_white_48dp.png"
        title: "Clean Current Group"
        description: "Clean all the output nodes in the current group."
        script: "node_graph_item.clean_group();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_refresh_white_48dp.png"
        title: "Reclean Current Group"
        description: "Reclean all the output nodes in the current group."
        script: "node_graph_item.reclean_group();
                 main_bar.switch_to_current_mode();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_close_white_48dp.png"
        title: "Dirty Current Group"
        description: "Dirty all the nodes in this group and below."
        script: "node_graph_item.dirty_group_recursively();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_crop_free_white_48dp.png"
        title: "Frame..."
        description: "Frame nodes so that they fill the screen."
        script: "push_by_names('Frame', 'NGMenuListPage', 'FrameNodeActions')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_select_all_white_48dp.png"
        title: "Select..."
        description: "Select or deselect all nodes."
        script: "push_by_names('Select', 'NGMenuListPage', 'SelectNodeActions')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_content_copy_white_48dp.png"
        title: "Copy and Paste..."
        description: "Copy, cut, and paste nodes."
        script: "push_by_names('Copy and Paste', 'NGMenuListPage', 'CopyPasteNodeActions')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_bubble_chart_white_48dp.png"
        title: "Explode and Collapse..."
        description: "Explode group nodes or collapse some nodes."
        script: "push_by_names('Explode and Collapse', 'NGMenuListPage', 'ExplodeCollapseNodeActions')"
    }

}


