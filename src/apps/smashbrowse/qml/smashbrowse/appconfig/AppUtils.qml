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
