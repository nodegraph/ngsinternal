import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4



import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

import octoplier.tools 1.0

Dialog {
    // Public Properties.
    property int entity_id: 0
    property alias node_name: node_name_field.text
    height: 100

    // Dependencies.
    property var copy_paste_bar
    
    // Title and buttons.
    title: "Enter a name for the node."
    standardButtons: StandardButton.Ok | StandardButton.Cancel
        
    // URL Text Entry Field.
    AppTextField {
        id: node_name_field
        copy_paste_bar: parent.copy_paste_bar
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: app_settings.action_bar_left_margin
            rightMargin: app_settings.action_bar_right_margin
        }
        onAccepted: {
            node_graph_page.node_graph.create_node(entity_id, node_name_field.text);
        }
    }
}

