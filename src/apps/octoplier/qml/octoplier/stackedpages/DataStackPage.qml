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


Rectangle{
    id: data_stack_page

    height: app_settings.page_height
    width: app_settings.page_width

    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    color: app_settings.menu_stack_bg_color

    // Settings.
    property var mode: app_settings.view_node_mode

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == data_stack_page.mode) {
            visible = true;
        } else {
            visible = false;
        }
    }

    function on_show_data(node_name, node_data) {
        app_settings.vibrate()
        stack_view.clear_pages()
        if (mode == app_settings.view_node_mode) {
            stack_view.view_object("Outputs For: " + node_name, node_data)
        } else {
        	stack_view.allow_editing = true // Allow data to be edited.
            stack_view.view_object("Parameters For: " + node_name, node_data)
       	}
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        // Dependencies.
        stack_view: stack_view
        // Properties.
        allow_back_to_last_mode: false
        title_text: (mode == app_settings.view_node_mode) ? "Outputs For: " : "Parameters For: "
    }

    // The scroll view.
    AppScrollView {
        id: scroll_view

        // The main stack view.
        AppStackView{
            id: stack_view

            // Dependencies.
            stack_view_header: stack_view_header

            // Push next model on the stack.
            function push_model(next_model) {
                var next_page = app_loader.load_component("qrc:///qml/octoplier/stackedpages/DataPage.qml", app_window, {})
                next_page.model = next_model
                stack_view.push_page(next_model.title, next_page)
            }

            // Create a list model.
            function create_model(name) {
                var script = "
                    import QtQuick 2.6;
                    ListModel {
                        property var title: \"" + name + "\";
                    } "
                return Qt.createQmlObject(script, data_stack_page, "view_node_dynamic_content")
            }
            // Display the contents of a dict.
            function view_object(name, obj) {
                var model = create_model(name)
                for (var prop in obj) {
                    if (obj.hasOwnProperty(prop)) {
                        model.append(create_element(prop, obj[prop]))
                    }
                }
                stack_view.push_model(model)
            }
            // Display the contents of an array.
            function view_array(name, obj) {
                var model = create_model(name)
                var arr = obj.payload
                for (var i=0; i<arr.length; i++) {
                    model.append(create_element(i.toString(), arr[i]))
                }
                stack_view.push_model(model)
            }
            // Create a ListElement.
            function create_element(name, value) {
                var element = {title: name}
                if (typeof value === 'string') {
                    element.description = value
                    element.image_url = 'qrc:///icons/ic_font_download_white_48dp.png'
                    element.our_string_value = value
                } else if (typeof value === 'boolean') {
                    element.description = value.toString()
                    element.image_url = 'qrc:///icons/ic_check_box_white_24dp.png'
                    element.our_boolean_value = value
                }else if (typeof value === 'object') {
                    element.type = 'object'
                    // can be array or object
                    if (Object.getPrototypeOf(value) === Object.prototype) {
                        element.description = "folder of values"
                        element.image_url = 'qrc:///icons/ic_folder_white_48dp.png'
                        element.our_object_value = value
                    } else if (Object.getPrototypeOf(value) === Array.prototype) {
                        element.description = "array of values"
                        element.image_url = 'qrc:///icons/ic_folder_white_48dp.png'
                        // Qt will try to interpret sub/child arrays/lists as child listmodels.
                        // To get around this we embed the array in an object.
                        element.our_array_value = {payload: value}
                    }
                } else if (typeof value === 'number'){
                    element.description = value.toString()
                    element.image_url = 'qrc:///icons/ic_looks_3_white_48dp.png'
                    element.our_number_value = value
                } else {
                    console.log("Error: DataStackPage encountered unknown type.")
                }

                return element
            }
        }
    }
}
