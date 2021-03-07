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
        if (mode == GUITypes.DownloadsMode) {
            update_header()
            visible = true
        } else {
            visible = false
        }
    }
    
    function update_header() {
    	stack_view_header.title_text = "double click to cancel"
    }
    
    function on_download_double_clicked(row) {
    	var obj = model.get(row)
    	download_manager.cancel_download(obj.id)
    	
    	// Remove the element.
    	model.remove(row,1)
    }

    function on_download_queued(id, url, dest_dir) {
        model.append({"id": id, "title": url, "dest_dir": dest_dir, "description": "queued", "video_filename": "", "audio_filename": "", "merged_filename": "", "download_state": GUITypes.Queued})
    }
    
    function on_download_started(row, filename) {
        var obj = model.get(row)
        if (obj.video_filename == "") {
        	obj.video_filename = filename
        	obj.title = filename
        } else {
        	obj.audio_filename = filename
        	obj.title = filename
        }
        obj.description = "downloading"
        obj.download_state = GUITypes.Downloading
    }
    
    function on_download_progress(row, percentage, file_size, speed, time) {
    	var obj = model.get(row)
    	//console.log('percentage: ' + percentage)
    	//console.log('file_size: ' + file_size)
    	//console.log('speed: ' + speed)
        obj.description = percentage + "% of " + file_size + " completed at a speed of " + speed
        obj.download_state = GUITypes.Downloading
    }
    
    function on_download_complete(row, percentage, file_size, time) {
    	var obj = model.get(row)
        obj.description = percentage + "% of " + file_size + " completed in a time of " + time
        obj.download_state = GUITypes.Downloading
    }
    
    function on_download_merged(row, merged_filename) {
        var obj = model.get(row)
        obj.merged_filename = merged_filename
    }
    
    
    function on_download_finished(row) {
    	var obj = model.get(row)
    	if (obj.download_state == GUITypes.Errored) {
    		obj.description = "Errors encountered downloading this file.\n" + obj.description
    	} else {
        	obj.description += "\nfinished"
        	obj.download_state = GUITypes.Finished
        }
        
        //console.log("finished video filename: " + obj.video_filename)
        //console.log("finished audio filename: " + obj.audio_filename)
        //console.log("finished merged filename: " + obj.merged_filename)
        
    	// Move the model object over to the downloaded_page.
    	downloaded_page.on_download_finished(obj)
    	
    	// Remove the element.
    	model.remove(row,1)
    }
    
    function on_download_errored(row, error) {
        var obj = model.get(row)
        obj.description = "error: " + error
        obj.download_state = GUITypes.Errored
        
        // Remove the element.
    	model.remove(row,1)
    }
    
    delegate: DownloadListDelegate{}
    
    model: ListModel {
    }
    model_is_dynamic: false

    DropArea {
        id: drop_area
        anchors.fill: parent
        onDropped: {
        	//console.log("drop has urls: " + drop.hasUrls)
        	//console.log("drop has num urls: " + drop.urls.length)
        	//console.log("drop has text: " + drop.hasText)
        	//console.log("drop text: " + drop.text)
        	//console.log("drop has color: " + drop.hasColor)
        	//console.log("drop color: " + drop.colorData)
        	//console.log("drop html: " + drop.html)
        	//console.log("drop keys: " + drop.keys)
            if (drop.hasText) {
            	// When you drag and drop you get the best quality video and audio.
                download_manager.download(drop.text, "", true, "", true, 0, 0, 0)
                drop.accept()
            }
        }
        onEntered: {
        }
    }
    
}
