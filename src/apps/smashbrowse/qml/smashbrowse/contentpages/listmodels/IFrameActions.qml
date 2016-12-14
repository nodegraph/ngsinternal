import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "This is being deprecated."
        description: "Don't use this anymore"
        script: "main_bar.switch_to_current_mode();"
    }
}


