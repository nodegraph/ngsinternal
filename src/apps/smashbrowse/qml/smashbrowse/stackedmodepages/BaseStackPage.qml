import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4
import QtQuick.Controls.Private 1.0


import smashbrowse.appconfig 1.0
import smashbrowse.stackedmodepages 1.0
import smashbrowse.appwidgets 1.0
import smashbrowse.contentpages.listmodels 1.0


// This class basically just holds an AppStackView.
Rectangle{
    id: stack_page

    height: app_settings.page_height
    width: app_settings.page_width

    x: app_settings.page_x
    y: app_settings.page_y
    z: app_settings.page_z

    // Appearance.
    color: app_settings.menu_stack_bg_color

    // Our internal objects.
    property alias stack_view: stack_view
    
    // Whether we are busy computing something in asynchronous fashion.
    // Currently this only pertains to whether the task_queuer is busy cleaning asynchronously.
    // The other computes in the nodes currently all happen synchronously.
    function is_busy() {
    	if (manipulator.is_busy_cleaning()) {
    		return true;
    	}
    	return false;
    }
    
    function show_busy_page() {
    	if (is_busy()) {
    		console.log('current task is cancelable: ' + manipulator.current_task_is_cancelable())
    	
	        var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/warningpages/ProcessingPage.qml", ng_menu_list_stack_page, {})
	        push_page.set_title('processing nodes')
	        push_page.set_cancelable(manipulator.current_task_is_cancelable())
	        stack_view.push_page(push_page)
	        push_page.visible = true
	        return true;
        }
        return false;
    }
    
    function on_action_ignored() {
        var push_page = app_loader.load_component("qrc:///qml/smashbrowse/contentpages/warningpages/IgnoredActionPage.qml", edit_data_list_stack_page, {})
        push_page.visible = true
        push_page.set_title("Busy")
        stack_view.push_page(push_page)
    }

    // The main stack view.
    AppStackView{
        id: stack_view
        property alias _stack_page: stack_page
    }
}
