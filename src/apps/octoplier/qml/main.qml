import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

import NodeGraphRendering 1.0
import octoplier.actionbars 1.0
import octoplier.pages 1.0
import octoplier.dialogs 1.0
import octoplier.menus 1.0
import octoplier.tools 1.0
import octoplier.apis 1.0
import octoplier.config 1.0

ApplicationWindow {

    id: app_window
    title: qsTr("octoplier")
    width: (Qt.platform.os == "android") ? Screen.width : 640
    height: (Qt.platform.os == "android") ? Screen.height : 480
    visible: true
    color: "blue"

    // Components tracking.
    property var num_components: 21
    property var incubators: []

    // Action Bars.
    property var main_bar
    property var copy_paste_bar
    property var splitter

    // Pages.
    property var web_browser_page
    property var empty_page
    property var node_graph_page
    property var posts_page
    property var settings_page
    property var edit_node_page
    property var view_node_page
    property var url_entry_page

    // Menus.
    property var web_browser_menu
    property var node_graph_menu
    property var node_menu
    property var group_node_menu
    property var view_node_menu

    // Dialogs.
    property var message_dialog
    //property var choose_file_dialog
    property var url_dialog
    property var node_name_dialog

    // App Tools.
    property var app_tooltip

    // Apis.
    property var node_pool_api

//    // Splash Page.
//    SplashPage {
//        id: splash_page
//        visible: true
//        num_components: app_window.num_components
//    }

    style: ApplicationWindowStyle {
        background: Rectangle {
            color: "#FF03A9F4"
        }
    }

    // Global App Objects.
    AppUnits {
        id: app_units
    }

    AppSettings {
        id: app_settings
    }
    
    // Our custom node graph quick item won't load dynamically in Qt 5.6.
    // So we declare it here.
    //NodeGraphPage {
    //    id: node_graph_page
    //}
    
    //NodeGraphQuickItem {
    //    id: deviceItem
    //    objectName: "node_graph_object"
    //    anchors.fill: parent
    //    anchors.margins: 0
    //    opacity: 1
    //}
    
//    NodeGraphQuickItem {
//        id: deviceItem
//        height: app_settings.page_height
//        width: app_settings.page_width
//        x: app_settings.page_x
//        y: app_settings.page_y
//        z: app_settings.page_z
//    }
    
    //Component.onCompleted: {
    //    deviceItem.build_test_graph()
    //}
    
    // Stuff to do when this component is completed.
    Component.onCompleted: {
    	//update()
        load_components()
    }

    // Methods.
    function load_component(member_name, file, parent, properties) {
        var comp = Qt.createComponent(file)
        var obj = comp.createObject(parent, properties)
        if (obj == null) {
            console.log("Error creating a componentL " + file)
        }
        this[member_name] = obj
        console.log("finshed creating: " + member_name)
        cpp_bridge.mark_load_progress()
        //cpp_bridge.process_events()
    }
    function load_component2(member_name, file, parent, properties) {
        var comp = Qt.createComponent(file)
        print("starting to load: ", file);
        var incubator = comp.incubateObject(parent, properties);
        // Check if the qml file has syntax errors.
        if (!incubator) {
            console.log("Error loading qml file: " + file)
        }
        // Hook up callbacks to the incubator.
        if (incubator.status != Component.Ready) {
            incubator.onStatusChanged = function(status) {
                if (status == Component.Ready) {
                    print ("Object", incubator.object, "is now ready!");
                    parent[member_name] = incubator.object
                    on_finished_loading()
                }
            }
        } else {
            print ("Object", incubator.object, "is ready immediately!");
            parent[member_name] = incubator.object
            on_finished_loading()
        }
        // Hold a reference to the incubator so that it won't get garbage collected.
        incubators.push(incubator)
    }

    function on_finished_loading() {
        cpp_bridge.mark_load_progress()
        if (cpp_bridge.at_marker_1()) {
            update_dependencies()
            // Now show the main app window.
            visible = true
            //cpp_bridge.process_events();
        }
    }

    function load_components() {
        console.log('wwwwww: '+num_components)
        cpp_bridge.set_max_load_progress(num_components + 1) // The +1 is for updating the dependencies between components.
        cpp_bridge.set_marker_1(num_components)
        //cpp_bridge.process_events();

        // Here we create qml objects dynamically from javascript.
        // Quote from Qt 5.6 docs:
        // Also, note that while dynamically created objects may be used the same as other objects, they do not have an id in QML.

        load_component("web_browser_page", "octoplier/pages/WebBrowserPage.qml",
                                          app_window, {
                                              "visible": false
                                          })

        load_component("posts_page","octoplier/pages/PostsPage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component("settings_page","octoplier/pages/SettingsPage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component("edit_node_page","octoplier/pages/EditNodePage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component("view_node_page","octoplier/pages/ViewNodePage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component("url_entry_page","octoplier/pages/UrlEntryPage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component("empty_page","octoplier/pages/EmptyPage.qml",
                                    app_window, {
                                        "visible": false
                                    })

        // Our Action Bars.
        load_component("main_bar","octoplier/actionbars/MainBar.qml", app_window, {})
        load_component("copy_paste_bar","octoplier/actionbars/CopyPasteBar.qml",
                                        app_window, {
                                            "visible": false,
                                            z: app_settings.copy_paste_bar_z
                                        })
        load_component("splitter","octoplier/actionbars/Splitter.qml",
                                  app_window, {"visible": false})

        // Our Menus.
        load_component("web_browser_menu","octoplier/menus/WebBrowserMenu.qml",
                                  app_window, {"visible": false})
        load_component("node_graph_menu","octoplier/menus/NodeGraphMenu.qml",
                                  app_window, {"visible": false})
        load_component("node_menu","octoplier/menus/NodeMenu.qml",
                                  app_window, {"visible": false})
        load_component("group_node_menu","octoplier/menus/GroupNodeMenu.qml",
                                  app_window, {"visible": false})
        load_component("view_node_menu","octoplier/menus/ViewNodeMenu.qml",
                                  app_window, {"visible": false})

//        // Our Dialogs.
        load_component("message_dialog","octoplier/dialogs/MessageDialog.qml",
                                        app_window, {"visible": false})
//        load_component("choose_file_dialog","octoplier/dialogs/ChooseFileDialog.qml",
//                                        app_window, {"visible": false})
        load_component("url_dialog","octoplier/dialogs/UrlDialog.qml",
                                        app_window, {"visible": false})
        load_component("node_name_dialog","octoplier/dialogs/NodeNameDialog.qml",
                                        app_window, {"visible": false})

        // Our App Tools.
        // This always draws on top of everything else.
        load_component("app_tooltip","octoplier/tools/AppTooltip.qml",
                                    app_window, {"z": app_settings.app_tooltip_z, "visible": false})

        // Our App Apis.
        load_component("node_pool_api","octoplier/apis/NodePoolAPI.qml",
                                    app_window, {"visible": false})
                                    
        load_component("node_graph_page","octoplier/pages/NodeGraphPage.qml",
                    app_window, {
                        "visible": false
                   })


        update_dependencies()
        // Now show the main app window.
        visible = true

        cpp_bridge.mark_load_progress()
    }

    function load_components2() {

        cpp_bridge.set_max_load_progress(num_components + 1) // The +1 is for updating the dependencies between components.
        cpp_bridge.set_marker_1(num_components)
        //cpp_bridge.process_events();

        // Here we create qml objects dynamically from javascript.
        // Quote from Qt 5.6 docs:
        // Also, note that while dynamically created objects may be used the same as other objects, they do not have an id in QML.

        load_component2("web_browser_page", "octoplier/pages/WebBrowserPage.qml",
                                          app_window, {
                                              "visible": false
                                          })
        load_component2("node_graph_page","octoplier/pages/NodeGraphPage.qml",
                                         app_window, {
                                             "visible": false,
                                             "z": 100
                                         })
        load_component2("posts_page","octoplier/pages/PostsPage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component2("settings_page","octoplier/pages/SettingsPage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component2("edit_node_page","octoplier/pages/EditNodePage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component2("view_node_page","octoplier/pages/ViewNodePage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component2("url_entry_page","octoplier/pages/UrlEntryPage.qml",
                                    app_window, {
                                        "visible": false
                                    })
        load_component2("empty_page","octoplier/pages/EmptyPage.qml",
                                    app_window, {
                                        "visible": false
                                    })

        // Our Action Bars.
        load_component2("main_bar","octoplier/actionbars/MainBar.qml", app_window, {})
        load_component2("copy_paste_bar","octoplier/actionbars/CopyPasteBar.qml",
                                        app_window, {
                                            "visible": false,
                                            z: app_settings.copy_paste_bar_z
                                        })
        load_component2("splitter","octoplier/actionbars/Splitter.qml",
                                  app_window, {"visible": false})

        // Our Menus.
        load_component2("web_browser_menu","octoplier/menus/WebBrowserMenu.qml",
                                  app_window, {"visible": false})
        load_component2("node_graph_menu","octoplier/menus/NodeGraphMenu.qml",
                                  app_window, {"visible": false})
        load_component2("node_menu","octoplier/menus/NodeMenu.qml",
                                  app_window, {"visible": false})
        load_component2("group_node_menu","octoplier/menus/GroupNodeMenu.qml",
                                  app_window, {"visible": false})
        load_component2("view_node_menu","octoplier/menus/ViewNodeMenu.qml",
                                  app_window, {"visible": false})

//        // Our Dialogs.
        load_component2("message_dialog","octoplier/dialogs/MessageDialog.qml",
                                        app_window, {"visible": false})
//        load_component2("choose_file_dialog","octoplier/dialogs/ChooseFileDialog.qml",
//                                        app_window, {"visible": false})
        load_component2("url_dialog","octoplier/dialogs/UrlDialog.qml",
                                        app_window, {"visible": false})
        load_component2("node_name_dialog","octoplier/dialogs/NodeNameDialog.qml",
                                        app_window, {"visible": false})

        // Our App Tools.
        // This always draws on top of everything else.
        load_component2("app_tooltip","octoplier/tools/AppTooltip.qml",
                                    app_window, {"z": app_settings.app_tooltip_z, "visible": false})

        // Our App Apis.
        load_component2("node_pool_api","octoplier/apis/NodePoolAPI.qml",
                                    app_window, {"visible": false})
    }

    function update_dependencies() {
        console.log("staring to update dependencies between components")

        // Add the dynamically loaded components under app_windows so that they now render.


        // Update dependencies.
        web_browser_menu.web_browser_page = web_browser_page
        url_entry_page.web_browser_page = web_browser_page
        url_dialog.web_browser_page = web_browser_page

        // Mode change connections.
        main_bar.switch_to_mode.connect(node_graph_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(web_browser_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(posts_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(settings_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(edit_node_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(view_node_page.on_switch_to_mode)
        main_bar.switch_to_mode.connect(url_entry_page.on_switch_to_mode)

        // More option connections.
        main_bar.open_more_options.connect(node_graph_page.on_open_more_options)
        main_bar.open_more_options.connect(
                    web_browser_page.on_open_more_options)
        main_bar.open_more_options.connect(view_node_page.on_open_more_options)

        // More option popup menu connections.
        node_graph_page.popup_menu.connect(node_graph_menu.on_popup_menu)
        node_graph_page.popup_menu_with_centering.connect(
                    node_graph_menu.on_popup_menu_with_centering)
        web_browser_page.popup_menu.connect(web_browser_menu.on_popup_menu)
        view_node_page.popup_menu.connect(view_node_menu.on_popup_menu)

        // Node graph connections.
        node_graph_page.node_graph.node_graph_context_menu_requested.connect(
                    node_graph_menu.on_popup_menu)
        node_graph_page.node_graph.node_context_menu_requested.connect(
                    node_menu.on_popup_menu)
        node_graph_page.node_graph.group_node_context_menu_requested.connect(
                    group_node_menu.on_popup_menu)

        // Copy paste menu.
        // Connection to bring up the copy paste menu on android.
        Qt.inputMethod.visibleChanged.connect(
                    copy_paste_bar.on_virtual_keyboard_visibility_changed)

        // Url Entry and back connections.
        web_browser_menu.switch_to_mode.connect(main_bar.on_switch_to_mode)
        url_entry_page.switch_to_mode.connect(main_bar.on_switch_to_mode)

        // The progress bar is visible when the web browser is loading something.
        splitter.progress_bar.visible = Qt.binding(function () {
            return web_browser_page.web_view_alias.loading
        })
        splitter.progress_bar.value = Qt.binding(function () {
            return web_browser_page.web_view_alias.loadProgress
                    > 100 ? 0 : web_browser_page.web_view_alias.loadProgress
        })

        node_name_dialog.copy_paste_bar = copy_paste_bar
        url_dialog.copy_paste_bar = copy_paste_bar
        url_entry_page.copy_paste_bar = copy_paste_bar

        // Other setup.
        main_bar.on_switch_to_mode(app_settings.node_graph_mode)
        node_graph_page.node_graph.build_test_graph()
        node_graph_page.node_graph.on_load()
        

        // Update the progress now that we've finished updating the dependencies.
        cpp_bridge.mark_load_progress()
        console.log("done updating dependencies between components")
    }

    // Intercept the android back button.
    // We allow this behavior on desktops also for testing.
    // On desktops trying to close the app_window will just pop off the browser stack, until it's empty.
    onClosing: {
        //if (Qt.platform.os == "android") {
        if (web_browser_page.visible == true) {
            if (web_browser_page.web_view_alias.canGoBack) {
                web_browser_page.web_view_alias.goBack()
                close.accepted = false
            }
        }
        //}

        // Cleanup the node pool if we're closing down.
        if (close.accepted) {
            node_graph_page.node_graph.shutdown()
            console.log("Shutting down qml app.")
        }
    }

    // --------------------------------------------
    // Styles
    // --------------------------------------------

    //DesktopMenuStyle {
    //    id: desktop_menu_style
    //}

    //    ListModel {
    //        id: testModel
    //        ListElement {
    //            name: "Bill Smith"
    //            number: "555 3264"
    //        }
    //        ListElement {
    //            name: "John Brown"
    //            number: "555 8426"
    //        }
    //        ListElement {
    //            name: "Sam Wise"
    //            number: "555 0473"
    //        }
    //    }
}
