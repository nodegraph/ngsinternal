import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
        var script = "var page = app_loader.load_component('qrc:///qml/smashbrowse/pages/LimitPage.qml', node_action_stack_page, {})
                      stack_view.push_page(page);"

        if (props.edition == "lite") {
            get(5).script = script
            get(6).script = script
        }

        if (props.node_limit_reached) {
            get(1).script = script
        }
    }

    ListElement {
        image_url: "qrc:///icons/ic_arrow_upward_white_48dp.png"
        title: "Surface"
        description: "Surface out of the current group."
        script: "node_graph_item.surface();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_build_white_48dp.png"
        title: "Create..."
        description: "Create a new node."
        script: "push_by_names('Create', 'NGMenuListPage', 'CreateNodeActions')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_delete_forever_white_48dp.png"
        title: "Delete"
        description: "Delete the selected nodes."
        script: "node_graph_item.destroy_selection();
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
        image_url: "qrc:///icons/ic_zoom_out_map_white_48dp.png"
        title: "Explode and Collapse..."
        description: "Explode group nodes or collapse some nodes."
        script: "push_by_names('Explode and Collapse', 'NGMenuListPage', 'ExplodeCollapseNodeActions')"
    }

}


