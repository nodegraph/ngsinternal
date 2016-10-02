import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Above"
        description: "Expand to similar elements above."
        script: "app_worker.record_expand_above()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Below"
        description: "Expand to similar elements below."
        script: "app_worker.record_expand_below()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Left"
        description: "Expand to similar elements on the left."
        script: "app_worker.record_expand_left()"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Right"
        description: "Expand to similar elements on the right."
        script: "app_worker.record_expand_right()"
    }

}
