import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }
    
    
    
    ListElement {
        image_url: "qrc:///icons/ic_open_in_new_white_48dp.png"
        title: "Open"
        description: "Open another graph."
        script: "file_menu_list_stack_page.on_open_file_list();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_save_white_48dp.png"
        title: "Save"
        description: "Save current graph."
        script: "file_menu_list_stack_page.on_save_current()"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_add_to_queue_white_48dp.png"
        title: "Add"
        description: "Add a new graph."
        script: "file_menu_list_stack_page.on_create_file_info();"
    }

	ListElement {
        image_url: "qrc:///icons/ic_remove_from_queue_white_48dp.png"
        title: "Remove"
        description: "Remove the current graph."
        script: "file_menu_list_stack_page.on_destroy_current();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_dvr_white_48dp.png"
        title: "Meta Info"
        description: "Edit current graph's meta info."
        script: "file_menu_list_stack_page.on_edit_file_info()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_import_export_white_48dp.png"
        title: "Import and Export"
        description: "Import and export of macros."
        script: "push_by_names('import and export', 'FileMenuListPage', 'ImportExportActions')"
    }

}
