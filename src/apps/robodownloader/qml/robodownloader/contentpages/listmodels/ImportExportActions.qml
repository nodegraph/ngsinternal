import QtQuick 2.7

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_file_upload_white_48dp.png"
        title: "Export a Private Macro"
        description: "Export the current graph as an encrypted private macro."
        script: "file_menu_list_stack_page.on_publish_private_macro();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_file_upload_white_48dp.png"
        title: "Export a Public Macro"
        description: "Export the current graph as a non-encrypted public macro."
        script: "file_menu_list_stack_page.on_publish_public_macro();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_file_download_white_48dp.png"
        title: "Import a Private Macro"
        description: "Import an encrypted private macro."
        script: "file_menu_list_stack_page.on_import_private_macro();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_file_download_white_48dp.png"
        title: "Import a Public Macro"
        description: "Import a non-encrypted public macro."
        script: "file_menu_list_stack_page.on_import_public_macro();"
    }
    
    ListElement {
        image_url: "qrc:///icons/ic_file_download_white_48dp.png"
        title: "Import an App Macro"
        description: "Import an app macro."
        script: "file_menu_list_stack_page.on_import_app_macro();"
    }

}
