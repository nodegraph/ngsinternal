import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.appconfig 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0


BaseStackPage{
    id: page

    // Settings.
    property var _allow_edits: false

    // Internal Properties.
    property var _values
    property var _hints

    // --------------------------------------------------------------------------------------------------
    // Methods.
    // --------------------------------------------------------------------------------------------------

    // Framework Methods.
    function on_switch_to_mode(mode) {
        if (_allow_edits) {
            if (mode == app_settings.edit_node_mode) {
                visible = true
            } else {
                visible = false
            }
        } else {
            if (mode == app_settings.view_node_mode) {
                visible = true
            } else {
                visible = false
            }
        }
    }

    // Show data values with hints on how to show it.
    function on_show_data(name, values, hints) {
        app_settings.vibrate()
        stack_view.clear_pages()

        // Show the next object.
        _values = values
        _hints = hints
        view_object(name, _values)
    }

    // --------------------------------------------------------------------------------------------------
    // Hints.
    // --------------------------------------------------------------------------------------------------

    function get_hints(path) {
        return app_utils.get_sub_object(_hints, path)
    }

    // --------------------------------------------------------------------------------------------------
    // Values.
    // --------------------------------------------------------------------------------------------------

    // Set the value at the given path in _values.
    function set_value(path, value) {
        app_utils.set_sub_object(_values, path, value)
    }

    // Get the value at the given path in _values.
    function get_value(path) {
        return app_utils.get_sub_object(_values, path)
    }

    // Get a string which represents the value's type or actual value.
    function get_string_for_value(value) {
        var value_type = app_enums.get_js_enum(value)
        switch(value_type) {
        case js_enum.kString:
        case js_enum.kBoolean:
        case js_enum.kNumber:
            return value.toString()
        case js_enum.kObject:
            return "folder of values"
        case js_enum.kArray:
            return "array of values"
        case js_enum.kUndefined:
            return "undefined"
        case js_enum.kNull:
            return "null"
        default:
            console.log("Error: DataStackPage::get_string_for_value encountered unknown type: " + value_type + " for value: " + value)
            console.log(new Error().stack);
        }
        return "unknown value"
    }

    // Get an image which represents the value's type.
    function get_image_for_value(value) {
        var value_type = app_enums.get_js_enum(value)
        switch(value_type) {
        case js_enum.kString:
            return 'qrc:///icons/ic_font_download_white_48dp.png'
        case js_enum.kBoolean:
            return 'qrc:///icons/ic_check_box_white_24dp.png'
        case js_enum.kNumber:
            return 'qrc:///icons/ic_looks_3_white_48dp.png'
        case js_enum.kObject:
            return 'qrc:///icons/ic_folder_white_48dp.png'
        case js_enum.kArray:
            return 'qrc:///icons/ic_folder_white_48dp.png'
        case js_enum.kUndefined:
            return 'qrc:///icons/ic_warning_white_48dp.png'
        case js_enum.kNull:
            return 'qrc:///icons/ic_warning_white_48dp.png'
        default:
            console.log("Error: DataStackPage::get_image_for_value encountered unknown type.")
            console.log(new Error().stack);
        }
        return ""
    }

    // --------------------------------------------------------------------------------------------------
    // Methods used to push and pop pages onto this stack view.
    // --------------------------------------------------------------------------------------------------


    // Push next model on the stack.
    function push_by_model(title, model) {
        var data_list = stack_view.create_page("DataListPage")
        stack_view.push_by_components(title, data_list, model)
    }

    // Create a list model.
    function create_list_model() {
        var script = "
                import QtQuick 2.6;
                ListModel {
                } "
        return Qt.createQmlObject(script, page, "view_node_dynamic_content")
    }

    // Display the contents of a dict.
    function view_object(name, obj) {
        var model = create_list_model()
        for (var prop in obj) {
            if (obj.hasOwnProperty(prop)) {
                model.append({name: prop, depth_index: stack_view.depth})
            }
        }
        push_by_model(name, model)
    }

    // Display the contents of an array.
    function view_array(name, arr) {
        var model = create_list_model()
        for (var i=0; i<arr.length; i++) {
            model.append({name: i.toString(), depth_index: stack_view.depth})
        }
        push_by_model(name, model)
    }

}
