import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.appconfig 1.0
import octoplier.stackedpages 1.0
import octoplier.pages 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0

// MenuPage's are created dynamically and given a MenuModel.
// They are then pushed on to the menu_stack_page.
Rectangle {
    id: menu_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: 0
    y: 0
    z: 0

    // Appearance.
    color: app_settings.menu_page_bg_color

    // Our Model.
    property var title: "hello"
    property var model:  NodeActions {}

    // Our Dependencies.
    property var parent_stack_view

    // Our ListView.
    ListView {
        id: list_view

        // Dimensions.
        height: app_settings.menu_page_height
        width: app_settings.menu_page_width

        // Positioning.
        x: 0
        y: 0
        z: 0

        // Our model and delegate.
        model: menu_page.model
        delegate: AppMenuItemDelegate{}
        currentIndex: -1

        // Appearance.
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        highlightMoveDuration: 0
        focus: true

        // Our parenting stack page. This should be set when pushed onto the stack view.
        property var parent_stack_view: menu_page.parent_stack_view
    }
}
