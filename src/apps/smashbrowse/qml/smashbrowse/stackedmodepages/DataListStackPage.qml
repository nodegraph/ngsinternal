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
    function on_show_data(node_name, values, hints) {
        app_settings.vibrate()
        stack_view.clear_pages()

        // Show the next object.
        _values = values
        _hints = hints
        
        console.log("showing data hints: " + JSON.stringify(_hints))
        
        var path = []
        view_object(node_name, path)
    }
    
    function on_add_element() {
        // Push a page to get a string name or an array index to insert at in the object in or array respectively.
        // Then call __on_add_element with the result ... not you can set the call on the EnterStringPage for example.

    }

    // The element_name is a string when adding to an object.
    // The element_name is an index to insert at when adding to an array. Use -1 is you want to add at the end.
    function __on_add_element(element_name) {
    	var path = stack_view.get_title_path(1, stack_view.depth)
    	
    	// Get value info.
    	var value = stack_page.get_value(path)
    	var value_type = app_enums.get_js_enum(value)
    
    	// Hints are mandatory on objects or arrays.
        var hints = stack_page.get_hints(path)
        if (!hints) {
        	return
        }
        
        // A resizable hint is needed to be resizable.
        var resizable = hints[hint_type.kResizable]
        if (!resizable) {
        	return
        }
        
        // A js type hint is needed to be resizable.
        var js_type = hints[hint_type.kJSType]
        if (js_type === undefined) {
        	return
        }
        
        // Only arrays and objects can be resized.
        if (!(value_type == js_type.kArray || value_type == js_type.kObject)) {
        	return
        } 
        
        // Add a new element.
        if (value_type == js_type.kArray) {
        	// Add an array element.
	        if (js_type == js_type.kString) {
                value.splice(element_name, 0, '');
	        } else if (js_type == js_type.kNumber) {
                value.splice(element_name, 0, 0);
	        } else if (js_type == js_type.kBoolean) {
                value.splice(element_name, 0, false);
	        }
        } else if (value_type == js_type.kObject) {
        	// Add an object element.
        	if (js_type == js_type.kString) {
                value[element_name] = ''
	        } else if (js_type == js_type.kNumber) {
                value[element_name] = 0
	        } else if (js_type == js_type.kBoolean) {
                value[element_name] = false
	        }
        }
        
        // Record the new value.
        stack_page.set_value(path, value)
        
        // Refresh the page to show the new value.
        var tail_name = path[path.length-1]
        stack_view.pop_page()
        on_push_edit_page(tail_name)

//        if (value_type == js_enum.kObject) {
//       		var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/enterdatapages/EnterStringPage.qml", page, {})
//        	push_page.visible = true
//        	push_page.set_value("unnamed")
//        	push_page.set_title("Name for New Element.")
//        	push_page.callback = stack_page.on_add_element_with_name
//        	stack_view.push_page(push_page)
//        } else if (value_type == js_enum.kArray) {
//        	var value = app_utils.get_sub_object(_values, path)
//        	var name = path[path.length-1]
//        	value.length += 1
//        	stack_view.pop_page()
//        	on_push_edit_page(name)
//        }
        
    }
    
    function on_remove_element(name) {
    
    }
    
    function on_push_edit_page(name) {
        // Get our current value and type.
        var path = stack_view.get_title_path(1, stack_view.depth)
        path.push(name)
        var value = stack_page.get_value(path)
        var hints = stack_page.get_hints(path)
        var value_type = app_enums.get_js_enum(value)
        console.log('hints: ' + JSON.stringify(hints))

        // Push a different page depending on the value type.
        switch(value_type) {
        case js_enum.kString:
            if (stack_page._allow_edits) {
                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditStringPage.qml", edit_data_list_stack_page, {})
                page.set_value(value)
                page.set_title(name)
                if (hints) { 
                	if (hints.hasOwnProperty(hint_type.kDescription)) {
                		console.log('setting description: ' + hints[hint_type.kDescription])
                    	page.set_description(hints[hint_type.kDescription])
                	}
                	if (hints.hasOwnProperty(hint_type.kResizable)) {
                		console.log('setting resizable: true')
                    	page.resizable = true
                	}
                }
                stack_view.push_page(page)
            }
            break
        case js_enum.kBoolean:
            if (stack_page._allow_edits) {
                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditBooleanPage.qml", edit_data_list_stack_page, {})
                page.set_value(value)
                page.set_title(name)
                if (hints) {
                	if (hints.hasOwnProperty(hint_type.kDescription)) {
                    	page.set_description(hints[hint_type.kDescription])
                    }
                    if (hints.hasOwnProperty(hint_type.kResizable)) {
                    	page.resizable = true
                	}
                }
                stack_view.push_page(page)
            }
            break
        case js_enum.kNumber:
            if (stack_page._allow_edits) {
            	if (hints && hints.hasOwnProperty(hint_type.kEnum)) {
            		var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditEnumPage.qml", edit_data_list_stack_page, {})
                    page.set_enum_type(hints[hint_type.kEnum])
	                page.set_value(value)
	                page.set_title(name)
	                if (hints && hints.hasOwnProperty(hint_type.kDescription)) {
	                	page.set_description(hints[hint_type.kDescription])
	                }
	                if (hints && hints.hasOwnProperty(hint_type.kResizable)) {
                    	page.resizable = true
                	}
	                stack_view.push_page(page)
            	} else {
	                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditNumberPage.qml", edit_data_list_stack_page, {})
	                page.set_value(value)
	                page.set_title(name)
                    if (hints && hints.hasOwnProperty(hint_type.kDescription)) {
                        page.set_description(hints[hint_type.kDescription])
                    }
                    if (hints && hints.hasOwnProperty(hint_type.kResizable)) {
                    	page.resizable = true
                	}
	                stack_view.push_page(page)
                }
            } else {
//            	if (hints && hints.hasOwnProperty(hint_type.kDescription) {
//            		var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/ViewDescriptionPage.qml", edit_data_list_stack_page, {})
//	                page.set_title(name)
//	                page.description = hints[hint_type.kDescription]
//	                stack_view.push_page(page)
//            	}
            }
            break
        case js_enum.kObject:
        	console.log('----- object')
            stack_page.view_object(path[path.length-1], path);
            break
        case js_enum.kArray:
        	console.log('----- array')
            stack_page.view_array(path[path.length-1], path);
            break
        default:
            console.log("Error: DataListDelegate::onDoubleClicked encountered unknown type: " + value_type)
            break
        }
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
    function get_string_for_value(path) {
    	var value = get_value(path)
    	var value_type = app_enums.get_js_enum(value)
    	var hints = get_hints(path)
    	
    	// Use the hints to get a more descriptive string representation.
    	if (hints) {
    		if (hints.hasOwnProperty(hint_type.kEnum)) {
        		return app_enums.get_msg_enum_text(hints[hint_type.kEnum], value)
        	} else if (hints.hasOwnProperty(hint_type.kDescription) && (value_type == js_enum.kObject || value_type == js_enum.kArray)) {
        		return hints[hint_type.kDescription]
        	}
        }
    	
        // Use the raw values if the hints don't help.
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

    // Display the contents of a dict.
    function view_object(title, path) {
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

    // Display the contents of an array.
    function view_array(title, path) {
    	var arr = get_value(path)
    	var hints = get_hints(path)
        var model = create_list_model()
        for (var i=0; i<arr.length; i++) {
            model.append({name: i.toString(), depth_index: stack_view.depth})
        }
        console.log('pushing arr: ' + title + ' hints: ' + JSON.stringify(hints))
        push_by_model(title, model, hints)
    }
}
