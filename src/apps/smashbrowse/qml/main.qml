import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
//import Qt.labs.settings 1.0

//import NodeGraphRendering 1.0
import smashbrowse.actionbars 1.0
import smashbrowse.fullpages 1.0
import smashbrowse.modepages 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.contentpages.listpages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.appconfig 1.0
import smashbrowse.contentpages.listmodels 1.0

Rectangle {
    id: app_window
    anchors.fill: parent
    visible: true
    color: "blue"

    property alias node_graph_page: node_graph_page
    property bool tried_closing_browser: false

    // Clean up routine.
    function on_closing(close) {
        if (web_worker.is_polling()) {
            // Make sure the polling is stopped.
            web_worker.stop_polling()
            close.accepted = false
            close_timer.start()
        } else if (!tried_closing_browser) {
        	// Force close the browser.
        	web_worker.force_close_browser()
        	tried_closing_browser = true
        	close.accepted = false
            close_timer.start()
        } else if (web_worker.is_open()) {
            // Close down the websocket connection to nodejs.
            // Also terminate nodejs.
            web_worker.close()
            close.accepted = false
            close_timer.start()
        } else {
        	node_graph_item.parent = null
        	tried_closing_browser = false
        }
    }

    // Timer that delays closing/exit a bit so we can clean up.
    Timer {
        id: close_timer
        interval: 200
        running: false
        repeat: false
        onTriggered: quick_view.close()
    }

    // Enums.
    QtObject {
        id: js_enum
        property int kUndefined: 0
        property int kNull: 1
        property int kObject: 2
        property int kArray: 3
        property int kString: 4
        property int kNumber: 5
        property int kBoolean: 6
    }

    QtObject {
        id: msg_enum_enum // an enumeration of enumerations
        property int kNotEnumType: 0
        property int kMessageType: 1
        property int kInfoType: 2
        property int kMouseActionType: 3
        property int kTextActionType: 4
        property int kElementActionType: 5
        property int kWrapType: 6
        property int kDirectionType: 7
    }
    
    QtObject {
        id: hint_type // an enumeration of enumerations
        property int kEnum: 0
        property int kMin: 1
        property int kMax: 2
    }


    // Global App Objects.

    AppEnums {
        id: app_enums
    }

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

    FileMenuListStackPage {
        id: file_menu_list_stack_page
        visible: false
    }

    PostListPage {
        id: posts_page
        visible: false
    }

    SettingsPage {
        id: settings_page
        visible: false
    }

    DataListStackPage {
        id: edit_data_list_stack_page
        visible: false
        _allow_edits: true
    }

    DataListStackPage {
        id: view_data_list_stack_page
        visible: false
        _allow_edits: false
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

    NGMenuListStackPage {
        id: ng_menu_list_stack_page
        visible: false
    }

    WebMenuListStackPage {
        id: web_menu_list_stack_page
        visible: false
    }

    // Stuff to do when this component is completed.
    Component.onCompleted: {
        update_dependencies()
    }

    function update_dependencies() {
        // Mode change connections.
        main_bar.switch_to_mode.connect(file_menu_list_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(node_graph_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(view_data_list_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(edit_data_list_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(posts_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(settings_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(ng_menu_list_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(web_menu_list_stack_page.on_switch_to_mode)

        // Node graph connections.
        node_graph_item.node_graph_context_menu_requested.connect(ng_menu_list_stack_page.on_node_graph_context_menu)
        node_graph_item.node_context_menu_requested.connect(ng_menu_list_stack_page.on_node_context_menu)
        node_graph_item.group_node_context_menu_requested.connect(ng_menu_list_stack_page.on_group_node_context_menu)

        // Web actions mode.
        web_worker.show_web_action_menu.connect(web_menu_list_stack_page.on_show_web_action_menu)
        web_worker.show_iframe_menu.connect(web_menu_list_stack_page.on_show_iframe_menu)
        web_worker.select_option_texts.connect(web_menu_list_stack_page.on_select_option_texts)
        web_recorder.web_action_ignored.connect(web_menu_list_stack_page.on_busy)

        // Copy paste menu.
        // Connection to bring up the copy paste menu on android.
        Qt.inputMethod.visibleChanged.connect(copy_paste_bar.on_virtual_keyboard_visibility_changed)

		// Determine the next page to show.
		if (license_checker.license_is_valid()) {
			// Skip right to the node graph.
			main_bar.switch_to_mode(app_settings.node_graph_mode)
			app_utils.load_last_graph()
			app_utils.frame_all_on_idle()
		} else if (file_model.crypto_exists()) {
			// Go to the check password page.
            file_model.load_crypto()
            check_password_page.visible = true
        } else {
        	// Go to the create a new password page.
            create_password_page.visible = true
        }

        // Hook up some pre-close tasks.
        quick_view.closing.connect(app_window.on_closing)

        // Node viewing and editing.
        node_graph_item.view_node_outputs.connect(view_data_list_stack_page.on_show_data)
        node_graph_item.edit_node_params.connect(edit_data_list_stack_page.on_show_data)
    }

//    // Prevent the android hardware back from closing the app.
//    Keys.onReleased: {
//        if (event.key == Qt.Key_Back) {
//            event.accepted = true
//        }
//    }

}
