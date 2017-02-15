import QtQuick 2.6

ListModel {

    function update(props) {
    }

    ListElement {
        image_url: "qrc:///icons/ic_title_white_48dp.png"
        title: "To Element With Same Text"
        description: "Shift to the next element with the same text in this row or the following rows."
        script: "browser_recorder.record_shift_to_text_values_along_rows();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_crop_original_white_48dp.png"
        title: "To Element With Same Image"
        description: "Shift to the next element with the same image in this row or the following rows."
        script: "browser_recorder.record_shift_to_image_values_along_rows();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_title_white_48dp.png"
        title: "To Text Element"
        description: "Shift to the next text element in this row or the following rows."
        script: "browser_recorder.record_shift_to_text_type_along_rows();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_crop_original_white_48dp.png"
        title: "To Image Element"
        description: "Shift to the next image element in this row or the following rows."
        script: "browser_recorder.record_shift_to_image_type_along_rows();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_text_fields_white_48dp.png"
        title: "To Input Field Element"
        description: "Shift to the next input field in this row or the following rows."
        script: "browser_recorder.record_shift_to_input_type_along_rows();
        		main_bar.switch_to_current_mode();"
    }

    ListElement {
        image_url: "qrc:///icons/ic_format_line_spacing_white_48dp.png"
        title: "To Drop Down Field Element"
        description: "Shift to the next drop down field in this row or the following rows"
        script: "browser_recorder.record_shift_to_select_type_along_rows();
        		main_bar.switch_to_current_mode();"
    }

}


