import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
//import Qt.labs.settings 1.0

//import NodeGraphRendering 1.0
import smashbrowse.actionbars 1.0
import smashbrowse.pages 1.0
import smashbrowse.stackedpages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.appconfig 1.0
import smashbrowse.menumodels 1.0

import InputCompute 1.0

Rectangle {
    id: app_window
    anchors.fill: parent
    visible: true
    color: "blue"

    property alias node_graph_page: node_graph_page

    // Clean up routine.
    function on_closing(close) {
        if (app_comm.is_polling()) {
            // Make sure the polling is stopped.
            app_comm.stop_polling()
            close.accepted = false
            close_timer.start()
        } else if (app_comm.nodejs_is_connected()) {
            // Make nodejs shut itself down.
            // It will close the browser as part of its shutdown.
            var msg = new Message(Message.kShutdown)
            app_comm.handle_request_from_app(msg)
            
            //app_comm.handle_request_from_app('{"request": "shutdown"}')
            
            close.accepted = false
            close_timer.start()
        }
        node_graph_item.parent = null
    }

    // Timer that delays closing/exit a bit so we can clean up.
    Timer {
        id: close_timer
        interval: 100
        running: false
        repeat: false
        onTriggered: quick_view.close()
    }

    // Global App Objects.

    AppUnits {
        id: app_units
    }

    AppSettings {
        id: app_settings
    }

    AppUtils {
        id: app_utils
    }

    AppLoader {
        id: app_loader
    }

    AppTooltip {
        id: app_tooltip
        z: app_settings.app_tooltip_z
        visible: false
    }

    // Action Bars.

    MainBar {
        id: main_bar
    }

    CopyPasteBar {
        id: copy_paste_bar
        visible: false
        z: app_settings.copy_paste_bar_z
    }

    Splitter {
        id: splitter
        visible: true
    }

    // Pages.

    NodeGraphPage {
        id: node_graph_page
        visible: false
    }

    FileStackPage {
        id: file_stack_page
        visible: false
    }

    PostsPage {
        id: posts_page
        visible: false
    }

    SettingsPage {
        id: settings_page
        visible: false
    }

    DataStackPage {
        id: edit_node_page
        visible: false
        mode: app_settings.edit_node_mode
    }

    DataStackPage {
        id: view_node_page
        visible: false
        mode: app_settings.view_node_mode
    }

    CreatePasswordPage {
        id: create_password_page
        visible: false
    }

    CheckPasswordPage {
        id: check_password_page
        visible: false
    }

    LicensePage {
        id: license_page
        visible: false
    }

    NodeActionStackPage {
        id: node_action_stack_page
        visible: false
    }

    WebActionStackPage {
        id: web_action_stack_page
        visible: false
    }

    // Stuff to do when this component is completed.
    Component.onCompleted: {
        update_dependencies()
    }

    function update_dependencies() {
        // Mode change connections.
        main_bar.switch_to_mode.connect(file_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(node_graph_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(view_node_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(edit_node_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(posts_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(settings_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(node_action_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(web_action_stack_page.on_switch_to_mode)

        // Node graph connections.
        node_graph_item.node_graph_context_menu_requested.connect(node_action_stack_page.on_node_graph_context_menu)
        node_graph_item.node_context_menu_requested.connect(node_action_stack_page.on_node_context_menu)
        node_graph_item.group_node_context_menu_requested.connect(node_action_stack_page.on_group_node_context_menu)

        // Web actions mode.
        app_comm.show_web_action_menu.connect(web_action_stack_page.on_show_web_action_menu)

        // Copy paste menu.
        // Connection to bring up the copy paste menu on android.
        Qt.inputMethod.visibleChanged.connect(copy_paste_bar.on_virtual_keyboard_visibility_changed)

        // Present password pages.
        if (file_model.crypto_exists()) {
            file_model.load_crypto()
            check_password_page.visible = true
        } else {
            create_password_page.visible = true
        }

        // Hook up some pre-close tasks.
        quick_view.closing.connect(app_window.on_closing)

        // Node viewing and editing.
        node_graph_item.view_node_outputs.connect(view_node_page.on_show_data)
        node_graph_item.edit_node_params.connect(edit_node_page.on_show_data)
        

    }

//    // Prevent the android hardware back from closing the app.
//    Keys.onReleased: {
//        if (event.key == Qt.Key_Back) {
//            event.accepted = true
//        }
//    }

}
