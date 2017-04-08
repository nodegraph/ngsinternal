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
import youmacro.contentpages.listdelegates 1.0
import youmacro.appwidgets 1.0
import youmacro.contentpages.listmodels 1.0

// The list page is used in menus. It has a stack view header and a list view.
Rectangle {
    id: page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearance.
    color: app_settings.menu_page_bg_color

    // Settings.
    property alias show_back_button: stack_view_header.show_back_button
    property alias model: list_view.model
    property alias model_is_dynamic: list_view.model_is_dynamic
    property alias delegate: list_view.delegate

    // Our internal objects.
    property alias stack_view_header: stack_view_header
    property alias list_view: list_view

    // Our Methods.
    function set_title(title) {
   		try {
        stack_view_header.title_text = title
        }
        catch(e) {
        	console.log(new Error().stack);
        }
    }
    function get_title() {
        return stack_view_header.title_text
    }

    // The stack view header.
    AppStackViewHeader {
        id: stack_view_header
        stack_view: page.Stack.view
    }
			
	// Our ListView.
    ListView {
        id: list_view
        clip: true

        // Dimensions.
        height: app_settings.menu_page_height
        width: app_settings.menu_page_width

        // Positioning.
        x: 0
        y: app_settings.action_bar_height
        z: 0

        // Our model and delegate.
        model: ListModel {}
        property bool model_is_dynamic: false
        
        delegate: MenuItemListDelegate{}
        currentIndex: -1

        // Appearance.
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        highlightMoveDuration: 0
        focus: true

        // Our parenting stack view.
        property var _list_page: page
    }

}
