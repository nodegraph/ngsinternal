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
    //property var nodejs_connected_callback

//    // Public.
//    function connect_to_nodejs(callback) {
//        // Create connection.
//        app_comm.nodejs_connected.connect(__connected_to_nodejs)
//        // Cache callback.
//        nodejs_connected_callback = callback
//        // Start polling.
//        app_worker.start_polling()
//    }

//    // Private.
//    function __connected_to_nodejs() {
//        // Destroy the connection.
//        app_comm.nodejs_connected.disconnect(__connected_to_nodejs)
//        // Call the cached callback.
//        nodejs_connected_callback()
//        // Clear callback cache.
//        nodejs_connected_callback = null
//    }

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
        node_graph_item.frame_all()
        node_graph_item.update()
    }
}
