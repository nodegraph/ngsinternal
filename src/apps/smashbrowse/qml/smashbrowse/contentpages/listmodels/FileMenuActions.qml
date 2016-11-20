import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_note_add_white_48dp.png"
        title: "New"
        description: "Create a new graph."
        script: "file_menu_list_stack_page.on_create_file_info();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_unarchive_white_48dp.png"
        title: "Open"
        description: "Open another graph."
        script: "file_menu_list_stack_page.on_open_file_list();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_archive_white_48dp.png"
        title: "Save"
        description: "Save current graph."
        script: "file_menu_list_stack_page.on_save_current()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_insert_drive_file_white_48dp.png"
        title: "File Info"
        description: "Edit name and description of current graph."
        script: "file_menu_list_stack_page.on_edit_file_info()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_delete_forever_white_48dp.png"
        title: "Delete"
        description: "Delete current graph."
        script: "file_menu_list_stack_page.on_destroy_current();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_delete_forever_white_48dp.png"
        title: "Publish"
        description: "Publish the current graph as a macro."
        script: "file_menu_list_stack_page.on_publish_current();"
    }

}
