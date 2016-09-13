import QtQuick 2.6

// The first element contains the title to display in the stack view navigator.

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Of Images"
        description: "Create set of image elements."
        script: "app_worker.create_set_of_images();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Of Text"
        description: "Create set of text elements."
        script: "app_worker.create_set_of_text();
                main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Of Text Inputs"
        description: "Create set of text input elements."
        script: "app_worker.create_set_of_inputs();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Of Dropdowns"
        description: "Create set of select/dropdown elements."
        script: "app_worker.create_set_of_selects();
                main_bar.switch_to_current_mode();"
    }

}

