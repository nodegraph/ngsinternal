import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Mark Set"
        description: "Mark set under crosshair."
        script: "web_recorder.record_mark_set();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Unmark Set"
        description: "Unmark set under crosshair."
        script: "web_recorder.record_unmark_set();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Merge Sets"
        description: "Merge all marked sets."
        script: "web_recorder.record_merge_sets();
        		main_bar.switch_to_current_mode();"
    }

}
