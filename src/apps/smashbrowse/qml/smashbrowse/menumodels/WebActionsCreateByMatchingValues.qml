import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "By Matching Text"
        description: "Create set from text under crosshair."
        script: "app_comm.create_set_by_matching_text();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "By Matching Images"
        description: "Create set from images under crosshair."
        script: "app_comm.create_set_by_matching_images();
                main_bar.switch_to_current_mode();"
    }

}
