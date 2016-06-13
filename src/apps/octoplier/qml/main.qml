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
import octoplier.tools 1.0
import octoplier.apis 1.0
import octoplier.config 1.0
import octoplier.data 1.0
import octoplier.subpages 1.0

Rectangle {

    id: app_window
    //title: qsTr("octoplier")

    anchors.fill: parent
    //width: (Qt.platform.os == "android") ? Screen.width : 640
    //height: (Qt.platform.os == "android") ? Screen.height : 480
    visible: true
    color: "blue"

    property alias node_graph_page: node_graph_page

//    // Splash Page.
//    SplashPage {
//        id: splash_page
//        visible: true
//        num_components: app_window.num_components
//    }

//    style: ApplicationWindowStyle {
//        background: Rectangle {
//            color: "#FF03A9F4"
//        }
//    }

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

    NodePoolAPI {
        id: node_pool_api
        visible: false
    }

    // Dialogs.
    
    MessageDialog {
    	id: message_dialog
        title: "hello"
        icon: StandardIcon.Critical
        function show(text) {
            message_dialog.text = text
            message_dialog.open()
        }
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

    WebBrowserPage {
        id: web_browser_page
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

    UrlEntryPage {
        id: url_entry_page
        visible: false
    }

    EmptyPage {
        id: empty_page
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

    MenuStack {
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
        main_bar.switch_to_mode.connect(node_graph_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(web_browser_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(posts_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(settings_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(edit_node_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(view_node_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(file_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(menu_stack_page.on_switch_to_mode)

        // More option connections.
        main_bar.open_more_options.connect(node_graph_page.on_open_more_options)
        main_bar.open_more_options.connect(web_browser_page.on_open_more_options)
        main_bar.open_more_options.connect(view_node_page.on_open_more_options)
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

        // Browser.
        cpp_bridge.recording_received.connect(web_browser_page.web_view_alias.on_recording_received)

        // Other setup.
        //main_bar.on_switch_to_mode(app_settings.node_graph_mode)
        //file_model.load_graph()
        //node_graph_page.node_graph.frame_all() // We can't frame all here as it causes gl resources to be allocated before the first draw request.
        //node_graph_page.node_graph.update()

        //quick_view.closing.connect(app_window.onClosing)
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

    // Intercept the android back button.
    // We allow this behavior on desktops also for testing.
    // On desktops trying to close the app_window will just pop off the browser stack, until it's empty.
//    onClosing: {
//        //if (Qt.platform.os == "android") {
//        if (web_browser_page.visible == true) {
//            if (web_browser_page.web_view_alias.canGoBack) {
//                web_browser_page.web_view_alias.goBack()
//                close.accepted = false
//            }
//        }
//        //}

//        // Cleanup the node pool if we're closing down.
//        if (close.accepted) {
//            console.log("Shutting down qml app.")
//        }
//    }

}
