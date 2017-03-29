import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import robodownloader.appconfig 1.0
import robodownloader.stackedmodepages 1.0
import robodownloader.fullpages 1.0
import robodownloader.appwidgets 1.0
import robodownloader.contentpages.listmodels 1.0
import robodownloader.appconfig 1.0

import GUITypes 1.0

BaseImageIconListDelegate {
    id: post_delegate
    
   	// Displayed Content.
	_image_url: get_image_url()
	_title: title
	_content: get_content_as_string() + '\n' + date
	
	function get_image_url() {
		if (post_type == GUITypes.Pass) {
			return "qrc:///icons/check-circle-outline-white.png";
		} else if (post_type == GUITypes.Fail) {
			return "qrc:///icons/ic_highlight_off_white_48dp.png";
		} else if (post_type == GUITypes.Info) {
			return "qrc:///icons/ic_info_outline_white_48dp.png";
		}
	}
	
	function get_content_as_string() {
		var value = object.value
		var value_type = app_enums.determine_js_type(value)
        switch(value_type) {
        case GUITypes.String:
            return value
        case GUITypes.Boolean:
            return value.toString()
        case GUITypes.Number:
            return value.toString()
        case GUITypes.Object:
            return 'object'
        case GUITypes.Array:
            return 'array'
        case GUITypes.Undefined:
            return 'undefined'
        case GUITypes.Null:
            return 'null'
        default:
            console.log("Error: PostListDelegate::get_content_as_string()")
            console.log(new Error().stack);
        }
	}
	
	// Method Overrides.
    function on_double_clicked() {
    	get_list_page().on_post_double_clicked(index)
    }
	
}
