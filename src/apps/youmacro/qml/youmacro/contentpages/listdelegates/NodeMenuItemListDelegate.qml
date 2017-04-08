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

BaseTextIconListDelegate {
    id: delegate
    
   	// Displayed Content.
	_letters: letters
	_title: title
	_content: description
	
	function on_double_clicked() {
		if (typeof script !== "undefined") {
                get_stack_view().execute_script(script)
        }
    }
}
