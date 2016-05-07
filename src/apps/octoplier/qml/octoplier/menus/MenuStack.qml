import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.config 1.0
import octoplier.menus 1.0
import octoplier.tools 1.0
import octoplier.data 1.0

Rectangle{
    height: app_settings.page_height
    width: app_settings.page_width

    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    color: app_settings.action_bar_bg_color

    // Dependencies.
    property var copy_paste_bar

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.settings_mode) {
            visible = true;
        } else {
            visible = false;
        }
    }

    function test() {
        stack_view.test()
    }

    // The Stack View Header
    Rectangle {
        id: stack_view_header
        height: app_settings.action_bar_height
        width: app_settings.page_width

        property alias title_text: _title_text

        x: 0
        y: 0
        z: 0

        color: app_settings.menu_stack_header_bg_color
        border.width: app_settings.menu_stack_header_border_width
        border.color: app_settings.menu_stack_header_border_color
        radius: app_settings.menu_stack_header_radius

        AppImageButton {
            id: back_button
            anchors.verticalCenter: parent.verticalCenter
            x: app_settings.action_bar_left_margin

            tooltip_text: "Back up"
            image_url: "qrc:///icons/ic_arrow_back_white_48dp.png"
            opacity: stack_view.depth > 1 ? 1 : 0
            onClicked: {
                stack_view.pop()
            }
        }

        Text {
            id: _title_text
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            text: "Value Operators"
            font.pointSize: app_settings.font_point_size
            font.italic: false
            color: "white"
        }

    }

    // The Scroll View.
    ScrollView {
        id: scroll_view

        // Dimensions.
        height: app_settings.page_height - app_settings.action_bar_height
        width: app_settings.page_width

        // Positioning.
        x: 0
        y: app_settings.action_bar_height
        z: 0

        // Scrolling Policy.
        horizontalScrollBarPolicy: Qt.ScrollBarAlwaysOff
        verticalScrollBarPolicy: (Qt.platform.os == "android") || (Qt.platform.os == "ios") ? Qt.ScrollBarAlwaysOff : Qt.ScrollBarAsNeeded

        // The Stack View.
        StackView {
            id: stack_view
            anchors.fill: parent
            // Implements back key navigation
            focus: true
            Keys.onReleased: if (event.key === Qt.Key_Back && stack_view.depth > 1) {
                                 stack_view.pop();
                                 event.accepted = true;
                             }

            delegate: StackViewDelegate {
                function transitionFinished(properties)
                {
                    properties.exitItem.opacity = 1
                }

                pushTransition: StackViewTransition {
                    PropertyAnimation {
                        target: enterItem
                        property: "opacity"
                        from: 0
                        to: 1
                    }
                    PropertyAnimation {
                        target: exitItem
                        property: "opacity"
                        from: 1
                        to: 0
                    }
                }
            }

            property var header_title_stack: []
            function push_header_title(title) {
                header_title_stack.push(title)
                stack_view_header.title_text.text = title;
            }
            function pop_title() {
                var title = header_title_stack.pop()
                stack_view_header.title_text.text = title;
            }

            //Component.onCompleted:

            //initialItem: MenuPage {
            //    id: menu_page
            //}

            function push_model(model) {
                var next_page = app_loader.load_component("qrc:///qml/octoplier/menus/MenuPage.qml", app_window, {})
                next_page.model = model
                push(next_page);
                push_header_title(next_menu_model.get(0).navigator_title)
            }

            function push_model_url(url) {
                var next_menu_model = app_loader.load_component("qrc:///qml/octoplier/data/NodeContextChoices.qml", this, {})
                push_model(next_menu_model)
            }

            function test() {
                push_model_url("qrc:///qml/octoplier/data/NodeContextChoices.qml")
            }
        }



    }

}
