import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0

import smashbrowse.appconfig 1.0
import smashbrowse.stackedpages 1.0
import smashbrowse.pages 1.0
import smashbrowse.delegates 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.menumodels 1.0

AppListPage {
    id: file_page

    // Setttings.
    show_back_button: file_page.Stack.view && (file_page.Stack.view.depth > 1)
    model: file_model
    model_is_dynamic: false
    delegate: FileListDelegate{}

    // Our File Methods.
    function init() {
    	set_title("File List")
        list_view.currentIndex = file_model.get_working_row();
    }
}
