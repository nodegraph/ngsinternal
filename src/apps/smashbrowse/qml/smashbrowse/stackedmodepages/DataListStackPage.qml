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
    id: stack_page

    // Settings.
    property var _allow_edits: false

    // Internal Properties.
    property var _values
    property var _hints
    property var _exposure

    // --------------------------------------------------------------------------------------------------
    // Public Methods.
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
    
    function on_view_outputs(node_name, values) {
        app_settings.vibrate()
        stack_view.clear_pages()
        
    	_values = values
    	_hints = {}
    	_exposure = {}
    	
    	var path = []
        view_object(node_name, path)
        main_bar.on_switch_to_mode(app_settings.view_node_mode)
    }

    // Show data values with hints on how to show it.
    function on_edit_inputs(node_name, values, hints, exposure) {
        app_settings.vibrate()
        stack_view.clear_pages()
        
        console.log('node name: ' + node_name)
        console.log('values: ' + JSON.stringify(values))
        console.log('hints: ' + JSON.stringify(hints))
        console.log('exposure: ' + JSON.stringify(exposure))
        
        // Show the next object.
        _values = values
        _hints = hints
        _exposure = exposure
        
        var path = []
        edit_object(node_name, path)
        main_bar.on_switch_to_mode(app_settings.edit_node_mode)
    }

    function on_add_element() {
        // Push a page to get a string name or an array index to insert at in the object in or array respectively.
        // Then call __on_add_element with the result ... not you can set the call on the EnterStringPage for example.
        var path = stack_view.get_title_path(1, stack_view.depth)

        // Get value info.
        var value = stack_page.get_value(path)
        var hints = stack_page.get_hints(path)
        var value_type = hints[hint_type.kJSType]

		// Push a page to get the name or index of the element to add.
        if (value_type == js_type.kObject) {
            var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/EnterStringPage.qml", stack_page, {})
            push_page.visible = true
            push_page.set_value("unnamed")
            push_page.set_title("Add Element to Object.")
            push_page.set_description("Enter the name of the new element to add.")
            push_page.callback = stack_page.add_element.bind(stack_page)
            stack_view.push_page(push_page)
        } else if (value_type == js_type.kArray) {
            var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/EnterNumberPage.qml", stack_page, {})
            push_page.visible = true
            push_page.set_value(0)
            push_page.set_title("Add Element to Array.")
            push_page.set_description("Enter the index at which to insert the new element.")
            push_page.callback = stack_page.add_element.bind(stack_page)
            stack_view.push_page(push_page)
        }
    }
    
    function on_view_element(name) {
        var child_path = stack_view.get_title_path(1, stack_view.depth)
        child_path.push(name)
        var child_value = stack_page.get_value(child_path)
        var child_value_type = app_enums.determine_js_type(child_value)
        
        switch(child_value_type) {
        case js_type.kString:
        case js_type.kBoolean:
        case js_type.kNumber: {
        		// Can't do anything further.
            }
            break
        case js_type.kObject:
            stack_page.view_object(child_path[child_path.length-1], child_path);
            break
        case js_type.kArray:
            stack_page.view_array(child_path[child_path.length-1], child_path);
            break
        default:
            console.error("Error: DataListStackPage::on_view_element encountered unknown js type:" + child_value_type)
            break
        }
    }

    function on_edit_element(name) {
        if (!stack_page._allow_edits) {
            return
        }

        var child_path = stack_view.get_title_path(1, stack_view.depth)
        child_path.push(name)
        var child_hints = get_hints(child_path)

        // Push an edit page according to the js type.
        var child_value = stack_page.get_value(child_path)
        var exposed = stack_page.get_exposed(child_path)
        switch(child_hints[hint_type.kJSType]) {
        case js_type.kString:
        	if (child_hints.hasOwnProperty(hint_type.kMultiLineEdit)) {
        		var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditMultiLinePage.qml", edit_data_list_stack_page, {})
	            setup_edit_page(page, name, child_value, child_hints, exposed)
	            stack_view.push_page(page)
        	} else {
	            var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditLinePage.qml", edit_data_list_stack_page, {})
	            setup_edit_page(page, name, child_value, child_hints, exposed)
	            stack_view.push_page(page)
            }
            break
        case js_type.kBoolean:
            var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditBooleanPage.qml", edit_data_list_stack_page, {})
            setup_edit_page(page, name, child_value, child_hints, exposed)
            stack_view.push_page(page)
            break
        case js_type.kNumber:
            if (child_hints.hasOwnProperty(hint_type.kEnum)) {
                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditEnumPage.qml", edit_data_list_stack_page, {})
                setup_edit_page(page, name, child_value, child_hints, exposed)
                stack_view.push_page(page)
            } else {
                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditNumberPage.qml", edit_data_list_stack_page, {})
                setup_edit_page(page, name, child_value, child_hints, exposed)
                stack_view.push_page(page)
            }
            break
        case js_type.kObject:
            stack_page.edit_object(child_path[child_path.length-1], child_path);
            break
        case js_type.kArray:
            stack_page.edit_array(child_path[child_path.length-1], child_path);
            break
        default:
            console.error("Error: DataListStackPage::on_edit_element encountered unknown js type:" + child_hints[hint_type.kJSType])
            break
        }
    }
    
    function on_remove_element(name) {
        var path = stack_view.get_title_path(1, stack_view.depth)
        var value = get_value(path)
        var hints = get_hints(path)

        if (hints[hint_type.kJSType] == js_type.kObject) {
            delete value[name]
            set_value(path, value)
        } else if (hints[hint_type.kJSType] == js_type.kArray) {
            value.splice(Number(name),1)
            set_value(path, value)
        }

        var tail_name = path[path.length-1]
        stack_view.pop_page()
        on_edit_element(tail_name)
    }

    // --------------------------------------------------------------------------------------------------
    // Internal Methods.
    // --------------------------------------------------------------------------------------------------

    // Setup an editor page before pushing onto the stack view.
    function setup_edit_page(page, title, child_value, child_hints, exposed) {
        if (child_hints) {
            if (child_hints.hasOwnProperty(hint_type.kEnum)) {
                page.set_enum_type(child_hints[hint_type.kEnum])
            }
            if (child_hints.hasOwnProperty(hint_type.kResizable)) {
                page.resizable = true
            }
            if (child_hints.hasOwnProperty(hint_type.kDescription)) {
                page.set_description(child_hints[hint_type.kDescription])
            } else {
                // If there is not description, then we're editing an element of an object or array parameter.
                page.set_description("Edit the value for this element.")
            }
        }
        page.set_title(title)
        page.set_value(child_value)
        page.set_exposed(exposed)
    }
    
    // The element_name is a string when adding to an object.
    // The element_name is an index to insert at when adding to an array. 
    // Use a number greater than the last element if you want to add at the end.
    function add_element(element_name) {
        console.log('adding element with name: ' + element_name)
        var path = stack_view.get_title_path(1, stack_view.depth)

        // Get value info.
        var value = stack_page.get_value(path)

        // Hints are mandatory on objects or arrays.
        var hints = stack_page.get_hints(path)
        if (!hints) {
            console.error('Error: add_element requires hints')
            return
        }

        // A resizable hint is needed to be resizable.
        var resizable = hints[hint_type.kResizable]
        if (!resizable) {
            console.error('Error: add_element requires resizable hints')
            return
        }

        // All parameters should have a js type.
        var value_type = hints[hint_type.kJSType]
        if (value_type === undefined) {
            console.error('Error: add_element requires js type hints')
            return
        }

        // Arrays and Objects should have an element js type.
        var element_value_type = hints[hint_type.kElementJSType]
        if (element_value_type === undefined) {
            console.error('Error: add_element requires element js type hints')
            return
        }

        // Only arrays and objects can be resized.
        if ((value_type != js_type.kArray) && (value_type != js_type.kObject)) {
            console.error('Error: add_element requires an array or an object')
            return
        }

        // Add a new element.
        if (value_type == js_type.kArray) {
            // Add an array element.
            if (element_value_type == js_type.kString) {
                value.splice(element_name, 0, '');
            } else if (element_value_type == js_type.kNumber) {
                value.splice(element_name, 0, 0);
            } else if (element_value_type == js_type.kBoolean) {
                value.splice(element_name, 0, false);
            }
        } else if (value_type == js_type.kObject) {
            // Make sure the element_name is unique.
            var unique_name = element_name
            if (value.hasOwnProperty(unique_name)) {
                var count = 1
                unique_name = element_name + count.toString()
                while (value.hasOwnProperty(unique_name)) {
                    count++
                    unique_name = element_name + count.toString()
                }
            }

            // Add an object element.
            if (element_value_type == js_type.kString) {
                value[unique_name] = ''
            } else if (element_value_type == js_type.kNumber) {
                value[unique_name] = 0
            } else if (element_value_type == js_type.kBoolean) {
                value[unique_name] = false
            }
        }

        // Record the new value.
        stack_page.set_value(path, value)

        // Refresh the page to show the new value.
        var tail_name = path[path.length-1]
        stack_view.pop_page()
        on_edit_element(tail_name)
    }


    // --------------------------------------------------------------------------------------------------
    // Extract from our Hints. Hints cannot be modified.
    // --------------------------------------------------------------------------------------------------

    function get_hints(path) {
        // Child path.
        var child_path = path

        // Parent path.
        var parent_path = child_path.slice()
        parent_path.pop()

        // Parent hints.
        var parent_hints = app_utils.get_sub_object(_hints, parent_path)

        // Determine the child hints.
        // If the parent hints contains child element hints then we must use those
        // exclusively instead of the hints at the child. The child will generally
        // not have any hints in this case.
        var child_hints = {}
        if (parent_hints && parent_hints.hasOwnProperty(hint_type.kElementJSType)) {
            child_hints[hint_type.kJSType] = parent_hints[hint_type.kElementJSType]
            // Check for other child element hints.
            if (parent_hints.hasOwnProperty(hint_type.kElementEnum)) {
                child_hints[hint_type.kEnum] = parent_hints[hint_type.kElementEnum]
            }
        } else {
            // Otherwise we grab the child hints using the child path.
            child_hints = app_utils.get_sub_object(_hints, child_path)
        }
        return child_hints
    }
    
    // --------------------------------------------------------------------------------------------------
    // Get and Set Exposed Setting.
    // --------------------------------------------------------------------------------------------------
	
	function get_exposed(path) {
		return app_utils.get_sub_object(_exposure, path)
	}
	
	function set_exposed(path, value) {
		app_utils.set_sub_object(_exposure, path, value)
        node_graph_item.set_input_exposure(_exposure)
	}

    // --------------------------------------------------------------------------------------------------
    // Get and Set Values.
    // --------------------------------------------------------------------------------------------------

    // Set the value at the given path in _values.
    function set_value(path, value) {
        app_utils.set_sub_object(_values, path, value)
        node_graph_item.set_editable_inputs(_values)
    }

    // Get the value at the given path in _values.
    function get_value(path) {
        return app_utils.get_sub_object(_values, path)
    }
    
    // Get a string which represents the value's type or actual value.
    function get_string_for_value(path) {
    	var value = get_value(path)
    	var value_type = app_enums.determine_js_type(value)
    	
    	if (_allow_edits) {
    		var hints = get_hints(path)
    		if (hints) {
    			value_type = hints[hint_type.kJSType]
    			// Use the hints to get a more descriptive string representation.
				if (hints.hasOwnProperty(hint_type.kEnum)) {
		    		return app_enums.get_msg_enum_text(hints[hint_type.kEnum], value)
		    	} else if (hints.hasOwnProperty(hint_type.kDescription) && 
		    				(value_type == js_type.kObject || value_type == js_type.kArray)) {
		    		return hints[hint_type.kDescription]
		    	}
    		}
    	}

        
        // Use the js type if the hints don't help.
        switch(value_type) {
        case js_type.kString:
        case js_type.kBoolean:
        case js_type.kNumber:
            return value.toString()
        case js_type.kObject:
            return "folder of values"
        case js_type.kArray:
            return "array of values"
        case js_type.kUndefined:
            return "undefined"
        case js_type.kNull:
            return "null"
        default:
            console.log("Error: DataStackPage::get_string_for_value encountered unknown type: " + value_type + " for value: " + value)
            console.log(new Error().stack);
        }
        return "unknown value"
    }

    // Get an image which represents the value's type.
    function get_image_for_value(path) {
        var value = get_value(path)
    	var value_type = app_enums.determine_js_type(value)
    	
    	if (_allow_edits) {
    		var hints = get_hints(path)
    		if (hints) {
    		value_type = hints[hint_type.kJSType]
    		}
    	}
    	
        switch(value_type) {
        case js_type.kString:
            return 'qrc:///icons/ic_font_download_white_48dp.png'
        case js_type.kBoolean:
            return 'qrc:///icons/ic_check_box_white_24dp.png'
        case js_type.kNumber:
            return 'qrc:///icons/ic_looks_3_white_48dp.png'
        case js_type.kObject:
            return 'qrc:///icons/ic_folder_white_48dp.png'
        case js_type.kArray:
            return 'qrc:///icons/ic_folder_white_48dp.png'
        case js_type.kUndefined:
            return 'qrc:///icons/ic_warning_white_48dp.png'
        case js_type.kNull:
            return 'qrc:///icons/ic_warning_white_48dp.png'
        default:
            console.log("Error: DataStackPage::get_image_for_value encountered unknown type.")
            console.log(new Error().stack);
        }
        return ""
    }

    // --------------------------------------------------------------------------------------------------
    // Push Objects or Arrays onto our Stack View.
    // --------------------------------------------------------------------------------------------------


    // Push next model on the stack.
    function push_by_model(title, model, hints) {
        var list_page = stack_view.create_page("DataListPage")
        if (hints && hints.hasOwnProperty(hint_type.kResizable)) {
        	list_page.resizable = true
        }
        stack_view.push_by_components(title, list_page, model)
    }

    // Create a list model.
    function create_list_model() {
        var script = "
                import QtQuick 2.6;
                ListModel {
                } "
        return Qt.createQmlObject(script, stack_page, "view_node_dynamic_content")
    }

	// Used to bring up editable properties list of an object.
    function edit_object(title, path) {
    	var obj = get_value(path)
    	var hints = get_hints(path)
        var model = create_list_model()
        for (var prop in obj) {
            if (obj.hasOwnProperty(prop)) {
                model.append({name: prop, depth_index: stack_view.depth})
            }
        }
        console.log('pushing: ' + title + ' hints: ' + JSON.stringify(hints))
        push_by_model(title, model, hints)
    }

	// Used to bring up editable properties list of an array.
    function edit_array(title, path) {
    	var arr = get_value(path)
    	var hints = get_hints(path)
        var model = create_list_model()
        for (var i=0; i<arr.length; i++) {
            model.append({name: i.toString(), depth_index: stack_view.depth})
        }
        console.log('pushing arr: ' + title + ' hints: ' + JSON.stringify(hints))
        push_by_model(title, model, hints)
    }
    
    // Used to bring up non-editable properties list of an object.
    function view_object(title, path) {
       	var obj = get_value(path)
        var model = create_list_model()
        for (var prop in obj) {
            if (obj.hasOwnProperty(prop)) {
                model.append({name: prop, depth_index: stack_view.depth})
            }
        }
        push_by_model(title, model, {})
    }
    
    // Used to bring up non-editable properties list of an array.
    function view_array(title, path) {
    	var arr = get_value(path)
        var model = create_list_model()
        for (var i=0; i<arr.length; i++) {
            model.append({name: i.toString(), depth_index: stack_view.depth})
        }
        push_by_model(title, model, {})
    }
}
