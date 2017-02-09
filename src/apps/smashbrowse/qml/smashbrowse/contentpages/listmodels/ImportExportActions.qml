import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_file_upload_white_48dp.png"
        title: "Export a User Macro"
        description: "Export the current graph as a user macro."
        script: "file_menu_list_stack_page.on_publish_user_macro();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_file_download_white_48dp.png"
        title: "Import a User Macro"
        description: "Import a user macro."
        script: "file_menu_list_stack_page.on_import_user_macro();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_file_download_white_48dp.png"
        title: "Import an App Macro"
        description: "Import an app macro."
        script: "file_menu_list_stack_page.on_import_app_macro();"
    }

}
