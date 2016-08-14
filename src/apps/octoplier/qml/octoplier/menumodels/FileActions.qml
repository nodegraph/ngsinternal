import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    ListElement {
        image_url: "qrc:///icons/ic_open_in_browser_white_48dp.png"
        title: "Load"
        description: "Load the selected graph file."
        script: "file_stack_page.get_file_page().load_current();
        	     file_stack_page.on_close_file_options();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_note_add_white_48dp.png"
        title: "New"
        description: "Create a new graph file."
        script: "file_stack_page.on_create_file_page()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_insert_drive_file_white_48dp.png"
        title: "Edit"
        description: "Edit the settings on the selected file."
        script: "file_stack_page.on_edit_file_page()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_delete_forever_white_48dp.png"
        title: "Delete"
        description: "Delete the selected file."
        script: "file_stack_page.get_file_page().delete_current();
        		 file_stack_page.on_close_file_options()"
    }

}
