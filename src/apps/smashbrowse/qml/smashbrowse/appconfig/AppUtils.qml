import QtQml 2.2
import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4

// These utils basically wrap the tedious chore of connecting and disconnecting
// signal in order to receive results into callbacks.
QtObject {
    id: app_utils
    property var license_check_callback

	property Timer timer: Timer {
        interval: 0 
        running: false 
        repeat: false
        onTriggered: app_utils.frame_all()
    }

    function dump_stack_trace() {
        console.log(new Error().stack);
    }

    // Public.
    function check_license(edition, license, callback) {
        // Connect to callback.
        license_checker.license_checked.connect(__on_license_checked)

        // Cache the callbacks.
        license_check_callback = callback

        // Initiate license checking.
        license_checker.check_license(edition, license)
    }

    // Private.
    function __on_license_checked(valid) {
        // Disconnect callbacks.
        license_checker.license_checked.disconnect(__on_license_checked)

        // Perform callback.
        license_check_callback(valid)

        // Clear callback cache.
        license_check_callback = null
    }

    // Public.
    function load_last_graph() {
        // Load the last node graph.
        file_model.load_model()
        file_model.load_graph()
        node_graph_item.update()
        // Frame everything.
        frame_all_on_idle()
    }
    
    function frame_all() {
    	node_graph_item.frame_all()
        node_graph_item.update()
    }
    
    function frame_all_on_idle() {
    	timer.restart()
    }

    // Extracts a sub object using a path, which is an array of strings.
    function get_sub_object(obj, path) {
        if (typeof obj !== 'object') {
            console.error("Error: get_sub_object requires an object.")
            return undefined
        }

        if (path.length <=0) {
            return obj
        }

        // Drill down into the object according to the specified path.
        var data = obj
        for (var i=0; i<path.length; i++) {
            if (typeof data === 'object') {
                if (Object.getPrototypeOf(data) === Array.prototype) {
                    data = data[Number(path[i])]
                } else if (Object.getPrototypeOf(data) === Object.prototype){
                    data = data[path[i]]
                } else {
                    console.log("Error: app_utils::get_sub_object was expecting an object or an array.")
                    return {}
                }
            } else {
                //console.log("Error: app_utils::get_sub_object was expecting an object or an array: " + JSON.stringify(data) + " path: " + path)
                //dump_stack_trace()
                return {}
            }
        }
        return data
    }

    // Set the value inside an object at the given path, which is an array of strings.
    function set_sub_object(obj, path, value) {
        if (typeof obj !== 'object') {
            console.error("Error: set_sub_object requires an object.")
            return
        }

        if (path.length <=0) {
            obj = value
            return
        }

        // Drill down into the object according to the specified path.
        var data = obj
        for (var i=0; i<path.length; i++) {
            // Determine the index.
            var index = null
            if (typeof data === 'object') {
                if (Object.getPrototypeOf(data) === Array.prototype) {
                    // Arrays have number indexes.
                    index = Number(path[i])
                } else if (Object.getPrototypeOf(data) === Object.prototype){
                    // Objects have string indexes.
                    index = path[i]
                } else {
                    console.log("Error: DataListStackPage::set_value was expecting an object or an array 333.")
                }
            } else {
                console.log("Error: DataListStackPage::set_value was expecting an object or an array 4444.")
                return
            }
            // Determine if this is the last element of the path.
            var last = false
            if (i == (path.length-1)) {
                last = true
            }
            // If this is the last element, we set the value on the object or array.
            if (last) {
                data[index] = value
            } else {
                data = data[index]
            }
        }
    }

}
