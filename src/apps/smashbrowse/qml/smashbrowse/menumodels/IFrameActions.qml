import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Switch to IFrame"
        description: "The crosshair is in a another iframe."
        script: "app_recorder.record_switch_to_iframe();
                 web_action_stack_page.on_show_web_action_menu()"
    }

}


