import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls.Private 1.0

import QtQuick.Window 2.2
import QtQuick.Dialogs 1.2

import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.4


QtObject {
    function dp( x ) {
        return Math.round( x * Settings.dpiScaleFactor );
    }

    function em( x ) {
        return Math.round( x * TextSingleton.font.pixelSize );
    }

    // Load a component from a file synchronously.
    function load_component(file, parent, properties) {
        var comp = Qt.createComponent(file)
        var obj = comp.createObject(parent, properties)
        if (obj == null) {
            console.log("Error: unable to load: " + file)
            return null
        }
        return obj
    }

    // Load a component from a file asynchronously.
    function load_component_async(file, parent, properties) {
        var comp = Qt.createComponent(file)
        var incubator = comp.incubateObject(parent, properties);

        // Check if the qml file has syntax errors.
        if (!incubator) {
            console.log("Error loading qml file: " + file)
        }

        // The caller should hold a reference to the incubator being returned here
        // so that it won't get garbage collected.
        return incubator
    }

    // Hook up callbacks to the incubator.
    function hookup_incubator(incubator, on_finished_loading) {
        // Hook up callbacks to the incubator.
        if (incubator.status != Component.Ready) {
            incubator.onStatusChanged = function(status) {
                if (status == Component.Ready) {
                    print ("Object", incubator.object, "is now ready!");
                    on_finished_loading(incubator.object)
                }
            }
        } else {
            print ("Object", incubator.object, "is ready immediately!");
            on_finished_loading(incubator.object)
        }
    }
}
