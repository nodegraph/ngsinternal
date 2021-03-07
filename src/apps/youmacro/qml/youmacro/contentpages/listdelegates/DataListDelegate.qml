import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import youmacro.appconfig 1.0
import youmacro.stackedmodepages 1.0
import youmacro.contentpages.editdatapages 1.0
import youmacro.fullpages 1.0
import youmacro.appwidgets 1.0
import youmacro.contentpages.listmodels 1.0

BaseImageIconListDelegate {
    id: delegate

	// Displayed Content.
	_image_url: get_stack_view() ? get_image_for_value() : ""
	_title: name
	_content: get_stack_view() ? get_string_for_value() : ""

    function get_image_for_value() {
        var path = get_stack_view().get_title_path(1, depth_index+1)
        path.push(name)
        return get_stack_page().get_image_for_value(path)
    }

    function get_string_for_value() {
        var path = get_stack_view().get_title_path(1, depth_index+1)
        path.push(name)
        return get_stack_page().get_string_for_value(path)
    }

    function on_double_clicked() {
    	if (get_stack_page()._allow_edits) {
    		get_stack_page().on_edit_element(name)
    	} else {
    		get_stack_page().on_view_element(name)
    	}
    }
    
}
