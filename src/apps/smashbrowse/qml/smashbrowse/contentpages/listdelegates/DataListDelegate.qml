import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.appconfig 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.contentpages.editdatapages 1.0
import smashbrowse.fullpages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0

BaseListDelegate {
    id: delegate

	// Displayed Content.
	_image_url: get_stack_view() ? get_image_for_value() : ""
	_title: name
	_content: get_stack_view() ? get_string_for_value() : ""

    function get_image_for_value() {
        var path = get_stack_view().get_title_path(1, depth_index+1)
        path.push(name)
        var value = get_stack_page().get_value(path)
        return get_stack_page().get_image_for_value(value)
    }

    function get_string_for_value() {
        var path = get_stack_view().get_title_path(1, depth_index+1)
        path.push(name)
        var value = get_stack_page().get_value(path)
        return get_stack_page().get_string_for_value(value)
    }

    function on_double_clicked() {
        // Use functionality implemented on our containing stack view.
        var stack_view = get_stack_view()
        var stack_page = get_stack_page()

        // Get our current value and type.
        var path = stack_view.get_title_path(1, depth_index+1)
        path.push(name)
        var value = stack_page.get_value(path)
        var value_type = app_enums.get_js_enum(value)

        // Push a different page depending on the value type.
        switch(value_type) {
        case js_enum.kString:
            if (stack_page._allow_edits) {
                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditStringPage.qml", edit_data_list_stack_page, {})
                page.set_value(value)
                page.set_title(name)
                stack_view.push_page(page)
            }
            break
        case js_enum.kBoolean:
            if (stack_page._allow_edits) {
                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditBooleanPage.qml", edit_data_list_stack_page, {})
                page.set_value(value)
                page.set_title(name)
                stack_view.push_page(page)
            }
            break
        case js_enum.kNumber:
            if (stack_page._allow_edits) {
                var page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/editdatapages/EditNumberPage.qml", edit_data_list_stack_page, {})
                page.set_value(value)
                page.set_title(name)
                stack_view.push_page(page)
            }
            break
        case js_enum.kObject:
            stack_page.view_object(name, value);
            break
        case js_enum.kArray:
            stack_page.view_array(name, value);
            break
        default:
            console.log("Error: DataListDelegate::onDoubleClicked encountered unknown type: " + value_type)
            break
        }
    }
    
}
