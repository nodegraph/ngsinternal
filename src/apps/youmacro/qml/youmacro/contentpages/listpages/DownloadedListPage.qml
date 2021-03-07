import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import youmacro.appconfig 1.0
import youmacro.stackedmodepages 1.0
import youmacro.fullpages 1.0
import youmacro.contentpages.listdelegates 1.0
import youmacro.contentpages.listpages 1.0
import youmacro.appwidgets 1.0
import youmacro.contentpages.listmodels 1.0

import GUITypes 1.0

BaseListPage {
    id: page
    
    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z
    
    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == GUITypes.DownloadedMode) {
            update_header()
            visible = true
        } else {
            visible = false
        }
    }
    
    function update_header() {
    	stack_view_header.title_text = "double click to reveal file"
    }
    
    function on_download_double_clicked(row) {
    	var obj = model.get(row)
    	if (obj.merged_filename) {
    		download_manager.reveal_exact_filename(obj.dest_dir, obj.merged_filename)
    	} else {
    		download_manager.reveal_approximate_filename(obj.dest_dir, obj.video_filename)
    	}
    }

    function on_download_finished(model_obj) {
    	// Update the title.
    	if (model_obj.merged_filename) {
    		model_obj.title = model_obj.merged_filename
    	} else {
    		model_obj.title = model_obj.video_filename 
    	}
    	// If there's no file the title will be empty.
    	// In this case we don't add this to our model.
    	if (!model_obj.title) {
    		return
    	}
    	
    	// Update the description.
    	var date = new Date().toLocaleString(Qt.locale(), "yyyy-MM-dd hh:mm:ss");
    	model_obj.description = "finished at: " + date
    	model_obj.description += "\ndirectory: " + model_obj.dest_dir
    	// Insert the most recent at the top so that the user doesn't have to scroll to see the latest.
        model.insert(0, model_obj)
        
        // If the number of rows is getting too large trim it.
        if (model.count > 500) {
        	model.remove(model.count-1,1)
        }
    }
    
    delegate: DownloadListDelegate{}
    
    model: ListModel {
    }
    model_is_dynamic: false
    
}
