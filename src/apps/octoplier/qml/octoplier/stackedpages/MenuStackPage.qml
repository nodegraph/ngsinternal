import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import octoplier.appconfig 1.0
import octoplier.stackedpages 1.0
import octoplier.appwidgets 1.0
import octoplier.menumodels 1.0


AppStackPage{
    id: menu_stack_page

    // Our Properties.
    property bool center_new_nodes: false

    // Framework Methods.
    function on_switch_to_mode(mode) {
        if (mode == app_settings.settings_mode) {
            visible = true;
        } else {
            visible = false;
        }
    }

    // -------------------------------------------------------------------------------------------
    // Create/Remove Pages.
    // -------------------------------------------------------------------------------------------

    function on_node_context_menu() {
        console.log('aaa')
        app_settings.vibrate()
        console.log('bbb')
        center_new_nodes = false
        console.log('ccc')
        stack_view.clear_pages()
        console.log('ddd')
        stack_view.push_by_names("Node Options", "MenuPage", "NodeActions")
        console.log('eee')
        console.log('stack view depth: ' + stack_view.depth)
        console.log('fff')
        visible = true
        console.log('ggg')
    }

    function on_group_node_context_menu(){
        console.log('hhh')
        app_settings.vibrate()
        console.log('iii')
        center_new_nodes = false
        console.log('jjj')
        stack_view.clear_pages()
        console.log('kkk')
        stack_view.push_by_names("Group Node Options", "MenuPage", "GroupNodeActions")
        console.log('lll')
        visible = true
        console.log('mmm')
    }

    function on_node_graph_context_menu() {
        console.log('nnn')
        app_settings.vibrate()
        console.log('ooo')
        center_new_nodes = false
        console.log('ppp')
        stack_view.clear_pages()
        console.log('qqq')
        stack_view.push_by_names("Node Graph Options", "MenuPage", "NodeGraphActions")
        console.log('rrr')
        visible = true
        console.log('sss')
    }

    function show_options(model_name) {
        console.log('ttt')
        center_new_nodes = true
        console.log('uuu')
        visible = true
        console.log('vvv')
        stack_view.clear_pages()
        console.log('www')
        stack_view.push_by_names("Options", "MenuPage", model_name)
        console.log('xxx')
    }

}
