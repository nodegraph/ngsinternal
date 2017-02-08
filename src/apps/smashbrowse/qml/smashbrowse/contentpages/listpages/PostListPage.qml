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
    	stack_view_header.title_text = _num_posts.toString()
    	if (_num_posts == 1) {
    		stack_view_header.title_text += " post"
    	} else {
    		stack_view_header.title_text += " posts"
    	}
    }
    
    function on_post_double_clicked(row) {
    	var obj = model.get(row)
    	view_data_list_stack_page.view_outputs('posted value', obj.object)
    	
    }
    
    function on_post_value(post_type, title, obj) {
    	_num_posts += 1
    	model.append({"post_type": post_type, "title": title, "object": obj})
    }
    
    delegate: PostListDelegate{}
    
    model: ListModel {
    }
    model_is_dynamic: false

}
