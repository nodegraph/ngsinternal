import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    property var title: "File Actions"

    function update(props) {}

    ListElement {
        image_url: "qrc:///icons/ic_open_in_browser_white_48dp.png"
        title: "Load"
        description: "Load the selected graph file."
        script: "file_page.load_current()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_note_add_white_48dp.png"
        title: "New"
        description: "Create a new graph file."
        script: "menu_stack_page.on_create_file_page()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_insert_drive_file_white_48dp.png"
        title: "Edit"
        description: "Edit the name or description of selected graph file."
        script: "menu_stack_page.on_edit_file_page()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_delete_forever_white_48dp.png"
        title: "Delete"
        description: "Delete the selected graph file."
        script: "file_page.delete_current()"
    }

}
