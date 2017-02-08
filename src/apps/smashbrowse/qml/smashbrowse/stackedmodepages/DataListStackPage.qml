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

import GUITypes 1.0

BaseStackPage{
    id: stack_page

    // Settings.
    property var _allow_edits: false

    // Internal Properties.
    property var _values
    property var _hints
    property var _exposure
    property var _node_path
    
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
    
    function get_node_path_tail() {
        var node_path_splits = _node_path.split('/')
        return node_path_splits[node_path_splits.length-1]
    }
    
    function view_outputs(title, values) {
        app_settings.vibrate()
        stack_view.clear_pages()
        
    	_values = values
    	_hints = {}
    	_exposure = {}
    	_node_path = ""
    	
    	var data_path = []
        view_object(title, data_path)
        main_bar.on_switch_to_mode(app_settings.view_node_mode)
    }
    
    function on_view_outputs(node_path, values) {
        app_settings.vibrate()
        stack_view.clear_pages()
        
    	_values = values
    	_hints = {}
    	_exposure = {}
    	_node_path = node_path
    	
    	var data_path = []
    	var node_path_tail = get_node_path_tail()
        view_object(node_path_tail + ' node', data_path)
        main_bar.on_switch_to_mode(app_settings.view_node_mode)
    }

    // Show data values with hints on how to show it.
    function on_edit_inputs(node_path, values, hints, exposure) {
        app_settings.vibrate()
        stack_view.clear_pages()
        
        //console.log('node path: ' + node_path)
        //console.log('values: ' + JSON.stringify(values))
        //console.log('hints: ' + JSON.stringify(hints))
        //console.log('exposure: ' + JSON.stringify(exposure))
        
        // Show the next object.
        _values = values
        _hints = hints
        _exposure = exposure
        _node_path = node_path
        
        var data_path = []
        var node_path_tail = get_node_path_tail()
        edit_object(node_path_tail + ' node', data_path)
        main_bar.on_switch_to_mode(app_settings.edit_node_mode)
    }

    function on_add_element() {
        // Push a page to get a string name or an array index to insert at in the object in or array respectively.
        // Then call __on_add_element with the result ... not you can set the call on the EnterStringPage for example.
        var path = stack_view.get_title_path(1, stack_view.depth)

        // Get value info.
        var value = stack_page.get_value(path)
        var hints = stack_page.get_hints(path)
        var value_type = app_enums.determine_js_type(value)

		// Push a page to get the name or index of the element to add.
        if (value_type == GUITypes.Object) {
            var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/EnterStringAndDropdownPage.qml", stack_page, {})
            push_page.visible = true
            push_page.set_value("name")
            push_page.set_title("Add Element to Object.")
            push_page.set_description("Name and type of the new value.")
            push_page.set_option_texts(['string','number','boolean', 'array', 'object'])
            push_page.set_option_ids([2, 3, 4, 1, 0])
            
            
            push_page.callback = stack_page.add_element.bind(stack_page)
            stack_view.push_page(push_page)
        } else if (value_type == GUITypes.Array) {
        	if (hints.hasOwnProperty(GUITypes.ElementJSTypeHint)) {
        		var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/EnterNumberPage.qml", stack_page, {})
            	push_page.visible = true
            	push_page.set_value(0)
            	push_page.set_bottom_value(0)
            	push_page.set_top_value(value.length)
            	push_page.set_title("Add Element to Array.")
            	push_page.set_description("Index of the new element.")
            	
            	
            	var element_type = hints[GUITypes.ElementJSTypeHint]
	            push_page.callback = stack_page.add_element.bind(stack_page, element_type)
	            stack_view.push_page(push_page)
        	} else {
        		var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/EnterNumberAndDropdownPage.qml", stack_page, {})
            	push_page.visible = true
            	push_page.set_value(0)
            	push_page.set_bottom_value(0)
            	push_page.set_top_value(value.length)
            	push_page.set_title("Add Element to Array.")
            	push_page.set_description("Index of the new element.")
            	push_page.set_option_texts(['string','number','boolean', 'array', 'object'])
            	push_page.set_option_ids([2, 3, 4, 1, 0])
            	
            	
            	push_page.callback = stack_page.add_element.bind(stack_page)
            	stack_view.push_page(push_page)
            }
        }
    }
    
    function on_view_element(name) {
        var child_path = stack_view.get_title_path(1, stack_view.depth)
        child_path.push(name)
        var child_value = stack_page.get_value(child_path)
        var child_value_type = app_enums.determine_js_type(child_value)
        
        switch(child_value_type) {
        case GUITypes.String:
        case GUITypes.Boolean:
        case GUITypes.Number: {
        		// Can't do anything further.
            }
            break
        case GUITypes.Object:
            stack_page.view_object(child_path[child_path.length-1], child_path);
            break
        case GUITypes.Array:
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
        var child_type = app_enums.determine_js_type(child_value)
        switch(child_type) {
        case GUITypes.String:
        	if (child_hints.hasOwnProperty(GUITypes.MultiLineHint)) {
        		var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditMultiLinePage.qml", edit_data_list_stack_page, {})
	            setup_edit_page(page, name, child_value, child_hints, exposed)
	            stack_view.push_page(page)
        	} else {
	            var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditLinePage.qml", edit_data_list_stack_page, {})
	            setup_edit_page(page, name, child_value, child_hints, exposed)
	            stack_view.push_page(page)
            }
            break
        case GUITypes.Boolean:
            var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditBooleanPage.qml", edit_data_list_stack_page, {})
            setup_edit_page(page, name, child_value, child_hints, exposed)
            stack_view.push_page(page)
            break
        case GUITypes.Number:
            if (child_hints.hasOwnProperty(GUITypes.EnumHint)) {
                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditEnumPage.qml", edit_data_list_stack_page, {})
                setup_edit_page(page, name, child_value, child_hints, exposed)
                stack_view.push_page(page)
            } else {
                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditNumberPage.qml", edit_data_list_stack_page, {})
                setup_edit_page(page, name, child_value, child_hints, exposed)
                stack_view.push_page(page)
            }
            break
        case GUITypes.Object:
            stack_page.edit_object(child_path[child_path.length-1], child_path);
            break
        case GUITypes.Array:
            stack_page.edit_array(child_path[child_path.length-1], child_path);
            break
        default:
            console.error("Error: DataListStackPage::on_edit_element encountered unknown js type:" + child_type)
            break
        }
    }
    
    function on_remove_element(name) {
        var path = stack_view.get_title_path(1, stack_view.depth)
        var value = get_value(path)
        var hints = get_hints(path)
        var value_type = app_enums.determine_js_type(value)

        if (value_type == GUITypes.Object) {
            delete value[name]
            set_value(path, value)
        } else if (value_type == GUITypes.Array) {
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
            if (child_hints.hasOwnProperty(GUITypes.EnumHint)) {
                page.set_enum_type(child_hints[GUITypes.EnumHint])
            }
            var child_type = app_enums.determine_js_type(child_value)
            if (child_type == GUITypes.Array || child_type == GUITypes.Object) {
            	//page.resizable = true
            }
            if (child_hints.hasOwnProperty(GUITypes.DescriptionHint)) {
                page.set_description(child_hints[GUITypes.DescriptionHint])
            } else {
                // If there is not description, then we're editing an element of an object or array parameter.
                page.set_description("The current value of this element.")
            }
        }
        page.set_title(title)
        page.set_value(child_value)
        page.set_exposed(exposed)
    }
    
    // The element_name is a string when adding to an object.
    // The element_name is an index to insert at when adding to an array. 
    // Use a number greater than the last element if you want to add at the end.
    function add_element(element_type, element_name) {
        var path = stack_view.get_title_path(1, stack_view.depth)

        // Get value info.
        var parent_value = stack_page.get_value(path)
        var parent_type = app_enums.determine_js_type(parent_value)

        // Add a new element.
        if (parent_type == GUITypes.Array) {
            // Add an array element.
            switch(element_type) {
            	case GUITypes.String: {
            		parent_value.splice(element_name, 0, '');
            		break;
            	}
            	case GUITypes.Number: {
            		parent_value.splice(element_name, 0, 0);
            		break;
            	}
            	case GUITypes.Boolean: {
            		parent_value.splice(element_name, 0, false);
            		break;
            	}
            	case GUITypes.Array: {
            		parent_value.splice(element_name, 0, []);
            		break;
            	}
            	case GUITypes.Object: {
            		parent_value.splice(element_name, 0, {});
            		break;
            	}
            }
        } else if (parent_type == GUITypes.Object) {
            // Make sure the element_name is unique.
            var unique_name = element_name
            if (parent_value.hasOwnProperty(unique_name)) {
                var count = 1
                unique_name = element_name + count.toString()
                while (parent_value.hasOwnProperty(unique_name)) {
                    count++
                    unique_name = element_name + count.toString()
                }
            }

            // Add an object element.
        	switch(element_type) {
        		case GUITypes.String: {
        			parent_value[unique_name] = ''
        			break
        			}
        		case GUITypes.Number: {
        			parent_value[unique_name] = 0
        			break
        		}
        		case GUITypes.Boolean: {
        			parent_value[unique_name] = false
        			break
        		}
        		case GUITypes.Array: {
        			parent_value[unique_name] = []
        			break
        		}
        		case GUITypes.Object: {
        			parent_value[unique_name] = {}
        			break
        		}
        	}
        }

        // Record the new value.
        stack_page.set_value(path, parent_value)

        // Refresh the page to show the new value.
        var tail_name = path[path.length-1]
        stack_view.pop_page()
        on_edit_element(tail_name)
    }


    // --------------------------------------------------------------------------------------------------
    // Extract from our Hints. Hints cannot be modified.
    // --------------------------------------------------------------------------------------------------

	// Note there are hints only for paths of length 1 and 2.
	// For paths of length 2, the hints actually come from the parent path of length 1.
    function get_hints(path) {
		// The hints are expected to be a flat list of hints.
    	var hints = {}
    	
    	if (path.length == 0) {
    		return hints
    	}
    	
    	if (path.length >= 1) {
    		hints = app_utils.get_sub_object(_hints, path)
    		// Make a deep copy.
    		hints = JSON.parse(JSON.stringify(hints));
    	}
    	
    	if (path.length >= 2) {
    		// Some hints propagate from the parent to the child for one level only.
			var parent_path = get_parent_path(path)
			
    		// Merge hints from the parent.
    		var parent_hints = app_utils.get_sub_object(_hints, parent_path)
    		if (parent_hints.hasOwnProperty(GUITypes.ElementEnumHint)) {
    			hints[GUITypes.EnumHint] = parent_hints[GUITypes.ElementEnumHint]
    		}
    		if (parent_hints.hasOwnProperty(GUITypes.ElementJSTypeHint)) {
    			hints[GUITypes.JSTypeHint] = parent_hints[GUITypes.ElementJSTypeHint]
    		}
    	}
    	return hints
    }
    
    function get_parent_path(path) {
    	if (path.length == 0) {
    		return path
    	}
    	
    	// Pop off the tail element of the path.
		var parent_path = path.slice() // Make a copy of the path array.
		parent_path.pop()
		return parent_path
    }
    
    // --------------------------------------------------------------------------------------------------
    // Get and Set Exposed Setting.
    // --------------------------------------------------------------------------------------------------
	
	function get_exposed(path) {
		return app_utils.get_sub_object(_exposure, path)
	}
	
	function set_exposed(path, exposed) {
		if (!_allow_edits) {
			console.log('attempt to set exposed while in viewing outputs')
			return
		}
		app_utils.set_sub_object(_exposure, path, exposed)
        node_graph_item.set_input_exposure(_node_path, _exposure)
	}

    // --------------------------------------------------------------------------------------------------
    // Get and Set Values.
    // --------------------------------------------------------------------------------------------------

    // Set the value at the given path in _values.
    function set_value(path, value) {
        app_utils.set_sub_object(_values, path, value)
        node_graph_item.set_editable_inputs(_node_path, _values)
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
    		
    		// If we have an enum hint, return the respective text.
    		if (hints.hasOwnProperty(GUITypes.EnumHint)) {
    			return app_enums.get_enum_hint_value_text(hints[GUITypes.EnumHint], value)
    		}
    		
    		// Otherwise return the description for objects and arrays.
    		if (hints.hasOwnProperty(GUITypes.DescriptionHint) && 
	    				(value_type == GUITypes.Object || value_type == GUITypes.Array)) {
	    		return hints[GUITypes.DescriptionHint]
	    	}
    	}

        
        // Use the js type if the hints don't help.
        switch(value_type) {
        case GUITypes.String:
        case GUITypes.Boolean:
        case GUITypes.Number:
            return value.toString()
        case GUITypes.Object:
            return "folder of values"
        case GUITypes.Array:
            return "array of values"
        case GUITypes.Undefined:
            return "undefined"
        case GUITypes.Null:
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
    	
        switch(value_type) {
        case GUITypes.String:
            return 'qrc:///icons/ic_font_download_white_48dp.png'
        case GUITypes.Boolean:
            return 'qrc:///icons/ic_check_box_white_24dp.png'
        case GUITypes.Number:
            return 'qrc:///icons/ic_looks_3_white_48dp.png'
        case GUITypes.Object:
            return 'qrc:///icons/ic_folder_white_48dp.png'
        case GUITypes.Array:
            return 'qrc:///icons/ic_folder_white_48dp.png'
        case GUITypes.Undefined:
            return 'qrc:///icons/ic_warning_white_48dp.png'
        case GUITypes.Null:
            return 'qrc:///icons/ic_warning_white_48dp.png'
        default:
            console.log("Error: DataStackPage::get_image_for_value encountered unknown type at: " + path)
            console.log(new Error().stack);
        }
        return ""
    }

    // --------------------------------------------------------------------------------------------------
    // Push Objects or Arrays onto our Stack View.
    // --------------------------------------------------------------------------------------------------


    // Push next model on the stack.
    function push_by_model(title, model, hints, exposed) {
        var list_page = stack_view.create_page("DataListPage")
        list_page.set_exposed(exposed)
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
    	var exposed = get_exposed(path)
    	var hints = get_hints(path)
        var model = create_list_model()
        for (var prop in obj) {
            if (obj.hasOwnProperty(prop)) {
                model.append({name: prop, depth_index: stack_view.depth})
            }
        }
        push_by_model(title, model, hints, exposed)
    }

	// Used to bring up editable properties list of an array.
    function edit_array(title, path) {
    	var arr = get_value(path)
    	var exposed = get_exposed(path)
    	var hints = get_hints(path)
        var model = create_list_model()
        for (var i=0; i<arr.length; i++) {
            model.append({name: i.toString(), depth_index: stack_view.depth})
        }
        push_by_model(title, model, hints, exposed)
    }
    
    // Used to bring up non-editable properties list of an object.
    function view_object(title, path) {
       	var obj = get_value(path)
       	var exposed = get_exposed(path)
        var model = create_list_model()
        
        for (var prop in obj) {
            if (obj.hasOwnProperty(prop)) {
                model.append({name: prop, depth_index: stack_view.depth})
            }
        }
        push_by_model(title, model, {}, exposed)
    }
    
    // Used to bring up non-editable properties list of an array.
    function view_array(title, path) {
    	var arr = get_value(path)
    	var exposed = get_exposed(path)
        var model = create_list_model()
        for (var i=0; i<arr.length; i++) {
            model.append({name: i.toString(), depth_index: stack_view.depth})
        }
        push_by_model(title, model, {}, exposed)
    }
}
