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
import youmacro.appwidgets 1.0
import youmacro.contentpages.listmodels 1.0

import GUITypes 1.0

BaseImageIconListDelegate {
    id: delegate
    
    // Displayed Content.
    _image_url: get_image_url()
    _title: title
    _content: description
    
    title_point_size: app_settings.download_title_point_size
	description_point_size: app_settings.download_description_point_size
	item_height: app_settings.list_item_height_small
	
	function get_image_url() {
		if (download_state == GUITypes.Queued) {
			return "qrc:///icons/ic_cloud_queue_white_48dp.png"
		} else if (download_state == GUITypes.Downloading) {
			return "qrc:///icons/ic_cloud_download_white_48dp.png"
		} else if (download_state == GUITypes.Finished) {
			return "qrc:///icons/ic_cloud_done_white_48dp.png"
		} else {
			return "qrc:///icons/ic_cloud_off_white_48dp.png"
		}
	}
    
    // Method Overrides.
    function on_double_clicked() {
    	get_list_page().on_download_double_clicked(index)
    }
    
}
