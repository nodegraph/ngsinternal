import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

Menu {
    title: "View Node Output"

    // Methods.
    function on_popup_menu() {
        app_settings.vibrate()
        popup()
    }

    MenuItem {
        text: "Toggle Selected State"
        onTriggered: {
                node_graph_page.node_graph.toggle_selection_under_long_press();
            }
    }

    MenuItem {
        text: "View"
        onTriggered: {
            var test = {};
            test.aaa = "hello";
            test.bbb = 1;
            test.ccc = 1.123;
            test.ddd = [1,2,3];
            test.eee = ["hello",1,2,{a:1, b:2, c:{xxx: "hi", yyy: "there"}}];
            cpp_bridge.on_test_1(test);
        }
    }
    
    MenuItem {
        text: "Edit"
    }
}
