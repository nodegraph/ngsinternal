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
	_image_url: image_url
	_title: title
	_content: description
	
	function on_double_clicked() {
		if (typeof script !== "undefined") {
                get_stack_view().execute_script(script)
        }
    }
}
