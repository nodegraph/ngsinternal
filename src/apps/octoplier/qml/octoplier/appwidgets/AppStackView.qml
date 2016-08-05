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
            console.log("stack_view was empty!")
            return
        }
        console.log('popping page')
        // Get the top page.
        var page = stack_view.get(stack_view.depth-1)
        // Stops any further get_value and get_value_as_string evaluations.
        page.parent_stack_view = null
        // The model in the page is manually created, and so must be manually destroyed.
        if (page.model) {
            page.model.destroy()
        }
        // Finally we pop it.
        if (stack_view.depth == 1) {
            // Note that pop won't pop when there is only one item left. Hence we need to call clear.
            stack_view.clear()
        } else {
            stack_view.pop()
        }
    }

    function clear_pages(){
        var count = stack_view.depth;
        console.log('clearing num pages: ' + count)
        for (var i=0; i<count; i++) {
            stack_view.pop_page()
        }
        console.log("after clearing stack depth is: " + stack_view.depth)
    }

    function execute_script(script) {
        eval(script)
    }
}
