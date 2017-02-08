import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.appconfig 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.fullpages 1.0
import smashbrowse.contentpages.listpages 1.0
import smashbrowse.contentpages.listdelegates 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0

import GUITypes 1.0

// The max node posts is specified in app_settings.max_node_posts.

BaseListPage {
    id: page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z
    
    property int _num_posts: 0

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.posts_mode) {
            update_header()
            visible = true;
        } else {
            visible = false;
        }
    }
    
    function update_header() {
    	stack_view_header.title_text = _num_posts.toString() + " posts"
    }
    
    function on_post_double_clicked(row) {
    	var obj = model.get(row)
    	view_data_list_stack_page.on_view_outputs('value', {'value':obj.value})
    	
    }
    
    function on_post_value(post_type, title, value) {
    	console.log('post_type: ' + post_type)
    	console.log('title: ' + title)
    	console.log('value: ' + value)
    	
    	model.append({"post_type": post_type, "title": title, "value": value})
    }
    
    delegate: PostListDelegate{}
    
    model: ListModel {
    	ListElement {
    		post_type: GUITypes.Pass
	        title: "Testing passed"
	        value: "hello"
    	}
    	ListElement {
    		post_type: GUITypes.Fail
	        title: "Testing failed"
	        value: 123
    	}
    	ListElement {
    		post_type: GUITypes.Info
	        title: "Testing info"
	        value: true
    	}
    }
    model_is_dynamic: false

}
