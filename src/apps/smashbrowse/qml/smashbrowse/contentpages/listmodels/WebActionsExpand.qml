import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Above"
        description: "Expand to similar elements above."
        script: "web_recorder.record_expand_above();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Below"
        description: "Expand to similar elements below."
        script: "web_recorder.record_expand_below();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Left"
        description: "Expand to similar elements on the left."
        script: "web_recorder.record_expand_left();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Right"
        description: "Expand to similar elements on the right."
        script: "web_recorder.record_expand_right();
        		main_bar.switch_to_current_mode();"
    }

}
