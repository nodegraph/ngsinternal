import QtQuick 2.7
import QtQuick.Controls 1.5
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Styles 1.4
//import Qt.labs.settings 1.0

//import NodeGraphRendering 1.0
import youmacro.actionbars 1.0
import youmacro.fullpages 1.0
import youmacro.modepages 1.0
import youmacro.stackedmodepages 1.0
import youmacro.contentpages.listpages 1.0
import youmacro.appwidgets 1.0
import youmacro.appconfig 1.0
import youmacro.contentpages.listmodels 1.0
import youmacro.contentpages.enterdatapages 1.0

Rectangle {
    id: app_window
    anchors.fill: parent
    visible: true
    color: "blue"

    property alias node_graph_page: node_graph_page
    property bool tried_closing_browser: false
    property int num_close_tries: 0

    // Clean up routine.
    function on_closing(close) {
    	// Stop the web driver service.
    	if (num_close_tries == 0) {
    		manipulator.force_stack_reset()
    		task_queuer.close_browser()
    		task_queuer.queue_stop_service()
    		close.accepted = false
    		num_close_tries += 1
    		close_timer.start()
    	} else if (manipulator.is_busy_cleaning() || manipulator.is_waiting_for_response()){
    		close.accepted = false
    		num_close_tries += 1
    		close_timer.start()
    	} else {
    		node_graph_item.parent = null
    		close_timer.stop()
    	}
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
    
    ErrorPage {
    	id: error_page
    	visible: false
    }

    FileMenuListStackPage {
        id: file_menu_list_stack_page
        visible: false
    }
    
    MacroListStackPage {
    	id: macro_list_stack_page
    	visible: false
    }
    
    DownloadListPage {
    	id: downloads_page
        visible: false
    }
    
    DownloadedListPage {
    	id: downloaded_page
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

    PublishMacroPage {
        id: publish_macro_page
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
        main_bar.switch_to_mode.connect(posts_page.on_switch_to_mode)
        
		main_bar.switch_to_mode.connect(macro_list_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(downloads_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(downloaded_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(settings_page.on_switch_to_mode)
        
        // Modes without buttons.
        main_bar.switch_to_mode.connect(view_data_list_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(edit_data_list_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(ng_menu_list_stack_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(web_menu_list_stack_page.on_switch_to_mode)

        // Node graph connections.
        node_graph_item.node_graph_context_menu_requested.connect(ng_menu_list_stack_page.on_node_graph_context_menu)
        node_graph_item.node_context_menu_requested.connect(ng_menu_list_stack_page.on_node_context_menu)
        node_graph_item.group_node_context_menu_requested.connect(ng_menu_list_stack_page.on_group_node_context_menu)

        // Web actions mode.
        task_queuer.show_web_action_menu.connect(web_menu_list_stack_page.on_show_web_action_menu)
        browser_recorder.web_action_ignored.connect(web_menu_list_stack_page.on_action_ignored)
        
        // Download manager connections.
        download_manager.download_queued.connect(downloads_page.on_download_queued)
        download_manager.download_started.connect(downloads_page.on_download_started)
        download_manager.download_progress.connect(downloads_page.on_download_progress)
        download_manager.download_complete.connect(downloads_page.on_download_complete)
        download_manager.download_merged.connect(downloads_page.on_download_merged)
        download_manager.download_finished.connect(downloads_page.on_download_finished)
        download_manager.download_errored.connect(downloads_page.on_download_errored)
        
        // Manipuloatr connections.
        manipulator.post_value.connect(posts_page.on_post_value)

        // Copy paste menu.
        // Connection to bring up the copy paste menu on android.
        Qt.inputMethod.visibleChanged.connect(copy_paste_bar.on_virtual_keyboard_visibility_changed)

		// Determine the next page to show.
		if (license_checker.license_is_valid()) {
			// Skip right to the node graph.
			main_bar.switch_to_mode(app_settings.node_graph_mode)
			app_utils.load_last_graph()
			app_utils.frame_all_on_idle()
		} else if (crypto_logic.crypto_exists()) {
			// Go to the check password page.
            crypto_logic.load_crypto()
            check_password_page.visible = true
        } else {
        	// Go to the create a new password page.
            create_password_page.visible = true
        }

        // Hook up some pre-close tasks.
        quick_view.closing.connect(app_window.on_closing)

        // Node viewing and editing.
        node_graph_item.view_node_outputs.connect(view_data_list_stack_page.on_view_outputs)
        node_graph_item.edit_node_inputs.connect(edit_data_list_stack_page.on_edit_inputs)
        node_graph_item.set_error_message.connect(error_page.set_error_message)
        node_graph_item.show_error_page.connect(error_page.show_page)
    }

//    // Prevent the android hardware back from closing the app.
//    Keys.onReleased: {
//        if (event.key == Qt.Key_Back) {
//            event.accepted = true
//        }
//    }

}
