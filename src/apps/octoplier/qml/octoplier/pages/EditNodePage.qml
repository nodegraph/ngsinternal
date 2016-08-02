import QtQuick 2.6

Rectangle {
    id: edit_node_page
    
    // Dimensions.
    height: app_settings.page_height
    width: app_settings.page_width
    
    // Positioning.
    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z
    
    // Appearance.
    color: app_settings.menu_stack_bg_color

    // Members.
    property var inputs: []
    property var widgets: []

    // Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.edit_node_mode) {
            visible = true;
            menu_stack_page.visible = false
        } else {
            visible = false;
        }
    }

    function on_edit_node(inputs) {
        edit_node_page.inputs = inputs

        for (var i=0; i<inputs.length; i++) {
            var widget = app_loader.load_component("qrc:///qml/octoplier/appwidgets/AppLabel.qml", edit_node_page, {})
            edit_node_page.widgets.push(widget)
        }
    }

    function test() {
        for (var i=0; i<3; i++) {
            var widget = app_loader.load_component("qrc:///qml/octoplier/appwidgets/AppLabel.qml", edit_node_page, {})
            edit_node_page.widgets.push(widget)
        }
    }
}
