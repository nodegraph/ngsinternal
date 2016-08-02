import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.appconfig 1.0
import octoplier.stackedpages 1.0
import octoplier.pages 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0

// The Stack View.
StackView {
    id: stack_view
    anchors.fill: parent

    // Dependencies.
    property var stack_view_header

    // Implements back key navigation
    focus: true
    Keys.onReleased: if (event.key === Qt.Key_Back && stack_view.depth > 1) {
                         stack_view.pop();
                         event.accepted = true;
                     }

    // Delegate.
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

    function push_page(page) {
        page.parent_stack_view = stack_view
        push(page)
        stack_view_header.push_header_title(page.title)
    }

    function pop_model() {
        pop()
        stack_view_header.pop_header_title()
    }

    function clear_models(){
        clear()
        stack_view_header.clear_header_titles()
    }

    function execute_script(script) {
        eval(script)
    }
}
