import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import smashbrowse.appconfig 1.0
import smashbrowse.stackedpages 1.0
import smashbrowse.pages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.menumodels 1.0

// The Stack View.
StackView {
    id: stack_view

    // Geometry.
    anchors.fill: parent

//    // Implements back key navigation
//    focus: true
//    Keys.onReleased: if (event.key === Qt.Key_Back && stack_view.depth > 1) {
//                         stack_view.pop();
//                         event.accepted = true;
//                     }

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
        // We keep the delegate empty of transition because this helps with
        // the stack view seg faulting in Qt5Qmld.dll when popping items off the stack.
//        function transitionFinished(properties)
//        {
//            properties.exitItem.opacity = 1
//        }

//        pushTransition: StackViewTransition {
//            PropertyAnimation {
//                target: enterItem
//                property: "opacity"
//                from: 0
//                to: 1
//            }
//            PropertyAnimation {
//                target: exitItem
//                property: "opacity"
//                from: 1
//                to: 0
//            }
//        }
    }

    // --------------------------------------------------------------------------------------------------
    // Script evaluation.
    // --------------------------------------------------------------------------------------------------

    // Evaluates a script in the context of this stack view.
    function execute_script(script) {
        eval(script)
    }

    // --------------------------------------------------------------------------------------------------
    // General Page Management.
    // --------------------------------------------------------------------------------------------------

    function push_page(page) {
        // We don't use the "destroyOnPop' setting.
        // We destroy our pages manually in pop_page, for better control.
        push(page)
        page.parent_stack_view = stack_view
    }

    function pop_page() {
        // Return if we have no pages to pop.
        if (stack_view.depth <= 0) {
            return
        }

        try {
            // Get the top page.
            var page = stack_view.get(stack_view.depth-1)

            // Stops any further get_value and get_value_as_string evaluations.
            //page.parent_stack_view = null

            // The model in the page is manually created, and so must be manually destroyed as well.
            var model = null
            if (page.model_is_dynamic) {
                console.log("aaa")
                model = page.model
                console.log("bbb")
            }

            // Pop the page off the stack.
            if (stack_view.depth == 1) {
                // Note that pop won't pop when there is only one item left. Hence we need to call clear.
                stack_view.clear()
            } else {
                stack_view.pop()
            }

            // Make sure that all the animations have completed.
            // Otherwise we will get seg faults after destroying the page.
            stack_view.completeTransition()

            // Destroy the page.
            page.destroy();

            // Destroy the model.
            if (model) {
                model.destroy();
            }
        } catch (e) {
            console.error("AppStackView::pop_page exception: ", e.message, " stack: ", e.stack);
        }
    }

    function clear_pages(){
        var count = stack_view.depth;
        for (var i=0; i<count; i++) {
            pop_page()
        }
    }

    // --------------------------------------------------------------------------------------------------
    // Page and Model Creation.
    // --------------------------------------------------------------------------------------------------

    function create_component(page_url) {
        // We need to have a valid parent like stack_view, this prevents the
        // page from being destroyed, and lets us control when it gets destroyed.
        var comp = app_loader.load_component(page_url, stack_view, {})
        return comp
    }

    function create_page(page_name) {
        var page_url = "qrc:///qml/smashbrowse/stackedpages/" + page_name + ".qml"
        var page = create_component(page_url)
        return page
    }

    function create_model(model_name) {
        var model_url = "qrc:///qml/smashbrowse/menumodels/" + model_name + ".qml"
        var model = create_component(model_url)
        return model
    }

    // Assumes the model and page are dynamically created.
    function push_by_components(title, page, model) {
        // Configure the page.
        page.model = model
        page.model_is_dynamic = true
        page.set_title(title)

        // Add the page to the stack view.
        stack_view.push_page(page)
    }

    function push_by_names(title, page_name, model_name) {
        // Create the page.
        var page = create_page(page_name)

        // Create the model.
        var model = create_model(model_name)

        // Push the page onto the stack.
        push_by_components(title, page, model)
    }
}
