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

// The max node posts is specified in app_settings.max_node_posts.

AppListPage {
    id: posts_page

    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width

    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.posts_mode) {
            stack_view_header.title_text = "Posts from Nodes"
            visible = true;
        } else {
            visible = false;
        }
    }

    // Our settings.
    show_back_button: false
    delegate: AppPostDelegate{}
}
