import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import smashbrowse.appconfig 1.0
import smashbrowse.stackviewpages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.menumodels 1.0


// This class basically just holds an AppStackView.
Rectangle{
    id: stack_page

    height: app_settings.page_height
    width: app_settings.page_width

    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    // Appearance.
    color: app_settings.menu_stack_bg_color

    // Our internal objects.
    property alias stack_view: stack_view

    // The main stack view.
    AppStackView{
        id: stack_view
        property alias _stack_page: stack_page
    }
}
