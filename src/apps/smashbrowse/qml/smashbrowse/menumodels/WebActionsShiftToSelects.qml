import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "Above"
        description: "Shift to dropdown elements above."
        script: "app_comm.shift_to_selects_above();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "Below"
        description: "Shift to dropdown elements below."
        script: "app_comm.shift_to_selects_below();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "On Left"
        description: "Shift to dropdown elements on left."
        script: "app_comm.shift_to_selects_on_left();
                 main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "On Right"
        description: "Shift to dropdown elements on right."
        script: "app_comm.shift_to_selects_on_right();
                 main_bar.switch_to_current_mode();"
    }

}
