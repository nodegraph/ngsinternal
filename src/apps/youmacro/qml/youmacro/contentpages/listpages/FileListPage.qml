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
import youmacro.contentpages.listpages 1.0
import youmacro.contentpages.listdelegates 1.0
import youmacro.appwidgets 1.0
import youmacro.contentpages.listmodels 1.0

BaseListPage {
    id: page

    // Setttings.
    model: file_model
    model_is_dynamic: false
    delegate: FileListDelegate{}

    // Our File Methods.
    function init() {
    	set_title("available files")
        list_view.currentIndex = file_model.get_working_row();
    }
}
