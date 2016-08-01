import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import NodeGraphRendering 1.0
import octoplier.actionbars 1.0
import octoplier.pages 1.0
//import octoplier.dialogs 1.0
import octoplier.menus 1.0
import octoplier.appwidgets 1.0
import octoplier.appconfig 1.0
import octoplier.menumodels 1.0

import InputCompute 1.0

Rectangle {
    id: app_window
    anchors.fill: parent
    visible: true
    color: "blue"

    property alias node_graph_page: node_graph_page

    function on_closing(close) {
        console.log('on closing called')
        if (app_comm.is_polling()) {
            app_comm.stop_polling()
            app_comm.close_browser()
            close.accepted = false
            close_timer.start()
        }
    }

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

    FilePage {
        id: file_page
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

    EditNodePage {
        id: edit_node_page
        visible: false
    }

    ViewNodePage {
        id: view_node_page
        visible: false
    }

    CreatePasswordPage {
        id: create_password_page
        visible: false
    }

    CheckPasswordPage {
        id: check_password_page
        visible: false
    }

    MenuStackPage {
        id: menu_stack_page
        visible: false
    }

    // Stacked Pages.
    CreateFilePage {
        id: create_file_page
        visible: false
    }

    // Stuff to do when this component is completed.
    Component.onCompleted: {
        update_dependencies()
    }

    function update_dependencies() {
        // Mode change connections.
        main_bar.switch_to_mode.connect(file_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(node_graph_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(view_node_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(edit_node_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(posts_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(settings_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(menu_stack_page.on_switch_to_mode)

        // More option connections.
        main_bar.open_more_options.connect(node_graph_page.on_open_more_options)
//        main_bar.open_more_options.connect(view_node_page.on_open_more_options)
//        main_bar.open_more_options.connect(edit_node_page.on_open_more_options)
        main_bar.open_more_options.connect(file_page.on_open_more_options)

        // Node graph connections.
        node_graph_page.node_graph.node_graph_context_menu_requested.connect(
            menu_stack_page.on_node_graph_context_menu)
        node_graph_page.node_graph.node_context_menu_requested.connect(
            menu_stack_page.on_node_context_menu)
        node_graph_page.node_graph.group_node_context_menu_requested.connect(
            menu_stack_page.on_group_node_context_menu)

        // Copy paste menu.
        // Connection to bring up the copy paste menu on android.
        Qt.inputMethod.visibleChanged.connect(
      		copy_paste_bar.on_virtual_keyboard_visibility_changed)


        if (file_model.crypto_exists()) {
            file_model.load_crypto()
            check_password_page.visible = true
        } else {
            create_password_page.visible = true
        }

        // Other setup.
        //main_bar.on_switch_to_mode(app_settings.node_graph_mode)
        //file_model.load_graph()
        //node_graph_page.node_graph.frame_all() // We can't frame all here as it causes gl resources to be allocated before the first draw request.
        //node_graph_page.node_graph.update()

        //quick_view.closing.connect(app_window.onClosing)

        quick_view.closing.connect(app_window.on_closing)
    }

//    function onClosing() {
//        return false
//    }

//    focus: true

//    // Prevent the android hardware back from closing the app.
//    Keys.onReleased: {
//        if (event.key == Qt.Key_Back) {
//            event.accepted = true
//        }
//    }


}
