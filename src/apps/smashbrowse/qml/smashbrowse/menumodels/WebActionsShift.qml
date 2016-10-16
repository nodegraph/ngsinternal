import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Text"
        description: "Shift elements to text elements."
        script: "push_by_names('Shift To Text', 'WebMenuListPage', 'WebActionsShiftToText')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Images"
        description: "Shift to image elements."
        script: "push_by_names('Shift To Images', 'WebMenuListPage', 'WebActionsShiftToImages')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Text Inputs"
        description: "Shift to text input elements."
        script: "push_by_names('Shift To Text Inputs', 'WebMenuListPage', 'WebActionsShiftToInputs')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Dropdowns"
        description: "Shift to dropdown elements."
        script: "push_by_names('Shift To DropDowns', 'WebMenuListPage', 'WebActionsShiftToSelects')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To IFrames"
        description: "Shift to iframe elements."
        script: "push_by_names('Shift To IFrames', 'WebMenuListPage', 'WebActionsShiftToIFrames')"
    }

}


