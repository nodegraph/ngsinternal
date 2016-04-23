import QtQuick 2.4
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

AppMenu {
    title: "Settings"

    // Methods.
    function on_popup_menu() {
        app_settings.vibrate()
        popup()
    }

    MenuItem {
        text: "About"
        onTriggered: {
                // show info about todopile and perhaps show the licenses.
            }
    }

    MenuItem {
        text: "Restore Default Node Graph"
        onTriggered: {
            // make sure that the user is sure about this with a dialog before
            node_graph_page.node_graph.restore_default_node_graph()
        }
    }
}
