import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Similar Text Element"
        description: "Shift to a neighboring text element with the same text."
        script: "push_by_names('Shift To Similar Text', 'WebMenuListPage', 'WebActionsShiftToTextValues')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Similar Image Element"
        description: "Shift to a neighboring image element with the same images."
        script: "push_by_names('Shift To Similar Image', 'WebMenuListPage', 'WebActionsShiftToImageValues')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Text Element"
        description: "Shift to a neighboring text element containing any text."
        script: "push_by_names('Shift To Text', 'WebMenuListPage', 'WebActionsShiftToTextType')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Image Element"
        description: "Shift to a neighboring image element containing any images."
        script: "push_by_names('Shift To Image', 'WebMenuListPage', 'WebActionsShiftToImageType')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Input Field Element"
        description: "Shift to a neighboring input field."
        script: "push_by_names('Shift To Text Inputs', 'WebMenuListPage', 'WebActionsShiftToInputType')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Drop Down Field Element"
        description: "Shift to a neighboring drop down field."
        script: "push_by_names('Shift To DropDowns', 'WebMenuListPage', 'WebActionsShiftToDropDownType')"
    }

}


