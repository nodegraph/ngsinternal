import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Above Marked Sets"
        description: "Shrink to those above the marked sets."
        script: "app_worker.shrink_above_of_marked()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Below Marked Sets"
        description: "Shrink to those below the marked sets."
        script: "app_worker.shrink_below_of_marked()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Above and Below Marked Sets"
        description: "Shrink to those above and below the marked sets."
        script: "app_worker.shrink_above_and_below_of_marked()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "On Left of Marked Sets"
        description: "Shrink to those on the left of the marked sets."
        script: "app_worker.shrink_left_of_marked()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "On Right of Marked Sets"
        description: "Shrink to those on the right of the marked sets"
        script: "app_worker.shrink_right_of_marked()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "On Left and Right of Marked Sets"
        description: "Shrink to those on the left or right of the marked sets"
        script: "app_worker.shrink_left_and_right_of_marked()"
    }


}

