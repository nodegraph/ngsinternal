import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Text Element"
        description: "Shift to neighboring element containing any text."
        script: "push_by_names('Shift To Text', 'WebMenuListPage', 'WebActionsShiftToTextType')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Image Element"
        description: "Shift to neighboring element containing any images."
        script: "push_by_names('Shift To Images', 'WebMenuListPage', 'WebActionsShiftToImageType')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_settings_system_daydream_white_48dp.png"
        title: "To Input Field Element"
        description: "Shift to neighboring element containing an input field."
        script: "push_by_names('Shift To Text Inputs', 'WebMenuListPage', 'WebActionsShiftToInputType')"
    }

    ListElement {
        image_url: "qrc:///icons/ic_mode_edit_white_48dp.png"
        title: "To Drop Down Field Element"
        description: "Shift to neighboring element containing a drop down field."
        script: "push_by_names('Shift To DropDowns', 'WebMenuListPage', 'WebActionsShiftToDropDownType')"
    }

}


