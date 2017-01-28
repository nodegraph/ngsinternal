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
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0


BaseListDelegate {
    id: delegate
    
    // Displayed Content.
    _image_url: get_image_url()
    _title: title
    _content: description
    property var _status: status
    
    title_point_size: app_settings.download_title_point_size
	description_point_size: app_settings.download_description_point_size
	item_height: app_settings.list_item_height_small
	
	function get_image_url() {
		if (_status == "downloading") {
			return "qrc:///icons/ic_cloud_download_white_48dp.png"
		} else if (_status == "finished") {
			return "qrc:///icons/ic_cloud_done_white_48dp.png"
		} else if (_status == "errored") {
			return "qrc:///icons/ic_cloud_off_white_48dp.png"
		}
	}
    
    function update_icon_to_downloading() {
    	_image_url = "qrc:///icons/ic_cloud_download_white_48dp.png"
    }
    
    function update_icon_to_finished() {
    	_image_url = "qrc:///icons/ic_cloud_done_white_48dp.png"
    }
    
    // Method Overrides.
    function on_double_clicked() {
    }
}
