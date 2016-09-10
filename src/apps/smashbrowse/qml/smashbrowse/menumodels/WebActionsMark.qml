import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Mark Set"
        description: "Mark set under crosshair."
        script: "app_comm.mark_set()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Unmark Set"
        description: "Unmark set under crosshair."
        script: "app_comm.unmark_set()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Merge Sets"
        description: "Merge all marked sets."
        script: "app_comm.merge_sets()"
    }

}
