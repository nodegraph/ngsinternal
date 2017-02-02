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
import smashbrowse.contentpages.listdelegates 1.0
import smashbrowse.contentpages.listpages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0

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
        if (mode == app_settings.downloads_mode) {
            stack_view_header.title_text = "download queue"
            visible = true;
        } else {
            visible = false;
        }
    }
    
    function on_download_double_clicked(row) {
    	var obj = model.get(row)
    	download_manager.reveal_file(obj.dir, obj.title)
    }

    function on_download_queued(pid, url) {
        model.append({"title": url, "dir": "", "description": "queued", "pid": pid, "status": "queued"})
    }
    
    function on_download_started(pid, dir, filename) {
        for (var i=0; i<model.count; i++) {
            var obj = model.get(i)
            if (obj.pid == pid) {
                obj.title = filename
                obj.description = ""
                obj.dir = dir
                obj.status = "downloading"
                model.set(i, obj);
                break;
            }
        }
    }
    
    function on_download_progress(pid, progress) {
        for (var i=0; i<model.count; i++) {
            var obj = model.get(i)
            if (obj.pid == pid) {
                obj.description = progress
                obj.status = "downloading"
                model.set(i, obj);
                break;
            }
        }
    }
    
    function on_download_finished(pid) {
        for (var i=0; i<model.count; i++) {
            var obj = model.get(i)
            if (obj.pid == pid) {
                obj.description = obj.description + '\n' + 'finished'
                obj.status = "finished"
                model.set(i, obj);
                break;
            }
        }
    }
    
    function on_download_errored(pid, error) {
        for (var i=0; i<model.count; i++) {
            var obj = model.get(i)
            if (obj.pid == pid) {
                obj.description = "error: " + error
                obj.status = "errored"
                model.set(i, obj);
                break;
            }
        }
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
                download_manager.download_on_the_side(drop.text)
                drop.accept()
            }
        }
        onEntered: {
        }
    }
    
}
