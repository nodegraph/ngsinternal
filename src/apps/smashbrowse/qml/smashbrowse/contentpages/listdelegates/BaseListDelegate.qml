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

Rectangle {
    id: delegate
    width: parent.width
    height: app_settings.list_item_height_large
    border.color: "white"
    border.width: app_settings.list_item_border_width
    color: "transparent"
    
    // Displayed Content.
    property var _image_url: ""
    property var _title: ""
    property var _content: ""

	function on_pressed() {}
	function on_double_clicked() {}

    // -----------------------------------------------------------------
    // Methods.
    // -----------------------------------------------------------------

	// Returns the ListView that we're embedded in.
	function get_list_view() {
		return delegate.ListView.view
	}

	// Returns the ListPage that contains the ListView that we're embedded in.
	function get_list_page() {
		return delegate.ListView.view._list_page
	}

	// Returns the StackView that the ListPage is embedded in.
    function get_stack_view() {
        return delegate.ListView.view._list_page.Stack.view
    }

	// Returns the StackPage that contains the StackView.
    function get_stack_page() {
        return delegate.ListView.view._list_page.Stack.view._stack_page
    }

    Row {
        // The large icon on the left that indicates the data type.
        Rectangle {
            height: app_settings.list_item_height_large
            width: app_settings.list_item_height_large
            color: "transparent"
            Image {
                height: parent.height * 2/3
                width: parent.height * 2/3
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                source: delegate._image_url
            }
        }

        // The textual content on the right that refers to the data value.
        Column {
            anchors.verticalCenter: parent.verticalCenter
            // The name of the data in large text on top.
            Text {
                text: delegate._title
                font.family: "Arial"
                font.bold: true
                font.pointSize: app_settings.menu_page_title_point_size
                font.italic: false
                color: "white"
            }
            // The value of the data in smaller text on bottom.
            Text {
                text: delegate._content
                font.family: "Arial"
                font.pointSize: app_settings.menu_page_description_point_size
                font.italic: false
                color: "white"
                wrapMode: Text.Wrap
                maximumLineCount: 2
                width: app_settings.page_width - app_settings.list_item_height_large - app_settings.list_item_border_width - app_settings.page_right_margin
            }
        }
    }
    
    MouseArea {
        id: mouse_area
        anchors.fill: parent
        onPressed: {
            mouse.accepted = true
            // We need to specify the delegate id, in order to get the right ListView.
            delegate.ListView.view.currentIndex = index
            // Perform custom logic in derived QML items.
            on_pressed()
        }

        onDoubleClicked: {
            mouse.accepted = true
            // Set current index.
            // We need to specify the delegate id, in order to get the right ListView.
            delegate.ListView.view.currentIndex = index
			// Perform custom logic in derived QML items.
            on_double_clicked()
        }
    }
}
