import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

Menu {
    title: "Edit Group Node"

    // Methods.
    function on_popup_menu() {
        app_settings.vibrate()
        popup()
    }

    MenuItem {
        text: "Dive"
        onTriggered: {
            node_graph_page.node_graph.dive();
        }
    }
        
    MenuItem {
        text: "Explode"
        onTriggered: {
            node_graph_page.node_graph.explode_group();
        }
    }
    
    MenuItem {
        text: "View"
    }

    MenuItem {
        text: "Edit"
    }
}
