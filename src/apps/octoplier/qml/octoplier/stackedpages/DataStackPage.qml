import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import octoplier.appconfig 1.0
import octoplier.stackedpages 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0


AppStackPage{
    id: data_stack_page

    // Settings.
    property var mode: app_settings.view_node_mode

    // Internal Properties.
    property var data_values
    property var data_hints

    // Framework Methods.
    function on_switch_to_mode(mode) {
        if (mode == data_stack_page.mode) {
            visible = true;
        } else {
            visible = false;
        }
    }

    // Main method to show data.
    function on_show_data(data_name, data_values) {
//        app_settings.vibrate()
//        stack_view.clear_pages()
//        data_stack_page.data_values = data_values
//        if (mode != app_settings.view_node_mode) {
//            stack_view.allow_editing = true // Allow data to be edited.
//        }
//        view_object(data_name, data_stack_page.data_values)
    }

    // --------------------------------------------------------------------------------------------------
    // Hints for Data Display.
    // --------------------------------------------------------------------------------------------------

    function get_hints(path) {
        // The hints hierarchy doesn't have arrays.
        // Individual hints are represent by single json strings.

        // Drill down into the hints according to the specified path.
        var hints = data_hints
        for (var i=0; i<path.length; i++) {
            var element = path[i]
            if (hints.hasOwnProperty(element)) {
                hints = hints[element]
            } else {
                // We get here if hints doesn't have the expected element.
                // This mean we may have a primitive or bad hints.
                // Either way we return the closest hints we have.
                break;
            }
        }

        // Make sure we have hint string to return.
        if (typeof hints === 'string') {
            return hints
        }
        return ""
    }

    // --------------------------------------------------------------------------------------------------
    // Data Display.
    // --------------------------------------------------------------------------------------------------

    // Set the value at the given path in data_values.
    function set_value(path, value) {
        // Make sure the path has at least one element.
        if (path.length <=0) {
            return
        }

        // Drill down into data according to the specified path.
        var data = data_stack_page.data_values
        for (var i=0; i<path.length; i++) {
            // Determine the index.
            var index = null
            if (typeof data === 'object') {
                if (Object.getPrototypeOf(data) === Array.prototype) {
                    // Arrays have number indexes.
                    index = Number(path[i])
                } else if (Object.getPrototypeOf(data) === Object.prototype){
                    // Objects have string indexes.
                    index = path[i]
                } else {
                    console.log("Error: DataStackPage::set_value was expecting an object or an array 333.")
                }
            } else {
                console.log("Error: DataStackPage::set_value was expecting an object or an array 4444.")
                return
            }
            // Determine if this is the last element of the path.
            var last = false
            if (i == (path.length-1)) {
                last = true
            }
            // If this is the last element, we set the value on the object or array.
            if (last) {
                data[index] = value
            } else {
                data = data[index]
            }
        }
        console.log('data: ' + JSON.stringify(data_stack_page.data_values))
    }

    // Get the value at the given path in data_values.
    function get_value(path) {
        // Make sure the path has at least one element.
        if (path.length <=0) {
            console.log('Error: get_value called with empty path')
            return null
        }

        // Drill down into data according to the specified path.
        var data = data_stack_page.data_values
        for (var i=0; i<path.length; i++) {
            if (typeof data === 'object') {
                if (Object.getPrototypeOf(data) === Array.prototype) {
                    data = data[Number(path[i])]
                } else if (Object.getPrototypeOf(data) === Object.prototype){
                    data = data[path[i]]
                }  else {
                    console.log("Error: DataStackPage::get_value was expecting an object or an array 111.")
                    return null
                }
            } else {
                 console.log("Error: DataStackPage::get_value was expecting an object or an array 222: " + JSON.stringify(data) + " path: " + path)
                 console.log(new Error().stack);
                return null
            }
        }
        return data
    }

    // Get the value type at the given path in data_values.
    function get_value_type(path) {
        var value = get_value(path)
        if (value === null) {
            return "unknown_type"
        }else if (typeof value === 'string') {
            // String.
            return 'string_type'
        } else if (typeof value === 'boolean') {
            // Boolean.
            return 'boolean_type'
        } else if (typeof value === 'number'){
            // Number.
            return 'number_type'
        } else if (typeof value === 'object') {
            if (Object.getPrototypeOf(value) === Object.prototype) {
                // Object.
                return 'dictionary_type'
            } else if (Object.getPrototypeOf(value) === Array.prototype) {
                // Array.
                return "array_type"
            }
        }
        return "unknown_type"
    }


    // Get the value as a string at the given path in data_values.
    function get_value_as_string(path) {
        var value = get_value(path)
        var value_type = get_value_type(path)

        switch(value_type) {
        case 'string_type':
        case 'boolean_type':
        case 'number_type':
            return value.toString()
        case 'dictionary_type':
            return "folder of values"
        case 'array_type':
            return "array of values"
        default:
            console.log("Error: DataStackPage::get_value_as_string encountered unknown type: " + value_type + " for path: " + path + " for value: " + value)
            console.log(new Error().stack);
        }
        return "unknown value"
    }

    // Get the icon corresponding to the value type at the given path in data_values.
    function get_image_url(path) {
        var value_type = get_value_type(path)

        switch(value_type) {
        case 'string_type':
            return 'qrc:///icons/ic_font_download_white_48dp.png'
        case 'boolean_type':
            return 'qrc:///icons/ic_check_box_white_24dp.png'
        case 'number_type':
            return 'qrc:///icons/ic_looks_3_white_48dp.png'
        case 'dictionary_type':
            return 'qrc:///icons/ic_folder_white_48dp.png'
        case 'array_type':
            return 'qrc:///icons/ic_folder_white_48dp.png'
        default:
            console.log("Error: DataStackPage::get_image_url encountered unknown type.")
            console.log(new Error().stack);
        }
        return ""
    }

    // --------------------------------------------------------------------------------------------------
    // Methods used to push and pop pages onto this stack view.
    // --------------------------------------------------------------------------------------------------


    // Push next model on the stack.
    function push_by_model(title, model) {
        var page = stack_view.create_page("DataPage")
        stack_view.push_by_components(title, page, model)
    }

    // Create a list model.
    function create_list_model() {
        var script = "
                import QtQuick 2.6;
                ListModel {
                } "
        return Qt.createQmlObject(script, data_stack_page, "view_node_dynamic_content")
    }

    // Display the contents of a dict.
    function view_object(data_name, obj) {
        var model = create_list_model()
        for (var prop in obj) {
            if (obj.hasOwnProperty(prop)) {
                model.append({data_name: prop, depth_index: stack_view.depth})
            }
        }
        push_by_model(data_name, model)
    }

    // Display the contents of an array.
    function view_array(data_name, arr) {
        var model = create_list_model()
        for (var i=0; i<arr.length; i++) {
            model.append({data_name: i.toString(), depth_index: stack_view.depth})
        }
        push_by_model(data_name, model)
    }

}
