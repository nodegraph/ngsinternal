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
    id: post_delegate
    
   	// Displayed Content.
	_image_url: "qrc:///icons/ic_insert_drive_file_white_48dp.png"
	_title: title
	_content: description
	
	// Override on_double_clicked to add custom functionality.
}
