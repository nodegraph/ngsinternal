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

    function on_download_queued(pid, url) {
        model.append({"title": url, "description": "queued", "pid": pid, "status": "queued"})
    }
    
    function on_download_started(pid, filename) {
        for (var i=0; i<model.count; i++) {
            var obj = model.get(i)
            if (obj.pid == pid) {
                obj.title = filename
                obj.description = ""
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
            if (drop.hasUrls) {
                download_manager.download_on_the_side(drop.urls[0])
            }
        }
        onEntered: {
        }
    }
    
}
