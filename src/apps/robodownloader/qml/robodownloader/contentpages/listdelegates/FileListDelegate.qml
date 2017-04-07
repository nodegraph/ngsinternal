import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import robodownloader.appconfig 1.0
import robodownloader.stackedmodepages 1.0
import robodownloader.fullpages 1.0
import robodownloader.appwidgets 1.0
import robodownloader.contentpages.listmodels 1.0


BaseImageIconListDelegate {
    id: delegate
    
    // Displayed Content.
    _image_url: "qrc:///icons/ic_insert_drive_file_white_48dp.png"
    _title: title
    _content: description
    
    // Method Overrides.
    function on_double_clicked() {
    	get_stack_page().on_file_double_clicked(index)
    }
}
