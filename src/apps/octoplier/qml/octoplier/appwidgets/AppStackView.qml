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

    // Geometry.
    anchors.fill: parent

    // Implements back key navigation
    focus: true
    Keys.onReleased: if (event.key === Qt.Key_Back && stack_view.depth > 1) {
                         stack_view.pop();
                         event.accepted = true;
                     }

    // Dependencies.
    property var stack_view_header

    // Properties.
    property var allow_editing: false

    // Get the current path of the data displayed on the top stack page.
    function get_title_path(begin_index, end_index) {
        var path = []
        for (var i=begin_index; (i<stack_view.depth && i<end_index); i++) {
            var page = stack_view.get(i)
            path.push(page.get_title())
        }
        return path
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
        // Pages are manually created so they must be manually destroyed on pop.
        // The "destroyOnPop" doesn this for us.
        push({item:page, destroyOnPop: true})
        page.parent_stack_view = stack_view
    }

    function pop_page() {
        // Return if we have no pages to pop.
        if (stack_view.depth <= 0) {
            return
        }
        // Get the top page.
        var page = stack_view.get(stack_view.depth-1)
        page.parent_stack_view = null
        if (page.model) {
            // The model is manually created, and so must be manually destroyed.
            page.model.destroy()
        }
        var page = stack_view.pop()
    }

    function clear_pages(){
        var count = stack_view.depth;
        for (var i=0; i<count; i++) {
            stack_view.pop_page()
        }
    }

    function execute_script(script) {
        eval(script)
    }
}
